#include "Nodo.h"


/*CONSTRUCTORES
---------------*/

Nodo::Nodo(int hijoIzq,int nivel,Clave* clave,ArchivoIndice* archivo){
	this->hijoIzq = hijoIzq;
	this->nivel = nivel;
	this->espacioLibre = archivo->getTamanioNodo()- archivo->getTamanioHeader();
	/*Agrega la clave a la lista de claves del nodo*/
	this->claves = new ListaClaves;
	this->claves->agregar(clave);
	this->actualizarEspacioLibre(clave,true);
	;
}
/*--------------------------------------------------------------------------------------*/
Nodo::Nodo (int hijoIzq,int nivel,ArchivoIndice* archivo){
	this->hijoIzq = hijoIzq;
    this->nivel = nivel;
    this->espacioLibre = archivo->getTamanioNodo()- archivo->getTamanioHeader();
}
/*--------------------------------------------------------------------------------------*/
Nodo::Nodo(ArchivoIndice* archivoIndice,int referencia){
	
	archivoIndice->leer(referencia,this);
	//this->espacioLibre = archivoIndice->getTamanioNodo()-archivoIndice->getTamanioNodo();
	
}
              
/*-----------------------------------------------------------------------------*/
void Nodo::insertarEnHoja(ArchivoIndice* archivoIndice,Clave* &clave,Codigo* codigo){
       	   
           if (this->getNivel()==0){  
             
                /*INSERCION*/
                this->claves->insertarOrdenado(clave);
                 
                /*Si hay espacio suficiente para la nueva clave ...*/ 
                if (this->espacioLibre > clave->getTamanio()){
                                       
                          codigo->setValor("NODO MODIFICADO");     
                          this->actualizarEspacioLibre(clave,true); 		           
                
                }/*sino....*/
                else {
                           codigo->setValor("OVERFLOW");
                     
                           Nodo* nuevoNodo = new Nodo(this->hijoIzq,this->nivel,archivoIndice);
               
                     
                           /*Condici�n para overflow, devuelve la cantidad de claves que 
							* deben quedar en el nodo que se va a dividir
							* o sea la mitad y si el numero es impar la parte mas chica*/ 
                           int division = this->claves->getCantidadNodos() /2; 
                           
                           /*El dividir devuelve una lista nueva ya creada*/
                           ListaClaves* listaNueva = this->claves->dividir(division);   /*listaNueva va a ser la parte mas grande*/                     
                           nuevoNodo->claves = listaNueva;
                                                    
      					   this->actualizarEspacioLibre(listaNueva,false);
      					   this->actualizarEspacioLibre(clave,true);
      					   
                           /*Lo grabo en el archivo*/   
                           archivoIndice->grabarNuevoNodo(nuevoNodo);
                     
                           listaNueva->primero();
                           
                           /*Obtengo la primer clave de la lista del nuevo nodo*/
                           /*La quito de la lista, asi no se borra cuando borro el nodo
                            * OBSERVACION: El nodo ya fue grabado, y despues de esto se destruye
                            * (no se pierde info)*/
                           clave = (Clave*)listaNueva->quitar(1);
                           
                           clave->setReferencia(nuevoNodo->obtenerPosicionEnArchivo());
                           
                           /*Actualizpo el Hijo Izquierdo de estes nodo*/
                           this->setHijoIzq(nuevoNodo->obtenerPosicionEnArchivo());
          
                           /*Borrar de memoria el nuevo nodo creado*/
                           delete nuevoNodo;
                      } 
           
                     
           /*Sobreescribe el nodo, actualizando las modificaciones*/
           archivoIndice-> sobreescribirNodo(this);          
       
       } /*fin if (this->esHoja())*/
}              
/*----------------------------------------------------------------------------------*/
void Nodo::insertarEnNodo(ArchivoIndice* archivoIndice,Clave* &clave,Codigo* codigo){			
         /*INSERCION*/               
         this->claves->insertarOrdenado(clave);
                       
         if (this->espacioLibre> clave->getTamanio()){
                           
                    codigo->setValor("NODO MODIFICADO");    
                    this->actualizarEspacioLibre(clave,true); 
  
         	/*sino....*/
          }  
           else {
                         codigo->setValor("OVERFLOW");
							
                         Nodo* nuevoNodo = new Nodo(this->hijoIzq,this->getNivel(),archivoIndice);
                         
                         /*Condici�n para overflow, devuelve la cantidad de claves que 
							* deben quedar en el nodo que se va a dividir
							* o sea la mitad y si el numero es impar la parte mas chica*/                                                        
                         int division = this->claves->getCantidadNodos() /2;
                         
                         ListaClaves* listaNueva = this->claves->dividir(division);   /*listaNueva va a ser la parte mas grande*/
                     	
                     	 /*se le coloca la lista proveniente de la division al nuevoNodo*/
                         nuevoNodo->claves = listaNueva;
                         
                         /*se le quita la lista proveniente de la division this*/
                         this->actualizarEspacioLibre(listaNueva,false);
                         /*correcci�n debido a que la division de listas se hace luego de 
                          * efectuada la inserci�n*/
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
                            
               }
               
               /*Sobreescribe el nodo, actualizando las modificaciones*/
           	  archivoIndice-> sobreescribirNodo(this);      

       
}             
/*----------------------------------------------------------------------------------*/               
Nodo* Nodo::siguiente(ArchivoIndice* archivo,Clave* clave){

		/*Te tiene que devolver la mas cercana (menos o mayor) o la misma*/
		/*si tiene que ir al hijo izquierdo la clave cercana es NULL*/
        Clave* cercana = this->claves->buscar(clave);
        
        /*reveer el obtener Referencia de las claves*/ 
        Nodo* nodoEnMemoria;                	
            
        if (cercana==NULL)
        	/*caso de que la clave cercana sea el hijo izquierdo del nodo*/
         	nodoEnMemoria = new Nodo(archivo,this->getHijoIzq());
        else
        	nodoEnMemoria = new Nodo(archivo,cercana->obtenerReferencia());
             
        return nodoEnMemoria;
}
/*-----------------------------------------------------------------------------------*/
void Nodo::actualizarEspacioLibre(Clave* clave, bool insercion){
/*La clave es la que inserte o elimin�, el espacio libre es uno desde la creaci�n del nono
no necesito pasar siempre como par�metro el tamanio del mismo*/

if (insercion)
	this->espacioLibre = this->espacioLibre - clave->getTamanio();
else
   	this->espacioLibre = this->espacioLibre + clave->getTamanio();
}
/*-----------------------------------------------------------------------------------*/
void Nodo::actualizarEspacioLibre(ListaClaves* claves, bool insercion){
int suma = 0;
claves->primero();

while (!claves->fin()){
      suma = suma + ((Clave*)claves->obtenerDato())->getTamanio();
      claves->siguiente();
}
if (insercion)
	this->espacioLibre = this->espacioLibre - suma;
else
   	this->espacioLibre = this->espacioLibre + suma;

}
/*----------------------------------------------------------------------------------*/
void Nodo::actualizarEspacioLibre(ArchivoIndice* archivo){
int tamanioOcupado=0;
int tamanioTotal;

tamanioTotal = archivo->getTamanioNodo() - archivo->getTamanioHeader();
	
ListaClaves* lista = this->obtenerClaves();
lista->primero();
while(!lista->fin()){
	tamanioOcupado+=((Clave*)lista->obtenerDato())->getTamanio();
	lista->siguiente();
}

this->espacioLibre = tamanioTotal - tamanioOcupado;
}
/*----------------------------------------------------------------------------------*/
int Nodo::obtenerPosicionEnArchivo(){
     return this->posicionEnArchivo;
     }
/*------------------------------------------------------------------------------------*/
void Nodo::setHijoIzq(int hijo){
     this->hijoIzq = hijo;
     }
/*-----------------------------------------------------------------------------------*/
void Nodo::setNivel(int nivel){
     this->nivel = nivel;
     }
/*------------------------------------------------------------------------------------*/
void Nodo::setClaves(ListaClaves* lista){
     this->claves = lista;
     }
/*------------------------------------------------------------------------------------*/
void Nodo::setEspacioLibre(int cant){
	this->espacioLibre = cant;	
}
/*------------------------------------------------------------------------------------*/
int Nodo::getEspacioLibre(){
	return this->espacioLibre;	
}
/*------------------------------------------------------------------------------------*/
void Nodo::setPosicionEnArchivo(int posicion){
	this->posicionEnArchivo = posicion;	
}
/*--------------------------------------------------------------------------------------*/
int Nodo::getNivel(){
     return this->nivel;
     }
/*---------------------------------------------------------------------------------------*/
ListaClaves* Nodo::obtenerClaves(){
     return this->claves;        
     }

/*------------------------------------------------------------------------------------*/
int Nodo::getHijoIzq(){
     return this->hijoIzq;
     }

/*-------------------------------------------------------------------------------------*/
int Nodo::refPosterior(Clave* clave){
	
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
/*------------------------------------------------------------------------------------------*/
int Nodo::refAnterior(Clave* clave){
	
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
/*---------------------------------------------------------------------------------------------------*/
void Nodo::insertarFinal(ListaClaves* lista,ArchivoIndice* archivoIndice){
/*Llamar al insertarFinal de la lista,coomo no esta hecho uso 
insertar ordenado mientras, pero es mas ineficiente*/
	
	this->actualizarEspacioLibre(lista,true); 
	this->claves->insertarFinal(lista);    
     
}

/*---------------------------------------------------------------------------------------*/
bool Nodo::puedeDonar(ArchivoIndice* archivo){
	bool devolver = true;
	if(this->espacioLibre >= condicionMinima(archivo))     
   		devolver = false;
	return devolver;     
}
/*---------------------------------------------------------------------------------------*/
int Nodo::condicionMinima(ArchivoIndice* archivo){
	int tamanioReal = archivo->getTamanioNodo() - archivo->getTamanioHeader();
	return  (tamanioReal / 2);   
}
/*---------------------------------------------------------------------------------------*/
void Nodo::primero(){
	this->obtenerClaves()->primero();
}
/*---------------------------------------------------------------------------------------*/     
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
/*---------------------------------------------------------------------------------------*/
Clave* Nodo::buscar(Clave* claveBuscada){
       
ListaClaves* lista = this->obtenerClaves();
Clave* devolver = lista->buscar(claveBuscada);

return devolver;

}
/*-----------------------------------------------------------------------------------*/
bool Nodo::siguiente(){
bool devolver=true ;

this->obtenerClaves()->siguiente();
if (this->obtenerClaves()->fin())
	devolver= false;

return devolver;
}
/*-----------------------------------------------------------------------------------*/
Clave* Nodo::obtenerClaveActual(){
 	return (Clave*)this->obtenerClaves()->obtenerDato();
}
/*------------------------------------------------------------------------------------------*/
Clave* Nodo::claveSiguiente(Clave* clave){
/*CONTEMPLA CASO EN EL CUAL EXISTA UNA UNICA CLAVE EN EL NODO EN EL CUAL SE DEBE BUSCAR LA CLAVE SIGUIENTE*/
	Clave* devolver;
	ListaClaves* lista = this->obtenerClaves();
	devolver = lista->buscar(clave);
	if(devolver){
		lista->siguiente();
		devolver = (Clave*)lista->obtenerDato();
	}
	else{
		lista->primero();
		devolver = (Clave*)lista->obtenerDato();
	}
		 
	return devolver;
}
/*-----------------------------------------------------------------------------------*/
bool Nodo::vacio(ArchivoIndice* archivo){
	bool vacio = false;
	if((this->espacioLibre) == (archivo->getTamanioNodo() - ArchivoIndice::getTamanioHeader()))
		vacio = true;
	
	return vacio;
}
/*-----------------------------------------------------------------------------------------*/
Clave* Nodo::reemplazarClave(Clave* claveVieja,Clave* claveNueva,Codigo* codigo){
	/*Reemplaza la claveVieja del Nodo actual con una copia de la clave nueva*/
	this->claves->buscar(claveVieja);
	Clave* copia = claveNueva->copiar(); 
	Clave* reemplazada = (Clave*)this->claves->reemplazar(copia);
	codigo->setValor("NODO MODIFICADO");

    delete reemplazada;
    
	return copia;
}
/*---------------------------------------------------------------------------------------*/
Nodo::~Nodo(){
/*Se Liberan los recursos de Memoria tomados para la lista de claves*/ 
     delete	this->claves;
}   
