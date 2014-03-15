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

#ifndef TOOLS_SCOPED_PTR_H_
#define TOOLS_SCOPED_PTR_H_

namespace tools {

/// @brief Scoped pointer class.
/// This is a simple scoped pointer class, similar to C++11 unique_ptr.
template <class T>
class scoped_ptr {
  public:
    scoped_ptr() : m_ptr(0) {}

    scoped_ptr(T *ptr) : m_ptr(ptr) {}

    scoped_ptr(scoped_ptr<T> &other) : m_ptr(other.m_ptr) {
      other.m_ptr = 0;
    }

    ~scoped_ptr() {
      delete m_ptr;
    }

    scoped_ptr<T> &operator=(T *ptr) {
      reset(ptr);
      return *this;
    }

    scoped_ptr<T> &operator=(scoped_ptr<T> &other) {
      reset(other.m_ptr);
      other.m_ptr = 0;
      return *this;
    }

    T *operator->() const {
      return m_ptr;
    }

    void reset(T *ptr) {
      delete m_ptr;
      m_ptr = ptr;
    }

    T *get() const {
      return m_ptr;
    }

    T *release() {
      T *ptr = m_ptr;
      m_ptr = 0;
      return ptr;
    }

  private:
    T *m_ptr;
};

} // namespace tools

#endif // TOOLS_SCOPED_PTR_H_
