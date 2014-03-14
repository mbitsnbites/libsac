// -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
//-----------------------------------------------------------------------------
// Copyright (c) 2014 Marcus Geelnard
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//     1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software. If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//     2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//     3. This notice may not be removed or altered from any source
//     distribution.
//-----------------------------------------------------------------------------

#ifndef TOOLS_HIRES_TIME_H_
#define TOOLS_HIRES_TIME_H_

#if !defined(WIN32) && defined(_WIN32)
#  define WIN32
#endif

#ifdef WIN32
#  include <windows.h>
#else
#  include <sys/time.h>
#endif

#include <list>

namespace tools {

class hires_time_t {
  public:
    hires_time_t() {
#ifdef WIN32
      if(QueryPerformanceFrequency((LARGE_INTEGER*)&m_time_freq)) {
        QueryPerformanceCounter((LARGE_INTEGER*)&m_time_start);
      else
        mTimeFreq = 0;
#else
      struct timeval tv;
      gettimeofday(&tv, 0);
      m_time_start = (long long)tv.tv_sec * 1000000LL + (long long)tv.tv_usec;
#endif
    }

    /// @brief Get the current time in seconds.
    double get_time() {
#ifdef WIN32
      __int64 t;
      QueryPerformanceCounter((LARGE_INTEGER*)&t);
      return double(t - m_time_start) / double(m_time_freq);
#else
      struct timeval tv;
      gettimeofday(&tv, 0);
      long long t = (long long)tv.tv_sec * 1000000LL + (long long)tv.tv_usec;
      return (1e-6) * double(t - m_time_start);
#endif
    }

    /// @brief Push current time (start measuring).
    void push() {
      m_stack.push_back(get_time());
    }

    /// @brief Pop delta time, in seconds, since last push.
    double pop_delta() {
      double delta = get_time() - m_stack.back();
      m_stack.pop_back();
      return delta;
    }

  private:
    std::list<double> m_stack;
#ifdef WIN32
    __int64 m_time_freq;
    __int64 m_time_start;
#else
    long long m_time_start;
#endif
};

} // namespace tools

#endif // TOOLS_HIRES_TIME_H_
