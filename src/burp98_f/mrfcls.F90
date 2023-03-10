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
!.S MRFCLS
!**S/P MRFCLS - FERMER UN FICHIER BURP
!
      FUNCTION MRFCLS( IUN )
      use app
      IMPLICIT NONE
      INTEGER  MRFCLS, IUN
!
!AUTEUR  J. CAVEEN   OCTOBRE 1990
!REV 001 Y. BOURASSA MARS    1995 RATFOR @ FTN77
!
!OBJET( MRFCLS )
!     FERMER UN FICHIER BURP
!                                                                       
!IMPLICITES
#include "defi.cdk"
#include "burpopt.cdk"
#include "codes.cdk"
!
!ARGUMENT
!     IUN    ENTREE     NUMERO D'UNITE ASSOCIE AU FICHIER
!
!MODULES 
      EXTERNAL XDFCLS, QDFMSIG
      INTEGER  XDFCLS, QDFMSIG, IOUT
      DATA     IOUT /6/
!
!*
      MRFCLS = 0 
      IF(BADTBL .NE. 0) MRFCLS = QDFMSIG(IUN, 'bRp0')
      IF(MRFCLS .LT. 0) RETURN

      MRFCLS = XDFCLS( IUN )
      IF(MRFCLS .LT. 0) RETURN

      write(app_msg,*) 'MRFCLS: Unite ',IUN,' fichier rapport est ferme'
      call Lib_Log(APP_LIBFST,APP_INFO,app_msg)       
   RETURN

      END
