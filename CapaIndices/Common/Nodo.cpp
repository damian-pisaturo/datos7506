///////////////////////////////////////////////////////////////////////////
//	Archivo   : Nodo.cpp
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clases Nodo, NodoBp y NodoBStar.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel.
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#include "Nodo.h"


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Nodo (Abstracta)
//////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
    // Constructores/Destructor
	///////////////////////////////////////////////////////////////////////
	Nodo::Nodo(int hijoIzq,int nivel,Clave* clave)
	{
		this->hijoIzq = hijoIzq;
		this->nivel   = nivel;
		//this->espacioLibre = archivo->getTamanioNodo()- archivo->getTamanioHeader();
		/*Agrega la clave a la lista de claves del nodo*/
		this->claves = new ListaClaves; //TODO Cambiar a set STL.
		this->claves->agregar(clave);
		this->actualizarEspacioLibre(clave,true);
	}

	Nodo::Nodo (int hijoIzq,int nivel)
	{
		this->hijoIzq = hijoIzq;
	    this->nivel = nivel;
	    //this->espacioLibre = archivo->getTamanioNodo()- archivo->getTamanioHeader();
	}

	Nodo::Nodo(ArchivoIndice* archivoIndice, int referencia)
	{	
		archivoIndice->leer(referencia,this);
		//this->espacioLibre = archivoIndice->getTamanioNodo()-archivoIndice->getTamanioNodo();		
	}
	
	Nodo::~Nodo()
	{
		/*Se liberan los recursos de memoria tomados para la lista de claves*/
		//TODO actualizar a set STL.
		if(claves)
			delete this->claves;
	}
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////              
	Nodo* Nodo::siguiente(ArchivoIndice* archivo,Clave* clave)
	{
		/*buscar() debe devolver la misma clave que la buscada, si se
		 * encuentra ya en la lista, o la menor mas proxima.
		 * Si la clave a buscar es menor que la menor de la lista, el
		 * nodo a moverse es el hijo izquierdo de la menor de las claves y
		 * buscar() devuelve NULL.
		 */
        Clave* cercana = this->claves->buscar(clave);
        
        /*reveer el obtener Referencia de las claves*/ 
        Nodo* nodoEnMemoria;                	
            
        if (!cercana)
        	/*Caso de que la clave cercana sea el hijo izquierdo de la menor
        	 * de las claves dentro del nodo (hijo mas a la izquierda)
        	 */
         	nodoEnMemoria = new Nodo(archivo,this->getHijoIzq());
        else
        	nodoEnMemoria = new Nodo(archivo,cercana->obtenerReferencia());
             
        return nodoEnMemoria;
	}

	void Nodo::actualizarEspacioLibre(Clave* clave, bool insercion)
	{
		if (insercion)
			this->espacioLibre = this->espacioLibre - clave->getTamanio();
		else
		   	this->espacioLibre = this->espacioLibre + clave->getTamanio();
	}

	void Nodo::actualizarEspacioLibre(ListaClaves* claves, bool insercion)
	{
		unsigned int suma = 0;
		claves->primero();

		while (!claves->fin()){
	      suma += ((Clave*)claves->obtenerDato())->getTamanio();
	      claves->siguiente();
		}
		
		if (insercion)
			this->espacioLibre = this->espacioLibre - suma;
		else
		   	this->espacioLibre = this->espacioLibre + suma;

	}
	
	char Nodo::refPosterior(Clave* clave)
	{	
		int post;
		
		/*Busca la clave->la encuentra exacto porque esta en el nodo*/
		Clave* buscada = this->claves->buscar(clave);
	
	    if(!buscada){
	    	/*LA CLAVE BUSCADA ES MENOR A LA PRIMERA*/
	    	this->claves->primero();
	    	post = ((Clave*)this->claves->obtenerDato())->obtenerReferencia();
	    }
	    else{
	    	/*NO HAY SIGUIENTE*/
	    	this->claves->siguiente();
	  		Clave* posterior = (Clave*)this->claves->obtenerDato();
	        
	    	if(posterior)
	    		post = posterior->obtenerReferencia();
	    	else
	    		post = buscada->obtenerReferencia();
	    	    
	    }
	    return post;
	}

	char Nodo::refAnterior(Clave* clave)
	{
		int ant;
		Clave* buscada;
		Clave* anterior = NULL;
		/*Busca la clave->la encuentra exacto porque esta en el nodo*/
		buscada = this->claves->buscar(clave);
		
		if(buscada){
	    	this->claves->anterior();
	  		anterior = (Clave*)this->claves->obtenerDato();
		}
		
	    if(anterior)
	    	ant = anterior->obtenerReferencia();
	    else
	    	ant = this->hijoIzq;
	    	
	    return ant; 
    }      

	void Nodo::insertarFinal(ListaClaves* lista,ArchivoIndice* archivoIndice)
	{
		this->actualizarEspacioLibre(lista,true); 
		this->claves->insertarFinal(lista);	     
	}
	
	bool Nodo::puedeDonar(ArchivoIndice* archivo)
	{
		bool devolver = true;
		
		if(this->espacioLibre >= condicionMinima(archivo))     
	   		devolver = false;
		
		return devolver;     
	}
/*---------------------------------------------------------------------------------------*/
	unsigned int Nodo::condicionMinima(ArchivoIndice* archivo)
	{
		unsigned int tamanioReal = archivo->getTamanioNodo() - archivo->getTamanioHeader();
		
		return  (tamanioReal / 2);   
	}
/*---------------------------------------------------------------------------------------*/
/*
void Nodo::primero(){
	this->obtenerClaves()->primero();
}
*/
/*---------------------------------------------------------------------------------------*/     

	Clave* Nodo::buscar(Clave* claveBuscada)
	{
		ListaClaves* lista = this->obtenerClaves();
		Clave* devolver = lista->buscar(claveBuscada);
		
		return devolver;	
	}
/*-----------------------------------------------------------------------------------*/
/*
bool Nodo::siguiente(){
bool devolver=true ;

this->obtenerClaves()->siguiente();
if (this->obtenerClaves()->fin())
	devolver= false;

return devolver;
}
*/
/*-----------------------------------------------------------------------------------*/
/*
Clave* Nodo::obtenerClaveActual(){
 	return (Clave*)this->obtenerClaves()->obtenerDato();
}
*/
/*------------------------------------------------------------------------------------------*/
	Clave* Nodo::claveSiguiente(Clave* clave)
	{
		/*CONTEMPLA CASO EN EL CUAL EXISTA UNA UNICA CLAVE EN EL NODO EN EL CUAL SE DEBE BUSCAR LA CLAVE SIGUIENTE*/
		Clave* devolver;
		ListaClaves* lista = this->obtenerClaves();
		devolver = lista->buscar(clave);
		
		if(devolver){
			lista->siguiente();
			devolver = (Clave*)lista->obtenerDato();
		}else{
			lista->primero();
			devolver = (Clave*)lista->obtenerDato();
		}
			 
		return devolver;
	}


	bool vacio(ArchivoIndice* archivo){
		bool vacio = false;
		
		if((this->espacioLibre) == (archivo->getTamanioNodo() - archivo->getTamanioHeader()))
			vacio = true;
		
		return vacio;
	}


	Clave* Nodo::reemplazarClave(Clave* claveVieja,Clave* claveNueva,char* codigo)
	{		
		this->claves->buscar(claveVieja);
		Clave* copia = claveNueva->copiar(); 
		Clave* reemplazada = (Clave*)this->claves->reemplazar(copia);
		*codigo = Codigo::MODIFICADO;
		
	    delete reemplazada;
	    
		return copia;
	}
/*---------------------------------------------------------------------------------------*/

