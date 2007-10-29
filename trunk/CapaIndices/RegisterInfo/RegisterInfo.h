#ifndef REGISTERINFO_H_
#define REGISTERINFO_H_

#include <list>
#include <string>

typedef struct _nodoLista{
	int tipo;
	string pk;
}nodoLista;

using namespace std;

class RegisterInfo
{

private:
	/*
	 * Lista que contendrá en cada nodo tipo de atributo, e indicador si el mismo es o no primary key
	 * La estructura de un nodo será la siguiente:
	 * <tipo>;<true||false>
	 * En caso de ser una pk, el segundo campo será true y en caso contrario sera false
	 * El delimitador entre el tipo y el indicador es el ;
	 * Si tenemos un atributo del registro que es un int y es la pk, la definición será la siguiente:
	 * int;true
	 * El primer nodo de la lista posee la siguiente información:
	 * <Variable || Fijo>;<long>(solo si es fijo)?
	 * 
	 * */
	list <nodoLista> parameterList;
	int longReg;

public:
	RegisterInfo();
	virtual ~RegisterInfo();
	void setParameterList();
	list <nodoLista> getParameterList();
	void setLongitudRegistro(int longitud);
	int getLongitudRegistro();
};

#endif /*REGISTERINFO_H_*/
