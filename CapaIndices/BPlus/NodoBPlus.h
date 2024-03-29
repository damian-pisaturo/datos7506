///////////////////////////////////////////////////////////////////////////
//	Archivo   : NodoBPlus.h
//  Namespace : CapaIndice
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de la clase NodoBPlus.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura
///////////////////////////////////////////////////////////////////////////
#ifndef NODOBPLUS_H_
#define NODOBPLUS_H_

#include "../Common/Nodo.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: NodoBPlus (Implementa nodos de Arbol B+)
///////////////////////////////////////////////////////////////////////////
class NodoBPlus: public Nodo
{
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Contructores/Destructores
	//////////////////////////////////////////////////////////////////////
		
		/*Crea el nodo con los atributos dados*/
				
		NodoBPlus(unsigned int refNodo, unsigned char nivel, unsigned short tamanio);
		
		/*Crea un nuevo nodo para insertarle una Clave*/
		NodoBPlus(unsigned int refNodo, unsigned char nivel, Clave* clave,
			   unsigned short tamanio);
			
		/*Destructor*/
		virtual ~NodoBPlus() {}
		
		
		
	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////
		
		Nodo* copiar() const;
		
		bool puedePasarClaveHaciaIzq(Nodo* nodoHnoIzq, Nodo* nodoPadre, Clave* clavePadre) const;
				
		bool puedePasarClaveHaciaDer(Nodo* nodoHnoDer, Nodo* nodoPadre, Clave* clavePadre) const;
		
		//Devuelve el espacio (en bytes) destinado para almacenar claves (elementos)
		unsigned short getTamanioEspacioClaves() const {
			return (this->getTamanio() - Nodo::getTamanioHeader());
		}
		
		unsigned short getTamanioMinimo() const {
			return ((this->getTamanioEspacioClaves())/2);
		}		
		
}; //Fin clase NodoBPlus

#endif /*NODOBPLUS_H_*/
