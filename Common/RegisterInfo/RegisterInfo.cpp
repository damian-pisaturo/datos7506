#include "RegisterInfo.h"

RegisterInfo::RegisterInfo()
{	// Lista de tipos para pruebas
	
	// TODO: Cambiar el tipo de dato de pk
	nodoLista nodo;
	
	//El primer nodo de la lista indica s	cout << i << endl;i el registro es variable o no.
	nodo.tipo = TipoDatos::TIPO_FIJO;
	nodo.pk = "8";
	nodo.cantClaves = 1;
	
	parameterList.push_back(nodo);
	
	nodo.tipo = TipoDatos::TIPO_ENTERO;
	nodo.pk = "false";
	
	parameterList.push_back(nodo);
	
	nodo.tipo = TipoDatos::TIPO_CHAR;
	nodo.pk = "false";
	
	parameterList.push_back(nodo);
	
	nodo.tipo = TipoDatos::TIPO_SHORT;
	nodo.pk = "true";
		
	parameterList.push_back(nodo);
	
	nodo.tipo = TipoDatos::TIPO_BOOL;
	nodo.pk = "false";
		
	parameterList.push_back(nodo);
	
} 

RegisterInfo::~RegisterInfo()
{
}

void RegisterInfo::insertarElemento(){}

ListaNodos* RegisterInfo::getParameterList(){
	
	return &this->parameterList; 
}
