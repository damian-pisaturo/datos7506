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

#define SEPARATOR_CAMPOS ' '
#define SEPARATOR_ATRIBUTOS ','
#define SEPARATOR_TIPO_NOMBRE '.'
#define FIN_OPERACION ';'
#define CARACTER_AGRUPADOR '\''
#define CARACTER_COMENTARIO '#'

#define ALTA "INSERT"
#define BAJA "DELETE"
#define MODIFICACION "UPDATE"
#define CONSULTA "SELECT"

#define NO_OPERACION 127

#include <iostream>
#include <fstream>
#include <algorithm>

#include "../../Common/DefManager/DefinitionsManager.h"
#include "../DataManager/DataManager.h"
#include "../../Common/OperacionesCapas.h"
#include "../../Common/ExpresionesLogicas.h"
#include "../../Common/ResultadosParserOperaciones.h"

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
//		DefinitionsManager::ListaClaves* listaClaves; // Lista de claves presentes en cada operacion
		DefinitionsManager::EstructuraCampos estructuraCampos;
		DefinitionsManager::MapaValoresAtributos* mapValoresAtributos;
		DefinitionsManager::EstructuraConsulta estructuraConsulta;
		string nombreTipo;
		unsigned char tipoOperacion; // Tipo de operaciones actual (Alta, Baja o Modificacion)
		
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
		 * Devuelve el resultado de la operación.
		 */
		int proximaOperacion();
		
	private:
		
		list<string> parsearString(string s, char caracterDelimitador, char caracterAgrupador);
		void reemplazarCaracter(string* s, char caracterViejo, char caracterNuevo);
		void limpiarInicio(string* s, char caracterALimpiar);
		void removerCaracter(string* s, char caracter);
		string* mayus(string* s);
		
	public:
		
	///////////////////////////////////////////////////////////////////////////
	// Getters/Setters
	///////////////////////////////////////////////////////////////////////////
		
		DefinitionsManager::EstructuraCampos getEstructuraCampos()
		{
			return this->estructuraCampos;
		}
		
		DefinitionsManager::EstructuraConsulta getEstructuraConsulta()
		{
			return this->estructuraConsulta;
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
