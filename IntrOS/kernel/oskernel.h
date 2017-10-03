/******************************************************************************

    @file    IntrOS: oskernel.h
    @author  Rajmund Szymanski
    @date    03.10.2017
    @brief   This file defines set of kernel functions for IntrOS.

 ******************************************************************************

    IntrOS - Copyright (C) 2013 Rajmund Szymanski.

    This file is part of IntrOS distribution.

    IntrOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 3 of the License,
    or (at your option) any later version.

    IntrOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#ifndef __INTROSKERNEL_H
#define __INTROSKERNEL_H

#include <string.h>
#include <stdlib.h>
#include <oscore.h>

/* -------------------------------------------------------------------------- */

#ifndef  OS_ASSERT
#define  OS_ASSERT            0 /* do not include standard assertions         */
#endif

#if     (OS_ASSERT == 0)
#ifndef  NDEBUG
#define  NDEBUG
#endif
#endif

#ifndef  NDEBUG
#define  __ASSERT_MSG
#endif

#include <assert.h>

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

#if OS_FUNCTIONAL
#include <functional>
typedef std::function<void( void )> FUN_t;
#else
typedef     void (* FUN_t)( void );
#endif

#endif

/* -------------------------------------------------------------------------- */

#define  ASIZE( size ) \
    (((size_t)( size )+sizeof(stk_t)-1)/ (sizeof(stk_t)  ))

#define  ABOVE( size ) \
    (((size_t)( size )+sizeof(stk_t)-1)&~(sizeof(stk_t)-1))

#define  BELOW( size ) \
    (((size_t)( size )                )&~(sizeof(stk_t)-1))

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

extern sys_t System; // system data

/* -------------------------------------------------------------------------- */

#ifndef Counter
#define Counter System.cnt
#endif
#define Current System.cur

/* -------------------------------------------------------------------------- */

// initiating and running the system timer
// the port_sys_init procedure is normally called as a constructor
__CONSTRUCTOR
void port_sys_init( void );

/* -------------------------------------------------------------------------- */

// init task 'tsk' for context switch
void core_ctx_init( tsk_t *tsk );

// save status of current process and force yield system control to the next
void core_ctx_switch( void );

// system infinite loop for current process
__NO_RETURN
void core_tsk_loop( void );

// force yield system control to the next process
__NO_RETURN
void core_tsk_switch( void );

/* -------------------------------------------------------------------------- */

// insert object 'obj' into tasks/timers queue before the 'nxt' object
// set object id to 'id'
void core_rdy_insert( void *obj, unsigned id, void *nxt );

// remove object 'obj' from tasks/timers queue
// set object id to ID_STOPPED
void core_rdy_remove( void *obj );

/* -------------------------------------------------------------------------- */

// add timer 'tmr' to tasks/timers queue
// start countdown
__STATIC_INLINE
void core_tmr_insert( tmr_t *tmr ) { core_rdy_insert(tmr, ID_TIMER, Current); }

// remove timer 'tmr' from tasks/timers queue
__STATIC_INLINE
void core_tmr_remove( tmr_t *tmr ) { core_rdy_remove(tmr); }

/* -------------------------------------------------------------------------- */

// add task 'tsk' to tasks/timers queue with id ID_READY
__STATIC_INLINE
void core_tsk_insert( tsk_t *tsk ) { core_rdy_insert(tsk, ID_READY, Current); }

// remove task 'tsk' from tasks/timers queue
__STATIC_INLINE
void core_tsk_remove( tsk_t *tsk ) { core_rdy_remove(tsk); }

/* -------------------------------------------------------------------------- */

// internal handler of system timer
void core_sys_tick( void );

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__INTROSKERNEL_H
