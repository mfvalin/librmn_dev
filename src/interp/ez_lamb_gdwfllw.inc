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
      subroutine ez_lamb_gdwfllw(z1,z2,xlon,li,lj,grtyp,ig1,ig2,ig3,ig4,x1,y1,xlat)
      implicit none

      integer li,lj
      real z1(li*lj), z2(li*lj), xlon(li*lj)
      character*1 grtyp
      integer ig1,ig2,ig3,ig4   
      real x1(li*lj,2),y1(li*lj,2),xlat(li*lj,2)

      real delx,dely,uuu,vvv,alpha,psi

      integer i

!     
!     * 1.866025=(1+sin60),   6.371e+6=earth radius in meters.      
!     
!     rdtodg = 180/pie, dgtord = pie/180        
      
      real pie,rdtodg,dgtord
      
      data pie    /3.1415926535898/     
      data rdtodg /57.295779513082/
      data dgtord /1.7453292519943e-2/

      do i=1,li*lj
         xlat(i,1) = 45.0
         xlat(i,2) = 50.0
      enddo

      call ez_lambfll(x1(1,1),y1(1,1),xlat(1,1),xlon(1),li*lj,grtyp,ig1,ig2,ig3,ig4)
      call ez_lambfll(x1(1,2),y1(1,2),xlat(1,2),xlon(1),li*lj,grtyp,ig1,ig2,ig3,ig4)
      
      do i=1,li*lj
         delx = x1(i,2) - x1(i,1)
         dely = y1(i,2) - y1(i,1)
         psi = 270.0 -  z2(i)
         uuu = cos(psi*dgtord)* z1(i)
         vvv = sin(psi*dgtord)* z1(i)
         alpha = atan2(dely,delx) - 0.5*pie
         z1(i) = uuu*cos(alpha)-vvv*sin(alpha)
         z2(i) = uuu*sin(alpha)+vvv*cos(alpha)
      enddo
!     
      
      return
      end
