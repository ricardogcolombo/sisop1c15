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
//Generamos el numero magico para cada server y el maximo 
	int numero_magico_actual = 0;
	int numero_magico_maximo = 0;
	MPI_Status status;
	int origen, tag;
	int hay_pedido_local = FALSE;
	int listo_para_salir = FALSE;
	//Averiguo el rank del proceso servidor actual
	int mi_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &mi_rank);
	//saco la cantidad maxima de procesos servidores que hay e arreglo de servidores.
	int size = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	size = size / 2;
	//en este arreglo de servidores guardo informacion de los demas
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
			//En esta parte es cuando el proceso avisa a los demas servidores 
			// que necesita procesar el 
			assert(origen % 2 == 0 );
			debug("Un servidor pide acceso exclusivo");

			//Actualizo el número mágico máximo si corresponde
			if (numero_magico_maximo < numero)
			{
				numero_magico_maximo = numero;
			}
			//Avisa a todos los demas que pueden procesar 
			MPI_Send(NULL, 0, MPI_INT, origen, TAG_DALE, COMM_WORLD);
			debug("Le doy acceso exclusivo");
		} else if (tag == TAG_PERMISO_SERVER && hay_pedido_local == TRUE) {

			//Actualizo el número mágico máximo si corresponde
			if (numero_magico_maximo < numero)
			{
				numero_magico_maximo = numero;
			}
			//paso a la funcion de manejar permismos 
			servidor_espera(servidores, origen, size);
		} else if (tag == TAG_ADIOS) {
			//aviso a todos los servidores que no me cuenten para futuras esperas/avisos asi no se cuelga nadie esperandome
			servidor_muerto(servidores, origen, size);
		}
	}
	free(servidores);
}

void aviso_que_me_muero(Servidor *servidores, int size, int mi_rank) {
	int i;
	//Recorro a todos los servidores y si el servidor me esta esperando le aviso que me fui
	for (i = 0; i < size; i++)
		if (servidores[i].rank != mi_rank && servidores[i].servidor_esta_vivo == TRUE ) {
			MPI_Send(NULL, 0, MPI_INT, servidores[i].rank , TAG_ADIOS, COMM_WORLD);
		}
}

void negociar_acceso(int numero_magico_actual, int *numero_magico_maximo, int size, Servidor* servidores) {
	int i;
	//obtengo mi rank
	int mi_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &mi_rank);
	MPI_Status status;
	//recorro todos los servidores y aviso que necesito tener el permiso
	debug("Le pido acceso a los demas servidores");
	for (i = 0; i < size; i++) {
		if (servidores[i].rank != mi_rank && servidores[i].servidor_esta_vivo) {
			MPI_Send(&numero_magico_actual, 1, MPI_INT, servidores[i].rank, TAG_PERMISO_SERVER, COMM_WORLD);
		}
	}
	debug("Ok... ahora espero respuestas");
	//voy a contar la cantidad de ok que responden para poder ejecutar mi seccion critica
	int *dales = inicializar_dales(size);
	while (!obtengo_acceso(dales, servidores, size, mi_rank)) {
		//espero que me den acceso
		int otro_numero_magico;
		//recibo todos los mensajes posibles
		MPI_Recv(&otro_numero_magico, 1, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);
		debug("RECIBO DE LA GILADA");

		int tag = status.MPI_TAG;
		int origen = status.MPI_SOURCE;
		if (tag == TAG_DALE ) {
			//si es un dale sumo uno 
			nuevo_dale(dales, origen);
			debug("Me dan permiso!");
		} else if (tag == TAG_ADIOS) { 
			//si es un adios lo saco de mi lista de servidores 
			servidor_muerto(servidores, origen, size);
		} else if (tag == TAG_PERMISO_SERVER) {
		//si recibo otro pedido de permiso entonces negocio
			assert(origen % 2 == 0 );
			debug("Un servidor pide acceso exclusivo");

			//Actualizo el número mágico máximo si corresponde
			if (*numero_magico_maximo < otro_numero_magico)
			{
				*numero_magico_maximo = otro_numero_magico;
			}
// si mi numero magico es mayor al del otro
			if (numero_magico_actual > otro_numero_magico) {
			// entonces le doy permiso el tiene prioridad
				MPI_Send(NULL, 0, MPI_INT, origen, TAG_DALE, COMM_WORLD);
				debug("Le doy acceso exclusivo");
			} else if (numero_magico_actual < otro_numero_magico) {
				//si no lo dejo colgado esperandome que yo ejecute 
				servidor_espera(servidores, origen, size);
				debug("Espera");
			} else if (numero_magico_actual == otro_numero_magico) {
				debug("Numeros magicos iguales!");
				//si tenemos el mismo numero , entonces desempato por el rank
				if (origen < mi_rank) {
					//si yo tengo mayor rank entonces le doy permiso
					MPI_Send(NULL, 0, MPI_INT, origen, TAG_DALE, COMM_WORLD);
					debug("Le doy acceso exclusivo");
				} else {
					//caso contrario entonces lo dejo esperando
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
	//seteo el arreglo de dales en 0 
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
	//chequeo si tengo los dale de todos los servidores 
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
//recorro todos los servidores que me estan esperando y les aviso que termine asi pueden ejecutar ellos
	for (i = 0; i < size; i++) {
		if (servidores[i].servidor_espera_respuesta == TRUE) {
			MPI_Send(NULL, 0, MPI_INT, servidores[i].rank , TAG_DALE, COMM_WORLD);
			servidores[i].servidor_espera_respuesta = FALSE;
		}
	}
}

void servidor_muerto(Servidor* servidores, int origen, int size) {
	int i;
	//apago el servidor dentro del arreglo de servidores 
	for (i = 0; i < size; i++) {
		if (servidores[i].rank == origen) {
			servidores[i].servidor_esta_vivo = FALSE;
			break;
		}
	}
}

void servidor_espera(Servidor* servidores, int origen, int size) {
	int i;
	//marco el servidor que me esta esperando 
	for (i = 0; i < size; i++) {
		if (servidores[i].rank == origen) {
			servidores[i].servidor_espera_respuesta = TRUE;
			break;
		}
	}
}
