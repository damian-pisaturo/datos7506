#ifndef PARSEROPERACIONES_H_
#define PARSEROPERACIONES_H_

#define MAX_LINEA 256
#define SEPARATOR_CAMPOS ";"
#define SEPARATOR_ATRIBUTOS ","
#define ASIGNACION_ATRIBUTOS "="


#include <string>
#include <iostream>
#include <fstream>
#include <list>

#include "../../Common/DefManager/DefinitionsManager.h"
#include "../DataManager/DataManager.h"
#include "../../CapaIndices/Indices/Indice.h"
#include "../../Common/Bloque/Bloque.h"
#include "../../Common/Tamanios.h"
#include "../../Common/NombresCapas.h"
#include "../../Common/OperacionesCapas.h"
#include "../../Common/CodigosPipe.h"
#include "../../ComuDatos/ComuDatos.h"

using namespace std;

class ParserOperaciones {
	
	private:
		 
		ifstream archivo;
	
	public:
		ParserOperaciones(const string &nombreArchivo);
		virtual ~ParserOperaciones();
		
		//Devuelve false cuando se encuentra con una operacion que no entiende como ejecutar.
		bool ejecutarOperaciones();
	private:
		// Genera la clave para pasarsela a la capa de indices
		string generarPrototipoClave(DefinitionsManager::ListaClaves &listaClaves);
};

#endif /*PARSEROPERACIONES_H_*/
