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
				: Nodo(refNodo, nivel, tamanio, false)
{

}	

NodoBPlus::NodoBPlus(unsigned int refNodo, unsigned char nivel, Clave* clave,
		unsigned short tamanio) : Nodo(refNodo, nivel, clave, tamanio, false)
{ 

}


Nodo* NodoBPlus::copiar() const {
	
	NodoBPlus* nodoCopia = new NodoBPlus(this->getHijoIzq(), this->getNivel(), this->getTamanio());
	SetClaves* setClavesCopia = new SetClaves();
	*setClavesCopia = *(this->getClaves());
	nodoCopia->setClaves(setClavesCopia);
	nodoCopia->setPosicionEnArchivo(this->getPosicionEnArchivo());
	return nodoCopia;
	
}


bool NodoBPlus::puedePasarClaveHaciaIzq(Nodo* nodoHnoIzq, Nodo* nodoPadre, Clave* clavePadre) const {
	
	if (this->getNivel() == 0) {

		//Bytes requeridos por el hermano izquierdo
		unsigned short bytesRequeridos = nodoHnoIzq->obtenerBytesRequeridos();
		unsigned short bytesPropuestosPorMi = 0;
		unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco(false);
		unsigned char clavesPropuestas = 0;
		
		bytesPropuestosPorMi = this->bytesACeder(bytesRequeridos, clavesPropuestas);
		
		if ( (bytesPropuestosPorMi > 0) && (nodoHnoIzq->puedeRecibir(bytesPropuestosPorMi, 0)) ) {
			
			unsigned char i = 0;
			SetClaves::iterator iter;
			for (iter = this->getClaves()->begin(); i < clavesPropuestas; ++iter, ++i);
			//*iter es la clave que deberia quedar como separador
			return nodoPadre->puedeRecibir((*iter)->getTamanioEnDisco(false), tamanioClavePadre);
			
		}
		
		return false;
		
	} else return Nodo::puedePasarClaveHaciaIzq(nodoHnoIzq, nodoPadre, clavePadre);
	
}


bool NodoBPlus::puedePasarClaveHaciaDer(Nodo* nodoHnoDer, Nodo* nodoPadre, Clave* clavePadre) const {

	if (this->getNivel() == 0) {

		//Bytes requeridos por el hermano izquierdo
		unsigned short bytesRequeridos = nodoHnoDer->obtenerBytesRequeridos();
		unsigned short bytesPropuestosPorMi = 0;
		unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco(false);
		unsigned char clavesPropuestas = 0;
		
		bytesPropuestosPorMi = this->bytesACeder(bytesRequeridos, clavesPropuestas, false);
		
		if ( (bytesPropuestosPorMi > 0) && (nodoHnoDer->puedeRecibir(bytesPropuestosPorMi, 0)) ) {
			
			unsigned char i = 0;
			SetClaves::reverse_iterator iter;
			for (iter = this->getClaves()->rbegin(); i < (clavesPropuestas-1); ++iter, ++i);
			//*iter es la clave que deberia quedar como separador
			return nodoPadre->puedeRecibir((*iter)->getTamanioEnDisco(false), tamanioClavePadre);
			
		}
		
		return false;
		
	} else return Nodo::puedePasarClaveHaciaDer(nodoHnoDer, nodoPadre, clavePadre);
	
}

