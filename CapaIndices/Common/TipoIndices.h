#ifndef TIPOINDICES_H_
#define TIPOINDICES_H_

#include <list>

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: TipoIndices
//			(Clase utilitaria con constantes que designan tipos
//			de indices utilizados).
///////////////////////////////////////////////////////////////////////////

typedef list<int> ListaTipos;

class TipoIndices
{
	public:
		static const unsigned char GRIEGO	= 0;
		static const unsigned char ROMANO	= 1;
		static const unsigned char ARBOL_BS = 2;
		static const unsigned char ARBOL_BP = 3;
		static const unsigned char HASH     = 4;		
};

#endif /*TIPOINDICES_H_*/
