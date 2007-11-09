#ifndef DATAMANAGER_H_
#define DATAMANAGER_H_

#include "../../Common/DefinitionsManager.h"
#include "../../Common/TipoDatos.h"
#include "../../Common/Tamanios.h"
#include "../../CapaIndices/Common/Clave.h"
#include "../../CapaIndices/Bloque/Bloque.h"
#include "../../CapaIndices/Common/ClaveFactory.h"
#include <string>

using namespace std;

#define CAMPO_NO_MODIFICADO "" 

class DataManager
{
public:
	DataManager();
	virtual ~DataManager();

	char* getRegistro() const{
		return registro;
	}
	/*
	 * Inserta un registro en disco 
	 **/
	int insertar(const DefinitionsManager::ListaValoresAtributos* listaVA,
				 const DefinitionsManager::ListaTiposAtributos* listaTipos, Bloque* bloque);
	/*
	 * Elimina un registro del disco 
	 **/
	int eliminar(DefinitionsManager::ListaClaves* listaClaves,
				 const DefinitionsManager::ListaTiposAtributos* listaTipos, Bloque* bloque);
	
	int modificar(const DefinitionsManager::ListaValoresAtributos* listaVA,
				  const DefinitionsManager::ListaTiposAtributos* listaTipos,
				  DefinitionsManager::ListaClaves* listaClaves, Bloque* bloque,
				  char* registroEnDisco);
	

private:
	// Registro que se genera para realizar un alta o modificacion en disco
	char *registro;
	/*
	 * Calcula la longitud del registro para reservar memoria para el mismo
	 * */ 
	unsigned short getTamanioRegistro(const DefinitionsManager::ListaTiposAtributos &listaTiposAtributos,
									  const DefinitionsManager::ListaValoresAtributos &listaVA);
	/*
	 * Genera  el registro modificado
	 * */
	void generarRegistroModificado(char *registroNuevo, char *registroEnDisco, unsigned short longNuevoReg, 
							  const DefinitionsManager::ListaTiposAtributos &listaTiposAtributos,
							  const DefinitionsManager::ListaValoresAtributos &listaVA);
	
	void crearRegistroAlta(const DefinitionsManager::ListaValoresAtributos &listaVA,
						   const DefinitionsManager::ListaTiposAtributos &listaTiposAtributos);
	
	/*
	 * Crea un registro a partir de modificaciones de otro 
	 **/
	void crearRegistroModificacion(const DefinitionsManager::ListaTiposAtributos &listaTiposAtributos,
								   const DefinitionsManager::ListaValoresAtributos &listaVA,
								   char *registroEnDisco);
	
};


#endif /*DATAMANAGER_H_*/
 
