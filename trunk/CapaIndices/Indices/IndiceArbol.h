#ifndef INDICEARBOL_H_
#define INDICEARBOL_H_

#include "../BTree/BTree.h"
#include "../BStar/BStarTree.h"
#include "../BPlus/BPlusTree.h"
#include "../Manager/BloqueManager/BloqueDatosManager/BloqueDatosManager.h"
#include "../Manager/BloqueManager/BloqueListaManager/BloqueListaManager.h"
#include "../Common/IndiceManagerFactory.h"
#include "../../Common/Bloque/Bloque.h"
#include "../Common/TipoIndices.h"
#include "Indice.h"

class IndiceArbol: public Indice {
	
	private:
		
		BTree* bTree;
		BloqueManager* bloqueManager;
		ListaNodos* listaNodos;
		
	public:
		
		IndiceArbol(unsigned char tipoIndice, unsigned short tamBloqueLista,
					int tipoDato, ListaNodos* listaTipos, unsigned char tipoEstructura,
					unsigned short tamNodo, unsigned short tamBloqueDato,
					const string& nombreArchivo, unsigned char tipoOrg);
		
		virtual ~IndiceArbol();
		
		/*
		 * Este metodo inserta un elemento en un indice y guarda el bloque de datos.
		 **/
		virtual int insertar(Clave *clave, char* &registro, unsigned short tamanioRegistro);
		
		virtual int insertar(Clave *clave, char* &registro);
		
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
		virtual int buscar(Clave *clave, char* &registro, unsigned short &tamanioRegistro) const;
		
		
		virtual int buscar(Clave *clave, char* &registro) const;
		
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
		
		
	private:
		ListaTipos* getListaTipos(ListaNodos *listaNodos) const;
						                       
		
};

#endif /*INDICEARBOL_H_*/
