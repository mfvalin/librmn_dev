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


#include <stdio.h>
#include <stdlib.h>

#include "ez_funcdef.h"


int32_t f77name(gdllvval)(int32_t *gdid, float *uuout, float *vvout, float *uuin, float *vvin,
                      float *lat, float *lon, int32_t *n) {
    int32_t icode;

    icode = c_gdllvval(*gdid, uuout,vvout, uuin, vvin, lat, lon, *n);
    return icode;
}


int32_t c_gdllvval(int32_t gdid, float *uuout, float *vvout, float *uuin, float *vvin,
               float *lat, float *lon, int32_t n) {
    float *x, *y;
    int32_t ier,gdrow_id,gdcol_id;

    c_gdkey2rowcol(gdid,  &gdrow_id,  &gdcol_id);
    if (Grille[gdrow_id][gdcol_id].nsubgrids > 0 ) {
        fprintf(stderr, "<gdllvval>: This operation is not supported for 'U' grids\n");
        return -1;
    } else {

        x = (float *) malloc(n * sizeof(float));
        y = (float *) malloc(n * sizeof(float));

        ier = c_gdxyfll_orig(gdid, x, y, lat, lon, n);
        ier = c_gdxyvval(gdid, uuout, vvout, uuin, vvin, x, y, n);

        free(x);
        free(y);
        return 0;
    }
}
