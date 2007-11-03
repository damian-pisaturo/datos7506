///////////////////////////////////////////////////////////////////////////
//	Archivo   : NodoBPlus.cpp
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de la clase NodoBPlus.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel.
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#include "NodoBPlus.h"
#include "../Common/Codigo.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: NodoBPlus (Implementa nodos de Arbol B+)
//////////////////////////////////////////////////////////////////////////
	
//////////////////////////////////////////////////////////////////////
// Constructores/Destructores
//////////////////////////////////////////////////////////////////////
NodoBPlus::NodoBPlus(unsigned int refNodo, unsigned char nivel, unsigned short tamanio) 
				: Nodo(refNodo, nivel, tamanio)
{
	
}	
		
NodoBPlus::NodoBPlus(unsigned int refNodo, unsigned char nivel, Clave* clave,
		unsigned short tamanio) : Nodo(refNodo, nivel, clave, tamanio)
{
	
} 

/*
NodoBPlus(unsigned int referencia) : Nodo(referencia)
{}
*/


Nodo* NodoBPlus::copiar() const {
	
	NodoBPlus* nodoCopia = new NodoBPlus(this->getHijoIzq(), this->getNivel(), this->getTamanio());
	SetClaves* setClavesCopia = new SetClaves();
	*setClavesCopia = *(this->getClaves());
	nodoCopia->setClaves(setClavesCopia);
	nodoCopia->setPosicionEnArchivo(this->getPosicionEnArchivo());
	return nodoCopia;
	
}

