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

/* =================================================================== */
 
/* manipulate timeout for read/write requests */
/* on the client side for each data exchange channel */

#include <stdio.h>
#include <stdlib.h>

#include <App.h>
#include <rmn/mgi.h>

typedef struct 
{
    int client_chan;
    int timeout;
} client_timeout;


//! Client timeout table
static client_timeout ctimeout[MAX_CHANNELS];
//! Number of channels
static int ichan = 0;
//! Default timeout
static int default_timeout = 180;


//! Initialize client timeout table with default timeout
void init_client_table(
    //! [in] Socket descriptor
    const int channel
) {
    if ( ichan >= MAX_CHANNELS ) {
        Lib_Log(APP_LIBRMN,APP_ERROR,"%s: Too many channels assigned; MAX = %d\n",__func__,MAX_CHANNELS);
        exit(1);
    } else {
        ctimeout[ichan].client_chan = channel;
        ctimeout[ichan].timeout = default_timeout;
        ichan++;
    }
}


//! Set client timeout
void set_client_timeout(
    //! [in] Channel descriptor
    const int fclient,
    //! [in] Tiemout (read/write wait time in seconds)
    const int timeout
) {
    int found = -1;

    for(int i = 0; i < ichan; i++) {
        if ( fclient == ctimeout[i].client_chan ) {
            ctimeout[i].timeout = timeout;
            found = i;
            break;
        }
    }

    if ( found < 0 ) {
        ctimeout[ichan].client_chan = fclient;
        ctimeout[ichan].timeout = timeout > default_timeout ? default_timeout : timeout;
    }
}


//! Get client timeout
//! \return Positive value if setted by the user to a negative
int get_client_timeout(
    //! [in] Channel descriptor
    const int fclient
) {
    for (int i = 0; i < MAX_CHANNELS; i++ ) {
        if ( fclient == ctimeout[i].client_chan ) {
            int new_timeout = ctimeout[i].timeout > 3 ? ctimeout[i].timeout : -ctimeout[i].timeout;
            return new_timeout;
        }
    }

    return default_timeout;
}


//! Get client timeout
//! \return Client timeout or default timeout value if not setted by the user
int get_timeout_value(
    //! [in] Channel descriptor
    const int fclient
) {
    for (int i = 0; i < MAX_CHANNELS; i++ ) {
        if ( fclient == ctimeout[i].client_chan ) {
            return ctimeout[i].timeout ;
        }
    }

    return default_timeout;
}
