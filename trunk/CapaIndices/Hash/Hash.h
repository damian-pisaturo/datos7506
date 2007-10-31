///////////////////////////////////////////////////////////////////////////
//	Archivo   : Hash.h
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clase Hash.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef HASH_H_
#define HASH_H_

#include <string>
#include "Tabla.h"
#include "Bucket.h"

#define OK	 		0
#define FAIL	 	1
#define DUPLICATED	2


///////////////////////////////////
////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Hash (Implementa indices de dispersion extensible)
//////////////////////////////////////////////////////////////////////////
class Hash 
{
	private:
	//////////////////////////////////////////////////////////////////////
	// Atributos
	//////////////////////////////////////////////////////////////////////
		Tabla *tabla;
		ArchivoIndiceHash* archivo;
		const list<nodoLista> listaParam;
	
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		Hash(ArchivoIndiceHash* indiceHash);
		virtual ~Hash();	
		
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////

		/*
		 * Este método se utiliza para hacer una operación de alta en el archivo.
		 * Si el registro se puede insertar devuelve OK; si ya existe un registro
		 * con la misma clave, devuelve DUPLICATED, y si no hay lugar en el bucket
		 * correspondiente para insertarlo, devuelve FAIL.
		 */
		int insertarRegistro(char* registro,char* clave);
		int eliminarRegistro(char* clave);
	
	private:
	///////////////////////////////////////////////////////////////////////
	// Metodos privados
	///////////////////////////////////////////////////////////////////////	
		/*
		 * Este método aplica una función de dispersión a la clave.
		 */
		int aplicarHash(char* clave);
		
		/*string
		 * Método utilizado cuando se aplica la función de dispersión a una clave.
		 */
		int hashInterno(char* clave);
		
		/*
		 * Inserta "registro" en el bucket.
		 */
		int insertarReg(char * registro);
		
		void redistribuirElementos(Bucket * bucket, Bucket * bucketNuevo);

};

#endif /*HASH_H_*/
