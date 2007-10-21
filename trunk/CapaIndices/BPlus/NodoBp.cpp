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
#include "Codigo.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: NodoBp (Implementa nodos de Arbol B+)
//////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	/////////////////////////////////////////////////////////////////////// 
	virtual char NodoBp::insertarClave(Clave* &clave)
	{
		char codigo = Codigo::OK;
		
		if (this->getNivel() == 0)
			codigo = insertarEnHoja(clave);
		else
			codigo = insertarEnNodo(clave);
		
		return codigo;		
	}
	
	char NodoBp::insertarEnHoja(Clave* &clave)
	{
		char codigo = Codigo::OK;
		
		/*Insercion ordenada de la clave en la lista*/
		this->claves->insertarOrdenado(clave);
		
		/*Si hay espacio suficiente para la nueva clave ...*/ 
		if (this->espacioLibre > clave->getTamanio()){
			
			/*Actualizar el espacio libre del nodo en que se acaba
			 * de insertar la clave.
			 */			
			this->actualizarEspacioLibre(clave,true);
			codigo = Codigo::MODIFICADO;
		
		/*No hay espacio libre suficiente para insertar la clave...*/
		}else{
			
			Nodo* nuevoNodo = new NodopBp(this->hijoIzq,this->nivel);
			
			/*Condición para overflow, devuelve la cantidad de claves que
			 * deben quedar en el nodo que va a dividirse:
			 * la mitad exacta o, si el numero es impar, la mitad con menor 
			 * cantidad de claves.*/ 
			unsigned int division = this->claves->getCantidadNodos()/2; 
			
			/*dividir() devuelve una lista nueva ya creada (mitad mas grande)*/
			ListaClaves* listaNueva = this->claves->dividir(division);
			nuevoNodo->claves = listaNueva;
			
			/* Actualizar el espacio libre en el nodo que acaba de producirse
			 * split.
			 */
			this->actualizarEspacioLibre(listaNueva,false);
			this->actualizarEspacioLibre(clave,true);
      					   
			//TODO URGENTE VER EL TEMITA DEL ACCESO A DISCOOO !
			/*Lo grabo en el archivo*/   
			//archivoIndice->grabarNuevoNodo(nuevoNodo);
			
			listaNueva->primero();
			/*Obtengo la primer clave de la lista del nuevo nodo*/
			/*La quito de la lista, asi no se borra cuando borro el nodo
			 *OBSERVACION: El nodo ya fue grabado, y despues de esto se destruye
			 * (no se pierde info)
			 */
			clave = (Clave*)listaNueva->quitar(1);
			clave->setReferencia(nuevoNodo->obtenerPosicionEnArchivo());
			
			/*Actualizo el Hijo Izquierdo de este nodo*/
			this->setHijoIzq(nuevoNodo->obtenerPosicionEnArchivo());
			
			/*Borrar de memoria el nuevo nodo creado*/
			delete nuevoNodo;
			
			/*Se devuelve codigo de sobreflujo*/
			codigo = Codigo::OVERFLOW;
		}
		
		//TODO ACCESO A DISCOOO !
		/*Sobreescribe el nodo, actualizando las modificaciones*/
		//archivoIndice-> sobreescribirNodo(this); 
		
		return codigo;
	}
	
	char NodoBp::insertarEnNodo(Clave* &clave)
	{			
		char codigo = Codigo::OK;
		
		/*Insercion ordenada de la clave en la lista*/
		this->claves->insertarOrdenado(clave);
		
		/*Si hay espacio suficiente para la nueva clave ...*/ 
		if (this->espacioLibre> clave->getTamanio()){
			this->actualizarEspacioLibre(clave,true);
			codigo = Codigo::MODIFICADO;
		
		/*No hay espacio libre suficiente para insertar la clave...*/
		}else{
			
			Nodo* nuevoNodo = new Nodo(this->hijoIzq,this->getNivel());
			
			/*Condición para overflow, devuelve la cantidad de claves que 
			 * deben quedar en el nodo que se va a dividir
			 * o sea la mitad y si el numero es impar la parte mas chica*/                                                        
			unsigned int division = this->claves->getCantidadNodos()/2;
			ListaClaves* listaNueva = this->claves->dividir(division);
                     	
			/*se le coloca la lista proveniente de la division al nuevoNodo*/
			nuevoNodo->claves = listaNueva;
			
			/*se le quita la lista proveniente de la division this*/
			this->actualizarEspacioLibre(listaNueva,false);
			/*corrección debido a que la division de listas se hace luego de 
			 *efectuada la inserción
			 */
			this->actualizarEspacioLibre(clave,true);
                         
                         /*El quitar quita de la lista y devuelve un puntero a la clave quitada 
                          * para que se devuelva al salir*/
                         clave = (Clave*)nuevoNodo->claves->quitar(1);//COOREGIR LUEGO, EL ACUTALIZAR SE DEBE LLAMAR DENTRO DEL QUITAR
                                                  
                         /*El hijo izq del nuevo nodo es la referencia de la clave promovida*/
                         nuevoNodo->hijoIzq = clave->obtenerReferencia();
                         
                         /*Lo grabo en el archivo*/
                         archivoIndice->grabarNuevoNodo(nuevoNodo);
                                                                                            
                         clave->setReferencia(nuevoNodo->posicionEnArchivo);                 
                                                   
                        /*Borrar de memoria el nuevo nodo creado*/
                     	delete nuevoNodo;
                     	
                     	codigo = Codigo::OVERFLOW;
                            
               }
               
		//TODO Y.... CRUZAAA EL DISCOOOOOOOOO!
               /*Sobreescribe el nodo, actualizando las modificaciones*/
           	  //archivoIndice-> sobreescribirNodo(this);      

       
}     

void Nodo::quitarClave(ArchivoIndice* archivo,Clave* clave,Codigo* codigo){

	ListaClaves* lista = this->obtenerClaves();
	
	this->actualizarEspacioLibre(clave,false);
	
	lista->quitarClave(clave);
	
	if (this->espacioLibre > (this->condicionMinima(archivo))){
   		if (this->getNivel()==0)                               
      		codigo->setValor("SUBFLOWenHOJA");
   		else
      		codigo->setValor("SUBFLOW");
 	}
 	else
 		codigo->setValor("NODO MODIFICADO");
	
}