#ifndef INDICEARBOL_H_
#define INDICEARBOL_H_

#include "../BTree/BTree.h"
#include "../BStar/BStarTree.h"
#include "../BPlus/BPlusTree.h"
#include "Indice.h"

class IndiceArbol: public Indice {
	
	private:
		
		BTree* bTree;
		Bloque* bloque;
		unsigned char tipoOrg;
		
	public:
		
		IndiceArbol(unsigned char tipoIndice, int tipoDato,
					ListaTipos* listaTiposClave, ListaNodos* listaTipos,
					unsigned char tipoEstructura, unsigned short tamBloqueDatos,
					unsigned short tamNodo, unsigned short tamBloqueLista,
					const string& nombreArchivo, unsigned char tipoOrg);
		
		virtual ~IndiceArbol();
		
		/*
		 * Este metodo inserta un elemento en un indice y guarda el bloque de datos.
		 **/
		virtual int insertar(Clave *clave, char* &registro, unsigned short tamanioRegistro);
		
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
		virtual int buscar(Clave *clave, char* &registro, unsigned short &tamanioRegistro) const;
		
		
		virtual int buscar(Clave *clave, ListaClaves* &listaClavesPrimarias) const;
		
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
		virtual int modificar(Clave *claveVieja, Clave *claveNueva, char* &registroNuevo,
							  unsigned short tamanioRegistroNuevo);
		
		/*
		 * Método que llama a la capa física para pedirle un bloque que contenga espacio suficiente
		 * para insertar un nuevo registro de tamaño 'tamRegistro'
		 */
		virtual int buscarBloqueDestino(unsigned short tamRegistro, char* bloqueDatos);
		
		virtual SetEnteros getConjuntoBloques();
				
		virtual Bloque* leerBloque(unsigned int nroBloque);
		
		/*
		 * Método que devuelve el siguiente bloque de disco que contiene
		 * registros de datos.
		 */
		virtual int siguienteBloque(Bloque* &bloque);
		
		virtual void primero();
		virtual void mayorOIgual(Clave* clave);
		virtual void mayor(Clave* clave);
		
		virtual Clave* siguiente();
		
		unsigned char getTipoOrganizacion() const { return this->tipoOrg; }
						                       
		
};

#endif /*INDICEARBOL_H_*/
