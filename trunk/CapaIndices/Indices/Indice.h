#ifndef INDICE_H_
#define INDICE_H_

#include "../../Common/DefinitionsManager.h"
#include "../Common/Clave.h"
#include <vector>
#include <map>

#define OK				0
#define NO_ENCONTRADO	1

class Indice;

typedef std::map<DefinitionsManager::ListaNombresClaves, Indice*> MapaIndices;

typedef std::map<std::string, MapaIndices*> MapaMapaIndices;

typedef std::vector<Indice*> VectorIndices;

class Indice
{
	protected:
		unsigned char tipoIndice;
		
	public:
		Indice(ListaNodos *listaParam, unsigned int tamBucket, const string& nombreArchivo) {}
		
		virtual ~Indice() {}
		
		/*
		 * Este metodo inserta un elemento en un indice.
		 * Si es un arbol insertara solo una clave, pero si es un hash inserta 
		 * tambien el registro.
		 **/
		virtual int insertar(Clave *clave, char* registro = NULL) = 0;
		
		/*
		 * Este metodo elimina un elemento del indice.
		 * Si es un arbol elimina una clave, pero si es un hash elimina un 
		 * registro.
		 **/
		virtual int eliminar(Clave *clave) = 0;
		
		/*
		 * Este metodo busca un elemento dentro del indice.
		 * Si es un arbol retorna una Clave.
		 * Si es un hash retorna NULL y devuelve el registro de clave "clave"
		 * dentro de "registro".
		 **/
		virtual Clave* buscar(Clave *clave, char* registro = NULL) const = 0; 
		
		/*
		 * Devuelve false si claveVieja no se encuentra en el indice. 
		 * En caso contrario devuelve true; y si es un arbol, la reemplaza 
		 * por claveNueva; si es un hash reemplaza el viejo registro correspondiente
		 * a "claveVieja" por "registroNuevo".
		 **/
		virtual bool modificar(Clave *claveVieja, Clave *claveNueva,
				                       char* registroNuevo = NULL) = 0;
		
		unsigned char getTipo() const { return this->tipoIndice; }
		
		
};

#endif /*INDICE_H_*/
