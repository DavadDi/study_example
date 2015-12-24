//
//  redis_client.hpp
//  pt_server
//
//  Created by davad.di on 12/23/15.
//  Copyright © 2015 davad.di. All rights reserved.
//
#ifndef redis_client_h
#define redis_client_h

#include "reactor/define.hpp"
#include "reactor/event_handler.hpp"
#include "hiredis.h"
#include "async.h"

using namespace reactor;

static  void redisReactorAddRead(void *arg);
static  void redisReactorDelRead(void *arg);
static  void redisReactorAddWrite(void *arg);
static  void redisReactorDelWrite(void *arg);
static  void redisReactorCleanup(void *arg);

void connectCallBack(const redisAsyncContext *c, int status);
void disconnectCallBack(const redisAsyncContext *c, int status);

void get_call_fun(redisAsyncContext *c, void *r, void *arg)
{
    redisReply *reply = (redisReply *)r;
    std::string *key_str = (std::string *)arg;

    if (reply == NULL)
    {
        delete key_str;
        return;
    }

    LOG_INFO("[%s] -> %s\n", key_str->c_str(), reply->str);

    delete key_str;

    /* Disconnect after receiving the reply to GET */
    // redisAsyncDisconnect(c);
}

// -------------------------------------------------------------------
// !!NOTE, if obj conneted to server faild and unregister from epoll, 
// prog exit, this object my leak memory
// -------------------------------------------------------------------

class CRedisClient : public CEventHandler
{
    public:
        // enum {MAX_BUF_SIZE = 4096};
        typedef CEventHandler super;

        CRedisClient(const char *srv_ip, uint16_t srv_port, reactor::CReactor *reactor) 
            : super(reactor)
        {
            m_srv_ip_str = srv_ip;
            m_srv_port = srv_port;
        }

        int connect()
        {
            LOG_DEBUG("Enter CRedisClient connect()");
            m_client_status = CONNECT_STATUS::CLIENT_CONNECTING;

            clear_redis_context();

            m_redis_context = redisAsyncConnect(m_srv_ip_str.c_str(), m_srv_port);
            if (m_redis_context == nullptr)
            {
                return -1;
            }

            if (m_redis_context->err)
            {
                LOG_INFO("Connect to %s:%d Error: %s", 
                        m_srv_ip_str.c_str(), m_srv_port, m_redis_context->errstr);

                return -1;
            }

            if (m_timer_id == 0)
            {
                m_timer_id = this->reactor()->regist_timer(this, m_timeout_value); // only one time
                LOG_DEBUG("Client regist timer to reactor id %d, timeout %d", m_timer_id, m_timeout_value);
            }

            this->attach();

            return 0;
        }

        virtual ~CRedisClient()
        {
            // maybe should not free redis context in deconstuct!!
            m_delete_redis_context = true;
            clear_redis_context();
        }

        virtual int open(void *data = nullptr)
        {
            m_client_status = CONNECT_STATUS::CLIENT_CONNECTED;

            m_delete_redis_context = false;

            if (m_timer_id == 0)
            {
                m_timer_id = this->reactor()->regist_timer(this, m_timeout_value); // only one time
                LOG_DEBUG("Client regist timer to reactor id %d, timeout %d", 
                                                    m_timer_id, m_timeout_value);
            }
            
            LOG_INFO("Connect to RedisServer %s:%d succeed!!", 
                                               m_srv_ip_str.c_str(), m_srv_port);
            
            return 0;
        }

        virtual int handle_input(socket_t socket)
        {
            redisAsyncHandleRead(m_redis_context);
            return 0;
        }

        virtual int handle_output(socket_t socket)
        {
            redisAsyncHandleWrite(m_redis_context);
            return 0;
        }

        virtual int handle_timeout(uint32_t tm, void *data = nullptr)
        {
            // LOG_DEBUG("Enter into timeout function....");
            if (m_client_status == CONNECT_STATUS::CLIENT_CONNECTED)
            {
                /* just for test */
                std::string key = std::to_string(tm);

                LOG_DEBUG("Set key %s", key.c_str());
                redisAsyncCommand(m_redis_context, NULL, NULL, "SET %s %s",key.c_str(), "aaa");
                redisAsyncCommand(m_redis_context, get_call_fun, (char*)new string(key), "GET %s", key.c_str());
            }
            else
            {
                static uint32_t last_tm = 0;
                if ((tm - last_tm) >= m_timeout_interval)
                { 
                    //reconnect
                    LOG_DEBUG("Start reconnect now ...");
                    this->connect();

                    m_timeout_interval = m_timeout_interval * 2;
                    if (m_timeout_interval > 32)
                    {
                        m_timeout_interval = 1;
                    }

                    last_tm = tm;
                }
            }

            return 0;
        }

        virtual int handle_close(socket_t socket = INVALID_SOCKET, uint32_t mask = 0)
        {
            LOG_DEBUG("Enter into handle_close()"); 
            m_client_status = CONNECT_STATUS::CLIENT_UNCONNECTED;
            
            // epoll call delete this handler
            if (mask & RE_MASK_DEL)
            {
                LOG_DEBUG("Call RE_MASK_DEL now");

                if (this->m_timer_id && (this->reactor() != nullptr))
                {
                    this->reactor()->unregist_timer(this->m_timer_id);
                    this->m_timer_id = 0;
                }

                delete this;
                return 0;
            }
            
            this->reactor()->del_event(this,0);
            return 0;
        }

        void clear_redis_context()
        {
            if (m_delete_redis_context && m_redis_context != nullptr)
            {
                LOG_DEBUG("Call redisAsynFree() now");
                redisAsyncFree(m_redis_context);
                m_redis_context = nullptr;
            }
        }

        int attach()
        {
            LOG_DEBUG("Enter attatch function... ");

            redisContext *context = &(m_redis_context->c);
            if (m_redis_context->ev.data != NULL)
            {
                return -1;
            }

            // set callback function
            redisAsyncSetConnectCallback(m_redis_context,connectCallBack);
            redisAsyncSetDisconnectCallback(m_redis_context,disconnectCallBack);

            this->set_handle(context->fd); // set handler

            m_redis_context->ev.addRead = redisReactorAddRead;
            m_redis_context->ev.delRead = redisReactorDelRead;
            m_redis_context->ev.addWrite = redisReactorAddWrite;
            m_redis_context->ev.delWrite = redisReactorDelWrite;
            m_redis_context->ev.cleanup = redisReactorCleanup;
            m_redis_context->ev.data = this;

            LOG_DEBUG("ac->ev.data %p", m_redis_context->ev.data);

            this->add_read();
            this->add_write();

            return 0;
        }

        void add_read()
        {
            LOG_TRACE_METHOD(__func__);

            if( (this->m_current_event_mask & reactor::EVENT_READ) > 0)
            {
                LOG_DEBUG("EV_READ(0x%0x) already in event_mask 0x%x",
                        reactor::EVENT_READ, this->m_current_event_mask);

                return;
            }

            this->reactor()->add_event(this, reactor::EVENT_READ);
        }

        void del_read()
        {
            LOG_TRACE_METHOD(__func__);
            this->reactor()->mod_event(this, this->m_current_event_mask&(~reactor::EVENT_READ));
        }

        void add_write()
        {
            LOG_TRACE_METHOD(__func__);
            this->schedule_write();
        }

        void del_write()
        {
            LOG_TRACE_METHOD(__func__);
            this->cancel_schedule_write();
        }

        void clean_up()
        {
            LOG_TRACE_METHOD(__func__);
        }

        // note!!!
        // connenct not succeed. we can free redis context. ]
        // But if connect succeed and borken, we don't connect 

    protected:
        std::string m_srv_ip_str;
        uint16_t    m_srv_port;

        CONNECT_STATUS  m_client_status = CONNECT_STATUS::CLIENT_UNCONNECTED;

        int m_timer_id  = 0;
        uint32_t m_timeout_value = 1;
        uint32_t  m_timeout_interval = 1;

        bool    m_delete_redis_context = true;
        redisAsyncContext *m_redis_context = nullptr;
};

static void redisReactorAddRead(void *arg) 
{
    LOG_DEBUG("Enter redisReactorAddRead() arg %p", arg);
    CRedisClient *event_handler = (CRedisClient *)arg;
    event_handler->add_read();
}

static void redisReactorDelRead(void *arg) 
{
    CRedisClient *event_handler = (CRedisClient *)arg;
    event_handler->del_read();
}

static void redisReactorAddWrite(void *arg) 
{
    CRedisClient *event_handler = (CRedisClient *)arg;
    event_handler->add_write();
}

static void redisReactorDelWrite(void *arg) 
{
    CRedisClient *event_handler = (CRedisClient *)arg;
    event_handler->del_write();
}

static void redisReactorCleanup(void *arg) 
{
    CRedisClient *event_handler = (CRedisClient *)arg;
    event_handler->clean_up();
}

void connectCallBack(const redisAsyncContext *ac, int status)
{
    if (status != REDIS_OK)
    {
        LOG_ERROR("connectCallBack() Error: %s", ac->errstr);
        return;
    }

    CRedisClient *event_handler = (CRedisClient *)ac->ev.data;
    event_handler->open();

    LOG_INFO("RedisClient Connected...");    
}

void disconnectCallBack(const redisAsyncContext *ac, int status)
{ 
    CRedisClient *event_handler = (CRedisClient *)ac->ev.data;
    event_handler->handle_close(0,0);

    if (status != REDIS_OK)
    {
        LOG_INFO("disconnectCallBack()!! Error: %s", ac->errstr);
        return;
    }

    LOG_INFO("RedisClient Disconnected...");
}

#endif /* redis_client_h */
