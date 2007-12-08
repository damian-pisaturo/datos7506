#ifndef INDICE_H_
#define INDICE_H_

#include "../../Common/DefManager/DefinitionsManager.h"
#include "../../Common/Clave/Clave.h"
#include "../../Common/Bloque/Bloque.h"
#include "../../Common/ResultadosIndices.h"
#include "../../Common/Tamanios.h"
#include "../Common/TipoIndices.h"
#include "../Common/SetClaves.h"
#include "../Common/IndiceManagerFactory.h"
#include "../Common/BloqueListaPrimaria/BloqueListaPrimaria.h"
#include "../Manager/BloqueManager/BloqueDatosManager/BloqueDatosManager.h"
#include "../Manager/BloqueManager/BloqueListaManager/BloqueListaManager.h"
#include <set>

using namespace std;

class Indice;

typedef map<ListaNombresClaves, Indice*> MapaIndices;

typedef map<std::string, MapaIndices*> MapaMapaIndices;

typedef vector<Indice*> VectorIndices;

typedef set<unsigned int> SetEnteros;

class Indice
{
	protected:
		IndiceManager* indiceManager;
		unsigned char tipoIndice;
		unsigned char tipoEstructura;
		unsigned short tamBloqueDatos;
		unsigned short tamBloqueLista;
		ListaInfoRegistro* listaInfoReg;
		BloqueManager* bloqueManager;
		
	public:
		Indice() {}
		
		virtual ~Indice() { 
			if (indiceManager) delete indiceManager;
			if (bloqueManager) delete bloqueManager;
			if (listaInfoReg) delete listaInfoReg;
		}
		
		/*
		 * Este metodo inserta un elemento en un indice y guarda el bloque de datos.
		 * Si pudo insertar devuelve ResultadosIndices::OK, en caso contrario
		 * devuelve ResultadosIndices::CLAVE_DUPLICADA.
		 **/
		virtual int insertar(Clave *clave, char* &registro, unsigned short tamanioRegistroNuevo) = 0;
		
		virtual int insertar(Clave *claveSecundaria, Clave* clavePrimaria) = 0;
		
		/*
		 * Este metodo elimina un elemento del indice.
		 * Si es un índice primario también se elimina el registro de datos.
		 * Si pudo eliminar devuelve ResultadosIndices::OK, en caso contrario
		 * devuelve ResultadosIndices::REGISTRO_NO_ENCONTRADO.
		 **/
		virtual int eliminar(Clave *clave) = 0;
		
		/*
		 * Este método elimina una clave primaria de la lista de claves primarias
		 * correspondiente a la clave secundaria.
		 * Si la lista queda vacía, también se elimina la clave secundaria.
		 */
		virtual int eliminar(Clave* claveSecundaria, Clave *clavePrimaria) = 0;
		
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
		 * Busca una clave secundaria y devuelve la lista de claves primarias
		 */
		virtual int buscar(Clave *clave, SetClaves* &setClavesPrimarias) const = 0;
		
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
		 * Este método modifica claves secundarias y sus correspondientes listas de claves primarias.
		 */
		virtual int modificar(Clave* claveSecundariaVieja, Clave* claveSecundariaNueva,
							  Clave* clavePrimariaVieja, Clave* clavePrimariaNueva);
		
		/*
		 * Método que llama a la capa física para pedirle un bloque que contenga espacio suficiente
		 * para insertar un nuevo registro de tamaño 'tamRegistro'
		 * Si el índice es un hash se devuelve ResultadosIndices::REQUIERE_REGISTRO.
		 * Si encontró un bloque con espacio libre (el ìndices es un árbol) devuelve 
		 * ResultadosIndices::REQUIERE_BLOQUE, en caso contrario devuelve ResultadosIndices::SIN_ESPACIO_LIBRE.
		 */
		virtual int buscarBloqueDestino(unsigned short tamRegistro, char* bloqueDatos) = 0;
		
		/*
		 * Retorna un conjunto con todos los numeros de bloques
		 * */
		virtual SetEnteros getConjuntoBloques() = 0;
		
		virtual Bloque* leerBloque(unsigned int nroBloque) = 0;
		
		/*
		 * Método que devuelve el siguiente bloque de disco que contiene
		 * registros de datos.
		 */
		virtual int siguienteBloque(Bloque* &bloque) = 0;
		
		unsigned char getTipoIndice() const { return this->tipoIndice; }
		
		unsigned char getTipoEstructura() const { return this->tipoEstructura; }
		
		unsigned short getTamanioBloqueDatos() const { return this->tamBloqueDatos; }
		
		unsigned short getTamanioBloqueLista() const { return this->tamBloqueLista; }
		
		ListaInfoRegistro* getListaInfoReg() const { return this->listaInfoReg; }
		
		ListaTipos* getListaTipos() const;

		ListaTipos* getListaTiposClavePrimaria() const;
		
		ListaInfoRegistro* getListaInfoRegClavePrimaria() const;
		
		virtual void primero() = 0;
		virtual void mayorOIgual(Clave* clave) = 0;
		virtual void mayor(Clave* clave) = 0;
		virtual Clave* siguiente() = 0;
		
};

#endif /*INDICE_H_*/
