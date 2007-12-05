#ifndef DATAMANAGER_H_
#define DATAMANAGER_H_

#include "../../Common/DefManager/DefinitionsManager.h"
#include "../../Common/TipoDatos.h"
#include "../../Common/Clave/Clave.h"
#include "../../Common/Bloque/Bloque.h"
#include "../../Common/ClaveFactory/ClaveFactory.h"
#include "../../Common/CodigosPipe.h"
#include <string>
#include <sstream>

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
	 * Calcula la longitud del registro para reservar memoria para el mismo
	 * */ 
	unsigned short getTamanioRegistro(const ListaTiposAtributos &listaTiposAtributos,
									  const ListaValoresAtributos &listaVA);

	/*
	 * Crea un registro a partir de modificaciones de otro 
	 **/
	unsigned short crearRegistroModificacion(const ListaTiposAtributos &listaTiposAtributos,
								   const ListaValoresAtributos &listaVA, char *registroEnDisco);
	
	unsigned short crearRegistroAlta(const ListaValoresAtributos &listaVA,
						   const ListaTiposAtributos &listaTiposAtributos);
	
	unsigned short crearRegistroAltaRandom(string &valoresClaves, const ListaTiposAtributos &listaTiposAtributos);	

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
							  const ListaTiposAtributos &listaTiposAtributos,
							  const ListaValoresAtributos &listaVA);
	
	
	
};


#endif /*DATAMANAGER_H_*/
 
