#ifndef __IMPRESORERROR_H__
#define __IMPRESORERROR_H__

#include "common_error.h"
#include <ostream>

using namespace std;

//Operador utilizado para imprimir el contenido del objeto
//(en caso que la salida sea la salida est�ndar) o para enviar
//el contenido del objeto a alg�n otro stream de salida
ostream& operator << (ostream &out, const Error &e) {
    switch (e.getValue()){
        case Error::ERR_COMANDO_INVALIDO:
            out << "El comando no es valido"; break;
        case Error::ERR_ENTERO_NO_VALIDO:
            out << "El numero no es valido"; break;
        case Error::ERR_DIVISION_POR_CERO:
            out << "Division por cero"; break;
        case Error::ERR_FORMATO_ARCHIVO:
            out << "Error en el formato del archivo"; break;
        case Error::ERR_ABRIR_ARCHIVO:
            out << "Error al abrir el archivo"; break;
        case Error::ERR_SINTAXIS:
            out << "Error de sintaxis"; break;
        case Error::ERR_SOCKET:
            out << "Error de Socket"; break;
        case Error::ERR_THREAD:
            out << "Error al crear Thread"; break;
        case Error::ERR_IP:
            out << "La direccion IP no es valida"; break;
        case Error::ERR_PUERTO:
            out << "El puerto no es valido"; break;
        case Error::ERR_CANT_CLIENTES:
            out << "La cantidad de clientes no es valida"; break;
        case Error::ERR_ENTERO_NO_PRIMO:
            out << "El numero no es primo"; break;
        case Error::ERR_OVERFLOW:
            out << "Se produjo overflow";
    }
    return out;
}

#endif
