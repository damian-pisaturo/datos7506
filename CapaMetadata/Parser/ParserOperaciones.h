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
#include "../../Common/ResultadosMetadata.h"
#include "../../Common/ResultadosIndices.h"
#include "../../Common/CodigosPipe.h"
#include "../../ComuDatos/ComuDatos.h"

using namespace std;

class ParserOperaciones {
	
	private:		 
		ifstream archivo;
	
	public:
		ParserOperaciones(const string &nombreArchivo);
		virtual ~ParserOperaciones();
		
		/* Ejecuta las operaciones indicadas en el archivo de operaciones.
		 * Devuelve un codigo acorde al resultado de cada una de ellas indicado por
		 * la Capa de Indices.
		 */
		char ejecutarOperaciones();
	
	private:
		/* Genera la clave para pasarsela a la capa de indices
		 */
		string generarPrototipoClave(DefinitionsManager::ListaClaves &listaClaves);
};

#endif /*PARSEROPERACIONES_H_*/
