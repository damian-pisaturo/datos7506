///////////////////////////////////////////////////////////////////////////
//	Archivo   : ValidadorDatos.h
//  Namespace : CapaMetadata
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de la clase ValidadorDatos.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefanía;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef VALIDADORDATOS_H_
#define VALIDADORDATOS_H_

#include "../../Common/CommonDefinitions.h"
#include "../../Common/ResultadosMetadata.h"
#include "../../Common/TipoDatos.h"
#include <sstream>
#include <algorithm>

#define VAL_VACIO ""
#define SIGNO_MENOS '-'
#define VALORES_NUMERICOS "-0123456789"
#define VALORES_NUMERICOS_POS "0123456789"
#define VAL_TRUE "TRUE"
#define VAL_FALSE "FALSE"

using namespace std;

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ValidadorDatos 
//	(Valida los registros y claves cuyos tipos y valores corresponden a
//	las estructuras pasadas como argumento de los metodos)
///////////////////////////////////////////////////////////////////////////
class ValidadorDatos
{
	public:
	///////////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////////
		ValidadorDatos();
		virtual ~ValidadorDatos();
		
	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////
		char validarRegistro(const ListaTipos& tiposDatos, const ListaValoresAtributos& valoresAtributos) const;
		char validarClave(const ListaTipos& tiposDatos, const ListaValoresClaves& valoresClaves) const;
		char validarNombresAtributos(const ListaNombresAtributos& nombresAtributos, const MapaValoresAtributos& mapValAtributos) const;
		char validarNombresClaves(const ListaNombresAtributos& nombresAtributos, const ListaNombresClaves& infoClaves) const;
		
	private:
	///////////////////////////////////////////////////////////////////////////
	// Metodos privados
	///////////////////////////////////////////////////////////////////////////
		char validarEntero(const string& valorEntero) const;
		char validarShort(const string& valorShort) const;
		char validarChar(const string& valorChar) const;
		char validarBool(string valorBool) const;
		char validarFecha(const string& fecha) const;
		char validarString(const string& s) const;
	
}; /*Fin clase ValidadorDatos*/

#endif /*VALIDADORDATOS_H_*/
