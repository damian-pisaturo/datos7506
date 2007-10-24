#ifndef NODOBSTAR_H_
#define NODOBSTAR_H_

#include "../Common/Nodo.h"

class NodoBStar : public Nodo {
	
	public:
		NodoBStar(unsigned int refNodo, unsigned int nivel, unsigned int condicionMinima) : Nodo(refNodo, nivel, condicionMinima) {}
		NodoBStar(unsigned int refNodo, unsigned int nivel, Clave* clave, unsigned int condicionMinima) : Nodo(refNodo, nivel, clave, condicionMinima) {}
		virtual ~NodoBStar() {}
	
		void insertarClave(Clave* &clave, char* codigo);
		void eliminarClave(Clave* clave, char* codigo);
		Nodo* siguiente(Clave* clave);
		unsigned getTamanioEnDisco() const;
		unsigned getTamanioEnDiscoSetClaves() const;
		unsigned puedeCeder(unsigned bytesRequeridos, bool izquierda) const;
		bool puedeRecibir(unsigned bytesPropuestos) const;
		unsigned obtenerBytesRequeridos() const;
		SetClaves* ceder(unsigned bytesRequeridos, bool izquierda);
		void recibir(SetClaves* set);

		
};

#endif /*NODOBSTAR_H_*/
