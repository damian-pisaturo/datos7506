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



///////////////////////////////////////////////////////////////////////////
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
		Tabla* tabla;
		IndiceHashManager* archivo;
		ListaNodos* listaParam;
	
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		Hash(IndiceHashManager* indiceHash, ListaNodos* lista, unsigned int tamBucket);
		virtual ~Hash();	
		
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////

		/*
		 * Este método se utiliza para hacer una operación de alta en el archivo.
		 * Si el registro se puede insertar devuelve OK; si ya existe un registro
		 * con la misma clave, devuelve DUPLICATED.
		 * En caso de que el registro no entre en el bucket correspondiente, se 
		 * toman las medidas necesarias para hacer extensible la función de hash.
		 * Si el registro es variable "registro" contendrá su longitud en los 
		 * primeros bytes.
		 **/
		int insertarRegistro(char *registro,Clave &clave);
		
		/* Este método se utiliza para hacer una operación de baja en el archivo.
		 * Si el registro se puede eliminar devuelve OK, si no existe el registro
		 * a eliminar, se devuelve NO_ENCONTRADO.
		 * En caso de que el bucket quede vacío, se considera la posibilidad de 
		 * disminuir el tamaño de la tabla de hash.
		 **/
		int eliminarRegistro(Clave &clave);
		
		bool modificarRegistro(Clave &claveVieja, Clave &claveNueva, char* registroNuevo);
	
		/*
		 * A partir de una clave recupera un registro 
		 **/
		void recuperarRegistro(Clave &clave,char* &registro); 		
		
		
	private:
	///////////////////////////////////////////////////////////////////////
	// Metodos privados
	///////////////////////////////////////////////////////////////////////			
		/*
		 * Este método aplica una función de dispersión a la clave.
		 **/
		int aplicarHash(Clave &clave);
		
		/*
		 * Método utilizado internamente cuando se aplica la función de dispersión a una clave.
		 **/
		int hashInterno(char *clave);
		
		/*
		 * Este método se encarga de redistribuir los registros contenidos en bucket 
		 * entre este mismo y bucketNuevo.
		 **/
		void redistribuirElementos(Bucket* &bucket, Bucket* bucketNuevo);
		
		/*
		 * Este método se engarga de dividir el tamaño de dispersión del bucket nroBucket
		 * y actualizarlo en el archivo.
		 **/
		void dividirDispersion(unsigned int nroBucket);		
		
		bool esRegistroVariable();		
		
		char* serializarClave(void** clave);
};

#endif /*HASH_H_*/
