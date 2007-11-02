#ifndef NODOBSTAR_H_
#define NODOBSTAR_H_

#include "../Common/Nodo.h"

class NodoBStar : public Nodo {
	
	public:
		NodoBStar(unsigned int refNodo, unsigned char nivel, unsigned short tamanio);
		
		NodoBStar(unsigned int refNodo, unsigned char nivel, Clave* clave,
				  unsigned short tamanio);
		
		virtual ~NodoBStar() {}
		
		unsigned short getTamanioEnDisco() const;
		
		//Devuelve el espacio (en bytes) destinado para almacenar claves (elementos).
		//Esta es una redefinición del método de la clase padre.
		virtual unsigned short getTamanioEspacioClaves() const {
			
			if (this->getNivel() == 0) //Nodo hoja ==> Tengo que sumarle el espacio que se le descuenta por la referencia al hijo izq
				return (this->getTamanio() - Nodo::getTamanioHeader() + Tamanios::TAMANIO_REFERENCIA);
			else return (this->getTamanio() - Nodo::getTamanioHeader());
			
		}
		
		unsigned short getTamanioMinimo() const {
			
			return (2*(this->getTamanioEspacioClaves())/3);
			
		}
		
		VectorConjuntos* split();
		
		Nodo* copiar() const;

};

#endif /*NODOBSTAR_H_*/
