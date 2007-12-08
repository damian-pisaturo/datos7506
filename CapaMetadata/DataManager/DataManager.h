///////////////////////////////////////////////////////////////////////////
//	Archivo   : DataManager.h
//  Namespace : CapaMetadata
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clase DataManager.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefanía;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef DATAMANAGER_H_
#define DATAMANAGER_H_

#include "../../Common/ResultadosMetadata.h"
#include "../../Common/DefManager/DefinitionsManager.h"
#include "../../Common/TipoDatos.h"
#include "../../Common/Clave/Clave.h"
#include "../../Common/Bloque/Bloque.h"
#include "../../Common/ClaveFactory/ClaveFactory.h"
#include "../../Common/CodigosPipe.h"
#include <string>
#include <sstream>
#include <algorithm>

#define CAMPO_NO_MODIFICADO ""
#define SIGNO_MENOS '-'
#define VALORES_NUMERICOS "-0123456789"
#define VALORES_NUMERICOS_POS "0123456789"

using namespace std;

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: DataManager 
//	(Crea registros de datos a partir de estructuras basicas obtenidas
//	desde la Capa de Consultas.)
///////////////////////////////////////////////////////////////////////////
class DataManager
{	
	private:
	///////////////////////////////////////////////////////////////////////////
	// Atributos
	///////////////////////////////////////////////////////////////////////////
		// Registro que se genera para realizar un alta o modificacion en disco
		char *registro;
		
	public:	
	///////////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////////
		DataManager();
		virtual ~DataManager();	
	
	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////
	/* 
		 * Calcula la longitud del registro para reservar memoria para el mismo
		 * */ 
		unsigned short getTamanioRegistro(const ListaTiposAtributos &listaTiposAtributos,
										  const ListaValoresAtributos &listaVA);
	
		/*
		 * Crea un registro a partir de modificaciones de otro 
		 **/
		unsigned short crearRegistroModificacion(const ListaTiposAtributos &listaTiposAtributos,
									   const ListaValoresAtributos &listaVA, char *registroEnDisco);
		
		unsigned short crearRegistroAlta(const ListaValoresAtributos &listaVA,
							   const ListaTiposAtributos &listaTiposAtributos);
		
		unsigned short crearRegistroAltaRandom(string &valoresClaves, const ListaTiposAtributos &listaTiposAtributos);	
	
	///////////////////////////////////////////////////////////////////////////
	// Getter/Setter
	///////////////////////////////////////////////////////////////////////////
		char* getRegistro() const
		{
			return this->registro;
		}
		
		void setRegistro(char* reg)
		{
			if(this->registro)
				delete[] this->registro;
			
			this->registro = reg;		
		}
	
	private:
	///////////////////////////////////////////////////////////////////////////
	// Metodo privado
	///////////////////////////////////////////////////////////////////////////
		/*
		 * Genera  el registro modificado
		 * */
		void generarRegistroModificado(char *registroNuevo, char *registroEnDisco, unsigned short longNuevoReg, 
								  const ListaTiposAtributos &listaTiposAtributos,
								  const ListaValoresAtributos &listaVA);
		

		
}; /*Fin clase DataManager*/


#endif /*DATAMANAGER_H_*/
 
