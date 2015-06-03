#ifndef Encabezado_h
#define Encabezado_h


#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <vector>
#include <iostream>
#include <list>

#define PORT 5481

#define MENSAJE_MAXIMO 1024
#define DATO_MAXIMO 100

#define VACIO 0x20

#define MSG_LETRA 1
#define MSG_PALABRA 2
#define MSG_UPDATE 3
#define MSG_INVALID 99


int recibir(int s, char* buf);
int enviar(int s, char* buf);


#endif