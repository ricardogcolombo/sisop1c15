#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 5

void pexit(char *error) {
	perror(error);
	exit(EXIT_FAILURE);
}

int siguiente(int i) {
	return (i + 1) % N;
}

int anterior(int i) {
	return (i - 1) % N;
}

int numeroMagico() {
	return rand() % 100;
}

double operacionComplicada() {
	return numeroMagico();
}

int main(int argc, char *argv[]) {
	int ring_pipes[N][2];
	int data_pipe[2];

	// Inicializamos los pipes para el anillo
	// TIP: Pensar por quÃ© se inicializan antes del fork()
	for (int i = 0; i < N; i++) {
		if (pipe(ring_pipes[i]) == -1)
			pexit("ring pipe");
	}

	// Inicializamos el pipe por donde se van a transmitir los datos al coordinador
	// TIP: Pensar por quÃ© no utilizamos ring_pings para eso
	if (pipe(data_pipe) == -1)
		pexit("data pipe");

	// Creamos los procesos
	// TIP: Prestar atenciÃ³n a que ningÃºn hijo cree procesos de mÃ¡s cuando cicla el for
	for (int i = 0; i < N; i++) {
		pid_t pid = fork();

		if (pid == -1)
			pexit("fork");

		if (pid == 0) {

			//Esto es sÃ³lo para numeroMagico
			srand ((int)getpid());

			//TIP: Prestar atenciÃ³n al uso de i como manera de numerar los procesos creados
			int me = i;
			int coordinadorTupla [2];
			int miNumeroMagico = numeroMagico();
			int miTupla[] = {me, miNumeroMagico};

			printf("Soy %d: Mi nÃºmero MÃ¡gico es %d \n", me, miNumeroMagico);

			// TIP: Entender bien por quÃ© se cierran estos pipe (para eso lo hice en dos for :P )
			// Cierro todos los pipes de lecturas menos del "mio" (donde voy a leer lo que me escriban a mÃ­)
			for (int j = 0; j < N; j++) {
				if (j != me) {
					if (close(ring_pipes[j][0]))
						pexit("pipe close lectura");
				}
			}

			// Cierro todos los pipes de escrituras menos el del "siguiente"
			for (int j = 0; j < N; j++) {
				if (j != siguiente(me)) {
					if (close(ring_pipes[j][1]))
						pexit("pipe close escritura");
				}
			}


			// Si soy el primer proceso, tengo un comportamiento diferente porque tengo que escribir el primer arreglo
			if (i == 0) {
				// Paso mi tupla al siguiente
				if (write(ring_pipes[siguiente(me)][1], miTupla, sizeof(miTupla)) == -1)
					pexit("initiator ring write");
				// Espero que se complete la vuelta
				if (read(ring_pipes[me][0], coordinadorTupla, sizeof(coordinadorTupla)) == -1)
					pexit("initiator ring read");

				// Inicio otra vuelta para avisar quien es el lider definitivo
				if (write(ring_pipes[siguiente(me)][1], coordinadorTupla, sizeof(coordinadorTupla)) == -1)
					pexit("initiator second ring write");
				// Espero que se complete la vuelta
				if (read(ring_pipes[me][0], coordinadorTupla, sizeof(coordinadorTupla)) == -1)
					pexit("initiator second ring read");
			}
			else {

				// Recibo el lider actual
				// TIP: Pensar por quÃ© puedo llamar a read a pesar de que (tal vez) todavÃ­a no se hayan creado los otros procesos o
				// que todavÃ­a no hayan escrito.
				// TIP: Pensar quÃ© pasarÃ­a si nadie hace un write primero y todos empiezan con un read.

				if (read(ring_pipes[me][0], coordinadorTupla, sizeof(coordinadorTupla)) == -1)
					pexit("ring read");

				// Preparo la tupla para mandar
				if (coordinadorTupla[1] < miNumeroMagico) {
					coordinadorTupla[0] = me;
					coordinadorTupla[1] = miNumeroMagico;
				}

				// Envio la tupla al siguiente
				if (write(ring_pipes[siguiente(me)][1], coordinadorTupla, sizeof(coordinadorTupla)) == -1)
					pexit("ring write");

				// Espero que me digan quien es el lider definitivo
				// TIP: Ver quÃ© estoy leyendo una tupla, aunque podrÃ­a mandar sÃ³lo el entero de quien es el coordinador
				if (read(ring_pipes[me][0], coordinadorTupla, sizeof(coordinadorTupla)) == -1)
					pexit("second ring read");

				printf("Soy %d: El coordinador es %d \n", me, coordinadorTupla[0]);

				// Envio al siguiente el lider definitivo
				// TIP: Ver quÃ© estoy mandando una tupla, aunque podrÃ­a mandar sÃ³lo el entero de quien es el coordinador
				if (write(ring_pipes[siguiente(me)][1], coordinadorTupla, sizeof(coordinadorTupla)) == -1)
					pexit("second ring write");
			}

			double result;
			if (coordinadorTupla[0] == me) {
				//Soy el coordinador

				// Como soy el coordinador, cierro el pipe de escritura para mandarle datos al coordinador.
				if (close(data_pipe[1]))
					pexit("data pipe close");

				// Recibo los resultados y los imprimo
				for (int j = 0; j < (N - 1); j++) {
					if (read(data_pipe[0], &result, sizeof(result)) == -1)
						pexit("result read");
					printf("Soy %d (el coordinador): Imprimo un resultado que me llego %f\n",me, result);
				}
			}
			else {
				//No soy el coordinador

				// Como no soy el coordinador, cierro el pipe de lectura para recibir datos del resto.
				if (close(data_pipe[0]))
					pexit("data pipe close");

				// Hago la operacion y le paso el resultado al lider
				result = operacionComplicada();
				printf("Soy %d: Mi resultado resultado es %f\n", me, result);
				write(data_pipe[1], &result, sizeof(result));
			}

			exit(EXIT_SUCCESS);
		}
	}
	return 0;
}


