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
		 * Este metodo inserta un elemento en un indice y guarda el bloque de datos.
		 **/
		virtual int insertar(Clave *clave, char* &registro, unsigned short tamanioRegistro);
		
		virtual int insertar(Clave *clave, char* &registro) {return 0;}
		/*
		 * Este metodo elimina un elemento del indice.
		 * Si es un índice primario también se elimina el registro de datos.
		 **/
		virtual int eliminar(Clave *clave);
		
		/*
		 * Este metodo busca un elemento dentro del indice.
		 * y devuelve el bloque que contiene el registro de clave "clave"
		 * dentro de "registro".
		 **/
		virtual int buscar(Clave *clave, char* &registro, unsigned short &tamanioBucket) const;
		
		virtual int buscar(Clave *clave, char* &registro) const { return 0; }
		
		/*
		 * Método utilizado para saber si una clave ya se encuentra insertada en el índice
		 */
		virtual int buscar(Clave *clave) const;
		
		/*
		 * Devuelve ResultadosIndices::ERROR_MODIFICACION si claveVieja no se encuentra en el indice. 
		 * En caso contrario devuelve ResultadosIndices::OK, reemplaza claveVieja
		 * por claveNueva y reemplaza el viejo registro correspondiente
		 * a "claveVieja" por "registroNuevo".
		 **/
		virtual int modificar(Clave *claveVieja, Clave *claveNueva,
				                       char* &registroNuevo, unsigned short tamanioRegistroNuevo);
		
		/*
		 * Método que llama a la capa física para pedirle un bloque que contenga espacio suficiente
		 * para insertar un nuevo registro de tamaño 'tamRegistro'
		 */
		virtual int buscarBloqueDestino(unsigned short tamRegistro, char* bloqueDatos);
		
};

#endif /*INDICEHASH_H_*/

