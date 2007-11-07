#ifndef DATAMANAGER_H_
#define DATAMANAGER_H_

#include "../Common/DefinitionsManager.h"
#include "../Common/TipoDatos.h"
#include "../../Common/Tamanios.h"
#include "../Common/Clave.h"
#include <string>

using namespace std;

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
	int insertar(const string& nombreTipo, const DefinitionsManager::ListaValoresAtributos* listaVA,
				 const DefinitionsManager::ListaTiposAtributos* listaTipos);
	/*
	 * Elimina un registro del disco 
	 **/
	int eliminar(const string& nombreTipo, DefinitionsManager::ListaClaves* listaClaves);
	
	int modificar(const string& nombreTipo, const DefinitionsManager::ListaValoresAtributos* listaVA,
				  const DefinitionsManager::ListaTiposAtributos* listaTipos,
				  DefinitionsManager::ListaClaves* listaClaves);
	
	void crearRegistroAlta(DefinitionsManager::ListaValoresAtributos &listaVA,
						   DefinitionsManager::ListaTiposAtributos &listaTiposAtributos);
	
	/*
	 * Crea un registro a partir de modificaciones de otro 
	 **/
	void crearRegistroModificacion(DefinitionsManager::ListaTiposAtributos &listaTiposAtributos,
								   DefinitionsManager::ListaValoresAtributos &listaVA,
								   DefinitionsManager::ListaClaves &listaClaves);
private:
	// Registro que se genera para realizar un alta o modificacion en disco
	char *registro;
	// Calcula la longitud del registro para reservar memoria para el mismo
	unsigned short getTamanioRegistro(DefinitionsManager::ListaTiposAtributos &listaTiposAtributos,
									  DefinitionsManager::ListaValoresAtributos &listaVA);
	
};


#endif /*DATAMANAGER_H_*/
 
