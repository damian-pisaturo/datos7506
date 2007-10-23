#include "ArchivoIndicePadre.h"
#include "Nodo.h"

/******************************************************************************/
/* Clase Archivo Indice - (Abstracta)*/
/*-----------------------------------*/

void ArchivoIndice::escribirRaizVacia(){
	
	char* buffer = new char[this->tamanioNodo];
	char* puntero = buffer;
	Header headerNodo;
	
	/*Copiar el header al buffer*/
	headerNodo.nivel = 0;  
	headerNodo.hijoIzq = 0;	 
	headerNodo.espacioLibre = this->tamanioNodo - this->getTamanioHeader();
	
	memcpy(buffer,&headerNodo,this->getTamanioHeader());
	puntero += this->getTamanioHeader();
	
	/*Escribir la raiz*/
	this->archivoIndex->escribir(buffer);
	
	delete[] buffer;
}

/*--------------------------------------------------------------------------------------------*/
ArchivoIndice::ArchivoIndice(int tamNodo,string nombreArchivo){
	
	this->tamanioNodo = tamNodo;
	
	/*Abre el archivo indice*/
	this->archivoIndex = new ArchivoRegistros(nombreArchivo,tamNodo);
    	
	/*Abrir el archivo de nodos liberados - Archivo de Registros de Int*/
	this->archivoLiberados = new ArchivoRegistros(nombreArchivo + ".lbds",sizeof(int));
	
	/*Si el archivo de indice esta vacio creo una raiz hoja sin claves*/
	if(this->archivoIndex->fin())
    		escribirRaizVacia();
    		
    this->cantEscrituras = 1; /*Debido a la escritura de la raiz vacia*/
    this->cantLecturas = 0;
    this->cantEscriturasControl = 0;
    this->cantLecturasControl = 0;	
}
/*--------------------------------------------------------------------------------------------*/
int ArchivoIndice::getTamanioNodo(){
	return this->tamanioNodo;	
}
/*--------------------------------------------------------------------------------------------*/
int ArchivoIndice::getTamanioHeader(){
	return sizeof(Header);	
}
/*--------------------------------------------------------------------------------------------*/
int ArchivoIndice::getCantLecturas(){
	return this->cantLecturas;
}
/*--------------------------------------------------------------------------------------------*/
int ArchivoIndice::getCantEscrituras(){
	return this->cantEscrituras;
}
/*--------------------------------------------------------------------------------------------*/
int ArchivoIndice::getCantLecturasControl(){
	return this->cantLecturasControl;
}
/*--------------------------------------------------------------------------------------------*/
int ArchivoIndice::getCantEscriturasControl(){
	return this->cantEscriturasControl;
}
/*--------------------------------------------------------------------------------------------*/
void ArchivoIndice::leer(int numeroRegistro, Nodo* nodoLeido){
	
	/*Variables de Lectura del buffer*/
	
	char* buffer = new char[this->tamanioNodo];
	char* puntero = buffer;
	char* punteroFinal;
	Header headerNodo;
	
	/*Variables de interpretacion del nodo*/
	Clave* claveNueva;
	ListaClaves* lista = new ListaClaves();
	
	/*Posicionarse en el numeroRegistro*/
	this->archivoIndex->posicionarse(numeroRegistro);
	
	/*Leer el bloque de 512 bytes y dejarlo apuntado a un char*/
	try{
		this->archivoIndex->leer(buffer);
	}
	catch(string s){
		delete[] buffer;
		delete lista;
		throw string("Bloque de Archivo Indice Inexistente");	
	}
	this->cantLecturas++;
	
	/*Castear el header*/
	memcpy(&headerNodo,puntero,sizeof(Header));
	puntero += sizeof(Header);
	
	/*Setear el espacio libre, si es hoja y el HijoIzq*/
	nodoLeido->setNivel(headerNodo.nivel);
	nodoLeido->setEspacioLibre(headerNodo.espacioLibre); 
	nodoLeido->setHijoIzq(headerNodo.hijoIzq);
	
	/*Recorrer el buffer desde donde quedo hasta que supere
	 * el espacio libre casteando a de a pedazos de a una clave*/
	 /*Apunto al final de los datos validos para saber cuando parar de leer*/
	 punteroFinal = buffer + (tamanioNodo - headerNodo.espacioLibre);
	
	if(nodoLeido->getNivel()==0){
		while(puntero < punteroFinal){	
			/*leer la clave*/	
			claveNueva = leerClaveHoja(puntero);
			/*Agregarla a la lista*/	
			lista->agregar(claveNueva);
		}	
	}
	else{
		while(puntero < punteroFinal){	
			/*leer la clave*/	
			claveNueva = leerClaveNoHoja(puntero);
			/*Agregarla a la lista*/	
			lista->agregar(claveNueva);
		}
	}
	/*libero el buffer*/
	delete[] buffer;
	
	/*Agregar la listaClaves al nodo*/
	nodoLeido->setClaves(lista);
	/*Setear la posicion del nodo en el archivo*/
	nodoLeido->setPosicionEnArchivo(numeroRegistro);
}
/*--------------------------------------------------------------------------------------------*/
void ArchivoIndice::grabarNuevoNodo(Nodo* nodoNuevo){
	
	/*Variables de escritura del buffer*/
	const int registroUsado = -1;
	int posicion = -1;
	int numero;
	bool ubicado = false;
	char* buffer = new char[this->tamanioNodo];
	char* puntero = buffer;
	
	/*Variables de interpretacion del nodo*/
	Header headerNodo;
	ListaClaves* lista;
	
	nodoNuevo->actualizarEspacioLibre(this);
	/*Buscar una posicion en el archivo a partir del archivo de nodos liberados
	 * (buscar el ultimo agregado a la lista)*/
	this->archivoLiberados->posicionarse(0);
	while( (!this->archivoLiberados->fin()) && (!ubicado) ){
		
		this->archivoLiberados->leer(&numero);
		this->cantLecturasControl++;
		if(numero==-1)
			ubicado = true;
		else
			posicion = numero;	
	}

	
	if(posicion!=-1){ /*Si se encontro un nodo liberado*/
		/*Posicionarse en el numeroRegistro*/
		this->archivoIndex->posicionarse(posicion);
		
		/*Grabar un -1 en esa posicion indicando que ese registro liberados dejo de existir*/	
		if(numero==-1){
			/*grabar dos posiciones antes*/
			this->archivoLiberados->posicionarse(archivoLiberados->posicion()-2);
			this->archivoLiberados->escribir(&registroUsado); /*Escribir un -1*/
			this->cantEscriturasControl++;
		}
		else{
			/*grabar una posicion antes*/
			this->archivoLiberados->posicionarse(archivoLiberados->posicion()-1);
			this->archivoLiberados->escribir(&registroUsado); /*Escribir un -1*/
			this->cantEscriturasControl++;
		}
	
	} /*fin if(posicion!=-1) */
	else{
		this->archivoIndex->posicionarseFin(); /* Si no hay ninguno libre me posicion al final*/
		posicion = this->archivoIndex->posicion();
		}
	/*Copiar el header al buffer*/
	headerNodo.nivel = nodoNuevo->getNivel();  
	headerNodo.hijoIzq = nodoNuevo->getHijoIzq();
	headerNodo.espacioLibre = nodoNuevo->getEspacioLibre();
	
	memcpy(puntero,&headerNodo,sizeof(Header));
	puntero += sizeof(Header);
	
	/*Obtener la lista de claves*/
	lista = nodoNuevo->obtenerClaves();
	
	/*Recorrer la lista de claves copiando cada clave al buffer*/
	if(nodoNuevo->getNivel()==0){
		lista->primero();
		while(!lista->fin()){
			copiarClaveHoja((Clave*)lista->obtenerDato(),puntero);			
			lista->siguiente();	
		}
	}
	else{
		lista->primero();
		while(!lista->fin()){
			copiarClaveNoHoja((Clave*)lista->obtenerDato(),puntero);			
			lista->siguiente();	
		}
	}
	
	/*Grabar el buffer en el archivo*/ 
	this->archivoIndex->escribir(buffer);
	this->cantEscrituras++;
	 
	/*Setear en el nodo la posicion donde se grabo el nodo*/
	nodoNuevo->setPosicionEnArchivo(posicion);
	
	delete[] buffer;
	
}
/*--------------------------------------------------------------------------------------------*/
void ArchivoIndice::eliminarNodo(int posicion){
/*Setear en el archivo de nodos liberados la posicion al final*/

	bool ubicado = false;
	int numero=0;
	
	/*Buscar el fin del archivo o un -1 para sobreescribir*/
	this->archivoLiberados->posicionarse(0);
	while( (!this->archivoLiberados->fin()) && (!ubicado) ){	
		this->archivoLiberados->leer(&numero);
		this->cantLecturasControl++;
		if(numero==-1)
			ubicado = true;
	}
	
	if(numero==-1){
		/*grabar una posicion antes (sobre el -1 encontrado)*/
		this->archivoLiberados->posicionarse(archivoLiberados->posicion()-1);
		this->archivoLiberados->escribir(&posicion); /*Escribir un -1*/
		this->cantEscriturasControl++;
	}
	else{
		/*grabar en esta posicion (se llego al fin sin ningun -1)*/
		this->archivoLiberados->escribir(&posicion);
		this->cantEscriturasControl++;
	}
}
/*--------------------------------------------------------------------------------------------*/
void ArchivoIndice::sobreescribirNodo(Nodo* nodoModif){
	
	Header headerNodo;
	char* buffer = new char[this->tamanioNodo];
	char* puntero = buffer;
	ListaClaves* lista;
	
	/*Actualizar el espacio libre*/
	nodoModif->actualizarEspacioLibre(this);
	
	/*Copiar el header al buffer*/
	headerNodo.nivel = nodoModif->getNivel();  
	headerNodo.hijoIzq = nodoModif->getHijoIzq();
	headerNodo.espacioLibre = nodoModif->getEspacioLibre();
	
	memcpy(buffer,&headerNodo,sizeof(Header));
	puntero += sizeof(Header);
	
	/*Obtener la lista de claves*/
	lista = nodoModif->obtenerClaves();
	
	/*Recorrer la lista de claves copiando cada clave al buffer*/
	if(nodoModif->getNivel()==0){
		lista->primero();
		while(!lista->fin()){
			copiarClaveHoja((Clave*)lista->obtenerDato(),puntero);			
			lista->siguiente();	
		}
	}
	else{
		lista->primero();
		while(!lista->fin()){
			copiarClaveNoHoja((Clave*)lista->obtenerDato(),puntero);			
			lista->siguiente();	
		}
	}
		
	/*Grabar el buffer en el archivo*/
	this->archivoIndex->posicionarse(nodoModif->obtenerPosicionEnArchivo()); 
	this->archivoIndex->escribir(buffer);
	this->cantEscrituras++;
	
	delete[] buffer;
	
}
/*--------------------------------------------------------------------------------------------*/
void ArchivoIndice::exportar(ostream &archivoTexto,int posicion){
	
	ListaClaves* listaClaves;
	Clave* claveLeer;
	/*Leo el nodo*/
	
	Nodo* nodo = new Nodo(this,posicion);

	/*Imprime el header del nodo*/	
	archivoTexto<<"Pos. de Archivo: "<<posicion<<endl;
	listaClaves = nodo->obtenerClaves();
	archivoTexto<<"Hijo Izquierdo: "<<nodo->getHijoIzq()<<endl;
	archivoTexto<<"Nivel: "<<nodo->getNivel()<<endl;
	
	/*Recorro la lista de claves*/
	listaClaves->primero();
	while(!listaClaves->fin()){
		claveLeer = static_cast<Clave*>(listaClaves->obtenerDato());
		claveLeer->imprimir(archivoTexto);
		listaClaves->siguiente();	
	}
	archivoTexto<<endl;
	delete nodo;
	
}
/*--------------------------------------------------------------------------------------------*/
ArchivoIndice::~ArchivoIndice(){
	
	delete this->archivoIndex;
	delete this->archivoLiberados;
}



/******************************************************************************/
/* Clase Archivo de Indice Secundario (Nombre o marca) */
/*-----------------------------------------------------*/

/*Constructor del hijo llama al constructor del padre*/
ArchivoIndiceSecundario::ArchivoIndiceSecundario(int tamNodo,string nombreArchivo,int tamanioBloqueLista):ArchivoIndice(tamNodo,nombreArchivo){
		 
		this->tamanioArray = tamanioBloqueLista/sizeof(int);
		/*Abre el archivo de lista de claves*/ 
		this->archivoLista = new ArchivoRegistros(nombreArchivo + ".list",tamanioBloqueLista);
		this->cantLecturasLista = 0;
		this->cantEscriturasLista = 0;
}
/*--------------------------------------------------------------------------------------------*/
void ArchivoIndiceSecundario::sobreEscribirListaClavesP(int posicion, Lista* listaClaves){
	
	/*Variables*/
	int arrayRef[this->tamanioArray]; 
	int arrayAnt[this->tamanioArray];
	int i = 0;
	int parciales = 0;
	bool fin = false;
	bool haySiguiente;
	int posicionSiguiente = posicion;
	
	/*Alamceno la cantidad de claves*/
	int totales = listaClaves->getCantidadNodos();
	 
	listaClaves->primero();
	while(!fin){
			/*Se posiciona en el primer registro de la lista a sobreescribir*/
			this->archivoLista->posicionarse(posicionSiguiente);
			
			while( (i<tamanioArray-2) && (!listaClaves->fin()) ){
				i++;
				parciales++;
				arrayRef[i] = *((int*)listaClaves->obtenerDato());
				listaClaves->siguiente();
			}
			/*Almaceno la cantidad de claves de la lista*/		
			arrayRef[0] = i;
			
			/*Chequeo si tengo que seguir escribiendo en otro bloque*/
			if(parciales<totales){
				
				/*Obtener la proxima posicion donde leer en el archivo, si es que
				 * hay definida una*/
				if(!this->archivoLista->fin()){
					this->archivoLista->leer(arrayAnt);
					this->cantLecturasLista++;
					this->archivoLista->posicionarse(archivoLista->posicion()-1);
					if(arrayAnt[tamanioArray-1]!=-1)
						haySiguiente = true;
					else
						haySiguiente = false;
				}
				else
					haySiguiente = false;
					
				if(haySiguiente)
					/*Caso en el que ya esta definido donde debe seguir la lista*/
					posicionSiguiente = arrayAnt[tamanioArray-1];
				
				else{
					/*Si no hay otro bloque definido sigo al final del archivo*/
					int posBck = this->archivoLista->posicion();
					this->archivoLista->posicionarseFin();
					posicionSiguiente = this->archivoLista->posicion();
					this->archivoLista->posicionarse(posBck); 			
				}								
			}		
			else{
				/*Almaceno que no hay mas bloques de la lista*/
				posicionSiguiente = -1; 
				fin = true;
			}
			/*Almaceno el bloque donde se va a continuar escribiendo*/
			arrayRef[tamanioArray-1] = posicionSiguiente;
			/*Escribo el bloque en el archivo*/
			this->archivoLista->escribir(arrayRef);
			this->cantEscriturasLista++;
			/*Se posiciona en el proximo registro donde escribir lo que falta de lista*/
			i = 0;	
	}
	
	
}
/*--------------------------------------------------------------------------------------------*/
Lista* ArchivoIndiceSecundario::obtenerLstClavesP(int posicion){
	
	/*Variables*/
	Lista* listaClaves = new Lista;
	int arrayRef[this->tamanioArray];
	int cantClaves;
	bool fin = false;
	
	/*Se posiciona al final del archivo*/
	this->archivoLista->posicionarse(posicion);
	
	while(!fin){
		
		/*Obtengo la cantidad de claves*/
		this->archivoLista->leer(arrayRef);
		this->cantLecturasLista++;
		cantClaves = arrayRef[0];
			
		for(int i=0;i<cantClaves;i++)
			listaClaves->agregar(new int(arrayRef[i+1]));
		
		if(arrayRef[tamanioArray-1]!=-1)
			archivoLista->posicionarse(arrayRef[tamanioArray-1]);
		else
			fin = true;
		
	}
	
	return listaClaves;
}
/*--------------------------------------------------------------------------------------------*/
int ArchivoIndiceSecundario::grabarNuevaLstClavesP(Lista* listaClaves){

	/*Variables*/
	int arrayRef[this->tamanioArray]; 
	int i = 0;
	int parciales = 0;
	bool fin = false;
	
	/*Me posiciono al final del archivo*/
	this->archivoLista->posicionarseFin();
	
	/*Almaceno la posicion donde se grabo para devolverla*/
	int posicion = this->archivoLista->posicion();
	
	/*Alamceno la cantidad de claves*/
	int totales = listaClaves->getCantidadNodos();
	 
	listaClaves->primero();
	while(!fin){
			
			while( (i<tamanioArray-2) && (!listaClaves->fin()) ){
				i++;
				parciales++;
				arrayRef[i] = *((int*)listaClaves->obtenerDato());
				listaClaves->siguiente();
			}
			/*Almaceno la cantidad de claves de la lista*/		
			arrayRef[0] = i;
			
			/*Chequeo si tengo que seguir escribiendo en otro bloque*/
			if(parciales<totales){
				/*Almaceno el bloque donde se va a continuar escribiendo*/
				arrayRef[tamanioArray-1] = this->archivoLista->posicion()+1;	
			}		
			else{
				/*Almaceno que no hay un bloque donde continuar*/
				arrayRef[tamanioArray-1] = -1; 
				fin = true;
			}
			/*Escribo el bloque en el archivo*/
			this->archivoLista->escribir(arrayRef);
			this->cantEscriturasLista++;
			i = 0;
	}
	
	return posicion;
}
/*--------------------------------------------------------------------------------------------*/
void ArchivoIndiceSecundario::exportar(ostream &archivoTexto,int posicion){
	
	ListaClaves* listaClaves;
	Lista* listaClavesPrim;
	ClaveCadena* claveLeer; 
	/*Leo el nodo*/
	
	Nodo* nodo = new Nodo(this,posicion);

	/*Imprime el header del nodo*/	
	archivoTexto<<"Pos. de Archivo: "<<posicion<<endl;
	listaClaves = nodo->obtenerClaves();
	archivoTexto<<"Hijo Izquierdo: "<<nodo->getHijoIzq()<<endl;
	archivoTexto<<"Nivel: "<<nodo->getNivel()<<endl;
	
	/*Recorro la lista de claves*/
	listaClaves->primero();
	while(!listaClaves->fin()){
		claveLeer = static_cast<ClaveCadena*>(listaClaves->obtenerDato());
		claveLeer->imprimir(archivoTexto);
		
		archivoTexto<<"Lista de Claves Primarias:"<<endl;
		/*Leer la lista de claves primarias del nombre*/
		listaClavesPrim = this->obtenerLstClavesP(claveLeer->getRefListaPrim());
		
		/*Recorrer la lista de claves primarias imprimiendo*/
		listaClavesPrim->primero();
		while(!listaClavesPrim->fin()){
			archivoTexto<<*(int*)listaClavesPrim->obtenerDato()<<endl;
			listaClavesPrim->siguiente();	
		}
		
		/*Borrar la lista*/
		listaClavesPrim->primero();
		while(!listaClavesPrim->fin()){
			delete static_cast<int*>(listaClavesPrim->obtenerDato());;
			listaClavesPrim->siguiente();	
		}
		delete listaClavesPrim;
		
		
		listaClaves->siguiente();	
	}
	archivoTexto<<"------------------------------"<<endl;
	delete nodo;
	
}
/*--------------------------------------------------------------------------------------------*/
int ArchivoIndiceSecundario::getCantEscriturasLista(){
	return this->cantEscriturasLista;
}
/*--------------------------------------------------------------------------------------------*/
int ArchivoIndiceSecundario::getCantLecturasLista(){
	return this->cantLecturasLista;
}
/*--------------------------------------------------------------------------------------------*/
ArchivoIndiceSecundario::~ArchivoIndiceSecundario(){
	delete this->archivoLista;
}
