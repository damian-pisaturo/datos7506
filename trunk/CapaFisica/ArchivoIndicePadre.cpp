///////////////////////////////////////////////////////////////////////////
//	Archivo   : ArchivoIndicePadre.cpp
//  Namespace : CapaFisica
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clases ArchivoIndice, 
//		ArchivoIndiceArbol, ArchivoIndiceArbolSecundario,
//		ArchivoIndiceHash.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#include "ArchivoIndicePadre.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndice
//			(Abstracta. Clase que sirve de abstraccion de la capa 
//			fisica para los indices de la capa de indices).
///////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////////////////
	// Constructor
	///////////////////////////////////////////////////////////////////////
	ArchivoIndice::ArchivoIndice(string nombreArchivo, unsigned int tamanioBloque, t_indice tipoIndice)
	{		
		this->tamanioBloque = tamanioBloque;
		this->tipoIndice    = tipoIndice;
		
		switch(tipoIndice){
		case ARBOL_BP:
			this->nombreArchivo = nombreArchivo + ".plus";
			break;
		case ARBOL_BS:
			this->nombreArchivo = nombreArchivo + ".star";
			break;
		case HASH:
			this->nombreArchivo = nombreArchivo + ".hash";
			break;
		default:
			this->nombreArchivo = nombreArchivo;
			break;
		}		
	}
	
	ComuDatos* ArchivoIndice::instanciarPipe(string nombreEjecutable)
	{
		return new ComuDatos(nombreEjecutables);
	}			

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceArbol
//			(Abstracta. Clase que sirve de abstraccion de la capa 
//			fisica para los indices en arboles de la capa de indices).
///////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////////////////
	// Constructor
	///////////////////////////////////////////////////////////////////////
	ArchivoIndiceArbol::ArchivoIndiceArbol(unsigned int tamNodo, string nombreArchivo, ):
		ArchivoIndice(nombreArchivo, tamNodo, tipoIndice)
	{ 
		//TODO Usar ComuDato para escribir la raiz vacia si el 
		//archivo de indice esta vacio.
		
		this->nombreArchivoEL = nombreArchivo + ".nfo";
		/*
		//Si el archivo de indice esta vacio creo una raiz hoja sin claves
		if(this->archivoIndex->fin())
			escribirRaizVacia();
		*/
	}
	
	///////////////////////////////////////////////////////////////////////
	// Metodos privados
	///////////////////////////////////////////////////////////////////////	
	void ArchivoIndiceArbol::escribirRaizVacia()
	{
		char* buffer = new char[this->getTamanioBloque()];
		char* puntero = buffer;
		Header headerNodo;
		
		/*Copiar el header al buffer*/
		headerNodo.nivel   = 0;  
		headerNodo.hijoIzq = 0;	 
		headerNodo.espacioLibre = this->getTamanioBloque() - this->getTamanioHeader();
		
		memcpy(buffer,&headerNodo,this->getTamanioHeader());
		puntero += this->getTamanioHeader();
		
		/*Escribir la raiz*/
		//TODO Usar ComuDato
		//this->archivoIndex->escribir(buffer);
		
		delete[] buffer;
	}
	
	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////	
	void ArchivoIndiceArbol::leerBloque(int numeroRegistro, Bloque* bloqueLeido)
	{
		Nodo* nodoLeido = (Nodo*) bloqueLeido;
		
		/*Variables de Lectura del buffer*/		
		char* buffer = new char[this->getTamanioNodo()];
		char* puntero = buffer;
		char* punteroFinal;
		Header headerNodo;
		
		/*Variables de interpretacion del nodo*/
		Clave* claveNueva;
		SetClaves* set = new SetClaves();
		
		/*Posicionarse en el numeroRegistro*/
		//TODO Usar ComuDato.
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
		
		if(nodoLeido->getNivel() == 0){
			while(puntero < punteroFinal){	
				/*leer la clave*/	
				claveNueva = leerClaveHoja(puntero);
				/*Agregarla a la lista*/	
				set->insert(claveNueva);
			}	
		}else{
			while(puntero < punteroFinal){	
				/*leer la clave*/	
				claveNueva = leerClaveNoHoja(puntero);
				/*Agregarla a la lista*/	
				lista->agregar(claveNueva);
			}
		}
		
		/*libero el buffer*/
		delete[] buffer;
		
		/*Agregar el setClaves al nodo*/
		nodoLeido->setClaves(set);
		/*Setear la posicion del nodo en el archivo*/
		nodoLeido->setPosicionEnArchivo(numeroRegistro);
	}
	
	void ArchivoIndiceArbol::escribirBloque(Bloque* bloqueNuevo)
	{
		Nodo* nodoNuevo = (Nodo*) bloqueNuevo;
		
		/*Variables de escritura del buffer*/
		const int registroUsado = -1;
		int posicion = -1;
		int numero;
		bool ubicado  = false;
		char* buffer  = new char[this->tamanioNodo];
		char* puntero = buffer;
		
		/*Variables de interpretacion del nodo*/
		Header headerNodo;
		SetClaves* set;
		
		nodoNuevo->actualizarEspacioLibre(this);
		
		//TODO Usar ComuDatos para buscar en el archivo de espacio libre.
		//Implementar en la capa fisica alguna clase que permita hacer esto mismo.
		
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
	
	void ArchivoIndiceArbol::sobreescribirBloque(Bloque* bloqueModif)
	{	
		Nodo* nodoModif = (Nodo*) nodoModif;
		Header headerNodo;
		char* buffer = new char[this->tamanioNodo];
		char* puntero = buffer;
		SetClaves* set;
		
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
		}else{
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

	void ArchivoIndiceArbol::eliminarNodo(unsigned int posicion)
	{
		/*Setear en el archivo de nodos liberados la posicion al final*/	
		bool ubicado = false;
		int numero   = 0;
		
		//TODO Usar ComuDato para recorrer el archivo de espacio liberado.
		/*Buscar el fin del archivo o un -1 para sobreescribir*/
		this->archivoLiberados->posicionarse(0);
		while( (!this->archivoLiberados->fin()) && (!ubicado) ){	
			this->archivoLiberados->leer(&numero);
			this->cantLecturasControl++;
			if(numero==-1)
				ubicado = true;
		}
		
		if(numero == -1){
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
	
	void ArchivoIndice::exportar(ostream &archivoTexto,int posicion)
	{	
		SetClaves* listaClaves;
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
	
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceHash
//			(Abstracta. Clase que sirve de abstraccion de la capa 
//			fisica para los indices de dispersion de la capa de indices).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceHash : ArchivoIndice
{
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoIndiceHash::ArchivoIndiceHash(unsigned int tamBucket, string nombreArchivo, t_indice tipoIndice):
			ArchivoIndice(nombreArchivo, tamNodo, tipoIndice)
		{
			this->nombreArchivoTabla = nombreArchivo + ".tbl";
		}
		
		ArchivoIndiceHash::~ArchivoIndiceHash() { };
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		void ArchivoIndiceHash::leerBloque(unsigned int numeroBloque, Bloque* bloqueLeido)
		{
			Bucket* bucketLeido = (Bucket*) bloqueLeido;
			
			// Obtengo el espacio libre.
			char * auxEspLibre = new char[5];
			memcpy(auxEspLibre,datos,4);
			auxEspLibre[4] = '\0';
			espacioLibre = atoi(auxEspLibre);
			delete[] auxEspLibre;

			//Obtengo el tamaño de dispersión.
			char* auxTD = new char[2];
			memcpy(auxTD,&datos[4],1);
			auxTD[1]='\0';
			tamDispersion = atoi(auxTD); 
			delete[] auxTD;
			
			//Obtengo la cantidad de registros.
			char* auxCRegs = new char[2];
			memcpy(auxCRegs,&datos[5],1);
			auxCRegs[1] = '\0';
			cantRegs = atoi(auxCRegs);
			delete[] auxCRegs;
		}
		
		void ArchivoIndiceHash::escribirBloque(Bloque* nuevoBloque)
		{
			// TODO: pedir a la capa fisica q escriba datos a partir de offset en el archivo.
		}
		
		void ArchivoIndiceHash::sobreEscribirBloque(Bloque* bloqueModif)
		{
			//TODO: Sobre-escribir datos del bloque usando el ComuDatos
			//para acceder a la capa fisica.
		}
};

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceSecundario
//			(Abstracta. Clase que sirve de abstraccion de la capa 
//			fisica para los indices secundarios de la capa de indices).
///////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
	ArchivoIndiceSecundario::ArchivoIndiceSecundario(unsigned int tamNodo, string nombreArchivo, unsigned int tamanioBloqueLista, t_indice tipoIndice):
		ArchivoIndice(tamNodo, nombreArchivo, tipoIndice)
	{			 
			this->tamanioArray = tamanioBloqueLista/sizeof(int); 
			this->nombreArchivoLista = nombreArchivo + ".list";
	}
	
	ArchivoIndiceSecundario::~ArchivoIndiceSecundario(){ }
	
	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////
	void ArchivoIndiceSecundario::sobreEscribirListaClavesP(unsigned int posicion, SetClaves* setClaves)
	{	
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
	
	SetClaves* ArchivoIndiceSecundario::obtenerLstClavesP(unsigned int posicion)
	{	
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

	int ArchivoIndiceSecundario::grabarNuevaLstClavesP(SetClaves* setClaves)
	{

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
