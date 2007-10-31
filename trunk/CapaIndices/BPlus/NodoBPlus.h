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
	private:		
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		void insertarEnHoja(Clave* &clave, char* codigo);
		void insertarEnNodo(Clave* &clave, char* codigo);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Contructores/Destructores
	//////////////////////////////////////////////////////////////////////
		
		/*Crea el nodo sin setear ningun atributo*/
		NodoBPlus(unsigned int refNodo, unsigned char nivel, unsigned short tamanio);
			
		/*Crea un nuevo nodo para insertarle una Clave*/
		NodoBPlus(unsigned int refNodo, unsigned char nivel, Clave* clave,
			   unsigned short tamanio);
			
		/*Lee el archivo y crea ese nodo*/
		//NodoBPlus(unsigned int referencia);
			
		/*Destructor*/
		virtual ~NodoBPlus() {}
			
	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////
		void insertarClave(Clave* &clave, char* codigo);	
		void eliminarClave(Clave* clave, char* codigo);
		Nodo* siguiente(Clave* clave);
		
		//Devuelve el espacio (en bytes) destinado para almacenar claves (elementos)
		unsigned short getTamanioEspacioClaves() const {
			return (this->getTamanio() - Nodo::getTamanioHeader());
		}
		
		unsigned short getTamanioMinimo() const {
			return ((this->getTamanioEspacioClaves())/2);
		}		
		
}; //Fin clase NodoBPlus

#endif /*NODOBPLUS_H_*/
