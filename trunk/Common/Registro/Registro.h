#ifndef REGISTRO_H_
#define REGISTRO_H_

#include "../Clave/Clave.h"
#include "../DefManager/DefinitionsManager.h"
#include "../ClaveFactory/ClaveFactory.h"
#include <sstream>

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Registro 
//			(Permite el manejo y parseo de los atributos contenidos
//				en un registro).
//////////////////////////////////////////////////////////////////////////

class Registro
{	
	private:
    //////////////////////////////////////////////////////////////////////
	// Atributos
	//////////////////////////////////////////////////////////////////////
		
		unsigned char tipoOrg;                                             // Indica si el registro es de longitud variable o no.
		ListaTipos* listaTipos;	                                           // Contiene una lista que indica como se compone un registro.
		ListaValoresClaves listaAtributos;             // Lista contenedora de los atributos del registros
		ListaNombresAtributos*  listaNombresAtributos; // Lista contenedora de los nombres de los atributos en el registro.
	
	public:
	///////////////////////////////////////////////////////////////////////
    // Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		Registro(unsigned char tipoOrg, char* data, ListaTipos* listaTipos, 
				ListaNombresAtributos* listaNombresAtributos);
		virtual ~Registro(); 
	
	///////////////////////////////////////////////////////////////////////
    // Metodos publicos
	///////////////////////////////////////////////////////////////////////
		unsigned char getCantidadAtributos();
		string getAtributo(unsigned char posicion);
		int getTipoAtributo(unsigned char posicion);
		string getNombreAtributo(unsigned char posicion);
		
		Clave* getClave(const ListaNombresClaves &listaNombreClaves);
		
	///////////////////////////////////////////////////////////////////////
    // Getter
	///////////////////////////////////////////////////////////////////////		
		unsigned char getTipoOrg()
		{
			return this->tipoOrg;
		}
		
}; /*Fin clase Registro*/

#endif /*REGISTRO_H_*/
