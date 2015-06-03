#include "Enviar_recibir.h"

int recibir(int s, char* buf) {
    ssize_t n;
    n = recv(s, buf, MENSAJE_MAXIMO, 0);
    if (n == 0)
        return -1; /* Se terminó la conexión. */
    if (n < 0) {
        printf(">> Error recibiendo %s", buf);
        return -2; /* Se produjo un error. */
    }
    buf[n] = '\0'; /* Agregar caracter de fin de cadena a lo recibido. */
    return 0;
}

int enviar(int s, char* buf) {
    ssize_t n;
    n = send(s, buf, strlen(buf), 0);
    if (n < 0) {
        printf(">> Error enviando %s", buf);
        return -2; /* Se produjo un error. */
    }
    return 0;
}
