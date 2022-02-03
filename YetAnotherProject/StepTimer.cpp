#include "StepTimer.hpp"

#include <cmath>

StepTimer::StepTimer()
    : _elapsed_ticks(0),
    _total_ticks(0),
    _left_over_ticks(0),
    _frame_count(0),
    _frames_per_second(0),
    _frames_this_second(0),
    _qpc_second_counter(0),
    _target_elapsed_ticks(ticks_per_second / 60)
{
    QueryPerformanceFrequency(&_qpc_frequency);
    QueryPerformanceCounter(&_qpc_last_time);
    _qpc_max_delta = _qpc_frequency.QuadPart / 10;
}

void StepTimer::reset_elapsed_time()
{
    QueryPerformanceCounter(&_qpc_last_time);

    _left_over_ticks = 0;
    _frames_per_second = 0;
    _frames_this_second = 0;
    _qpc_second_counter = 0;
}

void StepTimer::tick()
{
    // Query the current time.
    LARGE_INTEGER currentTime;

    QueryPerformanceCounter(&currentTime);

    UINT64 timeDelta = currentTime.QuadPart - _qpc_last_time.QuadPart;

    _qpc_last_time = currentTime;
    _qpc_second_counter += timeDelta;

    // Clamp excessively large time deltas (e.g. after paused in the debugger).
    if (timeDelta > _qpc_max_delta)
    {
        timeDelta = _qpc_max_delta;
    }

    // Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
    timeDelta *= ticks_per_second;
    timeDelta /= _qpc_frequency.QuadPart;

    UINT32 lastFrameCount = _frame_count;

    // Fixed timestep update logic
    // If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
    // the clock to exactly match the target value. This prevents tiny and irrelevant errors
    // from accumulating over time. Without this clamping, a game that requested a 60 fps
    // fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
    // accumulate enough tiny errors that it would drop a frame. It is better to just round 
    // small deviations down to zero to leave things running smoothly.

    if (std::abs(static_cast<int>(timeDelta - _target_elapsed_ticks)) < ticks_per_second / 4000)
    {
        timeDelta = _target_elapsed_ticks;
    }

    _left_over_ticks += timeDelta;

    while (_left_over_ticks >= _target_elapsed_ticks)
    {
        _elapsed_ticks = _target_elapsed_ticks;
        _total_ticks += _target_elapsed_ticks;
        _left_over_ticks -= _target_elapsed_ticks;
        _frame_count++;

        // UPDATE here!
        /*if (update)
        {
            update();
        }*/
    }

    // Track the current framerate.
    if (_frame_count != lastFrameCount)
    {
        _frames_this_second++;
    }

    if (_qpc_second_counter >= static_cast<UINT64>(_qpc_frequency.QuadPart))
    {
        _frames_per_second = _frames_this_second;
        _frames_this_second = 0;
        _qpc_second_counter %= _qpc_frequency.QuadPart;
    }
}

UINT32 StepTimer::get_fps() const
{
    return _frames_per_second;
}