///////////////////////////////////////////////////////////////////////////
//	Archivo   : Bucket.h
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clase Bucket.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef BUCKET_H_
#define BUCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Common/BloqueIndice.h"
#include "RegistroV.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Bucket (Permite el manejo de buckets en memoria)
//////////////////////////////////////////////////////////////////////////

class Bucket : public BloqueIndice
{
	private:
	//////////////////////////////////////////////////////////////////////
	// Atributos
	//////////////////////////////////////////////////////////////////////
		char* datos;
		//unsigned int tamanio; TODO El tamanio del bucket vendra en el ArchivoIndiceHash.
		char* registro;
		unsigned bool vacio; //Indica si el bloque contiene al menos un registro.
		unsigned short numBucket; //Indica el numero de bucket dentro del archivo.
		unsigned short tamDispersion;
		unsigned short cantRegs;
	
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		/*Crea un Bucket vacio*/
		Bucket(ArchivoIndice *indiceHash, unsigned short tamDispersion);
		
		/*Crea un Bucket con los datos del bloque cuya 
		 * referencia en el archivo es la pasada por parametro.
		 */
		Bucket(ArchivoIndice* indiceHash, unsigned short referencia);
		virtual ~Bucket();
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		/*
		 * Busca un registro por su clave dentro del bucket. Si existe lo carga en el 
		 * atributo registro y devuelve true; de lo contrario devuelve false.
		 */ 
		bool buscarRegistro(Clave* clave);
	
	private:
	///////////////////////////////////////////////////////////////////////
	// Metodos privados
	///////////////////////////////////////////////////////////////////////
	
		/*
		 * Busca un registro de clave de longitud variable en el bucket.
		 */
		bool buscarRegClaveV(Clave* clave);
	
		/*
		 * Busca un registro de clave de longitud fija en el bucket.
		 */
		bool buscarRegClaveF(Clave* clave);
};

#endif /*BUCKET_H_*/
