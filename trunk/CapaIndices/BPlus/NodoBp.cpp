///////////////////////////////////////////////////////////////////////////
//	Archivo   : NodoBp.cpp
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de la clase NodoBp.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel.
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#include "NodoBp.h"
#include "../Common/Codigo.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: NodoBp (Implementa nodos de Arbol B+)
//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////
	// Constructores/Destructores
	//////////////////////////////////////////////////////////////////////
	NodoBp::NodoBp(unsigned int refNodo, unsigned char nivel, unsigned short tamanio)
					: Nodo(refNodo, nivel, tamanio)
	{
		this->setTamanioMinimo((tamanio-this->getTamanioHeader())/2);		
	}	
			
	NodoBp::NodoBp(unsigned int refNodo, unsigned char nivel, Clave* clave,
			unsigned short tamanio) : Nodo(refNodo, nivel, clave, tamanio)
	{
		this->setTamanioMinimo((tamanio-this->getTamanioHeader())/2);
	} 
	
	/*
	NodoBp(unsigned int referencia) : Nodo(referencia)
	{}
	*/
			
	NodoBp::~NodoBp(){}
	
	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	////////////////////////////////////////////////////////////////////// 
	void NodoBp::insertarClave(Clave* &clave, char* codigo)
	{
		if (this->getNivel() == 0)
			*codigo = insertarEnHoja(clave);
		else
			*codigo = insertarEnNodo(clave);	
	}
	
	char NodoBp::insertarEnHoja(Clave* &clave)
	{
		char codigo = Codigo::NO_MODIFICADO;
		
		/*Insercion ordenada de la clave en la lista*/
		this->getClaves()->insert(clave);		
	
		/*Si hay espacio suficiente para la nueva clave ...*/ 
		if (this->getEspacioLibre() > clave->getTamanioEnDisco()){
			
			/*Actualizar el espacio libre del nodo en que se acaba
			 * de insertar la clave.
			 */			
			this->actualizarEspacioLibre(clave,true);
			codigo = Codigo::MODIFICADO;
		
		/*No hay espacio libre suficiente para insertar la clave...*/
		}else{			
			Nodo* nuevoNodo = new NodoBp(this->getHnoDer(), this->getNivel(), this->getTamanio());
			
			/*Condicion para overflow. Devuelve la cantidad de claves que
			 * deben quedar en el nodo que va a dividirse:
			 * la mitad exacta o, si el numero es impar, la mitad con menor 
			 * cantidad de claves.
			 */ 
			
			/*splitBPlus() devuelve un set nuevo ya creado con las
			 * claves mayores.
			 */
			//TODO Reemplazar "10" por el tamanio en bytes minimo
			//de claves en un nodo (condicion de underflow).
			SetClaves* setNuevo = this->getClaves()->splitBPlus((unsigned short)10);
			nuevoNodo->setClaves(setNuevo);
			
			/* Actualizar el espacio libre en el nodo que acaba de producirse
			 * split.
			 */
			this->actualizarEspacioLibre(setNuevo, false);
			this->actualizarEspacioLibre(clave, true);
      					   
			//TODO URGENTE VER EL TEMITA DEL ACCESO A DISCOOO !
			/*Lo grabo en el archivo*/   
			//archivoIndice->grabarNuevoNodo(nuevoNodo);
			
			/*Obtengo la primer clave de la lista del nuevo nodo*/
			/*La quito de la lista, asi no se borra cuando borro el nodo
			 *OBSERVACION: El nodo ya fue grabado, y despues de esto se destruye
			 * (no se pierde info)
			 */
			SetClaves::iterator iter = nuevoNodo->getClaves()->begin();
			clave = *iter;
			nuevoNodo->getClaves()->erase(iter); //Lo saco para que no se borre cuando borre de memoria al nodo
			clave->setHijoDer(nuevoNodo->getPosicionEnArchivo());
			
			/*Se actualiza el hermano derecho de este nodo. hijoIzq contiene
			 * una referencia al hermano derecho si el nodo es hoja.
			 */
			this->setHnoDer(nuevoNodo->getPosicionEnArchivo());
			
			/*Borrar de memoria el nuevo nodo creado*/
			delete nuevoNodo;
			
			/*Se devuelve codigo de sobreflujo*/
			codigo = Codigo::OVERFLOW;
		}
		
		//TODO ACCESO A DISCOOO !
		/*Sobreescribe el nodo, actualizando las modificaciones*/
		//archivoIndice->sobreescribirNodo(this); 
		
		return codigo;
	}
	
	char NodoBp::insertarEnNodo(Clave* &clave)
	{			
		char codigo = Codigo::NO_MODIFICADO;
		
		/*Insercion ordenada de la clave en la lista*/
		this->getClaves()->insert(clave);
		
		/*Si hay espacio suficiente para la nueva clave ...*/ 
		if (this->getEspacioLibre() > clave->getTamanioEnDisco()){
			this->actualizarEspacioLibre(clave, true);
			codigo = Codigo::MODIFICADO;
		
		/*No hay espacio libre suficiente para insertar la clave...*/
		}else{
			
			Nodo* nuevoNodo = new NodoBp(this->getRefNodo(), this->getNivel(), this->getTamanio());
			
			/*Condicion para overflow, devuelve la cantidad de claves que 
			 * deben quedar en el nodo que se va a dividir
			 * o sea la mitad y si el numero es impar la parte mas chica*/                                                        
			SetClaves* setNuevo = this->getClaves()->splitBPlus(10);
                     	
			/*se le coloca la lista proveniente de la division al nuevoNodo*/
			nuevoNodo->setClaves(setNuevo);
			
			/*se le quita la lista proveniente de la division this*/
			this->actualizarEspacioLibre(setNuevo, false);
			/*correccion debido a que la division de listas se hace luego de 
			 *efectuada la insercion
			 */
			this->actualizarEspacioLibre(clave, true);
			
			SetClaves::iterator iter = nuevoNodo->getClaves()->begin();
			clave = *iter;
			nuevoNodo->getClaves()->erase(iter); //Lo saco para que no se borre cuando borre de memoria al nodo
			
			/*El hijo izq del nuevo nodo es el hijo derecho de la clave promovida.*/
			 nuevoNodo->setHijoIzq(clave->getHijoDer());
			 
			 /*Lo grabo en el archivo*/
			 //TODO SUPERMERCADO DISCO.
			 //archivoIndice->grabarNuevoNodo(nuevoNodo);
			 clave->setHijoDer(nuevoNodo->getPosicionEnArchivo());                 
			 
			 /*Borrar de memoria el nuevo nodo creado*/
			 delete nuevoNodo;
			 codigo = Codigo::OVERFLOW;
		}               
		//TODO Y.... CRUZAAA EL DISCOOOOOOOOO!
               /*Sobreescribe el nodo, actualizando las modificaciones*/
           	  //archivoIndice-> sobreescribirNodo(this);
		
		return codigo;
	}  
	
	
	
	void NodoBp::eliminarClave(Clave* clave, char* codigo) {
		
		SetClaves* set = this->getClaves();
		SetClaves::iterator iter = set->find(clave);
		
		if (iter != set->end()) { //Se encontró la clave
			//Se libera la memoria utilizada por la clave
			delete *iter;
			//Se elimina el puntero a la clave que estaba dentro del conjunto
			set->erase(iter);
			//Se actualiza el espacio libre del nodo
			this->actualizarEspacioLibre(clave, false);
			
			/*
			if (this->espacioLibre > this->condicionMinima(archivo)){
		   		if (this->getNivel() == 0)                               
	      			*codigo = Codigo::UNDERFLOW_HOJA;
	   			else
	   				*codigo = Codigo::UNDERFLOW;
		 	}else
		 		*codigo = Codigo::MODIFICADO;
		 	*/
		}
		
		*codigo = Codigo::NO_MODIFICADO;
		
	}

	
	Nodo* NodoBp::siguiente(/*ArchivoIndice* archivo,*/Clave* clave)
	{
		Nodo* nodo;
		Clave* claveResultante = this->getClaves()->findClave(clave);
		
		if (claveResultante == NULL) {
			//Cargar un nuevo nodo en memoria a partir del hijo izquierdo
			//de este nodo.
			//nodo = new NodoBp(archivo, this->getHijoIzq());
		} else {
			//Cargar un nuevo nodo en memoria a partir del hijo derecho
			//de claveResultante.
			//nodo = new NodoBp(archivo, claveResultante->getHijoDer());
		}
		
		return nodo;
	}
