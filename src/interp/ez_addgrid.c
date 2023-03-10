#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ez_funcdef.h"

// #ifndef MUTEX
// #error "MUTEX macro not found!"
// #endif


#ifdef MUTEX
static pthread_mutex_t EZ_MTX = PTHREAD_MUTEX_INITIALIZER;
#endif

//! \bug This function never worked as intended: The first parameter of c_gdkey2rowcol is an input and it's not initialized before the call!
int c_ez_refgrid(int grid_index) {
    int gdrow, gdcol, gdindex;

    c_gdkey2rowcol(gdindex, &gdrow, &gdcol);

#ifdef MUTEX
    pthread_mutex_lock(&EZ_MTX);
#endif
    Grille[gdrow][gdcol].access_count++;
#ifdef MUTEX
    pthread_mutex_unlock(&EZ_MTX);
#endif

   return Grille[gdrow][gdcol].access_count;
}


int c_ez_addgrid(
    int32_t grid_index,
    _Grille *newgr
) {
    int  gdrow, gdcol, gdindex, next_index, nxt_row, nxt_col, cur_gdid;
    _Grille *cur_gr;

#ifdef MUTEX
    pthread_mutex_lock(&EZ_MTX);
#endif

    newgr->access_count++;
    newgr->grid_index = grid_index;

    cur_gr = gr_list[grid_index];
    if (cur_gr == NULL) {
        gdindex = nGrilles;
        c_gdkey2rowcol(gdindex, &gdrow, &gdcol);
        Grille[gdrow][gdcol].grid_index = grid_index;

        gr_list[grid_index] = &Grille[gdrow][gdcol];
    } else {
        cur_gdid = cur_gr->index;
        c_gdkey2rowcol(cur_gdid, &gdrow, &gdcol);
        next_index = Grille[gdrow][gdcol].next_gd;
        nxt_row = gdrow;
        nxt_col = gdcol;
        while (next_index != -1) {
            c_gdkey2rowcol(next_index, &nxt_row, &nxt_col);
            next_index = Grille[nxt_row][nxt_col].next_gd;
        }
        Grille[nxt_row][nxt_col].next_gd = nGrilles;
    }

    c_gdkey2rowcol(nGrilles, &gdrow, &gdcol);
    memcpy(&(Grille[gdrow][gdcol]), newgr, sizeof(_Grille));
    Grille[gdrow][gdcol].index = nGrilles;
    Grille[gdrow][gdcol].next_gd = -1;

    nGrilles++;
    if (nGrilles >= chunks_sq[cur_log_chunk]) {
        fprintf(stderr, "<c_ez_addgrid> : Message from the EZSCINT package\n");
        fprintf(stderr, "<c_ez_addgrid> : Maximum number of definable grids attained : %d\n", nGrilles);
        fprintf(stderr, "               : Please contact RPN support to increase the maximum number\n");
        exit(13);
    }

    if (0 == (nGrilles % chunks[cur_log_chunk])) {
        c_gdkey2rowcol(nGrilles, &gdrow, &gdcol);
        Grille[gdrow] = (_Grille *) calloc(chunks[cur_log_chunk], sizeof(_Grille));
        for (int i = 0; i < chunks[cur_log_chunk]; i++) {
            Grille[gdrow][i].index = -1;
        }
    }

#ifdef MUTEX
    pthread_mutex_unlock(&EZ_MTX);
#endif
    return nGrilles - 1;
}
