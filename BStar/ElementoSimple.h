#ifndef ELEMENTOSIMPLE_H_
#define ELEMENTOSIMPLE_H_

#include "Elemento.h"

class ElementoSimple : public Elemento {
	
	private:
		Clave clave;
	
	public:
		ElementoSimple();
		virtual ~ElementoSimple();
		
		Clave getClave();
		void setClave(Clave clave);
		
};

#endif /*ELEMENTOSIMPLE_H_*/
