#include "SetClaves.h"

SetClaves::~SetClaves() {
	for(SetClaves::iterator iter = this->begin(); iter != this->end(); ++iter)
		delete *iter;
}

Clave* SetClaves::findClave(Clave* clave) const {
	
	//Busco la clave mayor o igual a la que me pasan por parametro
	SetClaves::const_iterator iter = this->lower_bound(clave);
	
	if (iter == this->end()) return *(--iter);
	
	if (*(*iter) == *clave) return *iter;
	
	if (iter == this->begin()) return NULL;
	
	return *(--iter);
	
}

SetClaves* SetClaves::splitBPlus(unsigned minClaves) {
	
	//minClaves contiene la cantidad minima de claves (en bytes) que debe
	//contener cada conjunto.
	
	SetClaves* conjClavesMayores = new SetClaves();
	SetClaves::iterator iter = this->begin();
	
	for(unsigned i = 0; (iter != this->end()) && (i < minClaves); ++iter)
		i += (*iter)->getTamanioEnDisco();
	
	for (SetClaves::iterator iterMayores = iter;
		iterMayores != this->end(); ++iterMayores)
		conjClavesMayores->insert(*iterMayores);
	
	this->erase(iter, this->end());
	
	return conjClavesMayores;
}

VectorConjuntos* SetClaves::splitBStar(unsigned minClaves) {
	
	VectorConjuntos* vectorConj = new VectorConjuntos();
	SetClaves* nuevoConj;
	SetClaves::iterator iter;
	unsigned tamanioAcumulado;
	
	for (unsigned i = 0; i < 2; ++i) {
		
		tamanioAcumulado = 0;
		
		nuevoConj = new SetClaves();
		
		for (iter = this->begin();
			(iter != this->end()) && (tamanioAcumulado < minClaves);
			++iter) {
			nuevoConj->insert(*iter);
			tamanioAcumulado += (*iter)->getTamanioEnDisco();
		}
		
		vectorConj->push_back(nuevoConj);
		
		this->erase(this->begin(), iter);
		
	}
	
	return vectorConj;
	
}

