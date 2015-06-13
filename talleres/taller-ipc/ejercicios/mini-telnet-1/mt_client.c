#include "mt.h"

int main(int argc, char* argv[]) {
	/* Implementar */

	char* IP;

	int                 socket_fd;
    int                 len;
    struct sockaddr_in  remote;
    char                buf[MENSAJE_MAXIMO];
   if (argc != 2)
   {
      printf("usage:  client <IP address>\n");
      exit(1);
   }

    /* Crear un socket de tipo UNIX con SOCK_STREAM */
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("creando socket");
        exit(1);
    }

    /* Crear nombre, usamos INADDR_ANY para indicar que cualquiera puede enviar aquí. */
	remote.sin_family = AF_INET;	
	IP = argv[1];
	remote.sin_port = htons(PORT);



	//Copiamos la ip
	if (inet_aton(IP, &remote.sin_addr) == 0) {
    	fprintf(stderr, "Invalid IP address\r\n");
    	exit(1);
	}

	len = sizeof(remote.sin_addr) + sizeof(remote.sin_family) + sizeof(remote.sin_port);


    /* Establecer la dirección a la cual conectarse para escuchar. */
    while( printf("> "), fgets(buf, MENSAJE_MAXIMO, stdin), !feof(stdin) ) {
        if (sendto(socket_fd, buf, strlen(buf), 0, (struct sockaddr *)&remote, sizeof(remote)) == -1) {
            perror("enviando");
            exit(1);
        }


    	//buf[MENSAJE_MAXIMO]="\0";
        if(strcmp(buf,"chau\n") == 0){

            break;
        }

        memset(&buf[0],0,MENSAJE_MAXIMO);

    }

    /* Cerrar el socket. */
    close(socket_fd);

	return 0;
}
