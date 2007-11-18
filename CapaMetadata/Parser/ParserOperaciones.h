///////////////////////////////////////////////////////////////////////////
//	Archivo   : ParserOperaciones.h
//  Namespace : CapaIndices
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de la clase ParserOperaciones.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef PARSEROPERACIONES_H_
#define PARSEROPERACIONES_H_

#define SEPARATOR_CAMPOS ';'
#define SEPARATOR_ATRIBUTOS ','
#define ASIGNACION_ATRIBUTOS '='

#define ALTA 'A'
#define BAJA 'B'
#define MODIFICACION 'M'
#define CONSULTA 'C'

#include <iostream>
#include <fstream>

#include "../../Common/DefManager/DefinitionsManager.h"
#include "../DataManager/DataManager.h"
#include "../../Common/OperacionesCapas.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ParserOperaciones 
//	(Parsea el archivo de operaciones, indicando altas, bajas 
//	y modificaciones)
///////////////////////////////////////////////////////////////////////////
class ParserOperaciones
{	
	private:		 
	//////////////////////////////////////////////////////////////////////
	//Atributos privados
	//////////////////////////////////////////////////////////////////////
		ifstream archivo; // Archivo de operaciones.
		unsigned short numOperacion; // Numero de operacion actual. 		
		DefinitionsManager::ListaClaves* listaClaves; // Lista de claves presentes en cada operacion
		DefinitionsManager::MapaValoresAtributos* mapValoresAtributos;
		string nombreTipo; // Nombre del tipo de 
		unsigned char tipoOperacion; // Tipo de operaciones actual (Alta, Baja o Modificacion)
		
		/* Genera la clave para pasarsela a la capa de indices
		 */
	//	string generarPrototipoClave(DefinitionsManager::ListaClaves &listaClaves);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Contructor/Destructor
	//////////////////////////////////////////////////////////////////////
		ParserOperaciones(const string &nombreArchivo);
		virtual ~ParserOperaciones();
	
	//////////////////////////////////////////////////////////////////////
	// Metodo publico
	//////////////////////////////////////////////////////////////////////
		
		/* Obtiene la siguiente operacion indicada en el archivo de operaciones.
		 * Si no existe una proxima linea, devuelve false; en caso contrario, 
		 * devuelve true.
		 */
		bool proximaOperacion();
		
		
	///////////////////////////////////////////////////////////////////////////
	// Getters/Setters
	///////////////////////////////////////////////////////////////////////////
		
		DefinitionsManager::ListaClaves* getListaClaves()
		{
			return this->listaClaves;
		}
		
		DefinitionsManager::MapaValoresAtributos* getMapaValoresAtributos()
		{
			return this->mapValoresAtributos;
		}
		
		const string getNombreTipo()
		{
			return this->nombreTipo;
		}
		
		unsigned char getTipoOperacion()
		{
			return this->tipoOperacion;
		}
		
		unsigned short getNumOperacion()
		{
			return this->numOperacion;
		}

};

#endif /*PARSEROPERACIONES_H_*/
