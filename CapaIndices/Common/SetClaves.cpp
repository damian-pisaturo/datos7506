#include "SetClaves.h"

SetClaves::~SetClaves()
{
	for(SetClaves::iterator iter; iter != this->begin(); ++iter)
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
	
	SetClaves* conjClavesMayores = new SetClaves();
	SetClaves::iterator posFinal;
	SetClaves::iterator iter = this->begin();
	
	for(unsigned i = 0; (iter != this->end()) && (i < minClaves); ++iter, ++i) {
		conjClavesMayores->insert(*iter);
		posFinal = iter;
	}
	
	this->erase(this->begin(), posFinal);
	
	return conjClavesMayores;
}

VectorConjuntos* SetClaves::splitBStar(unsigned minClaves) {
	
	VectorConjuntos* vectorConj = new VectorConjuntos();
	
	return vectorConj;
	
}

