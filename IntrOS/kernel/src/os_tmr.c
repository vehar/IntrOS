/******************************************************************************

    @file    IntrOS: os_tmr.c
    @author  Rajmund Szymanski
    @date    30.12.2017
    @brief   This file provides set of functions for IntrOS.

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

#include "inc/os_tmr.h"

/* -------------------------------------------------------------------------- */
void tmr_init( tmr_t *tmr, fun_t *state )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	port_sys_lock();

	memset(tmr, 0, sizeof(tmr_t));

	tmr->state = state;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_tmr_start( tmr_t *tmr )
/* -------------------------------------------------------------------------- */
{
	if (tmr->id == ID_STOPPED)
		core_tmr_insert(tmr);
}

/* -------------------------------------------------------------------------- */
void tmr_startUntil( tmr_t *tmr, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	port_sys_lock();

	tmr->start  = core_sys_time();
	tmr->delay  = time - tmr->start;
	if (tmr->delay > ((CNT_MAX)>>1))
		tmr->delay = 0;
	tmr->period = 0;

	priv_tmr_start(tmr);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tmr_start( tmr_t *tmr, cnt_t delay, cnt_t period )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	port_sys_lock();

	tmr->start  = core_sys_time();
	tmr->delay  = delay;
	tmr->period = period;

	priv_tmr_start(tmr);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tmr_startFrom( tmr_t *tmr, cnt_t delay, cnt_t period, fun_t *proc )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	port_sys_lock();

	tmr->state  = proc;
	tmr->start  = core_sys_time();
	tmr->delay  = delay;
	tmr->period = period;

	priv_tmr_start(tmr);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned tmr_take( tmr_t *tmr )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(tmr);

	if (tmr->id == ID_STOPPED)
		event = E_SUCCESS;

	return event;
}

/* -------------------------------------------------------------------------- */
void tmr_wait( tmr_t *tmr )
/* -------------------------------------------------------------------------- */
{
	unsigned signal;

	assert(tmr);

	if (tmr->id != ID_STOPPED)
	{
		signal = tmr->signal;
		while (tmr->signal == signal) core_ctx_switch();
	}
}

/* -------------------------------------------------------------------------- */
