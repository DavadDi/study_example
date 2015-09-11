#ifndef _timer_wheel_mgr_h__
#define _timer_wheel_mgr_h__

#include "reactor/timer_mgr.hpp"
#include "reactor/itimer.hpp"
#include <unordered_map>

namespace reactor
{
    class CTimerWheelMgr : public CTimerMgr
    {
        public:
            CTimerWheelMgr();
            ~CTimerWheelMgr();

            virtual int add_timer(CEventHandler *handler, int period, int repeat = 0, void *data = nullptr);
            virtual int del_timer(int timer_id);

            virtual int update_timer();

        protected:
            itimer_mgr                                 m_timer_mgr;
            std::unordered_map<uint32_t, itimer_evt*>  m_timer_evt_map;
            int32_t                                    m_last_timer_id;
    };
}

#endif


