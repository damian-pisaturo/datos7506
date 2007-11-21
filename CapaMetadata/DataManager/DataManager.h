#ifndef DATAMANAGER_H_
#define DATAMANAGER_H_

#include "../../Common/DefManager/DefinitionsManager.h"
#include "../../Common/TipoDatos.h"
#include "../../Common/Tamanios.h"
#include "../../Common/Clave/Clave.h"
#include "../../Common/Bloque/Bloque.h"
#include "../../Common/ClaveFactory/ClaveFactory.h"
#include <string>

#define CAMPO_NO_MODIFICADO ""

using namespace std;

class DataManager
{
	
private:
	// Registro que se genera para realizar un alta o modificacion en disco
	char *registro;
	
public:
	DataManager();
	virtual ~DataManager();

/*	
	 * Inserta un registro en disco 
	 *
	int insertar(const DefinitionsManager::ListaValoresAtributos* listaVA,
				 const DefinitionsManager::ListaTiposAtributos* listaTipos, Bloque* bloque);
	
	 * Elimina un registro del disco 
	 *
	int eliminar(DefinitionsManager::ListaClaves* listaClaves,
				 const DefinitionsManager::ListaTiposAtributos* listaTipos, Bloque* bloque);
	
	int modificar(const DefinitionsManager::ListaValoresAtributos* listaVA,
				  const DefinitionsManager::ListaTiposAtributos* listaTipos,
				  DefinitionsManager::ListaClaves* listaClaves, Bloque* bloque,
				  char* registroEnDisco);
*/	
	/*
	 * Calcula la longitud del registro para reservar memoria para el mismo
	 * */ 
	unsigned short getTamanioRegistro(const DefinitionsManager::ListaTiposAtributos &listaTiposAtributos,
									  const DefinitionsManager::ListaValoresAtributos &listaVA);

	/*
	 * Crea un registro a partir de modificaciones de otro 
	 **/
	unsigned short crearRegistroModificacion(const DefinitionsManager::ListaTiposAtributos &listaTiposAtributos,
								   const DefinitionsManager::ListaValoresAtributos &listaVA,
								   char *registroEnDisco);
	
	unsigned short crearRegistroAlta(const DefinitionsManager::ListaValoresAtributos &listaVA,
						   const DefinitionsManager::ListaTiposAtributos &listaTiposAtributos);
	

	char* getRegistro() const
	{
		return this->registro;
	}
	
	void setRegistro(char* reg)
	{
		if(this->registro)
			delete[] this->registro;
		
		this->registro = reg;		
	}
	
private:

	/*
	 * Genera  el registro modificado
	 * */
	void generarRegistroModificado(char *registroNuevo, char *registroEnDisco, unsigned short longNuevoReg, 
							  const DefinitionsManager::ListaTiposAtributos &listaTiposAtributos,
							  const DefinitionsManager::ListaValoresAtributos &listaVA);
	
	
	
};


#endif /*DATAMANAGER_H_*/
 
