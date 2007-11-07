#include "RegisterInfo.h"

RegisterInfo::RegisterInfo()
{	// Lista de tipos para pruebas
	
	// TODO: Cambiar el tipo de dato de pk
	nodoLista nodo;
	
	//El primer nodo de la lista indica si el registro es variable o no.
	nodo.tipo = TipoDatos::TIPO_VARIABLE;
	nodo.pk = "9";
	nodo.cantClaves = 2;
	
	parameterList.push_back(nodo);
	
	nodo.tipo = TipoDatos::TIPO_ENTERO;
	nodo.pk = "false";
	
	parameterList.push_back(nodo);
	
	nodo.tipo = TipoDatos::TIPO_STRING;
	nodo.pk = "false";
	
	parameterList.push_back(nodo);
	
	nodo.tipo = TipoDatos::TIPO_ENTERO;
	nodo.pk = "true";
		
	parameterList.push_back(nodo);
	
} 

RegisterInfo::~RegisterInfo()
{
}

void RegisterInfo::insertarElemento(){}

ListaNodos* RegisterInfo::getParameterList(){
	
	return &this->parameterList; 
}
