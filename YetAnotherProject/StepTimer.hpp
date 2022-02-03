#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

class StepTimer
{
public:
    StepTimer();

    void reset_elapsed_time();
    void tick();
    UINT32 get_fps() const;
private:
    static const UINT64 ticks_per_second = 10000000;
    LARGE_INTEGER _qpc_frequency;
    LARGE_INTEGER _qpc_last_time;
    UINT64 _qpc_max_delta;

    UINT64 _elapsed_ticks;
    UINT64 _total_ticks;
    UINT64 _left_over_ticks;

    UINT32 _frame_count;
    UINT32 _frames_per_second;
    UINT32 _frames_this_second;
    UINT64 _qpc_second_counter;

    UINT64 _target_elapsed_ticks;
};