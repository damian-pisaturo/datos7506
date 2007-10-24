#ifndef NODOBSTAR_H_
#define NODOBSTAR_H_

#include "../Common/Nodo.h"

class NodoBStar : public Nodo {
	
	public:
		NodoBStar(unsigned int refNodo, unsigned int nivel, unsigned int condicionMinima)
				 : Nodo(refNodo, nivel, condicionMinima) {}
		
		NodoBStar(unsigned int refNodo, unsigned int nivel, Clave* clave,
				  unsigned int condicionMinima)
				  : Nodo(refNodo, nivel, clave, condicionMinima) {}
		
		virtual ~NodoBStar() {}
	
		void insertarClave(Clave* &clave, char* codigo);
		
		void eliminarClave(Clave* clave, char* codigo);
		
		Nodo* siguiente(Clave* clave);
		
		unsigned getTamanioEnDisco() const;
		
		unsigned getTamanioEnDiscoSetClaves() const;
		
		//Método que verifica si un nodo puede ceder 'bytesRequeridos' a otro nodo.
		//Si 'izquierda' es true, indica que el nodo debe ceder al hermano hizquierdo,
		//en caso contrario, debe ceder al hermano derecho.
		//Si puede ceder, devuelve la cantidad de bytes a ceder ( >= bytesRequeridos),
		//sino devuelve 0;
		unsigned puedeCeder(unsigned bytesRequeridos, bool izquierda = true) const;
		
		bool puedeRecibir(unsigned bytesPropuestos) const;
		
		unsigned obtenerBytesRequeridos() const;
		
		SetClaves* ceder(unsigned bytesRequeridos, bool izquierda);
		
		void recibir(SetClaves* set);
		
		bool esPadre(const NodoBStar* hijo, Clave* &clave) const;
		
		//Retorna un puntero la primera clave o NULL si el nodo no tiene claves.
		Clave* obtenerPrimeraClave() const;

};

#endif /*NODOBSTAR_H_*/
