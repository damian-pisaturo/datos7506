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
		
		unsigned short tamBloqueLista;
		
	public:
		Indice() {}
		
		virtual ~Indice() { if (indiceManager) delete indiceManager; }
		
		/*
		 * Este metodo inserta un elemento en un indice y guarda el bloque de datos.
		 * Si pudo insertar devuelve ResultadosIndices::OK, en caso contrario
		 * devuelve ResultadosIndices::CLAVE_DUPLICADA.
		 **/
		virtual int insertar(Clave *clave, char* &registro, unsigned short tamanioRegistroNuevo) = 0;
		
		/*
		 * Este metodo elimina un elemento del indice.
		 * Si es un índice primario también se elimina el registro de datos.
		 * Si pudo eliminar devuelve ResultadosIndices::OK, en caso contrario
		 * devuelve ResultadosIndices::REGISTRO_NO_ENCONTRADO.
		 **/
		virtual int eliminar(Clave *clave) = 0;
		
		/*
		 * Este metodo busca un elemento dentro del indice.
		 * y devuelve el bloque que contiene el registro de clave "clave"
		 * dentro de "registro".
		 * Si no se encontró la clave devuelve ResultadosIndices::CLAVE_NO_ENCONTRADA.
		 * Si pudo consultar devuelve ResultadosIndices::OK, en caso contrario
		 * devuelve ResultadosIndices::ERROR_CONSULTA.
		 **/
		virtual int buscar(Clave *clave, char* &registro, unsigned short &tamanioRegistro) const = 0;
		
		/*
		 * Busca una clave secundaria y devuelve el bloque de la lista de claves primarias
		 */
		virtual int buscar(Clave *clave, char* &registro) const = 0;
		
		/*
		 * Método utilizado para saber si una clave ya se encuentra insertada en el índice
		 * Si encontró la clave devuelve ResultadosIndices::CLAVE_ENCONTRADA, en caso contrario
		 * devuelve ResultadosIndices::CLAVE_NO_ENCONTRADA.
		 */
		virtual int buscar(Clave *clave) const = 0;
		
		/*
		 * Devuelve ResultadosIndices::CLAVE_NO_ENCONTRADA si claveVieja no se encuentra en el indice
		 * o ResultadosIndices::CLAVE_DUPLICADA si claveNueva ya se encuentra insertada.
		 * En caso contrario devuelve ResultadosIndices::OK si pudo reemplazar el registro o
		 * ResultadosIndices::ERROR_MODIFICACION si no pudo reemplazarlo.
		 **/
		virtual int modificar(Clave *claveVieja, Clave *claveNueva, char* &registroNuevo, 
							  unsigned short tamanioRegistroNuevo) = 0;
		
		/*
		 * Método que llama a la capa física para pedirle un bloque que contenga espacio suficiente
		 * para insertar un nuevo registro de tamaño 'tamRegistro'
		 * Si el índice es un hash se devuelve ResultadosIndices::REQUIERE_REGISTRO.
		 * Si encontró un bloque con espacio libre (el ìndices es un árbol) devuelve 
		 * ResultadosIndices::REQUIERE_BLOQUE, en caso contrario devuelve ResultadosIndices::SIN_ESPACIO_LIBRE.
		 */
		virtual int buscarBloqueDestino(unsigned short tamRegistro, char* &bloqueDatos,
										 unsigned int &nroBloque) = 0;
		
		unsigned char getTipo() const { return this->tipoIndice; }
		
		unsigned short getTamanioBloqueDato() const { return this->tamBloqueDato; }
		
		unsigned short getTamanioBloqueLista() const { return this->tamBloqueLista; }
		
};

#endif /*INDICE_H_*/
