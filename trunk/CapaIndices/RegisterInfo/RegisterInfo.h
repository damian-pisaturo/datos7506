#ifndef REGISTERINFO_H_
#define REGISTERINFO_H_
#include "../Common/TipoDatos.h"


#include <list>
#include <string>
using namespace std;

// Estructura utilizada para guardar la información de los atributos de
// un registro.
// 'tipo' almacena el tipo de dato del atributo y, en caso que sea el primer nodo,
// indica si el tamaño del registro es variable o no. Si el tamaño es fijo en 'pk'
// se almacena el tamaño del registro.
// 'cantClaves' indica la cantidad de claves que conforman el ID del registro.
// Si 'cantClaves = 1', habrá un solo atributo utilizado como clave primaria.
// En caso contrario, habrá muchos atributos 'pk = "true"' que indican si ese
// atributo del registro forma parte de la clave compuesta.

typedef struct _nodoLista{
	int tipo;
	string pk;
	unsigned char cantClaves;
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
	ListaNodos* getParameterList();
	void setLongitudRegistro(int longitud);
	int getLongitudRegistro();
	void insertarElemento();
};

#endif /*REGISTERINFO_H_*/
