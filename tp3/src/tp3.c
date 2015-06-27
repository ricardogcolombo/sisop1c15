#include "tp3.h"
#include "srv.h"

/* Variables globales */

int cant_ranks, mi_rank, mi_nro, mi_rol, mi_char;


/* Auxiliares para salida de debug (a stdout) */

void debug(const char *mje) {
#ifndef NDEBUG
    printf("\trk%-3d %c%-3d %c    %-50s\n",
        mi_rank, mi_rol ? 'c' : 's', mi_nro, mi_char, mje);
#endif
}

void debug_params(const t_params *p) {
#ifndef NDEBUG
    assert(mi_rol == ROL_CLIENTE);
    printf("\trk%-3d %c%-3d %c    \t%4d iters %8dms prev. %8dms crit.\n",
           mi_rank, 'c', mi_nro, p->caracter, p->cant_iteraciones,
           p->mseg_computo_previo, p->mseg_seccion_critica);
#endif
}


/* Cliente genérico provisto por la cátedra (no modificar) */

void computar(double mseg) { usleep((long)(1000.0 * mseg)); }

void cliente(int mi_serv_rank, t_params params)
{
    MPI_Status status; int i;
    int cant_ops = INSTR_POR_SECCION_CRITICA;
    double mseg_por_op = (double) params.mseg_seccion_critica / cant_ops;

    debug_params(&params);
    
    while(params.cant_iteraciones-- > 0) {

        debug("\tComputando de todo un poco ...");
        computar(params.mseg_computo_previo);

        debug("\t¡Necesito mutex!");
        MPI_Send(NULL, 0, MPI_INT, mi_serv_rank, TAG_PEDIDO, COMM_WORLD);

        //debug("\tEsperando respuesta de mi servidor");
        MPI_Recv(NULL, 0, MPI_INT, mi_serv_rank, TAG_OTORGADO, COMM_WORLD, &status);

        debug("\tEntrando en sección crítica");
        for(i = 0; i < cant_ops; ++i) {

            /* `Ejecutamos' una instrucción crítica. */
            fprintf(stderr, "%c", mi_char);
            
            /* Si fue la última mandamos un \n a stderr. */
            if(cant_ops == i + 1) {
                fprintf(stderr, "\n");
                if(mseg_por_op < MSEG_MIN_TRAS_NEWLINE) {
                    /* Cuando el delay artificial es cero (o casi cero) inyectamos
                     un épsilon de demora (sólo tras el \n) para reducir el riesgo
                     de que, al mergear stderrs de muchos procesos, mpiexec muestre
                     el primer char del siguiente proceso antes que nuestro \n. */
                    computar((double)MSEG_MIN_TRAS_NEWLINE - mseg_por_op);
                }
            }

            computar(mseg_por_op);
        }
        
        //debug("\tSaliendo de sección crítica");
        MPI_Send(NULL, 0, MPI_INT, mi_serv_rank, TAG_LIBERO, COMM_WORLD);
    }

    debug("\tAvisandole a mi servidor que voy a terminar");
    MPI_Ssend(NULL, 0, MPI_INT, mi_serv_rank, TAG_TERMINE, COMM_WORLD);
}


/* Parsea los primeros 4 strings del arreglo */

t_params parsear_primeros_args(char **argv)
{
    char *ep;
    t_params p;

    p.caracter = argv[0][0];
    p.cant_iteraciones = strtol(argv[1], &ep, 10); assert(!*ep);
    p.mseg_computo_previo = strtol(argv[2], &ep, 10); assert(!*ep);
    p.mseg_seccion_critica = strtol(argv[3], &ep, 10); assert(!*ep);

    return p;
}


/* Texto que mostramos ante -h y/o cantidad inválida de args. */

char ayuda[] = 
"uso:\n\n"
"  mpiexec -np <#ranks> ./tp3 [<char> <#iters> <ms_prev> <ms_crit>] [...]\n\n"
"ej.:\n\n"
"  mpiexec -np 2 ./tp3\n"
"  mpiexec -np 2 ./tp3  a 5 0 100\n"
"  mpiexec -np 4 ./tp3  a 5 0 100  b 3 1000 100\n\n"
"  mpiexec -np 4 ./tp3  @ 4 0 0\n"
"    (si la cant. de 4-uplas no alcanza se usan los defaults!)\n"
"\n"
"   agregar ...  >/dev/null     para ver sólo stderr (letritas)\n"
"           ... 2>/dev/null     para ver sólo stdout (debug)\n"
"\n";



/*
 * Función llamada al inicio por el rank 0 para parsear argc/argv
 * y repartir los parámetros a los clientes correspondientes.
 *
 * Notar que esto le confiere al rank 0 un status "especial" y
 * constituye un ejemplo de cuello de botella centralizado
 * (algo siempre indeseable en un sistema distribuido).
 *
 * Es necesario porque el standard sólo garantiza que el menor
 * rank tenga acceso a argc y argv. No es grave porque sólo
 * sucede una vez, durante la inicialización del sistema.
 */

void parsear_y_repartir_parametros(int argc, char *argv[], int cant_clis)
{
    int nro_cli;
    t_params pbase;
    
    assert(mi_rank == RANK_CERO);
    
    pbase.caracter = CHAR_INICIAL;
    pbase.cant_iteraciones = CANT_ITERACIONES;
    pbase.mseg_computo_previo = MSEG_COMPUTO_PREVIO;
    pbase.mseg_seccion_critica = MSEG_SECCION_CRITICA;

    --argc; ++argv;
    if(argc == 1)
        fprintf(stdout, "\n%s\n", ayuda);
    else if(argc % 4 != 0)
        fprintf(stderr, "\nprobaste con -h?\n\n");
    assert(argc % 4 == 0);

    for(nro_cli = 0; nro_cli < cant_clis; ++nro_cli) {
        t_params pcli;
        int n = nro_cli + 1;

        if(argc > 0) {
            assert(argc >= 4);
            pcli = parsear_primeros_args(argv);
            argc -= 4;
            argv += 4;
        } else {
            pcli.caracter = pbase.caracter + nro_cli;
            pcli.cant_iteraciones = pbase.cant_iteraciones * n;
            pcli.mseg_computo_previo = pbase.mseg_computo_previo * n;
            pcli.mseg_seccion_critica = pbase.mseg_seccion_critica * n;
        }

        debug("Enviando mensaje inicial con parámetros");
        MPI_Ssend(&pcli, 4, MPI_INT, 2*nro_cli+1, TAG_INICIAL, COMM_WORLD);
    }
}


/*
 * Función auxiliar llamada al inicio por cada cliente.
 * Bloquea hasta recibir sus parámetros. (Ver anterior.)
 */

t_params recibir_parametros(void)
{
    MPI_Status status;
    t_params params;
    
    assert(mi_rol == ROL_CLIENTE);
    
    MPI_Recv(&params, 4, MPI_INT, RANK_CERO, TAG_INICIAL, COMM_WORLD, &status);
    mi_char = params.caracter;
    debug("\t¡Buenas! Recibí mensaje inicial con parámetros");
    
    return params;
}



/* Punto de entrada */

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &cant_ranks);

    /* Veamos si nos tocó ser cliente o servidor. */
    assert(cant_ranks % 2 == 0);
    mi_nro = mi_rank / 2;
    mi_rol = mi_rank % 2;
    mi_char = ' ';

    if(mi_rol == ROL_SERVIDOR) {
    
        if(mi_rank == RANK_CERO) {
            /* MPI sólo garantiza argc/argv en el proceso con menor rank. */
            parsear_y_repartir_parametros(argc, argv, cant_ranks / 2);
        }

        /* Por lo demás los servidores están listos para trabajar. */
        servidor(mi_rank + 1);

    } else {
        assert(mi_rol == ROL_CLIENTE);

        /* Cada cliente espera su mensaje inicial antes de empezar. */
        cliente(mi_rank - 1, recibir_parametros());
    }

    debug("Finalizando");
    MPI_Finalize();
    return 0;
}


