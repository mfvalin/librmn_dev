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

#include <ctype.h>
#include <string.h>

#include <rmn/ezscint.h>
#include "ez_funcdef.h"


int32_t f77name(ezgetval)(char *option, float *fvalue, F2Cl llenoption)
{
   int32_t icode;
   char local_opt[32];
   int32_t lenoption=llenoption;

   ftnstrclean(option,lenoption);
   strncpy(local_opt, option, lenoption);
   local_opt[lenoption] = '\0';

   icode = c_ezgetval(local_opt, fvalue);

   return icode;
}

int32_t f77name(ezgetival)(char *option, int32_t *ivalue, F2Cl llenoption)
{
   char local_opt[32];
   int32_t lenoption=llenoption;

   ftnstrclean(option,lenoption);
   strncpy(local_opt, option, lenoption);
   local_opt[lenoption] = '\0';

   return c_ezgetival(local_opt, ivalue);
}

int32_t c_ezgetval(char *option, float *fvalue)
{
   char local_opt[32];
   int32_t i;

   strcpy(local_opt, option);

   for (i=0; i < strlen(local_opt); i++)
      {
      local_opt[i] = (char) tolower((int)local_opt[i]);
      }

   if (0 == strcmp(local_opt, "extrap_value" ))
      {
      *fvalue = groptions.valeur_extrap;
      }

   if (0 == strcmp(local_opt, "missing_distance_threshold" ))
      {
      *fvalue = groptions.msg_dist_thresh;
      }

   if (0 == strcmp(local_opt, "weight_number" ))
      {
      *fvalue = (float) groptions.wgt_num;
      }

    if (0 == strcmp(local_opt, "missing_points_tolerance" ))
      {
      *fvalue = (float) groptions.msg_pt_tol;
      }
   return 0;
}

int32_t c_ezgetival(char *option, int32_t *ivalue)
{
   char local_opt[32];
   int32_t i;

   strcpy(local_opt, option);

   for (i=0; i < strlen(local_opt); i++)
      {
      local_opt[i] = (char) tolower((int)local_opt[i]);
      }

   if (0 == strcmp(local_opt, "subgridid" ))
      {
      *ivalue = groptions.valeur_1subgrid;
      }

   if (0 == strcmp(local_opt, "weight_number" ))
      {
      *ivalue = groptions.wgt_num;
      }

   if (0 == strcmp(local_opt, "missing_points_tolerance" ))
      {
      *ivalue = groptions.msg_pt_tol;
      }
   return 0;

}
