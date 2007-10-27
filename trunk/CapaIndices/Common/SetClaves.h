#ifndef SETCLAVES_H_
#define SETCLAVES_H_

#include "Clave.h"
#include <set>
#include <vector>

using namespace std;

class SetClaves;

typedef vector<SetClaves*> VectorConjuntos;

class SetClaves : public set<Clave*, Comparador> {
	
	public:
		//Constructor
		SetClaves() {}
		
		//Destructor
		virtual ~SetClaves();
		
		//Devuelve un puntero a la clave buscada (si la encuentra) o
		//un puntero a la clave inmediatamente anterior (clave menor 
		//a la buscada) o NULL si la clave es menor a todas las claves del
		//conjunto.
		Clave* findClave(Clave* clave) const;
		
		//Este metodo divide al conjunto actual en dos.
		//Si la cantidad de claves es impar, este conjunto se queda con la
		//menor cantidad de claves.
		//El conjunto devuelto contiene las claves mayores.
		//Recibe la minima cantidad de claves (en bytes) que debe tener
		//cada conjunto.
		SetClaves* splitBPlus(unsigned short minClaves);
		
		//Este metodo divide al conjunto actual en tres 2/3 llenos.
		//Devuelve un vector con punteros a los otros conjuntos creados.
		//Los conjuntos devueltos en el vector son los que contiene las 
		//claves a promocionar.
		//Recibe la minima cantidad de claves (en bytes) que debe tener
		//cada conjunto.
		VectorConjuntos* splitBStar(unsigned short minClaves);
		
		//Este metodo fusiona este conjunto con un conjunto hermano
		//y con una clave padre.
		void merge(SetClaves* set, Clave* c1, Clave* c2);
		
		//Este metodo fusiona este conjunto con dos conjuntos hermanos
		//y con dos claves padre.
		void merge(SetClaves* set1, SetClaves* set2, Clave* c1, Clave* c2);
		
};

#endif /*SETCLAVES_H_*/
