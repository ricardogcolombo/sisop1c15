#ifndef __tp3_h__
#define __tp3_h__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include "mpi.h"

#define FALSE 0
#define TRUE 1
#define RANK_CERO 0
#define ROL_SERVIDOR 0
#define ROL_CLIENTE 1


/* Tags para identificar distintas clases de de mensaje */

#define TAG_INICIAL           10    /* rk0 -> cli */
#define TAG_PEDIDO            20    /* cli -> srv */
#define TAG_OTORGADO          30    /* srv -> cli */
#define TAG_LIBERO            40    /* cli -> srv */
#define TAG_TERMINE           50    /* cli -> srv */

#define TAG_PERMISO_SERVER    60    /* srv -> srv */
#define TAG_DALE           70    /* srv -> srv */
#define TAG_ADIOS 		   80	/* srv -> srv */


/* Abreviaciones de constantes MPI que usaremos seguido */

#define COMM_WORLD      MPI_COMM_WORLD
#define ANY_SOURCE      MPI_ANY_SOURCE
#define ANY_TAG         MPI_ANY_TAG

/* Valores por defecto que se multiplican por (nro_cli + 1) */

#define CANT_ITERACIONES          1
#define MSEG_COMPUTO_PREVIO       200
#define MSEG_SECCION_CRITICA      100

/* Valores por defecto constantes */

#define CHAR_INICIAL                'a'
#define MSEG_MIN_TRAS_NEWLINE       10
#define INSTR_POR_SECCION_CRITICA   40

/* Parámetros que definen el comportamiento de un cliente */

typedef struct {
    int caracter;
    int cant_iteraciones;
    int mseg_computo_previo;
    int mseg_seccion_critica;
} t_params;


/* Dos o tres variables que se usan por doquier */

extern int cant_ranks, mi_rank, mi_nro, mi_rol, mi_char;

extern void debug(const char* mensaje);

#endif
