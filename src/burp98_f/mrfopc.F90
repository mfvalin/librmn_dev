!/* RMNLIB - Library of useful routines for C and FORTRAN programming
! * Copyright (C) 1975-2001  Division de Recherche en Prevision Numerique
! *                          Environnement Canada
! *
! * This library is free software; you can redistribute it and/or
! * modify it under the terms of the GNU Lesser General Public
! * License as published by the Free Software Foundation,
! * version 2.1 of the License.
! *
! * This library is distributed in the hope that it will be useful,
! * but WITHOUT ANY WARRANTY; without even the implied warranty of
! * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
! * Lesser General Public License for more details.
! *
! * You should have received a copy of the GNU Lesser General Public
! * License along with this library; if not, write to the
! * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
! * Boston, MA 02111-1307, USA.
! */

!.S MRFOPC
!**S/P MRFOPC - INITIALISER UNE OPTION DE TYPE CARACTERE
!
      FUNCTION MRFOPC(OPTNOM, OPVALC)
      use app
      IMPLICIT NONE
      INTEGER  MRFOPC
      CHARACTER*(*)   OPTNOM, OPVALC
!
!AUTEUR  J. CAVEEN   AVRIL  1991
!REV 001 Y. BOURASSA MARS   1995 RATFOR @ FTN77
!
!OBJET( MRFOPC )
!     FONCTION SERVANT A INITIALISER UNE OPTION DE TYPE CARACTERE
!     LA VALEUR DE L'OPTION EST CONSERVEE DANS LE COMMON XDFTLR
!
!ARGUMENTS
!     OPTNOM   ENTREE  NOM DE L'OPTION A INITIALISER
!     OPVALR     "     VALEUR A DONNER A L'OPTION
!
!IMPLICITES
#include "defi.cdk"
#include "burpopt.cdk"
#include "codes.cdk"
!
!MODULES 
      INTEGER  lvl

!     INITIALISER LA VALEUR DE L'OPTION AU NIVEAU BURP
      IF(INDEX(OPTNOM, 'MSGLVL') .NE. 0) THEN
          lvl=lib_loglevel(APP_LIBFST,OPVALC)
      ENDIF

      MRFOPC=0
      RETURN
      END

