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
#include "../Indices/Indice.h"
using namespace std;

class ParserOperaciones {
	
	private:
		 
		ifstream archivo;
		VectorIndices &indicesPersonas;
		VectorIndices &indicesPeliculas;
	
	public:
		ParserOperaciones(const string &nombreArchivo, VectorIndices &vectorIndicesPersonas,
						  VectorIndices &vectorIndicesPelicula);
		virtual ~ParserOperaciones();
		
		//Devuelve false cuando se encuentra con una operacion que no entiende como ejecutar.
		bool ejecutarOperaciones();
		
};

#endif /*PARSEROPERACIONES_H_*/
