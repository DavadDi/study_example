#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

//#include <hiredis.h>
//#include <async.h>

#include <adapters/redis_client.hpp>
//#include "redis_client.hpp"

#include <signal.h>

static void signal_handler(int sig)
{
    if (sig == SIGINT)
    {
        reactor::CReactor::instance()->end_event_loop();
    }
}

/*
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

    // Disconnect after receiving the reply to GET 
    // redisAsyncDisconnect(c);
}
*/


int main (int argc, char **argv) 
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signal_handler);

    CLoggerMgr logger("reactor.prop");

    reactor::CReactor *rt = reactor::CReactor::instance();
    CRedisClient *redis_client = new CRedisClient("127.0.0.1", 6379, rt);
    redis_client->connect();

    rt->run_event_loop();

    return 0;
}
