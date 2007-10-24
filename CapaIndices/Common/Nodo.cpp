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
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#include "Nodo.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: Nodo (Abstracta)
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
    // Constructores/Destructor
	///////////////////////////////////////////////////////////////////////
	Nodo::Nodo(unsigned int refNodo, unsigned int nivel, Clave* clave, unsigned int condicionMinima)
	{
		this->refNodo = refNodo;
		this->nivel   = nivel;
		this->condicionMinima = condicionMinima;
		//this->espacioLibre = archivo->getTamanioNodo()- archivo->getTamanioHeader();
		
		/*Agrega la clave a la lista de claves del nodo*/
		this->claves = new SetClaves();
		this->claves->insert(clave);
		
		
		this->actualizarEspacioLibre(clave,true);
	}

	Nodo::Nodo(unsigned int refNodo, unsigned int nivel, unsigned int condicionMinima)
	{
		this->refNodo = refNodo;
	    this->nivel = nivel;
	    this->condicionMinima = condicionMinima;
	    //this->espacioLibre = archivo->getTamanioNodo()- archivo->getTamanioHeader();
	    
	    /*Agrega la clave a la lista de claves del nodo*/
		this->claves = new SetClaves();
	}
	
	/*
	Nodo::Nodo(ArchivoIndice* archivoIndice, int referencia)
	{	
		archivoIndice->leer(referencia,this);
		//this->espacioLibre = archivoIndice->getTamanioNodo()-archivoIndice->getTamanioNodo();		
	}
	*/
	
	Nodo::~Nodo()
	{
		if(claves)
			delete this->claves;
	}
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////              
	
	void Nodo::actualizarEspacioLibre(Clave* clave, bool insercion)
	{
		if (insercion)
			this->setEspacioLibre(this->getEspacioLibre() - clave->getTamanioEnDisco());
		else
		   	this->setEspacioLibre(this->getEspacioLibre() + clave->getTamanioEnDisco());
	}

	void Nodo::actualizarEspacioLibre(SetClaves* claves, bool insercion)
	{
		unsigned int suma = 0;
		
		for (SetClaves::iterator iter = claves->begin(); iter != claves->end(); ++iter){
			suma += (*iter)->getTamanioEnDisco();			
		}
		
		if (insercion)
			this->setEspacioLibre(this->getEspacioLibre() - suma);
		else
			this->setEspacioLibre(this->getEspacioLibre() + suma);
	}
	
	/*
	bool Nodo::puedeDonar(ArchivoIndice* archivo)
	{
		bool devolver = true;
		
		if(this->espacioLibre >= condicionMinima(archivo))     
	   		devolver = false;
		
		return devolver;     
	}
	*/
	
	/*
	unsigned int Nodo::condicionMinima(ArchivoIndice* archivo)
	{
		unsigned int tamanioReal = archivo->getTamanioNodo() - archivo->getTamanioHeader();
		
		return  (tamanioReal / 2);   
	}
	*/

	Clave* Nodo::buscar(Clave* claveBuscada) const
	{
		return this->getClaves()->findClave(claveBuscada);
	}

	/*
	bool vacio(ArchivoIndice* archivo)
	{
		bool vacio = false;
		
		if((this->espacioLibre) == (archivo->getTamanioNodo() - archivo->getTamanioHeader()))
			vacio = true;
		
		return vacio;
	}
	*/
	
	/*
	Clave* Nodo::reemplazarClave(Clave* claveVieja, Clave* claveNueva, char* codigo)
	{		
		this->claves->findClave(claveVieja);
		Clave* copia = claveNueva->copiar();
		Clave* reemplazada = (Clave*)this->claves->reemplazar(copia);
		*codigo = Codigo::MODIFICADO;
		
	    delete reemplazada;
	    
		return copia;
	}
	*/

	
