/* RMNLIB - Library of useful routines for C and FORTRAN programming
 * Copyright (C) 1975-2001  Division de Recherche en Prevision Numerique
 *                          Environnement Canada
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation,
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <math.h>

#include <rmn/ezscint.h>
#include "ez_funcdef.h"


void c_llfgr(float *lat, float *lon, float *x, float *y, int32_t npts,
    float latOrigine, float lonOrigine, float deltaLat, float deltaLon)
{
    for (int32_t i = 0; i < npts; i++) {
        lon[i] = lonOrigine + deltaLon * (x[i] - 1.0);
        lon[i]=  fmod(fmod(lon[i], 360.0) + 360.0, 360.0);
        lat[i] = latOrigine + deltaLat * (y[i] - 1.0);
    }
}
