#include "DefinicionesManager.h"

DefinicionesManager::DefinicionesManager()
{
}

DefinicionesManager::~DefinicionesManager()
{
}

/*
void DefinicionesManager::cargarDefiniciones() {
	
	ListaNodos* listaNodos = NULL;
	nodoLista nodo;
	
	//CARGO LAS DEFINICIONES DEL TIPO "PERSONA"
	
	listaNodos = new ListaNodos();
	
	//El primer nodo de la lista indica si el registro es variable o no.
	nodo.tipo = TipoDatos::TIPO_VARIABLE;
	nodo.pk = "0";
	nodo.cantClaves = 1;
	
	listaNodos->push_back(nodo);
	
	//DNI
	nodo.tipo = TipoDatos::TIPO_ENTERO;
	nodo.pk = "true";
	listaNodos->push_back(nodo);
	
	//Nombre
	nodo.tipo = TipoDatos::TIPO_STRING;
	nodo.pk = "false";
	listaNodos->push_back(nodo);
	
	//FechaNacimiento
	nodo.tipo = TipoDatos::TIPO_FECHA;
	nodo.pk = "false";
	listaNodos->push_back(nodo);
	
	mapaDef["Persona"] = listaNodos;
	
	
	//CARGO LAS DEFINICIONES DEL TIPO "PELICULA"

	listaNodos = new ListaNodos();
	
	//El primer nodo de la lista indica si el registro es variable o no.
	nodo.tipo = TipoDatos::TIPO_FIJO;
	nodo.pk = "34";
	nodo.cantClaves = 2;
	
	listaNodos->push_back(nodo);
	
	//Titulo (cadena de 15 carecteres)
	nodo.tipo = TipoDatos::TIPO_STRING;
	nodo.pk = "true";
	listaNodos->push_back(nodo);
	
	//Director (cadena de 10 caracteres)
	nodo.tipo = TipoDatos::TIPO_STRING;
	nodo.pk = "true";
	listaNodos->push_back(nodo);
	
	//Genero
	nodo.tipo = TipoDatos::TIPO_CHAR;
	nodo.pk = "false";
	listaNodos->push_back(nodo);
	
	mapaDef["Pelicula"] = listaNodos;
	
}
*/
