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
!**s/p gdwfllw - passe des composantes speed, psi
!                aux composantes u,v selon le type de grille.

      subroutine ez_gdwfllw(z1,z2,xlon,li,lj,grtyp,ig1,ig2,ig3,ig4)
      implicit none
      integer li,lj
      real z1(li,lj), z2(li,lj), xlon(li,lj)
      character*1 grtyp
      integer ig1,ig2,ig3,ig4

      external cigaxg
!
!auteur- y. chartier - avril 91
!
!langage  - ratfor
!
!objet(gdwfllw)
!         - passe de vent de grille (composantes u et v)
!         - a vitesse et direction.
!
!librairies
!
!appel    - call gdwfllw(spd,psi,li,lj,iyp,xg1,xg2,xg3,xg4)
!
!modules  - xgaig
!
!arguments
!  in/out - spd   - a l'entree contient la vitesse du vent et
!                   a la sortie la composante u.
!  in/out - psi   - a l'entree contient la direction du vent et
!                   a la sortie la composante v.
!   in    - li    - premiere dimension des champs spd et psi
!   in    - lj    - deuxieme dimension des champs spd et psi
!   in    - igtyp  - type de grille (voir ouvrir)
!   in    - xg1   - ** descripteur de grille (reel),
!   in    - xg2   -    igtyp = 'n', pi, pj, d60, dgrw
!   in    - xg3   -    igtyp = 'l', lat0, lon0, dlat, dlon,
!   in    - xg4   -    igtyp = 'a', 'b', 'g', xg1 = 0. global,
!                                                 = 1. nord
!                                                 = 2. sud **
!
!messages - "erreur mauvaise grille (gdwfllw)"
!
!-------------------------------------------------------------
!
!aout 2019 ! Modernisation du code - Y. Chartier  


!
!     * 1.866025=(1+sin60),   6.371e+6=earth radius in meters.
!
!     rdtodg = 180/pie, dgtord = pie/180

      real pie,rdtodg,dgtord

      data pie    /3.1415926535898/
      data rdtodg /57.295779513082/
      data dgtord /1.7453292519943e-2/

!

      integer i, j
      real psi,u,v
      real xg1, xg2, xg3, xg4
      real x1(2*li*lj),y1(2*li*lj),lat(2*li*lj)


!     les #define qui suivent rendent le code plus lisible

      if (grtyp .eq. '!') then
         call ez_lamb_gdwfllw(z1,z2,xlon,li,lj,grtyp,ig1,ig2,ig3,ig4,x1,y1,lat)
      endif

      if (grtyp .eq. 'N') then
         call cigaxg(grtyp,xg1,xg2,xg3,xg4,ig1,ig2,ig3,ig4)

         do j=1,lj
            do i=1,li
               psi =xlon(i,j)+xg4-z2(i,j)
               u = cos(psi*dgtord)*z1(i,j)
               v = sin(psi*dgtord)*z1(i,j)
               z1(i,j) = u
               z2(i,j) = v
           enddo
        enddo
         return
      endif

      if (grtyp .eq. 'S') then
         call cigaxg(grtyp,xg1,xg2,xg3,xg4,ig1,ig2,ig3,ig4)
            do j=1,lj
               do i=1,li
               psi =180.0 - xlon(i,j)+xg4-z2(i,j)
               u = cos(psi*dgtord)*z1(i,j)
               v = sin(psi*dgtord)*z1(i,j)
               z1(i,j) = u
               z2(i,j) = v
            enddo
         enddo
         return
      endif

      if (grtyp.eq.'A'.or.grtyp.eq.'B'.or.grtyp.eq.'G'.or.      grtyp.eq.'L') then
            do j=1,lj
               do  i=1,li
               psi = 270.0 - z2(i,j)
               u = cos(psi*dgtord)*z1(i,j)
               v = sin(psi*dgtord)*z1(i,j)
               z1(i,j) = u
               z2(i,j) = v
            enddo
         enddo
         return
      endif

 600  format('0',' erreur, mauvaise grille (gdwfllw) - grtyp = ', A1)
      return
      end
