///////////////////////////////////////////////////////////////////////////
//	Archivo   : IndiceHash.h
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clase IndiceHash.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#ifndef INDICEHASH_H_
#define INDICEHASH_H_

#include "../Hash/Hash.h"
#include "../Manager/IndiceManager.h"
#include "../Common/IndiceManagerFactory.h"
#include "Indice.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceHash (Interfaz comun entre los distintos tipos de indices
//				y el hash extensible)
//////////////////////////////////////////////////////////////////////////
class IndiceHash :public Indice
{	
	private:	
	//////////////////////////////////////////////////////////////////////
	// Atributos
	//////////////////////////////////////////////////////////////////////
		Hash* hash;		
		
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		IndiceHash(ListaNodos *listaParam, unsigned int tamBucket, const string& nombreArchivo);		
		virtual ~IndiceHash();
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////

		/*
		 * Este metodo inserta un registro en el indice.
		 **/
		int insertar(Clave *clave, char* &registro = NULL);
		
		/*
		 * Este metodo elimina un registro del indice.
		 **/
		int eliminar(Clave *clave);
		
		/*
		 * Este metodo busca un registro dentro del indice.
		 * Siempre retorna NULL y devuelve el registro de clave "clave"
		 * dentro de "registro".
		 **/
		Clave* buscar(Clave *clave, char* &registro = NULL) const;
		
		Clave* buscar(Clave* clave, SetClaves* &setClavesPrimarias) const;
	
		/*
		 * Elimina al registro de clave "claveVieja" e inserta al registro "registroNuevo".
		 **/
		bool modificar(Clave *claveVieja, Clave *claveNueva, char* &registroNuevo);
		
		char buscarBloqueDestino(unsigned short tamRegistro, char* &bloqueDatos, unsigned int &nroBloque);
	
};

#endif /*INDICEHASH_H_*/

