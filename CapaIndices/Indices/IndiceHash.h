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
#include <set>

using namespace std;

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
		ListaTipos* listaTiposClave;
		ListaInfoRegistro* listaNodosClave;
		
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		IndiceHash(unsigned char tipoIndice, ListaTipos *listaTiposClave,
				ListaInfoRegistro* listaInfoReg, unsigned int tamBloqueLista,
				   unsigned int tamBucket, const string& nombreArchivo);
		
		virtual ~IndiceHash();
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////

		/*
		 * Este metodo inserta un elemento en un indice y guarda el bloque de datos.
		 **/
		virtual int insertar(Clave *clave, char* &registro, unsigned short tamanioRegistro);
		
		/*
		 * Este método hace una inserción en un indice secundario; si la clave secundaria
		 * ya está insertada, agrega la clave primaria a su lista invertida; sino,
		 * agrega la clave secundaria al indice, y crea su lista invertida correspondiente
		 * con clave primaria como su unico elemento.
		 **/
		virtual int insertar(Clave *claveSecundaria, Clave* clavePrimaria);
		
		/*
		 * Este metodo elimina un elemento del indice.
		 * Si es un índice primario también se elimina el registro de datos.
		 **/
		virtual int eliminar(Clave *clave);
		
		/*
		 * Este método elimina una clave primaria de la lista de claves primarias
		 * correspondiente a la clave secundaria.
		 * Si la lista queda vacía, también se elimina la clave secundaria.
		 */
		virtual int eliminar(Clave* claveSecundaria, Clave *clavePrimaria);
		
		/*
		 * Este metodo busca un elemento dentro del indice.
		 * y devuelve el bloque que contiene el registro de clave "clave"
		 * dentro de "registro".
		 **/
		virtual int buscar(Clave *clave, char* &registro, unsigned short &tamanioBucket) const;
		
		/*
		 * Busca una clave en un indice secundario, y si la encuntra retorna su lista invertida,
		 * que contiene las claves primarias en listaClaves.
		 **/
		virtual int buscar(Clave *clave, SetClaves* &setClavesPrimarias) const ;
		
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
		
		SetEnteros getConjuntoBloques();
		
		Bloque* leerBloque(unsigned int nroBloque);
		
		/*
		 * Método que devuelve el siguiente bloque de disco que contiene
		 * registros de datos.
		 */
		virtual int siguienteBloque(Bloque* &bloque);
		
		unsigned int getOffsetToList(char *registro, unsigned short tamanioRegistro) const;
		
		void setOffsetToList(unsigned int offset, char *registro, unsigned short tamanioRegistro);
		
		virtual void primero() {}
		virtual void mayorOIgual(Clave* clave) {}
		virtual void mayor(Clave* clave) {}
		virtual Clave* siguiente() { return NULL; }
		
	private:
		ListaInfoRegistro* getListaNodosClave() const;
};

#endif /*INDICEHASH_H_*/

