#ifndef INDICE_H_
#define INDICE_H_

#include "../../Common/DefinitionsManager.h"
#include <vector>
#include <map>

class Indice;

typedef std::map<DefinitionsManager::ListaNombresClaves, Indice*> MapaIndices;

typedef std::map<std::string, MapaIndices*> MapaMapaIndices;

typedef std::vector<Indice*> VectorIndices;

class Indice
{
	public:
		Indice();
		virtual ~Indice();
};

#endif /*INDICE_H_*/
