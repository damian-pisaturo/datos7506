#ifndef DATAMANAGER_H_
#define DATAMANAGER_H_

#include "../RegisterInfo/RegisterInfo.h"
#include "../Common/TipoDatos.h"
#include "../../Common/Tamanios.h"
#include "../Common/Clave.h"
#include <string>
using namespace std;

typedef list<string> ListaValoresAtributos;
//typedef  ListaClaves;
typedef  ListaNodos ListaTiposAtributos;

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
	int insertar(const string& nombreTipo, const ListaValoresAtributos* listaVA, const ListaTiposAtributos* listaTipos);
	/*
	 * Elimina un registro del disco 
	 **/
	int eliminar(const string& nombreTipo, ListaClaves* listaClaves);
	
	int modificar(const string& nombreTipo, const ListaValoresAtributos* listaVA, const ListaTiposAtributos* listaTipos, ListaClaves* listaClaves);
	
	void crearRegistroAlta(ListaValoresAtributos listaVA,ListaTiposAtributos listaTiposAtributos);
	
	/*
	 * Crea un registro a partir de modificaciones de otro 
	 **/
	void crearRegistroModificacion(ListaTiposAtributos listaTiposAtributos, ListaValoresAtributos listaVA,ListaClaves listaClaves);
private:
	// Registro que se genera para realizar un alta o modificacion en disco
	char *registro;
	// Calcula la longitud del registro para reservar memoria para el mismo
	unsigned short getTamanioRegistro(ListaTiposAtributos listaTiposAtributos,ListaValoresAtributos listaVA);
	
};


#endif /*DATAMANAGER_H_*/
 
