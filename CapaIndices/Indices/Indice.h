#ifndef INDICE_H_
#define INDICE_H_

#include "../../Common/DefManager/DefinitionsManager.h"
#include "../../Common/Clave/Clave.h"
#include "../Common/SetClaves.h"
#include "../../Common/ResultadosIndices.h"
#include "../../Common/Tamanios.h"
#include <vector>
#include <map>



class Indice;

typedef std::map<DefinitionsManager::ListaNombresClaves, Indice*> MapaIndices;

typedef std::map<std::string, MapaIndices*> MapaMapaIndices;

typedef std::vector<Indice*> VectorIndices;

class Indice
{
	protected:
		IndiceManager* indiceManager;
		
		unsigned char tipoIndice;
		
		unsigned short tamBloqueDato;
		
	public:
		Indice() {}
		
		virtual ~Indice() { if (indiceManager) delete indiceManager; }
		
		/*
		 * Este metodo inserta un elemento en un indice y guarda el bloque de datos.
		 **/
		virtual int insertar(Clave *clave, char* &registro) = 0;
		
		/*
		 * Este metodo elimina un elemento del indice.
		 * Si es un índice primario también se elimina el registro de datos.
		 **/
		virtual int eliminar(Clave *clave) = 0;
		
		/*
		 * Este metodo busca un elemento dentro del indice.
		 * y devuelve el bloque que contiene el registro de clave "clave"
		 * dentro de "registro".
		 **/
		virtual int buscar(Clave *clave, char* &registro) const = 0;
		
		/*
		 * Método utilizado para saber si una clave ya se encuentra insertada en el índice
		 */
		virtual int buscar(Clave *clave) const = 0;
		
		/* Método que busca una clave secundaria. Si la encuentra devuelve en setClavesPrimarias
		 * una lista con las claves primarias correspondientes a esa clave secundaria.
		 */
		//virtual Clave* buscar(Clave* clave, SetClaves* &setClavesPrimarias) const = 0;
		
		/*
		 * Devuelve ResultadosIndices::ERROR_MODIFICACION si claveVieja no se encuentra en el indice. 
		 * En caso contrario devuelve ResultadosIndices::OK, reemplaza claveVieja
		 * por claveNueva y reemplaza el viejo registro correspondiente
		 * a "claveVieja" por "registroNuevo".
		 **/
		virtual int modificar(Clave *claveVieja, Clave *claveNueva,
				                       char* &registroNuevo) = 0;
		
		/*
		 * Método que llama a la capa física para pedirle un bloque que contenga espacio suficiente
		 * para insertar un nuevo registro de tamaño 'tamRegistro'
		 */
		virtual int buscarBloqueDestino(unsigned short tamRegistro, char* &bloqueDatos,
										 unsigned int &nroBloque) = 0;
		
		unsigned char getTipo() const { return this->tipoIndice; }
		
		unsigned short getTamanioBloqueDato() const { return this->tamBloqueDato; }
		
};

#endif /*INDICE_H_*/
