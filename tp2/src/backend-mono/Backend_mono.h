#ifndef Backend_mono_h
#define Backend_mono_h


#include "Encabezado.h"
#include "Casillero.h"
#include "Enviar_recibir.h"

using namespace std;
bool cargar_int(const char* numero, unsigned int& n);

void atendedor_de_jugador(int socket_fd);


// mensajes recibidos por el server
int recibir_nombre(int socket_fd, char* nombre);
int recibir_comando(int socket_fd, char* mensaje);
int parsear_casillero(char* mensaje, Casillero& ficha);


// mensajes enviados por el server
int enviar_dimensiones(int socket_fd);
int enviar_tablero(int socket_fd);
int enviar_ok(int socket_fd);
int enviar_error(int socket_fd);


// otras funciones
void cerrar_servidor(int signal);
void terminar_servidor_de_jugador(int socket_fd, list<Casillero>& palabra_actual);
void quitar_letras(list<Casillero>& palabra_actual);
bool es_ficha_valida_en_palabra(const Casillero& ficha, const list<Casillero>& palabra_actual);
Casillero casillero_mas_distante_de(const Casillero& ficha, const list<Casillero>& palabra_actual);
bool puso_letra_en(unsigned int fila, unsigned int columna, const list<Casillero>& letras);


#endif