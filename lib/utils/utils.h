/*****************************************************************************
 *   Copyright 2013 - 2014 Yichao Yu <yyc1992@gmail.com>                     *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU Lesser General Public License as          *
 *   published by the Free Software Foundation; either version 2.1 of the    *
 *   License, or (at your option) version 3, or any later version accepted   *
 *   by the membership of KDE e.V. (or its successor approved by the         *
 *   membership of KDE e.V.), which shall act as a proxy defined in          *
 *   Section 6 of version 3 of the license.                                  *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 *   Lesser General Public License for more details.                         *
 *                                                                           *
 *   You should have received a copy of the GNU Lesser General Public        *
 *   License along with this library. If not,                                *
 *   see <http://www.gnu.org/licenses/>.                                     *
 *****************************************************************************/

#ifndef _QTC_UTILS_UTILS_H_
#define _QTC_UTILS_UTILS_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <math.h>

#include "macros.h"

#include <utility>
#include <type_traits>

/**
 * \file utils.h
 * \author Yichao Yu <yyc1992@gmail.com>
 * \brief Some generic functions and macros.
 */

/**
 * Generic call back function.
 */
typedef void (*QtcCallback)(void*);

/**
 * Allocate memory and initialize it to zero.
 * \param size size of the memory
 */
QTC_ALWAYS_INLINE static inline void*
qtcAlloc0(size_t size)
{
    void *p = malloc(size);
    memset(p, 0, size);
    return p;
}

/**
 * Allocate memory of size \param size for a \param type.
 * The memory is initialized to zero.
 * \param type type pointed to by the pointer
 * \param size size of the memory
 * \sa qtcAlloc0
 */
#define qtcNewSize(type, size) ((type*)qtcAlloc0(size))

/**
 * Allocate memory for a \param type or an array of \param type.
 * The memory is initialized to zero.
 * \param type type pointed to by the pointer
 * \param n (optional) elements in the array (default: 1)
 * \sa qtcNewSize
 */
#define qtcNew(type, n...)                              \
    qtcNewSize(type, sizeof(type) * (QTC_DEFAULT(n, 1)))

namespace QtCurve {
/**
 * Turn a variable into a const reference. This is useful for range-based for
 * loop where a non-const variable may cause unnecessary copy.
 */
template <class T>
static inline const T&
const_(const T &t)
{
    return t;
}
}

/**
 * Define a buffer type (struct) for \param type.
 * \sa QTC_DEF_LOCAL_BUFF
 */
#define QTC_BUFF_TYPE(type)                     \
    struct {                                    \
        union {                                 \
            type *p;                            \
            void *_p;                           \
        };                                      \
        size_t l;                               \
        type *const static_p;                   \
        const size_t static_l;                  \
    }

/**
 * \brief Define a local buffer for holding an array of \param type.
 * \param type type of the array element.
 * \param name name of the variable
 * \param stack_size the maximum number of elements in the array that will be
 *                   kept on the stack.
 * \param size real size of the array.
 *
 * This macro define a buffer \param name for an array of \param type.
 * The buffer's field p points to the address of the array.
 * Use QTC_FREE_LOCAL_BUFF to free the buffer.
 * \sa QTC_FREE_LOCAL_BUFF
 * \sa QTC_RESIZE_LOCAL_BUFF
 */
#define QTC_DEF_LOCAL_BUFF(type, name, stack_size, size)                \
    type __##qtc_local_buff##name[stack_size];                          \
    QTC_BUFF_TYPE(type) name = {                                        \
        {__##qtc_local_buff##name},                                     \
        sizeof(__##qtc_local_buff##name) / sizeof(type),                \
        __##qtc_local_buff##name,                                       \
        sizeof(__##qtc_local_buff##name) / sizeof(type)                 \
    };                                                                  \
    QTC_RESIZE_LOCAL_BUFF(name, size)

/**
 * \brief Resize a local buffer defined with QTC_DEF_LOCAL_BUFF for holding
 * .      more elements.
 * \param name name of the buffer
 * \param size new minimum size of the array.
 *
 * This macro resizes a buffer \param name defined with QTC_DEF_LOCAL_BUFF for
 * holding at least \param size elements.
 * \sa QTC_DEF_LOCAL_BUFF
 * \sa QTC_FREE_LOCAL_BUFF
 */
#define QTC_RESIZE_LOCAL_BUFF(name, size) do {                          \
        size_t __new_size = (size);                                     \
        if (__new_size <= (name).l || __new_size <= (name).static_l)    \
            break;                                                      \
        (name).l = __new_size;                                          \
        size_t __alloc_size = sizeof(*(name).p) * __new_size;           \
        if ((name).p == (name).static_p) {                              \
            (name)._p = malloc(__alloc_size);                           \
        } else {                                                        \
            (name)._p = realloc((name)._p, __alloc_size);               \
        }                                                               \
    } while (0)

/**
 * \brief Free a local buffer defined with QTC_DEF_LOCAL_BUFF if necessary.
 * \param name name of the buffer
 *
 * \sa QTC_DEF_LOCAL_BUFF
 * \sa QTC_RESIZE_LOCAL_BUFF
 */
#define QTC_FREE_LOCAL_BUFF(name) do {          \
        if ((name).p != (name).static_p) {      \
            free((name)._p);                    \
        }                                       \
    } while (0)

const char *qtcGetProgName();
const char *qtcVersion();

template<typename T>
using qtcPtrType = typename std::remove_reference<
    typename std::remove_cv<
        typename std::remove_pointer<T>::type>::type>::type;

#define qtcMemPtr(ptr, name) &qtcPtrType<decltype(ptr)>::name

template<typename T, typename First>
static inline bool
qtcOneOf(T &&value, First &&first)
{
    return value == first;
}
template<typename T, typename First, typename... Rest>
static inline bool
qtcOneOf(T &&value, First &&first, Rest&&... rest)
{
    return value == first || qtcOneOf(std::forward<T>(value),
                                      std::forward<Rest>(rest)...);
}
template<typename... Args>
static inline bool
qtcNoneOf(Args&&... args)
{
    return !qtcOneOf(std::forward<Args>(args)...);
}

// Use lambda for lazy evaluation of \param def
#define qtcDefault(val, def)                    \
    (([&]() {                                   \
            auto __val = (val);                 \
            return __val ? __val : (def);       \
        })())
// Use lambda for lazy evaluation of \param args
// C++ allows returning void expression! =) See the quote of the standard
// (here)[http://gcc.gnu.org/ml/gcc/2006-10/msg00697.html]
// The current c++ implementation of this macro does not support functions
// with types that do not have accessible default constructor (including
// references) as return type.
#define qtcCall(func, args...)                                          \
    (([&]() {                                                           \
            auto __func = (func);                                       \
            return __func ? __func(args) : decltype(__func(args))();    \
        })())
#define qtcAssign(addr, exp) do {               \
        auto __addr = (addr);                   \
        if (__addr) {                           \
            *__addr = (exp);                    \
        }                                       \
    } while(0)

// Returning a void expression is valid c++, see above
// Or https://gcc.gnu.org/ml/gcc/2006-10/msg00697.html
#define QTC_RET_IF_FAIL(exp, val...) do {       \
        if (!qtcLikely(exp)) {                  \
            return (QTC_DEFAULT(val, (void)0)); \
        }                                       \
    } while (0)

#endif
