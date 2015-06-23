#include "srv.h"

/*
 *  Ejemplo de servidor que tiene el "sí fácil" para con su
 *  cliente y no se lleva bien con los demás servidores.
 *
 */

typedef struct {
	int servidor_espera_respuesta;
	int servidor_esta_vivo;
	int rank;
} Servidor;

void servidor_espera(Servidor* servidores, int origen, int size);
void aviso_que_me_muero(Servidor *servidores, int size, int mi_rank);
void negociar_acceso(int numero_magico_actual, int *numero_magico_maximo, int size, Servidor* servidores);
Servidor *inicializar_servidores(int size);
void avisoQueLibero(Servidor *servidores, int size);
void servidor_muerto(Servidor* servidores, int origen, int size);
int obtengo_acceso(int *dales, Servidor* servidores, int size, int mi_rank);
void nuevo_dale(int *dales, int origen);
int *inicializar_dales(int size);


void servidor(int mi_cliente) {
	int numero_magico_actual = 0;
	int numero_magico_maximo = 0;
	MPI_Status status;
	int origen, tag;
	int hay_pedido_local = FALSE;
	int listo_para_salir = FALSE;
	int mi_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &mi_rank);
	int size = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	size = size / 2;
	Servidor *servidores = inicializar_servidores(size);

	while ( ! listo_para_salir ) {
		int numero;
		MPI_Recv(&numero, 1, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);
		origen = status.MPI_SOURCE;
		tag = status.MPI_TAG;

		if (tag == TAG_PEDIDO) {
			assert(origen == mi_cliente);
			debug("Mi cliente solicita acceso exclusivo");
			assert(hay_pedido_local == FALSE);
			hay_pedido_local = TRUE;
			numero_magico_actual = numero_magico_maximo + 1;
			numero_magico_maximo++;
			negociar_acceso(numero_magico_actual, &numero_magico_maximo, size, servidores);

			debug("Dándole permiso (frutesco por ahora)");
			MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
		}

		else if (tag == TAG_LIBERO) {
			assert(origen == mi_cliente);
			debug("Mi cliente libera su acceso exclusivo");
			assert(hay_pedido_local == TRUE);
			hay_pedido_local = FALSE;
			avisoQueLibero(servidores, size);
		}

		else if (tag == TAG_TERMINE) {
			assert(origen == mi_cliente);
			debug("Mi cliente avisa que terminó");
			listo_para_salir = TRUE;
			aviso_que_me_muero(servidores, size, mi_rank);
		} else if (tag == TAG_PERMISO_SERVER && hay_pedido_local == FALSE) {
			assert(origen % 2 == 0 );
			debug("Un servidor pide acceso exclusivo");

			//Actualizo el número mágico máximo si corresponde
			if (numero_magico_maximo < numero)
			{
				numero_magico_maximo = numero;
			}

			MPI_Send(NULL, 0, MPI_INT, origen, TAG_DALE, COMM_WORLD);
			debug("Le doy acceso exclusivo");
		} else if (tag == TAG_PERMISO_SERVER && hay_pedido_local == TRUE) {

			//Actualizo el número mágico máximo si corresponde
			if (numero_magico_maximo < numero)
			{
				numero_magico_maximo = numero;
			}

			servidor_espera(servidores, origen, size);
		} else if (tag == TAG_ADIOS) {
			servidor_muerto(servidores, origen, size);
		}
	}
	free(servidores);
}

void aviso_que_me_muero(Servidor *servidores, int size, int mi_rank) {
	int i;
	for (i = 0; i < size; i++)
		if (servidores[i].rank != mi_rank && servidores[i].servidor_esta_vivo == TRUE ) {
			MPI_Send(NULL, 0, MPI_INT, servidores[i].rank , TAG_ADIOS, COMM_WORLD);
		}
}

void negociar_acceso(int numero_magico_actual, int *numero_magico_maximo, int size, Servidor* servidores) {
	int i;
	int mi_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &mi_rank);
	MPI_Status status;
	debug("Le pido acceso a los demas servidores");
	for (i = 0; i < size; i++) {
		if (servidores[i].rank != mi_rank && servidores[i].servidor_esta_vivo) {
			MPI_Send(&numero_magico_actual, 1, MPI_INT, servidores[i].rank, TAG_PERMISO_SERVER, COMM_WORLD);
		}
	}
	debug("Ok... ahora espero respuestas");
	int *dales = inicializar_dales(size);
	while (!obtengo_acceso(dales, servidores, size, mi_rank)) {

		int otro_numero_magico;
		MPI_Recv(&otro_numero_magico, 1, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);
		debug("RECIBO DE LA GILADA");


		int tag = status.MPI_TAG;
		int origen = status.MPI_SOURCE;
		if (tag == TAG_DALE ) {
			nuevo_dale(dales, origen);
			debug("Me dan permiso!");
		} else if (tag == TAG_ADIOS) {
			servidor_muerto(servidores, origen, size);
		} else if (tag == TAG_PERMISO_SERVER) {
			assert(origen % 2 == 0 );
			debug("Un servidor pide acceso exclusivo");

			//Actualizo el número mágico máximo si corresponde
			if (*numero_magico_maximo < otro_numero_magico)
			{
				*numero_magico_maximo = otro_numero_magico;
			}

			if (numero_magico_actual > otro_numero_magico) {
				MPI_Send(NULL, 0, MPI_INT, origen, TAG_DALE, COMM_WORLD);
				debug("Le doy acceso exclusivo");
			} else if (numero_magico_actual < otro_numero_magico) {
				servidor_espera(servidores, origen, size);
				debug("Espera");
			} else if (numero_magico_actual == otro_numero_magico) {
				debug("Numeros magicos iguales!");
				if (origen < mi_rank) {
					MPI_Send(NULL, 0, MPI_INT, origen, TAG_DALE, COMM_WORLD);
					debug("Le doy acceso exclusivo");
				} else {
					servidor_espera(servidores, origen, size);
					debug("Espera");
				}
			}
		}
	}
	free(dales);
	debug("Todos me dieron acceso, empiezo a trabajar");
}

int *inicializar_dales(int size) {
	int i;
	int *aux = malloc(size * sizeof(int));
	for (i = 0; i < size; i++) {
		aux[i] = 0;
	}
	return aux;
}

void nuevo_dale(int *dales, int origen) {
	dales[origen / 2] = 1;
}

int obtengo_acceso(int *dales, Servidor* servidores, int size, int mi_rank) {
	int i;
	for (i = 0; i < size; i++)
		if (servidores[i].servidor_esta_vivo == TRUE && dales[i] == 0 && servidores[i].rank != mi_rank) {
			return FALSE;
		}
	return TRUE;
}

Servidor *inicializar_servidores(int size) {
	int i;
	Servidor *aux = malloc(size * sizeof(Servidor));
	for (i = 0; i < size; i++) {
		aux[i].servidor_espera_respuesta = FALSE;
		aux[i].servidor_esta_vivo = TRUE;
		aux[i].rank = i * 2;
	}
	return aux;
}


void avisoQueLibero(Servidor *servidores, int size) {
	int i;

	for (i = 0; i < size; i++) {
		if (servidores[i].servidor_espera_respuesta == TRUE) {
			MPI_Send(NULL, 0, MPI_INT, servidores[i].rank , TAG_DALE, COMM_WORLD);
			servidores[i].servidor_espera_respuesta = FALSE;
		}
	}
}

void servidor_muerto(Servidor* servidores, int origen, int size) {
	int i;
	for (i = 0; i < size; i++) {
		if (servidores[i].rank == origen) {
			servidores[i].servidor_esta_vivo = FALSE;
			break;
		}
	}
}

void servidor_espera(Servidor* servidores, int origen, int size) {
	int i;
	for (i = 0; i < size; i++) {
		if (servidores[i].rank == origen) {
			servidores[i].servidor_espera_respuesta = TRUE;
			break;
		}
	}
}