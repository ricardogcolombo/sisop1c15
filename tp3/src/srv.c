#include "srv.h"

/*
 *  Ejemplo de servidor que tiene el "sí fácil" para con su
 *  cliente y no se lleva bien con los demás servidores.
 *
 */
void negociar_acceso(int numero_magico, int *size, int *servidores_que_esperan_respuesta);
void avisoQueLibero(int *servidores_que_esperan_respuesta, int size);

void servidor(int mi_cliente)
{
    int numero_magico = 0;
    MPI_Status status; 
    int origen, tag;
    int hay_pedido_local = FALSE;
    int listo_para_salir = FALSE;
    int mi_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mi_rank);

    int size = 0;
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    int servidores_que_esperan_respuesta[size/2];
    int i;
    for(i = 0; i < size/2; i++)
    {
        servidores_que_esperan_respuesta[i] = FALSE;
    }
    while( ! listo_para_salir ) {
        int numero;
        MPI_Recv(&numero, 1, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);
        origen = status.MPI_SOURCE;
        tag = status.MPI_TAG;
        
        if (tag == TAG_PEDIDO) {
            assert(origen == mi_cliente);
            debug("Mi cliente solicita acceso exclusivo");
            assert(hay_pedido_local == FALSE);
            hay_pedido_local = TRUE;
            numero_magico++;
            negociar_acceso(numero_magico, &size, servidores_que_esperan_respuesta);

            debug("Dándole permiso (frutesco por ahora)");
            MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
        }
        
        else if (tag == TAG_LIBERO) {
            assert(origen == mi_cliente);
            debug("Mi cliente libera su acceso exclusivo");
            assert(hay_pedido_local == TRUE);
            hay_pedido_local = FALSE;
            avisoQueLibero(servidores_que_esperan_respuesta, size);
        }
        
        else if (tag == TAG_TERMINE) {
            assert(origen == mi_cliente);
            debug("Mi cliente avisa que terminó");
            listo_para_salir = TRUE;
            for(i = 0; i < size/2;i++)
                if(i != mi_rank)
                    MPI_Send(NULL, 0, MPI_INT, i*2, TAG_ADIOS, COMM_WORLD);
        }
        else if(tag == TAG_PERMISO_SERVER){
            assert(origen % 2 == 0 );
            debug("Un servidor pide acceso exclusivo");
            MPI_Send(NULL, 0, MPI_INT, origen, TAG_DALE, COMM_WORLD);
            debug("Le doy acceso exclusivo");
        }
        else if(tag == TAG_ADIOS){
            size = size - 2;
        }
    }
}


void negociar_acceso(int numero_magico, int *size, int *servidores_que_esperan_respuesta)
{
    int i;
    int mi_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mi_rank);
    MPI_Status status; 
    assert(*size > 0); 
    debug("Le pido acceso a los demas servidores");
    
    for(i = 0; i < *size/2; i++)
    {
        if(i*2 != mi_rank)
            MPI_Send(&numero_magico, 1, MPI_INT, i*2, TAG_PERMISO_SERVER, COMM_WORLD);
    }
    debug("Ok... ahora espero respuestas");
    int numero_de_dales = 0;
    while(numero_de_dales < (*size/2) - 1)
    {
        char str[15];
        sprintf(str, "%d", *size);
        debug(str);

        int otro_numero_magico;
        MPI_Recv(&otro_numero_magico, 1, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);
        debug("RECIBO DE LA GILADA");

        
        int tag = status.MPI_TAG;
        int origen = status.MPI_SOURCE;
        if(tag == TAG_DALE )
        {
            numero_de_dales++;
        }
        else if(tag == TAG_ADIOS){
            *size = *size - 2;
        }
        else if(tag == TAG_PERMISO_SERVER){
            assert(origen % 2 == 0 );
            debug("Un servidor pide acceso exclusivo");
            if(numero_magico > otro_numero_magico)
            {
                MPI_Send(NULL, 0, MPI_INT, origen, TAG_DALE, COMM_WORLD);
            }
            else if(numero_magico < otro_numero_magico)
            {
                servidores_que_esperan_respuesta[origen] = TRUE;
            }else if(numero_magico == otro_numero_magico)
            {
                if(origen < mi_rank)
                    MPI_Send(NULL, 0, MPI_INT, origen, TAG_DALE, COMM_WORLD);
                else
                    servidores_que_esperan_respuesta[origen/2] = TRUE;
            }
            debug("Le doy acceso exclusivo");
        }
    }
}


void avisoQueLibero(int *servidores_que_esperan_respuesta, int size){
    int i;

    for(i = 0; i < size/2; i++)
    {
        if(servidores_que_esperan_respuesta[i] == TRUE){
            MPI_Send(NULL, 0, MPI_INT, i*2, TAG_DALE, COMM_WORLD);
            servidores_que_esperan_respuesta[i] = FALSE;
        }
    }
}



