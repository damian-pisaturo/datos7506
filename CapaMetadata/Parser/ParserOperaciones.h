#ifndef PARSEROPERACIONES_H_
#define PARSEROPERACIONES_H_


#define SEPARATOR_CAMPOS ';'
#define SEPARATOR_ATRIBUTOS ','
#define ASIGNACION_ATRIBUTOS '='

#define ALTA 'A'
#define BAJA 'B'
#define MODIFICACION 'M'
#define CONSULTA 'C'



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
		ifstream archivo; // Archivo de operaciones.
		unsigned short numOperacion; // Numero de operacion actual. 
		
		//TODO(S)
		
		/*
		 * 1) Definicion de ListaClaves no puede estar en DefinitionsManager si se va a usar desde esta cosa.
		 * 2) Listarme atributos que debiera tener el ParserOperaciones para interpretar cada operacion del archivo.
		 * 3) Darme una idea de como tengo que manejarme desde afuera para manejar los mapas locos que me devuelve el Parser.
		 * 4) ParserOperaciones debe devolver (mediante getters) los mapas y las listas debidas, correspondientes a la operacion
		 * 		actual (que se modifica por cada llamada a proximaOperacion()).
		 * 5) Ver 2.
		 * 6) Tifi = pony en detrimento.
		 */
		
		DefinitionsManager::ListaClaves* listaClaves; // Lista de claves presentes en cada operacion
		DefinitionsManager::MapaValoresAtributos* mapValoresAtributos;
		string nombreTipo;
		unsigned char tipoOperacion;
		
		/* Genera la clave para pasarsela a la capa de indices
		 */
	//	string generarPrototipoClave(DefinitionsManager::ListaClaves &listaClaves);
	
	public:
		ParserOperaciones(const string &nombreArchivo);
		virtual ~ParserOperaciones();
		
		/* Obtiene la siguiente operacion indicada en el archivo de operaciones.
		 * Si no existe una proxima linea, devuelve false; en caso contrario, 
		 * devuelve true.
		 */
		bool proximaOperacion();
		
		
		///////////////////////////////////////////////////////////////////////////
		// Getters/Setters
		///////////////////////////////////////////////////////////////////////////
		
		DefinitionsManager::ListaClaves* getListaClaves(){
			return this->listaClaves;
		}
		
		DefinitionsManager::MapaValoresAtributos* getMapaValoresAtributos(){
			return this->mapValoresAtributos;
		}
		
		string getNombreTipo(){
			return this->nombreTipo;
		}
		
		unsigned char getTipoOperacion(){
			return this->tipoOperacion;
		}
		
		unsigned short getNumOperacion(){
			return this->numOperacion;
		}
	
};

#endif /*PARSEROPERACIONES_H_*/
