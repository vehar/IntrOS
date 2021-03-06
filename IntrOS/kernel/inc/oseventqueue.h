/******************************************************************************

    @file    IntrOS: oseventqueue.h
    @author  Rajmund Szymanski
    @date    19.11.2018
    @brief   This file contains definitions for IntrOS.

 ******************************************************************************

   Copyright (c) 2018 Rajmund Szymanski. All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

 ******************************************************************************/

#ifndef __INTROS_EVQ_H
#define __INTROS_EVQ_H

#include "oskernel.h"

/******************************************************************************
 *
 * Name              : event queue
 *
 ******************************************************************************/

typedef struct __evq evq_t, * const evq_id;

struct __evq
{
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned head;  // first element to read from data buffer
	unsigned tail;  // first element to write into data buffer
	unsigned*data;  // data buffer
};

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : _EVQ_INIT
 *
 * Description       : create and initialize an event queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *   data            : event queue data buffer
 *
 * Return            : event queue object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _EVQ_INIT( _limit, _data ) { 0, _limit, 0, 0, _data }

/******************************************************************************
 *
 * Name              : _EVQ_DATA
 *
 * Description       : create an event queue data buffer
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *
 * Return            : event queue data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _EVQ_DATA( _limit ) (unsigned[_limit]){ 0 }
#endif

/******************************************************************************
 *
 * Name              : OS_EVQ
 *
 * Description       : define and initialize an event queue object
 *
 * Parameters
 *   evq             : name of a pointer to event queue object
 *   limit           : size of a queue (max number of stored events)
 *
 ******************************************************************************/

#define             OS_EVQ( evq, limit )                                      \
                       struct { evq_t evq; unsigned buf[limit]; } evq##__wrk = \
                       { _EVQ_INIT( limit, evq##__wrk.buf ), { 0 } };           \
                       evq_id evq = & evq##__wrk.evq

/******************************************************************************
 *
 * Name              : static_EVQ
 *
 * Description       : define and initialize a static event queue object
 *
 * Parameters
 *   evq             : name of a pointer to event queue object
 *   limit           : size of a queue (max number of stored events)
 *
 ******************************************************************************/

#define         static_EVQ( evq, limit )                                      \
                static struct { evq_t evq; unsigned buf[limit]; } evq##__wrk = \
                       { _EVQ_INIT( limit, evq##__wrk.buf ), { 0 } };           \
                static evq_id evq = & evq##__wrk.evq

/******************************************************************************
 *
 * Name              : EVQ_INIT
 *
 * Description       : create and initialize an event queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *
 * Return            : event queue object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                EVQ_INIT( limit ) \
                      _EVQ_INIT( limit, _EVQ_DATA( limit ) )
#endif

/******************************************************************************
 *
 * Name              : EVQ_CREATE
 * Alias             : EVQ_NEW
 *
 * Description       : create and initialize an event queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *
 * Return            : pointer to event queue object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                EVQ_CREATE( limit ) \
           (evq_t[]) { EVQ_INIT  ( limit ) }
#define                EVQ_NEW \
                       EVQ_CREATE
#endif

/******************************************************************************
 *
 * Name              : evq_init
 *
 * Description       : initialize an event queue object
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   data            : event queue data buffer
 *   bufsize         : size of the data buffer (in bytes)
 *
 * Return            : none
 *
 ******************************************************************************/

void evq_init( evq_t *evq, unsigned *data, unsigned bufsize );

/******************************************************************************
 *
 * Name              : evq_take
 * Alias             : evq_tryWait
 *
 * Description       : try to transfer event data from the event queue object,
 *                     don't wait if the event queue object is empty
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   data            : pointer to store event data
 *
 * Return
 *   E_SUCCESS       : event data was successfully transferred from the event queue object
 *   E_FAILURE       : event queue object is empty
 *
 ******************************************************************************/

unsigned evq_take( evq_t *evq, unsigned *data );

__STATIC_INLINE
unsigned evq_tryWait( evq_t *evq, unsigned *data ) { return evq_take(evq, data); }

/******************************************************************************
 *
 * Name              : evq_wait
 *
 * Description       : try to transfer event data from the event queue object,
 *                     wait indefinitely while the event queue object is empty
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   data            : pointer to store event data
 *
 * Return            : none
 *
 ******************************************************************************/

void evq_wait( evq_t *evq, unsigned *data );

/******************************************************************************
 *
 * Name              : evq_give
 *
 * Description       : try to transfer event data to the event queue object,
 *                     don't wait if the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   data            : event value
 *
 * Return
 *   E_SUCCESS       : event data was successfully transferred to the event queue object
 *   E_FAILURE       : event queue object is full
 *
 ******************************************************************************/

unsigned evq_give( evq_t *evq, unsigned data );

/******************************************************************************
 *
 * Name              : evq_send
 *
 * Description       : try to transfer event data to the event queue object,
 *                     wait indefinitely while the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   data            : event value
 *
 * Return            : none
 *
 ******************************************************************************/

void evq_send( evq_t *evq, unsigned data );

/******************************************************************************
 *
 * Name              : evq_push
 *
 * Description       : transfer event data to the event queue object,
 *                     remove the oldest event data if the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   data            : event value
 *
 * Return            : none
 *
 ******************************************************************************/

void evq_push( evq_t *evq, unsigned data );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : EventQueueT<>
 *
 * Description       : create and initialize an event queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored events)
 *
 ******************************************************************************/

template<unsigned limit_>
struct EventQueueT : public __evq
{
	EventQueueT( void ): __evq _EVQ_INIT(limit_, data_) {}

	unsigned take   ( unsigned*_data ) { return evq_take   (this, _data); }
	unsigned take   ( unsigned&_data ) { return evq_take   (this,&_data); }
	unsigned tryWait( unsigned*_data ) { return evq_tryWait(this, _data); }
	unsigned tryWait( unsigned&_data ) { return evq_tryWait(this,&_data); }
	void     wait   ( unsigned*_data ) {        evq_wait   (this, _data); }
	void     wait   ( unsigned&_data ) {        evq_wait   (this,&_data); }
	unsigned give   ( unsigned _data ) { return evq_give   (this, _data); }
	void     send   ( unsigned _data ) {        evq_send   (this, _data); }
	void     push   ( unsigned _data ) {        evq_push   (this, _data); }
	unsigned count  ( void )           { return evq_count  (this);        }
	unsigned space  ( void )           { return evq_space  (this);        }
	unsigned limit  ( void )           { return evq_limit  (this);        }

	private:
	unsigned data_[limit_];
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_EVQ_H
