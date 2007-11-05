#ifndef PARSEROPERACIONES_H_
#define PARSEROPERACIONES_H_

#define MAX_LINEA 256
#define SEPARATOR_CAMPOS ";"
#define SEPARATOR_ATRIBUTOS ","
#define ASIGNACION_ATRIBUTOS "="

#include <string>
#include <iostream>
#include <fstream>
#include "../Common/DefinitionsManager.h"
#include "../DataManager/DataManager.h"
using namespace std;

class ParserOperaciones {
	
	private:
		
		ifstream archivo;
	
	public:
		ParserOperaciones(string nombreArchivo);
		virtual ~ParserOperaciones();
		
		//Devuelve false cuando se encuentra con una operacion que no entiende como ejecutar.
		bool ejecutarOperaciones();
		
};

#endif /*PARSEROPERACIONES_H_*/
