#ifndef REGISTERINFO_H_
#define REGISTERINFO_H_
#include "../Common/TipoDatos.h"

#include <list>
#include <string>
using namespace std;

typedef struct _nodoLista{
	int tipo;
	string pk;
}nodoLista;

typedef  list<nodoLista> ListaNodos;
class RegisterInfo
{

private:
	/*
	 * Lista que contendrá en cada nodo tipo de atributo, e indicador si el mismo es o no primary key
	 * La estructura de un nodoLista el cual es un struct que posee el tipo de campo y si es pk.
	 * 
	 * */
	ListaNodos parameterList;
	int longReg;

public:
	RegisterInfo();
	virtual ~RegisterInfo();
	void setParameterList();
	ListaNodos getParameterList();
	void setLongitudRegistro(int longitud);
	int getLongitudRegistro();
	void insertarElemento();
};

#endif /*REGISTERINFO_H_*/
