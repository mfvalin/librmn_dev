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

int32_t c_ezyywdint(float *uuout, float *vvout, float *uuin,  float *vvin, int32_t gdout,int32_t gdin) {
  int idx_gdin;
  int32_t icode,i,j,k,ierc1,ierc2,ierc;
  int32_t yancount_yin,yincount_yin, yancount_yan,yincount_yan;
  int32_t yin_gdin,yan_gdin,yin_gdout,yan_gdout,yyin,yyout;
  int32_t yin_gdrow_in, yin_gdcol_in, yin_gdrow_out, yin_gdcol_out;
  int32_t yan_gdrow_in, yan_gdcol_in, yan_gdrow_out, yan_gdcol_out;
  int32_t     gdrow_in,     gdcol_in,     gdrow_out,     gdcol_out;
  int32_t ni, nj;
  float *yin2yin_uuout,*yan2yin_uuout, *yin2yin_vvout,*yan2yin_vvout;
  float *yin2yan_uuout,*yan2yan_uuout, *yin2yan_vvout,*yan2yan_vvout;
  float *yin2yin_spdout,*yan2yin_spdout, *yin2yin_wdout,*yan2yin_wdout;
  float *yin2yan_spdout,*yan2yan_spdout, *yin2yan_wdout,*yan2yan_wdout;
  float *spdout,*wdout;

  _Grille *lgdin, *lgdout;
 /*  need only access to either yin or Yang info for the lat and lon val */

  yyin=0; yyout=0;
  ierc=0;
  ierc1=0;ierc2=0;

  c_gdkey2rowcol(gdin,  &gdrow_in,  &gdcol_in);
  c_gdkey2rowcol(gdout, &gdrow_out, &gdcol_out);
  idx_gdin = c_find_gdin(gdin, gdout);

/* setup for input grid */
  if (Grille[gdrow_in][gdcol_in].nsubgrids > 0)
     {
     yyin=1;
     yin_gdin = Grille[gdrow_in][gdcol_in].subgrid[0];
     yan_gdin = Grille[gdrow_in][gdcol_in].subgrid[1];
     c_gdkey2rowcol(yin_gdin,  &yin_gdrow_in,  &yin_gdcol_in);
     c_gdkey2rowcol(yan_gdin,  &yan_gdrow_in,  &yan_gdcol_in);
     }
  else
     {
     yin_gdin = gdin;
     yin_gdrow_in = gdrow_in;
     yin_gdcol_in = gdcol_in;
     }

/* setup for input grid */
  if (Grille[gdrow_out][gdcol_out].nsubgrids > 0)
     {
     yyout=1;
     yin_gdout = Grille[gdrow_out][gdcol_out].subgrid[0];
     yan_gdout = Grille[gdrow_out][gdcol_out].subgrid[1];
     c_gdkey2rowcol(yin_gdout,  &yin_gdrow_out,  &yin_gdcol_out);
     c_gdkey2rowcol(yan_gdout,  &yan_gdrow_out,  &yan_gdcol_out);
     }
  else
     {
     yin_gdout = gdout;
     yin_gdrow_out = gdrow_out;
     yin_gdcol_out = gdcol_out;
     }
  lgdin = &(Grille[yin_gdrow_in ][yin_gdcol_in ]);
  lgdout= &(Grille[gdrow_out][gdcol_out]);

  ni = Grille[yin_gdrow_out][yin_gdcol_out].ni;
  nj = Grille[yin_gdrow_out][yin_gdcol_out].nj;

/* interp input one grid to yygrid - no masking needed*/
  if (yyin == 0 && yyout == 1)
    {
    icode = c_ezdefset(yin_gdout,gdin);
    ierc1 = c_ezwdint_orig(uuout,vvout,uuin,vvin);
    icode = c_ezdefset(yan_gdout,gdin);
    ierc2 = c_ezwdint_orig(&uuout[(ni*nj)],
                           &vvout[(ni*nj)],uuin,vvin);
    if (ierc1 == 2 || ierc2 == 2)
       {
        ierc=2;
       }
    return ierc;
    }

  /* check if one sub grid is identical to one of the sub grids */
  if (yin_gdin == gdout)
     {
     icode = c_ezdefset(gdout,yin_gdin);
     icode = c_ezwdint_orig(uuout,vvout,uuin,vvin);
     return icode;
     }
  if (yan_gdin == gdout)
     {
     icode = c_ezdefset(gdout,yan_gdin);
     icode = c_ezwdint_orig(uuout,vvout, &uuin[(lgdin->ni)*(lgdin->nj)],
                                         &vvin[(lgdin->ni)*(lgdin->nj)]);
     return icode;
     }

  /* User specifies to use 1 subgrid for interpolation ezsetopt(USE_1SUBGRID) */
  /* User must specify one specific grid ezsetival(SUBGRIDID) */
  /* This is only appropriate if the destination grid is non yin-yang grid */
  if (groptions.use_1subgrid == 1) /* User specifies to use 1 subgrid only */
     {
     if (yyout == 1) /* output is a Yin-Yang grid */
        {
         fprintf(stderr,"<c_ezyywdint> cannot use 1 subgrid to interpolate to a Yin-Yang grid  Aborting...\n");
         return -1;
        }
     if (groptions.valeur_1subgrid != yin_gdin &&
         groptions.valeur_1subgrid != yan_gdin)  /* chosen subgrid is neither Yin or Yang in source grid */
        {
         fprintf(stderr,"<c_ezyywdint> define src subgridid in ezsetival(subgridid)! Aborting...\n");
         return -1;
        }
     if (groptions.valeur_1subgrid == yin_gdin) /*Use input Yin grid */
        {
        icode = c_ezdefset(yin_gdout,groptions.valeur_1subgrid);
        ierc = c_ezwdint_orig(uuout,vvout,uuin,vvin);
        return ierc;
        }
     if (groptions.valeur_1subgrid == yan_gdin) /* Use input Yang grid */
        {
        icode = c_ezdefset(yin_gdout,groptions.valeur_1subgrid);
        ierc = c_ezwdint_orig(uuout,vvout,&uuin[(lgdin->ni)*(lgdin->nj)],&vvin[(lgdin->ni)*(lgdin->nj)]);
        return ierc;
        }
     }
  /*End of ONE grid option*/

  /* To use both Yin and Yang grids in Yin-yang input grid */
  /* Masquer les grilles YY input pour enlever overlap et calculer les X,Y */
  icode = c_ezyy_calcxy(gdout,gdin);

/* interp yinyang to one grid */
  if (yyin == 1 && yyout == 0)
    {
    yincount_yin = lgdout->gset[idx_gdin].yincount_yin;
    yancount_yin = lgdout->gset[idx_gdin].yancount_yin;
    yin2yin_uuout = (float *) malloc(yincount_yin*sizeof(float));
    yin2yin_vvout = (float *) malloc(yincount_yin*sizeof(float));
    yin2yin_spdout = (float *) malloc(yincount_yin*sizeof(float));
    yin2yin_wdout = (float *) malloc(yincount_yin*sizeof(float));
    yan2yin_uuout = (float *) malloc(yancount_yin*sizeof(float));
    yan2yin_vvout = (float *) malloc(yancount_yin*sizeof(float));
    yan2yin_spdout = (float *) malloc(yancount_yin*sizeof(float));
    yan2yin_wdout = (float *) malloc(yancount_yin*sizeof(float));

    icode = c_gdxyvval_orig(yin_gdin,yin2yin_uuout,yin2yin_vvout,uuin,vvin,lgdout->gset[idx_gdin].yin2yin_x,lgdout->gset[idx_gdin].yin2yin_y,lgdout->gset[idx_gdin].yincount_yin);
    icode = c_gdwdfuv_orig(yin_gdin,yin2yin_spdout,yin2yin_wdout,yin2yin_uuout,yin2yin_vvout,lgdout->gset[idx_gdin].yin2yin_lat,lgdout->gset[idx_gdin].yin2yin_lon,yincount_yin);

    icode = c_gdxyvval_orig(yan_gdin,yan2yin_uuout,yan2yin_vvout,&uuin[(lgdin->ni)*(lgdin->nj)],&vvin[(lgdin->ni)*(lgdin->nj)],lgdout->gset[idx_gdin].yan2yin_x,lgdout->gset[idx_gdin].yan2yin_y,yancount_yin);
    icode = c_gdwdfuv_orig(yan_gdin,yan2yin_spdout,yan2yin_wdout,yan2yin_uuout, yan2yin_vvout,lgdout->gset[idx_gdin].yan2yin_lat,lgdout->gset[idx_gdin].yan2yin_lon,yancount_yin);
    yincount_yin=0;
    yancount_yin=0;
    for(j=0; j<nj; j++)
      {
      for (i=0;i<ni; i++)
        {
        k=(j*ni)+i;
        if (lgdout->gset[idx_gdin].yin_maskout[k] == 1.0)
          {
          uuout[k]=yan2yin_spdout[yancount_yin];
          vvout[k]=yan2yin_wdout[yancount_yin];
          yancount_yin++;
          }
        else
          {
          uuout[k]=yin2yin_spdout[yincount_yin];
          vvout[k]=yin2yin_wdout[yincount_yin];
          yincount_yin++;
          }
        }
    }
    free(yin2yin_uuout);
    free(yin2yin_vvout);
    free(yin2yin_spdout);
    free(yin2yin_wdout);
    free(yan2yin_uuout);
    free(yan2yin_vvout);
    free(yan2yin_spdout);
    free(yan2yin_wdout);
    }

/* interp yinyang to yinyang*/
  if (yyout == 1 && yyin == 1)
    {
/* interp input YY grid to YIN */
    yincount_yin = lgdout->gset[idx_gdin].yincount_yin;
    yancount_yin = lgdout->gset[idx_gdin].yancount_yin;
    yincount_yan = lgdout->gset[idx_gdin].yincount_yan;
    yancount_yan = lgdout->gset[idx_gdin].yancount_yan;
    yin2yin_uuout = (float *) malloc(yincount_yin*sizeof(float));
    yin2yin_vvout = (float *) malloc(yincount_yin*sizeof(float));
    yin2yin_spdout = (float *) malloc(yincount_yin*sizeof(float));
    yin2yin_wdout = (float *) malloc(yincount_yin*sizeof(float));
    yan2yin_uuout = (float *) malloc(yancount_yin*sizeof(float));
    yan2yin_vvout = (float *) malloc(yancount_yin*sizeof(float));
    yan2yin_spdout = (float *) malloc(yancount_yin*sizeof(float));
    yan2yin_wdout = (float *) malloc(yancount_yin*sizeof(float));

    yin2yan_uuout = (float *) malloc(yincount_yan*sizeof(float));
    yin2yan_vvout = (float *) malloc(yincount_yan*sizeof(float));
    yin2yan_spdout = (float *) malloc(yincount_yan*sizeof(float));
    yin2yan_wdout = (float *) malloc(yincount_yan*sizeof(float));
    yan2yan_uuout = (float *) malloc(yancount_yan*sizeof(float));
    yan2yan_vvout = (float *) malloc(yancount_yan*sizeof(float));
    yan2yan_spdout = (float *) malloc(yancount_yan*sizeof(float));
    yan2yan_wdout = (float *) malloc(yancount_yan*sizeof(float));

    icode = c_gdxyvval_orig(yin_gdin,yin2yin_uuout,yin2yin_vvout,uuin,vvin,lgdout->gset[idx_gdin].yin2yin_x,lgdout->gset[idx_gdin].yin2yin_y,lgdout->gset[idx_gdin].yincount_yin);
    icode = c_gdwdfuv_orig(yin_gdin,yin2yin_spdout,yin2yin_wdout,yin2yin_uuout,yin2yin_vvout,lgdout->gset[idx_gdin].yin2yin_lat,lgdout->gset[idx_gdin].yin2yin_lon,lgdout->gset[idx_gdin].yincount_yin);
    icode = c_gdxyvval_orig(yan_gdin,yan2yin_uuout,yan2yin_vvout,&uuin[(lgdin->ni)*(lgdin->nj)],&vvin[(lgdin->ni)*(lgdin->nj)],lgdout->gset[idx_gdin].yan2yin_x,lgdout->gset[idx_gdin].yan2yin_y,lgdout->gset[idx_gdin].yancount_yin);
    icode = c_gdwdfuv_orig(yan_gdin,yan2yin_spdout,yan2yin_wdout,yan2yin_uuout,yan2yin_vvout,lgdout->gset[idx_gdin].yan2yin_lat,lgdout->gset[idx_gdin].yan2yin_lon,yancount_yin);

    icode = c_gdxyvval_orig(yin_gdin,yin2yan_uuout,yin2yan_vvout,uuin,vvin,lgdout->gset[idx_gdin].yin2yan_x,lgdout->gset[idx_gdin].yin2yan_y,lgdout->gset[idx_gdin].yincount_yan);
    icode = c_gdwdfuv_orig(yin_gdin,yin2yan_spdout,yin2yan_wdout,yin2yan_uuout,yin2yan_vvout,lgdout->gset[idx_gdin].yin2yan_lat,lgdout->gset[idx_gdin].yin2yan_lon,lgdout->gset[idx_gdin].yincount_yan);

    icode = c_gdxyvval_orig(yan_gdin,yan2yan_uuout,yan2yan_vvout,&uuin[(lgdin->ni)*(lgdin->nj)],&vvin[(lgdin->ni)*(lgdin->nj)],lgdout->gset[idx_gdin].yan2yan_x,lgdout->gset[idx_gdin].yan2yan_y,lgdout->gset[idx_gdin].yancount_yan);
    icode = c_gdwdfuv_orig(yan_gdin,yan2yan_spdout,yan2yan_wdout,yan2yan_uuout,yan2yan_vvout,lgdout->gset[idx_gdin].yan2yan_lat,lgdout->gset[idx_gdin].yan2yan_lon,lgdout->gset[idx_gdin].yancount_yan);

 /*Build output for YIN output grid */
    yincount_yin=0; yancount_yin=0;
    for(j=0; j<nj; j++)
      {
      for (i=0;i<ni; i++)
        {
        k=(j*ni)+i;
        if (lgdout->gset[idx_gdin].yin_maskout[k] == 1.0)
          {
          uuout[k]=yan2yin_spdout[yancount_yin];
          vvout[k]=yan2yin_wdout[yancount_yin];
          yancount_yin++;
          }
        else
          {
          uuout[k]=yin2yin_spdout[yincount_yin];
          vvout[k]=yin2yin_wdout[yincount_yin];
          yincount_yin++;
          }
        }
      }

 /*Build output for YANG output grid */
    yincount_yan=0; yancount_yan=0;
    for(j=0; j<nj; j++)
      {
      for (i=0;i<ni; i++)
        {
        k=(j*ni)+i;
        if (lgdout->gset[idx_gdin].yan_maskout[k] == 1.0)
          {
          uuout[k+(ni*nj)]=yan2yan_spdout[yancount_yan];
          vvout[k+(ni*nj)]=yan2yan_wdout[yancount_yan];
          yancount_yan++;
          }
        else
          {
          uuout[k+(ni*nj)]=yin2yan_spdout[yincount_yan];
          vvout[k+(ni*nj)]=yin2yan_wdout[yincount_yan];
          yincount_yan++;
          }
        }
      }
   free(yin2yin_uuout);
   free(yin2yin_vvout);
   free(yin2yin_spdout);
   free(yin2yin_wdout);
   free(yan2yin_uuout);
   free(yan2yin_vvout);
   free(yan2yin_spdout);
   free(yan2yin_wdout);
   free(yin2yan_uuout);
   free(yin2yan_vvout);
   free(yin2yan_spdout);
   free(yin2yan_wdout);
   free(yan2yan_uuout);
   free(yan2yan_vvout);
   free(yan2yan_spdout);
   free(yan2yan_wdout);
   }

   return icode;
}

