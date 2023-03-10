/* RMNLIB - Library of useful routines for C and FORTRAN programming
 * Copyright (C) 1975-2000  Division de Recherche en Prevision Numerique
 *                          Environnement Canada
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

//! \file gossip_timeout.c Gossip timeout counter functions

static int timeout = 0;
static int baseTimeout = 20;
static int pingInterval = 15;

void reset_timeout_counter() {
    timeout = baseTimeout;
}


void increment_timeout_counter() {
    timeout++;
}


void decrement_timeout_counter() {
    timeout--;
}


int set_timeout_counter(int timeout_value) {
    timeout = timeout_value;
    baseTimeout = timeout_value;
    return timeout;
}


int get_timeout_counter() {
    return timeout;
}


int get_ping_interval() {
    return pingInterval;
}


void set_ping_interval(int ping_interval) {
    pingInterval = ping_interval;
}
