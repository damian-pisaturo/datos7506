#ifndef DATAMANAGER_H_
#define DATAMANAGER_H_

#include "../RegisterInfo/RegisterInfo.cpp"
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
	void crearRegistroAlta(ListaValoresAtributos listaVA,ListaTiposAtributos listaTiposAtributos);
	//void crearRegistroModificacion(ListaTiposAtributos listaTiposAtributos, ListaClaves listaClaves);
private:
	// Registro que se genera para realizar un alta o modificacion en disco
	char *registro;
	// Calcula la longitud del registro para reservar memoria para el mismo
	unsigned short getTamanioRegistro(ListaTiposAtributos listaTiposAtributos,ListaValoresAtributos listaVA);
	
};


#endif /*DATAMANAGER_H_*/
 
