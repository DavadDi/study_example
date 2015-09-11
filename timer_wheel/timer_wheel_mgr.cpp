#include "reactor/timer_wheel_mgr.hpp"
#include "reactor/event_handler.hpp"

namespace reactor
{
    void evt_timer_cb(void *event_handler, void *data)
    {
        CEventHandler *evt = static_cast<CEventHandler *>(event_handler);
        evt->handle_timeout(time(0), data);    
    }

    CTimerWheelMgr::CTimerWheelMgr(): m_last_timer_id(0)
    {
        itimer_mgr_init(&m_timer_mgr, time(0)*1000, 1); // ms
    }

    int CTimerWheelMgr::add_timer(CEventHandler *handler, int period, int repeat, void *data) // second
    {
        itimer_evt  *tm_evt = new itimer_evt;
        itimer_evt_init(tm_evt, evt_timer_cb, handler, data);
        itimer_evt_start(&m_timer_mgr, tm_evt, period*1000, repeat);


        // add tm_evt to map
        m_last_timer_id++;
        if (m_last_timer_id > 0x7FFFFFFF)
        {
            m_last_timer_id = 0;
        }

        m_timer_evt_map.insert({m_last_timer_id, tm_evt});

        return m_last_timer_id;
    }

    int CTimerWheelMgr::del_timer(int timer_id)
    {
        auto it = m_timer_evt_map.find(timer_id);

        if (it != m_timer_evt_map.end())
        {
            itimer_evt  *tm_evt = it->second; // get from map
            itimer_evt_stop(&m_timer_mgr, tm_evt);
            itimer_evt_destroy(tm_evt);

            return 0;
        }

        return -1; // not found
    }

    int CTimerWheelMgr::update_timer()
    {
        itimer_mgr_run(&m_timer_mgr, time(0)*1000);
        return 0;
    }

    CTimerWheelMgr::~CTimerWheelMgr()
    {
        itimer_mgr_destroy(&m_timer_mgr);

        for (auto it = m_timer_evt_map.begin(); it != m_timer_evt_map.end(); it++)
        {
            if (it->second != nullptr)
            {
                itimer_evt_destroy(it->second);
            }
        }
    }
}
