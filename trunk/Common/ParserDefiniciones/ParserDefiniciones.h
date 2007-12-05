///////////////////////////////////////////////////////////////////////////
//	Archivo   : ParserDefiniciones.h
//  Namespace : Common
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clase ParserOperaciones.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefanía;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef PARSERDEFINICIONES_H_
#define PARSERDEFINICIONES_H_

#include <algorithm>
#include <iostream>
#include <vector>
#include "../Archivo/Archivo.h"
#include "../CommonDefinitions.h"
#include "../TipoDatos.h"
#include "../../CapaIndices/Common/TipoIndices.h"

using namespace std;

// Cabeceras de las secciones del archivo
// de definiciones.
#define HEADER_TIPO "[DEFINICIONTIPO]"
#define HEADER_ORGREG "[ORGANIZACIONREGISTRO]"
#define HEADER_ATRIBUTOS "[DEFINICIONATRIBUTOS]"
#define HEADER_INDICES "[DEFINICIONINDICES]"
#define HEADER_GRIEGO "[GRIEGOS]"
#define HEADER_ROMANO "[ROMANOS]"
#define HEADER_INTEGRIDAD "[INTEGRIDAD]"
#define HEADER_FIN "[FIN]"

// Nombres de los atributos a ser pardeados
// en cada seccion del archivo de definiciones.
#define NOMBRE_TIPO "NOMBRETIPO"
#define ORG_REGISTROS "ORGANIZACIONREGISTROS"
#define ATRIBUTO_NOMBRE "ATRIBUTONOMBRE"
#define ATRIBUTO_TIPO "ATRIBUTOTIPO"
#define TIPO_INDICE "TIPOINDICE"
#define TAM_BUCKET "TAMANOBUCKET"
#define CAMPO_INDICE "CAMPOINDICE"
#define TAM_NODO "TAMANONODOINDICE"
#define CAMPO_DATO "CAMPODATO"
#define TIPO_INTEGRIDAD "TIPOINTEGRIDAD"
#define CAMPO_INTEGRIDAD "CAMPOINTEGRIDAD"

#define I_HASH "HASH"
#define I_ARBOL_BS "ARBOLB*"
#define I_ARBOL_BP "ARBOLB+"

#define REG_RVB "RVB"
#define REG_RF "RF"

#define C_IGUAL '='
#define C_VACIO ' '
#define C_COMENTARIO '#'

#define T_STRING "STRING"
#define T_FECHA "FECHA"
#define T_ENTERO "ENTERO"
#define T_CHAR "CHAR"
#define T_SHORT "SHORT"
#define T_REAL "REAL"
#define T_BOOL "BOOL"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ParserDefiniciones 
//	(Parsea el archivo de definiciones y arma las estructuras necesarias
// 	para ser consultadas desde el DefinitionsManager)
///////////////////////////////////////////////////////////////////////////
class ParserDefiniciones
{
	private:	
	///////////////////////////////////////////////////////////////////////////
	// Atributos
	///////////////////////////////////////////////////////////////////////////
		Archivo archivo; // Archivo de definiciones.
		ListaDefinicionesTipos definicionesTipos;
	
	public:
	///////////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////////
		ParserDefiniciones(const string& nombreArchivo);
		virtual ~ParserDefiniciones();
	
	///////////////////////////////////////////////////////////////////////////
	// Metodo publico
	///////////////////////////////////////////////////////////////////////////
		/* Parsea el archivo de definiciones cuyo nombre es nombreArchivo
		 * y llena las estructuras correspondientes.
		 */
		void parsearDefiniciones();

	///////////////////////////////////////////////////////////////////////////
	// Getter
	///////////////////////////////////////////////////////////////////////////
		ListaDefinicionesTipos& getDefinicionesTipos()
		{
			return this->definicionesTipos;
		}

	private:
		void leerLinea(string& linea);
		void aMayuscula(string& linea);
		void retrocederLinea(const string& linea);
		void parsearTipo(string& nombreTipo);
		void parsearOrgRegistro(unsigned char& orgRegistros);
		void parsearAtributos(ListaNombresAtributos* listaNombresAtributos, 
				VectorTiposAtributos* vTiposAtributos);
		void parsearIndices(DatosIndices* datosIndicesGriegos, DatosIndices* datosIndicesRomanos);
		void parsearIntegridad();
};

#endif /*PARSERDEFINICIONES_H_*/
