///////////////////////////////////////////////////////////////////////////
//	Archivo   : BpTree.cpp
//  Namespace : CapaIndice
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de la clase BpTree.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#include "BpTree.h"


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: BpTree (Implementa Arboles B+ en disco)
///////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////
	// Contructor/Destructor
	//////////////////////////////////////////////////////////////////////
	BpTree::BpTree(/*ArchivoIndice* archivo*/){
		this->nodoActual = NULL;
		//this->archivoIndice = archivo;
	}

	NodoBp* BpTree::getRaiz()
	{
		/*Lee el primer registro del archivo -> la raiz*/
		return new NodoBp(/*archivoIndice*/,0);
	}

	void BpTree::primero()
	{
		/*El nodo actual termina siendo el primer nodo hoja del arbol B+
		     (para recorridos)*/	
		this->nodoActual = this->getRaiz();
		int auxiliar;
		/*Busco hasta la hoja*/
		while(this->nodoActual->getNivel()!=0){
			auxiliar =  this->nodoActual->getHijoIzq();
			delete this->nodoActual;
			this->nodoActual = new Nodo(this->archivoIndice,auxiliar);
		}
	
		/*me posiciono en la primer clave del primer nodo si el arbol no esta vacio*/
		if(!nodoActual->vacio(this->archivoIndice))
			this->nodoActual->primero();
		else{
			delete nodoActual;
			nodoActual = NULL;
		}
	}
	/*
	Clave* BpTree::siguiente()
	{	       
		Clave* devolver = NULL;
		int aux;
		bool siguiente;
	
		if (this->nodoActual){
			devolver = (Clave*) this->nodoActual->obtenerClaveActual()->copiar();	
			aux = this->nodoActual->getHijoIzq();		
			siguiente = this->nodoActual->siguiente();
		
			if (!siguiente){
				if (aux!=0){
					delete this->nodoActual;	
					this->nodoActual = new Nodo(this->archivoIndice,aux);
					this->nodoActual->primero();
	 			}
	 			else{
	 				delete this->nodoActual;
	 				this->nodoActual = NULL;
	 			}
			}
	  	
		}else
			devolver = NULL;
		
		return devolver;
	
	}
	*/

	void BpTree::insertarClave(Clave* clave)
	{
		char codigo = Codigo::NO_MODIFICADO;
		NodoBp* anterior = NULL;
		
		/*Busco la raIz para comenzar el recorrido de InserciOn*/
		this->nodoActual = this->getRaiz();
		
		/*Se devuelve el codigo y la clave correspondiente*/
		insertarInterno(this->nodoActual,clave,&codigo,anterior);
		
		if (codigo == Codigo::OVERFLOW){
			/*Se graba la raiz en un nuevoNodo en el archivo,
			 *se setea en nuevoNodo su posicionEnArchivo al grabarlo
			 */
			this->archivoIndice->grabarNuevoNodo(this->nodoActual);
			
			/*Sobreescribo la raiz con la clave que viene del recursivo*/
			NodoBp* nuevaRaiz = new Nodo(nodoActual->obtenerPosicionEnArchivo(),nodoActual->getNivel()+1,clave,/*this->archivoIndice*/);
			nuevaRaiz->setPosicionEnArchivo(0);
			
			//this->archivoIndice->sobreescribirNodo(nuevaRaiz);
			
			delete this->nodoActual;
			this->nodoActual = nuevaRaiz;
		}
		
		delete this->nodoActual;				
	}
	
	void BpTree::insertarInterno(NodoBp* actual, Clave* &clave, char* codigo, NodoBp* &anterior)
	{
		/*Si se trata de una  hoja inserto el registro donde corresponda*/
		if (actual->getNivel() == 0){
			/*Clave devuelve un puntero a la clave que obtuvo overflow en caso de haberlo habido, sino null*/
			/*si devuelve como valor de codigo "nodo modificado" no se sigue el ciclo*/
			
            /*Inserto en las hojas SIEMPRE*/
            *codigo = actual->insertarClave(this->archivoIndice,clave); //TODO Nico, mira que lo que recibe no es esto.		
            anterior = actual;
		}
		
		/*Si se registra NO_MODIFICADO como codigo significa que no 
		 * se ha llegado a las hojas
		 */
		if (codigo == Codigo::NO_MODIFICADO)				
			insertarInterno(actual->siguiente(this->archivoIndice,clave), clave, codigo, anterior);
		                     
		if ( (codigo == Codigo::OVERFLOW) && (actual->getNivel()!=0) ){			
            /*Inserto en un nodo de forma similar al Arbol B*/   
            actual->insertarClave(/*this->archivoIndice,*/clave,codigo);			
	        
            delete anterior;
	        anterior = actual;
        }else if ( (codigo == Codigo::MODIFICADO) && (actual->getNivel()!= 0 ) ){            
            /*Elimino el nodo inmediatamente anterior(de menor nivel)*/
<<<<<<< .mine
            
        	delete anterior;
            anterior = Actual;
         }
=======
            delete anterior;
            anterior = actual;
        }
>>>>>>> .r45
}

	void BpTree::eliminarClave(Clave* clave)
	{
		char* codigo = Codigo::NO_MODIFICADO;
		NodoBp* nodoUnderflow = NULL;
		
		/*Busco la raiz para comenzar el recorrido de eliminarClave*/
		this->nodoActual = this->getRaiz();
	
		/*Se devuelve el codigo correspondiente*/
		eliminarInterno(this->nodoActual, clave, codigo, nodoUnderflow);
		
		/*Que no sea el unico nodo y vacio*/
		if((this->nodoActual->vacio(this->archivoIndice)) && (this->nodoActual->getNivel()! =0)){ 
		/*CASO EN EL CUAL LA RAIZ DESAPARECE POR QUEDARSE SIN CLAVES Y DEBO REEMPLAZAR ESE NODO POR EL 
		 * HIJO IZQUIERDO DEL MISMO	*/
			ListaClaves* lista = this->nodoActual->obtenerClaves();
			delete lista;
			Nodo* nodo = new Nodo(this->archivoIndice,this->nodoActual->getHijoIzq());
			
			/*copio el nodo hijo izquierdo a la raiz*/
			ListaClaves* claves = nodo->obtenerClaves()->obtenerCopia();
			this->nodoActual->setClaves(claves);
			this->nodoActual->setNivel(nodo->getNivel());
			this->nodoActual->setHijoIzq(nodo->getHijoIzq());
			
			this->archivoIndice->sobreescribirNodo(this->nodoActual);	
			this->archivoIndice->eliminarNodo(nodo->obtenerPosicionEnArchivo());
			delete nodo;
	}
	else if(codigo->getValor()!="NODO ESCRITO") 
		this->archivoIndice->sobreescribirNodo(this->nodoActual);
				
	delete this->nodoActual;
	delete codigo;

	}
/*----------------------------------------------------------------------------------------*/
	Clave* BpTree::buscarClave(Clave* clave)
	{	
		Clave* Buscada = NULL;
		this->nodoActual = this->getRaiz();
		
		/*Corroboro que el arbol no este vacio*/
		if(nodoActual->obtenerClaves()->getCantidadNodos() > 0)
			this->buscarInterno(clave,Buscada);		
		   
	    
	    delete nodoActual;
		return Buscada;
	}
/*----------------------------------------------------------------------------------------*/
void BpTree::buscarInterno(Clave* clave,Clave*& Buscada){
/*devuelve puntero a la clave en archivo o a la mas cercana*/
    
	if (this->nodoActual->getNivel() == 0){
		ListaClaves* lista = this->nodoActual->obtenerClaves();
 		Clave* claveBuscada = lista->buscar(clave);
 		/*ver que si la devuelta es NULL , tengo que pedir
 		 * la siguiente*/  
 		if(!claveBuscada){
 			lista->primero();
 			claveBuscada = (Clave*)lista->obtenerDato();	
 			}
 		Buscada = claveBuscada->copiar();
 	}
	else{
		Nodo* nodoSig = this->nodoActual->siguiente(this->archivoIndice,clave);
		delete this->nodoActual;
		this->nodoActual = nodoSig;
		this->buscarInterno(clave,Buscada);
	}
}

	bool BpTree::puedeDonar(Nodo* nodoDonador, Nodo* nodoAceptor)
	{  //?? al pedo, no se usa en el ext es solo interno
		bool devolver = false;
		
		if ((nodoDonador) && (nodoAceptor))
			devolver = nodoDonador->puedeDonar(this->archivoIndice);
			if(devolver){
				ListaClaves* listaDonador = nodoDonador->obtenerClaves();
				listaDonador->primero();
				int tamanio = ((Clave*)listaDonador->obtenerDato())->getTamanio();
				if (!(tamanio<=nodoAceptor->getEspacioLibre()))
					devolver = false;
			}	
		
		return devolver;
	}
	
	void BpTree::eliminarInterno(NodoBp* actual,Clave* clave, char* codigo,NodoBp* &nodoUnderflow)
	{
		/*si se trata de una hoja debo hacer la eliminacion de la lista de claves*/
		if (Actual->getNivel() == 0){            
            nodoUnderflow = actual;
            /*Si al quitar hay underflow las claves devueltas son las que quedan en el nodo del cual se elimino*/
            actual->eliminarClave(this->archivoIndice, clave, codigo);                     	
		}else if (codigo == Codigo::NO_MODIFICADO){
    	  /*Llamada recursiva hasta que llega a la hoja*/           
    	  eliminarInterno(actual->siguiente(archivoIndice,clave),clave,codigo,nodoUnderflow);
		}
		
		///////////////////////////////////////////////////////////////////////////////////////
		//CASO: Underflow en nodo hoja
		///////////////////////////////////////////////////////////////////////////////////////
		if ( (codigo == Codigo::UNDERFLOW_HOJA) && (actual->getNivel()!= 0)){
			/*Levanto los nodos anterior y siguiente en las hojas del nodo con underflow*/
			Nodo* anterior = NULL;
            Nodo* posterior = NULL;
            
            //TODO Implementar metodos que devuelvan hermanos izquierdos y derechos
            //del nodo actual (para chequear si pueden ceder claves).
           	anterior = new NodoBp(this->archivoIndice,actual->refAnterior(clave));
			
           	//Caso en que el nodo con underflow no tenga hermano izquierdo.
			if (anterior->obtenerPosicionEnArchivo() == nodoUnderflow->obtenerPosicionEnArchivo()){
				delete anterior;
				anterior =  NULL;		
			}
					
           	posterior = new Nodo(this->archivoIndice,Actual->refPosterior(clave));
        	
           	//Caso en que el nodo con underflow no tenga hermano derecho.        			
			if (posterior->obtenerPosicionEnArchivo()== nodoConSubflow->obtenerPosicionEnArchivo()){
				delete posterior;
				posterior=NULL;
			}
			
           if (!puedeDonar(anterior,nodoUnderflow)){
        	   if (!puedeDonar(posterior,nodoConSubflow)){       		   
        		   /*Split de nodos*/
        		   if(chequearEspacio(anterior,nodoUnderflow)){
        			   /*Desaparece un nodo anterior*/
        			   /*Entran las claves todas en el nodo anterior, las meto al final*/
        			   
        			   SetClaves* set = nodoUnderflow->obtenerClaves()->obtenerCopia();
        			   anterior->insertarFinal(set, this->archivoIndice);
                                           
                       delete set;
                       /*anterior->setHijoIzq(posterior->obtenerPosicionEnArchivo());*/
                       anterior->setHijoIzq(nodoConSubflow->getHijoIzq());
       
                       /*El destructor tiene que setear los cambios en el archivoIndice tambi�n!*/
                       this->archivoIndice->eliminarNodo(nodoConSubflow->obtenerPosicionEnArchivo());
                       delete nodoConSubflow;
                       
                       /* el buscar del nodo tiene que ser similar al del arbol , encontrar la mas cercana
                       o la clave misma*/
                       Actual->quitarClave(this->archivoIndice,Actual->buscar(clave),codigo);
                       
                       this->archivoIndice-> sobreescribirNodo(anterior);
                       
                       delete anterior;
                       if(posterior)
        					delete posterior;
                       
                       nodoUnderflow = Actual;
                       
        		   }else if(chequearEspacio(posterior,nodoUnderflow)){
                	   /*Desaparece nodo posterior*/
                	   SetClaves* set = posterior->obtenerClaves()->obtenerCopia();
	                   /*Entra en el posterior, meto las claves del nodo a eliminar  al principio*/
	                   nodoUnderflow->insertarFinal(lista,this->archivoIndice);
	                   
	                   delete set;
	                   
	                   nodoConSubflow->setHijoIzq(posterior->getHijoIzq());
	                   /*El destructor tiene que setear los cambios en el archivoIndice tambi�n!*/
	                   this->archivoIndice->eliminarNodo(Actual->refPosterior(clave));
	                   
	                   delete posterior;
	                   
	                   /*Quito la clave del nodo Actual*/
	                   actual->quitarClave(this->archivoIndice,Actual->claveSiguiente(clave),codigo);
	                   this->archivoIndice-> sobreescribirNodo(nodoConSubflow);
	                   
	                   delete nodoUnderflow;
                                            
                        if (anterior)
							delete anterior;
                        
                        nodoUnderflow = actual;
                  }else{
                	  /*Split con nodo posterior*/
                	  
                	  split(posterior, nodoUnderflow);
                	                  	  
	              	  /*Reemplaza la clave en el nodo actual por la primera clave del nodo posterior*/
                  	  posterior->obtenerClaves()->primero();
                  	  Clave* reemplazada;
                  	  
                  	  reemplazada = Actual->reemplazarClave(Actual->claveSiguiente(clave),(Clave*)posterior->obtenerClaves()->obtenerDato(),codigo);
                  	  reemplazada->setReferencia(posterior->obtenerPosicionEnArchivo());
                  	  
                  	  this->archivoIndice->sobreescribirNodo(nodoUnderflow);
                  	  this->archivoIndice->sobreescribirNodo(posterior);
                  	  
                  	  delete nodoUnderlow;
                  	  
                  	  if (anterior)
                  		  delete anterior;
                  	  
                  	  delete posterior;
                  	  
                  	  nodoUnderflow = actual;
                  }
        	   }else{
        		   /*Split con nodo anterior*/
        		   split(anterior,nodoConSubflow);            
                            
	                /*Reemplaza la clave en el nodo actual por la primera clave del nodo con subflow*/
	                nodoConSubflow->obtenerClaves()->primero();
	                Clave* reemplazada;
	                reemplazada = Actual->reemplazarClave(Actual->buscar(clave),(Clave*)nodoConSubflow->obtenerClaves()->obtenerDato(),codigo);
	                reemplazada->setReferencia(nodoConSubflow->obtenerPosicionEnArchivo());
	      			
	      			this->archivoIndice->sobreescribirNodo(nodoConSubflow);                                                        
	                this->archivoIndice->sobreescribirNodo(anterior);
	                
	                delete nodoConSubflow;
	                
                    if(posterior)
                    	delete posterior;
                    
                    delete anterior;
                    nodoUnderflow = actual;
               }   
      
		///////////////////////////////////////////////////////////////////////////////////////
		//CASO: Underflow en nodo interno
		///////////////////////////////////////////////////////////////////////////////////////
		}else if (codigo == Codigo::UNDERFLOW){
      	/*LA CLAVE DEBE SER LA CORRESPONDIENTE A LA MAS CERCANA DE ESTE NODO, TIENE QUE ALMACENARSE EN ESTA PORCION DE FUNCION RECURSIVA*/
      	/*NIVEL DE ARBOL B*/
                    /*Levanto los nodos anterior y siguiente en las hojas del nodo con 
                           SUBFLOW*/
                       
			Nodo* anterior  = NULL;
     		Nodo* posterior = NULL;
     		
     		anterior = new Nodo(this->archivoIndice,Actual->refAnterior(clave));
					
     		if (anterior->obtenerPosicionEnArchivo() == nodoConSubflow->obtenerPosicionEnArchivo()){
     			delete anterior;
				anterior =  NULL;		
			}
     		
     		posterior = new Nodo(this->archivoIndice,Actual->refPosterior(clave));
			
			if (posterior->obtenerPosicionEnArchivo()== nodoUnderflow->obtenerPosicionEnArchivo()){
				delete posterior;
				posterior=NULL;
			}
			
			if (puedeDonar(posterior,nodoConSubflow)){
				/*ROTACION PADRE-HIJO con POSTERIOR*/                
				/*--------------------------------*/
	
				/*(baja el siguiente al padre)*/	
	 
				/*busco la clave padre que debe bajar*/
				Clave* clavePosterior = Actual->claveSiguiente(clave);
	 
				/*Creo la clave que luego se insertara en el nodo con Subflow*/
				Clave* claveInsertar = clavePosterior->copiar();
	
				/*seteo la referencia de la clave a insertar*/
				claveInsertar->setReferencia(posterior->getHijoIzq());
	 
				/*ver bien esto, con cuentas JAMAS tiene que dar overflow estoooooo*/
				nodoConSubflow->obtenerClaves()->insertarOrdenado(claveInsertar);
	                             
		         /*actualizo el padre poniendole el dato del primero del posterior*/
		         posterior->obtenerClaves()->primero();
		         Clave* primera =(Clave*) posterior->obtenerClaves()->obtenerDato();
		         
		         /*seteo el nuevo hijo izquierdo del posterior*/
		         posterior->setHijoIzq(primera->obtenerReferencia());
		         Clave* reemplazada = Actual->reemplazarClave(clavePosterior,primera,codigo);
		         reemplazada->setReferencia(posterior->obtenerPosicionEnArchivo());
		 
		         /*quito del posterior la primer clave*/
		         Clave* quitada = (Clave*)posterior->obtenerClaves()->quitar(1);
		         delete quitada;
		         
		         this->archivoIndice->sobreescribirNodo(nodoConSubflow);
		         this->archivoIndice->sobreescribirNodo(posterior);
	         
		         if (anterior)
		         	delete anterior;
		         
		         delete nodoConSubflow;
		         delete posterior;
	 		
		         nodoConSubflow = Actual;
		    }else if (puedeDonar(anterior,nodoConSubflow)){	
		    	/*ROTACION PADRE-HIJO con ANTERIOR*/
		    	/*--------------------------------*/
			
		    	Clave* encontrada = Actual->buscar(clave);	
	
		    	/*BAJA EL PADRE*/
		    	/*Creo la clave que luego se insertara en el nodo con Subflow*/
		    	Clave* claveInsertar =encontrada->copiar();
		    	/*bajo padre a principio del nodo con subflow*/
		    	claveInsertar->setReferencia(nodoConSubflow->getHijoIzq());
		    	/*inserto en el nodoConsubflow*/
		    	nodoConSubflow->obtenerClaves()->insertarOrdenado(claveInsertar);
	         
	 
				 /*SUBE EL ULTIMO DEL ANTERIOR*/
		         anterior->obtenerClaves()->ultimo();   
		         Clave* claveUltima=(Clave*)anterior->obtenerClaves()->obtenerDato();
		         /*actualizo el hijo izquierdo del nodo con subflow*/
		         nodoConSubflow->setHijoIzq(claveUltima->obtenerReferencia());
		         
		         /*********/
		         claveUltima->setReferencia(nodoConSubflow->obtenerPosicionEnArchivo());
		          
		         Clave* reemplazar = claveUltima->copiar();
		         
		         /*QUITO LA CLAVE QUE SUBIO DEL ANTERIOR*/
		         anterior->quitarClave(this->archivoIndice,claveUltima,codigo);
		         
		         Actual->reemplazarClave(encontrada,reemplazar,codigo);
	         
			         /*SE CAMBIO POR LO DE ARRIBA*/
			                     	 	// Clave* reemplazada = Actual->reemplazarClave(encontrada,claveUltima,codigo);
			                       	 //	 reemplazada->setReferencia(nodoConSubflow->obtenerPosicionEnArchivo());        
			                       	   //      
			                       		// /*QUITO LA CLAVE QUE SUBIO DEL ANTERIOR*/
			                       	    // anterior->quitarClave(this->archivoIndice,claveUltima,codigo);
			         /*----------------------------*/
		         
		         this->archivoIndice->sobreescribirNodo(nodoConSubflow);
		         this->archivoIndice->sobreescribirNodo(anterior);
		         
		         delete reemplazar;
			     
		         if (posterior)
		        	 delete posterior;
			         	
		         delete anterior;
		         delete nodoConSubflow;
		         
		         nodoConSubflow = Actual;
		    }else{
                  	/*CASO EN EL CUAL DEBO HACER MERGE PORQUE NO PUEDO HACER SPLIT CON NADIE
                  	 * PROPAGACION DE LA ELIMINACION B HACIA ARRIBA, PORQUE BAJO UN PADRE Y DESAPARECE ASI UNA
                  	 * CLAVE DEL NODO ACTUAL*/
                  	
						/*nunca reacomodo punteros !!!!*/						 
						 /*primero intento con poesterior por simple convencion */
						
						
						if (posterior){
                         /*MERGE con POSTERIOR*/
                         /*-------------------*/

							Clave* clavePosterior = Actual->claveSiguiente(clave);
	
							/*BAJO EL POSTERIOR AL PADRE*/
							Clave* claveInsertar = clavePosterior->copiar();
							claveInsertar->setReferencia(posterior->getHijoIzq());
							/*la inserto*/
							nodoConSubflow->obtenerClaves()->insertarOrdenado(claveInsertar);
							nodoConSubflow->actualizarEspacioLibre(claveInsertar,true);
							/*hacer esta funcion*/
							nodoConSubflow->insertarFinal(posterior->obtenerClaves(),archivoIndice);
							
							/*borro el nodo posterio de archivo y memoria*/
							this->archivoIndice->eliminarNodo(clavePosterior->obtenerReferencia());
							delete posterior;
							/*Borra el anterior si fue creado*/
							if(anterior)
								delete anterior;
								
							Actual->quitarClave(this->archivoIndice,clavePosterior,codigo);
							
							this->archivoIndice->sobreescribirNodo(nodoConSubflow);
							delete nodoConSubflow;
							
							nodoConSubflow = Actual;
							
						}else{
								
						/*MERGE con ANTERIOR*/
						/*------------------*/
							
							/*Creo la clave  a insertar con la cual el padre va a "bajar"*/
							Clave* encontrada = Actual->buscar(clave);
							
							/*BAO EL PADRE*/
							Clave* claveInsertar = encontrada->copiar();
							claveInsertar->setReferencia(nodoConSubflow->getHijoIzq());
							/*la inserto*/
							anterior->obtenerClaves()->insertarOrdenado(claveInsertar);		
							anterior->actualizarEspacioLibre(claveInsertar,true);	
						    /*inserto en el anterior las claves del nodo con  subflow*/
							anterior->insertarFinal(nodoConSubflow->obtenerClaves(),this->archivoIndice);
							
							/*borro el nodo con subflow de archivo y memoria*/
							this->archivoIndice->eliminarNodo(clave->obtenerReferencia());
							delete nodoConSubflow;
							
							this->archivoIndice->sobreescribirNodo(anterior);
							delete anterior;
							/*Borra el anterior si fue creado*/
							if (posterior)
                       	         	delete posterior;
							
							Actual->quitarClave(this->archivoIndice,encontrada,codigo);
							
							nodoConSubflow = Actual;				
						}					 
	  
      			}/*fin else*/                                                  
                                   
      }/*fin if(codigo =="SUBFLOW")*/
	  else if( (codigo->getValor() == "NODO MODIFICADO")&&(Actual->getNivel()!= 0) ){
            	this->archivoIndice->sobreescribirNodo(nodoConSubflow);
            	delete nodoConSubflow;
            	nodoConSubflow = Actual;
            	/*Seteo el valor a NODO ESCRITO para que NO reescriba los nodos siguientes nodos*/
            	codigo->setValor("NODO ESCRITO");
	 }
		 else if (codigo->getValor() == "NODO ESCRITO"){
		 	delete nodoConSubflow;
	        nodoConSubflow = Actual;
		 }
	 
	}
	}

	bool BpTree::chequearEspacio(Nodo* otroNodo, Nodo* nodoUnderflow)
	{
	     bool devolver = false;
	     
	     if ( (otroNodo) && (nodoUnderflow) ){
	     	int espacioOcupado = archivoIndice->getTamanioNodo() - 
	     		nodoUnderflow->getEspacioLibre() - ArchivoIndice::getTamanioHeader();
	     	
	     	if (espacioOcupado <= otroNodo->getEspacioLibre())
	        	devolver = true;
	     }
	     
	     return devolver;
	}

	void BpTree::split(Nodo* donador, Nodo* aceptor){

/*chequear esta funcion*/
     int posicion = 1;
     Clave* clave;
     ListaClaves* listaDonador =donador->obtenerClaves();
     ListaClaves* listaAceptor =aceptor->obtenerClaves();
     
     listaDonador->primero();
     listaAceptor->primero();
     
     Clave* donadora =((Clave*)listaDonador->obtenerDato()); 
     Clave* aceptora=((Clave*)listaAceptor->obtenerDato()); 
     int comparacion = donadora->comparar(aceptora);
      if ( comparacion == -1 )
               /*donador->obtenerClaves()->ultimo();*/
               posicion = listaDonador->getCantidadNodos();
               
     clave = (Clave*)listaDonador->quitar(posicion);
    
     listaAceptor->insertarOrdenado(clave);
    
}

