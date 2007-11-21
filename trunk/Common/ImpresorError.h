#ifndef __IMPRESORERROR_H__
#define __IMPRESORERROR_H__

#include "Error.h"
#include <ostream>

using namespace std;

//Operador utilizado para imprimir el contenido del objeto
//(en caso que la salida sea la salida estandar) o para enviar
//el contenido del objeto a algun otro stream de salida
ostream& operator << (ostream &out, const Error &e) {
    switch (e.getValue()){
        case ResultadosFisica::OK:
            out << "Operacion de capaFisica finalizada con exito."; break;
        case ResultadosFisica::ARCHIVO_INVALIDO:
            out << "Se ha intentado abrir algun archivo que esta corrupto, fue modificado o no pertenece al Framework de Persistencia."; break;
        case ResultadosFisica::CANT_ARGUMENTOS_INVALIDA:
            out << "La cantidad de argumentos de la llamada a capaFisica es invalida. "; break;
        case ResultadosFisica::ERROR_ESCRITURA:
            out << "Se ha producido un error al intentar escribir un archivo desde capaFisica."; break;
        case ResultadosFisica::ERROR_LECTURA:
            out << "Se ha producido un error al intentar leer un archivo desde capaFisica."; break;
        case ResultadosFisica::ERROR_POSICION:
            out << "Se ha producido un error al intentar posicionarse dentro de un archivo desde capaFisica."; break;
        case ResultadosFisica::NO_ABIERTO:
            out << "Se ha producido un error al intentar acceder a un archivo no abierto desde capaFisica."; break;
        case ResultadosFisica::OPERACION_INVALIDA:
            out << "Operacion de capaFisica invalida."; break;
        case ResultadosIndices::OK:
            out << "Operacion de capaIndices finalizada con exito."; break;
        case ResultadosIndices::ERROR_CONSULTA:
            out << "Se ha producido un error al intentar realizar una consulta desde capaIndices."; break;
        case ResultadosIndices::ERROR_ELIMINACION:
            out << "Se ha producido un error al intentar eliminar un registro desde capaIndices."; break;
        case ResultadosIndices::ERROR_INSERCION:
            out << "Se ha producido un error al intentar insertar un registro desde capaIndices."; break;
        case ResultadosIndices::ERROR_MODIFICACION:
            out << "Se ha producido un error al intentar modificar un registro desde capaIndices."; break;
        case ResultadosIndices::REGISTRO_NO_ENCONTRADO:
        	out << "El registro buscado en alguna operacion de la capaIndices no fue encontrado."; break;
        case ResultadosIndices::CLAVE_DUPLICADA:
        	out << "La clave a insertar ya existe."; break;
        case ResultadosIndices::CLAVE_NO_ENCONTRADA:
        	out << "La clave solicitada no existe."; break;
        case ResultadosMetadata::OPERACION_INVALIDA:
        	out << "Operacion de capaMetadata invalida."; break;
        case ResultadosMetadata::OK:
        	out << "Operacion de capaIndices finalizada con exito."; break;	
    }
    
    return out;
}

#endif
