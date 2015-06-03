#ifndef Casillero_h
#define Casillero_h

struct Casillero {
    Casillero() : fila(0), columna(0), letra(VACIO) {};
    //Casillero(unsigned int _fila, unsigned int _columna, char _letra) : fila(_fila), columna(_columna), letra(_letra) {};

    unsigned int fila;
    unsigned int columna;
    char letra;
};

#endif