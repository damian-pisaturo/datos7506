#ifndef INDICE_H_
#define INDICE_H_

//#include "IndiceArbol.h"
//#include "IndiceHash.h"

class Indice;

#include <vector>

typedef std::vector<Indice*> VectorIndices;

class Indice
{
	public:
		Indice();
		virtual ~Indice();
};

#endif /*INDICE_H_*/
