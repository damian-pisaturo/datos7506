#include "SetClaves.h"

SetClaves::~SetClaves() {
	for(SetClaves::iterator iter = this->begin(); iter != this->end(); ++iter)
		if (*iter) delete *iter;
}

Clave* SetClaves::findClave(Clave* clave) const {
	
	if (this->empty()) return NULL;

	//Busco la clave mayor o igual a la que me pasan por parametro
	SetClaves::const_iterator iter = this->lower_bound(clave);

	if (iter == this->end()) return *(--iter);
	
	if (*(*iter) == *clave)	return *iter;
	
	if (iter == this->begin()) return NULL;
	
	return *(--iter);
	
}

SetClaves* SetClaves::splitBPlus(unsigned short minClaves, unsigned short maxClaves, bool bstar) {
	
	//minClaves contiene la cantidad minima de claves (en bytes) que debe
	//contener cada conjunto.
	
	SetClaves* conjClavesMayores = new SetClaves();
	SetClaves::iterator iter = this->begin();
	
	unsigned short i;
	
	for(i = 0; (iter != this->end()) && (i < minClaves); ++iter)
		i += (*iter)->getTamanioEnDisco(bstar);
	
	if (i > maxClaves) --iter;
	
	for (SetClaves::iterator iterMayores = iter;
		iterMayores != this->end(); ++iterMayores)
		conjClavesMayores->insert(*iterMayores);
	
	this->erase(iter, this->end());
	
	return conjClavesMayores;
}

VectorConjuntos* SetClaves::splitBStar(unsigned short minClaves, unsigned short maxClaves) {
	
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
			tamanioAcumulado += (*iter)->getTamanioEnDisco(true);
			if (tamanioAcumulado <= maxClaves)
				nuevoConj->insert(*iter);
		}
		
		if (iter != this->end()) nuevoConj->insert(*iter);
		
		vectorConj->push_back(nuevoConj);
		
		this->erase(this->begin(), ++iter);
		
	}
	
	if (this->size() == 0) {
		if (((*vectorConj)[0])->size() > 1){
			iter = ((*vectorConj)[1])->end();
			this->insert(*(--iter));
			((*vectorConj)[1])->erase(iter);
		}
		else{
			iter = ((*vectorConj)[0])->end();
			((*vectorConj)[1])->insert(*(--iter));
			((*vectorConj)[0])->erase(iter);
			
			iter = ((*vectorConj)[1])->end();
			this->insert(*(--iter));
			((*vectorConj)[1])->erase(iter);
		}
	}
	
	return vectorConj;
	
}


void SetClaves::merge(SetClaves* set, Clave* c1, Clave* c2) {
	
	if (c1) this->insert(c1);
	if (c2) this->insert(c2);
	
	for (SetClaves::iterator iter = set->begin(); iter != set->end(); ++iter)
		this->insert(*iter);
	
	//Borro los punteros dentro del set para que no libere la memoria de las claves
	//al destruirse.
	set->clear();
	
}

void SetClaves::merge(SetClaves* set1, SetClaves* set2, Clave* c1, Clave* c2) {
	
	this->merge(set1, c1, c2);
	
	for (SetClaves::iterator iter = set2->begin(); iter != set2->end(); ++iter)
		this->insert(*iter);
	
	//Borro los punteros dentro del set para que no libere la memoria de las claves
	//al destruirse.
	set2->clear();
	
}


SetClaves& SetClaves::operator = (const SetClaves& setClaves) {
	
	this->clear();
	
	for (SetClaves::const_iterator iter = setClaves.begin(); iter != setClaves.end(); ++iter)
		this->insert((*iter)->copiar());
	
	return *this;
	
}
	
