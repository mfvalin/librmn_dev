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
#include "ez_funcdef.h"


int32_t f77name(gdxpncf)(int32_t *gdin, int32_t *i1, int32_t *i2, int32_t *j1, int32_t *j2) {
    int32_t icode;

    icode = c_gdxpncf(*gdin, i1, i2, j1, j2);
    return icode;
}


int32_t c_gdxpncf(int32_t gdin, int32_t *i1, int32_t *i2, int32_t *j1, int32_t *j2) {
    int32_t gdrow_in, gdcol_in;

    if (gdin < 0 || gdin >= nGrilles) return -1;
    c_gdkey2rowcol(gdin,  &gdrow_in,  &gdcol_in);
    if (Grille[gdrow_in][gdcol_in].nsubgrids > 0) {
        fprintf(stderr, "<gdxpncf> This operation is not supported for 'U' grids.\n");
        return -1;
    }

    *i1 = Grille[gdrow_in][gdcol_in].i1;
    *i2 = Grille[gdrow_in][gdcol_in].i2;
    *j1 = Grille[gdrow_in][gdcol_in].j1;
    *j2 = Grille[gdrow_in][gdcol_in].j2;
    return 0;
}

