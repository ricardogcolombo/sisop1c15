#include "mt.h"

int main(int argc, char* argv[]) {
	/* Implementar */


	struct sockaddr_in local, remote;
    char str[MAX_MSG_LENGTH];
    int n ,s,t ,s1,len;

    int pipefd[2];
    pipe(pipefd);

    dup2(pipefd[1], 1);  // send stdout to the pipe
    dup2(pipefd[1], 2);  // send stderr to the pipe

    char buffer[MAX_MSG_LENGTH];

	/* Crear un socket de tipo UNIX con TCP (SOCK_STREAM). */
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("creando socket");
        exit(1);
    }
	/* Establecer la dirección a la cual conectarse para escuchar. */
    local.sin_family = AF_INET;
	local.sin_port = htons(PORT);
    local.sin_addr.s_addr=htonl(INADDR_ANY);

    if (bind(s, (struct sockaddr *)&local, sizeof(local)) == -1) {
        perror("haciendo bind");
        exit(1);
    }

	/* Escuchar en el socket y permitir 5 conexiones en espera. */
    if (listen(s, 5) == -1) {
        perror("escuchando");
        exit(1);
    }

	/* Aceptar una conexión entrante. */
    t = sizeof(remote);
    if ((s1 = accept(s, (struct sockaddr*) &remote, (socklen_t*) &t)) == -1) {
        perror("aceptando la conexión entrante");
        exit(1);
    }

	/* Mostrar en pantalla todo lo que recibimos. */
    for(;;) {

        memset(&str[0],0,MAX_MSG_LENGTH);
        memset(&buffer[0],0,MAX_MSG_LENGTH);

        n = recv(s1, str, MAX_MSG_LENGTH, 0);
        if (n == 0) 
            break;
        if (n < 0) { 
        	perror("recibiendo");
        	exit(1);
        }
        str[n] = '\0'; /* Agregar caracter de fin de cadena a lo recibido. */
        printf("Ejecutando: %s", str);

      if(strcmp(str,"chau\n") == 0){
            break;
      }
 
        system(str);

        int tamanio = read(pipefd[0], buffer, sizeof(buffer));
        
        sendto(s1,buffer,sizeof(buffer),0,(struct sockaddr *)&remote,tamanio);

        //fflush(pipefd[0]);
        fflush(stdout);
        //memset(&pipefd[0,0,tamanio);

    }

    close(pipefd[0]);
    close(pipefd[1]);

	/* Cerrar la conexión entrante. */
    close(s1);

	/* Cerrar la conexión que escucha. */
	close(s);
	
    return 0;
}

