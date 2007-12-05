#ifndef IMPRESORRESULTADO_H_
#define IMPRESORRESULTADO_H_

#include "Resultado.h"
#include <ostream>

using namespace std;

//Operador utilizado para imprimir el contenido del objeto
//(en caso que la salida sea la salida estandar) o para enviar
//el contenido del objeto a algun otro stream de salida
ostream& operator << (ostream &out, const Resultado &e) {
	
    switch (e.getValue()){
    
        case ResultadosFisica::OK:
            out << "Operación de CapaFisica finalizada con éxito."; break;
        case ResultadosFisica::ARCHIVO_INVALIDO:
            out << "Se ha intentado abrir un archivo que está corrupto, fue modificado o no pertenece al Framework de Persistencia."; break;
        case ResultadosFisica::CANT_ARGUMENTOS_INVALIDA:
            out << "La cantidad de argumentos de la llamada a CapaFisica es inválida."; break;
        case ResultadosFisica::ERROR_ESCRITURA:
            out << "Se ha producido un error al intentar escribir un archivo desde CapaFisica."; break;
        case ResultadosFisica::ERROR_LECTURA:
            out << "Se ha producido un error al intentar leer un archivo desde CapaFisica."; break;
        case ResultadosFisica::ERROR_POSICION:
            out << "Se ha producido un error al intentar posicionarse dentro de un archivo desde CapaFisica."; break;
        case ResultadosFisica::NO_ABIERTO:
            out << "Se ha producido un error al intentar acceder a un archivo no abierto desde CapaFisica."; break;
        case ResultadosFisica::OPERACION_INVALIDA:
            out << "Operación de CapaFisica inválida."; break;
        case ResultadosIndices::OK:
            out << "Operación de CapaIndices finalizada con éxito."; break;
        case ResultadosIndices::ERROR_CONSULTA:
            out << "Se ha producido un error al intentar realizar una consulta desde CapaIndices."; break;
        case ResultadosIndices::ERROR_ELIMINACION:
            out << "Se ha producido un error al intentar eliminar un registro desde CapaIndices."; break;
        case ResultadosIndices::ERROR_INSERCION:
            out << "Se ha producido un error al intentar insertar un registro desde CapaIndices."; break;
        case ResultadosIndices::ERROR_MODIFICACION:
            out << "Se ha producido un error al intentar modificar un registro desde CapaIndices."; break;
        case ResultadosIndices::ERROR_VALORES_CLAVES:
        	out << "Se ha producido un error al intentar parsear los valores de las claves desde CapaIndices."; break;
        case ResultadosIndices::REGISTRO_NO_ENCONTRADO:
        	out << "El registro buscado en alguna operacion de la CapaIndices no fue encontrado."; break;
        case ResultadosIndices::CLAVE_DUPLICADA:
        	out << "La clave a insertar ya existe."; break;
        case ResultadosIndices::CLAVE_ENCONTRADA:
        	out << "Se encontró la clave."; break;
        case ResultadosIndices::CLAVE_NO_ENCONTRADA:
        	out << "La clave solicitada no existe."; break;
        case ResultadosIndices::ERROR_NO_HAY_INDICE:
        	out << "No existe un índice por el campo pedido."; break; 
        case ResultadosIndices::ERROR_INDICE_NO_SOPORTA:
        	out << "El índice no soporta la operación requerida."; break;         	
        case ResultadosIndices::FIN_BLOQUES:
        case ResultadosFisica::FIN_BLOQUES:
        	out << "Se terminaron de consultar los bloques de datos."; break;
        case ResultadosIndices::OPERACION_INVALIDA:
        	out << "Operación de CapaIndices inválida."; break;
        case ResultadosMetadata::OK:
        	out << "Operación de CapaMetadata finalizada con éxito."; break;
        case ResultadosMetadata::OPERACION_INVALIDA:
        	out << "Operación de CapaMetadata inválida."; break;
        case ResultadosMetadata::ERROR_FORMATO_FECHA:
        	out << "El formato del campo de tipo Fecha es inválido.\nDebe ser de la forma AAAAMMDD."; break;
        case Resultado::SIN_RESULTADO:
        	out << "No se ha registrado ningún resultado."; break;
        case ResultadosParserOperaciones::ERROR_SINTAXIS_INTO:
        	out << "Error de Sintáxis: No se encontró cláusula INTO luego de INSERT."; break;
        case ResultadosParserOperaciones::ERROR_SINTAXIS_PARENTESIS:
        	out << "Error de Sintáxis: Se esperaban encontrar paréntesis."; break;
        case ResultadosParserOperaciones::ERROR_SINTAXIS_VALUES:
        	out << "Error de Sintáxis: No se encontró cláusula VALUES."; break;
        case ResultadosParserOperaciones::ERROR_SINTAXIS_CANT_VALORES:
        	out << "Error de Sintáxis: La cantidad de valores no se corresponde con la cantidad de campos."; break;
        case ResultadosParserOperaciones::ERROR_SINTAXIS_TIPO_OPERACION:
        	out << "Error de Sintáxis: Operación desconocida. Operaciones soportadas: INSERT, DELETE, UPDATE, SELECT."; break;
        case ResultadosParserOperaciones::ERROR_SINTAXIS_FROM:
        	out << "Error de Sintáxis: No se encontró la cláusula FROM."; break;
        case ResultadosParserOperaciones::ERROR_SINTAXIS_WHERE:
        	out << "Error de Sintáxis: Probablemente haya querido utilizar la cláusula WHERE."; break;
        case ResultadosParserOperaciones::ERROR_SINTAXIS_OPERACION:
        	out << "Error de Sintáxis: Operación no soportada. Operaciones soportadas: <, <=, >, >=, =."; break;
        case ResultadosParserOperaciones::ERROR_SINTAXIS_AND_OR:
        	out << "Error de Sintáxis: No se permite operaciones de AND y OR en una misma sentencia."; break;
        case ResultadosParserOperaciones::ERROR_SINTAXIS_SET:
        	out << "Error de Sintáxis: No se encontró cláusula SET."; break;
        case ResultadosParserOperaciones::ERROR_SINTAXIS_ON:
        	out << "Error de Sintáxis: No se encontró cláusula ON requerida por el JOIN."; break;
        case ResultadosParserOperaciones::ERROR_SINTAXIS_ORDER_BY:
        	out << "Error de Sintáxis: Probablemente haya querido utilizar la cláusula ORDER BY."; break;
        default:
        	out << "Resultado inválido"; break;
    }
    
    return out;
}

#endif
