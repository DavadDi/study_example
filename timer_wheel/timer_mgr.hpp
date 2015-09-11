#ifndef __timer_mgr_h__
#define __timer_mgr_h__

#include "reactor/define.hpp"

namespace reactor
{
    class CEventHandler;
    class CTimerMgr
    {
        public:
            virtual ~CTimerMgr()
            {
            }

            virtual int add_timer(CEventHandler *handler, int period, int repeat = 0, void *data= nullptr) = 0;
            virtual int del_timer(int timer_id) = 0;

            virtual int update_timer() = 0;
    };
}

#endif

