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
	ArchivoIndice::ArchivoIndice(unsigned int tamanioBloque, string nombreArchivo, t_indice tipoIndice)
	{		
		this->tamanioBloque = tamanioBloque;
		this->tipoIndice    = (unsigned int)tipoIndice;
		
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
		return new ComuDatos(nombreEjecutable);
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
	ArchivoIndiceArbol::ArchivoIndiceArbol(unsigned int tamNodo, string nombreArchivo, t_indice tipoIndice):
		ArchivoIndice(tamNodo, nombreArchivo, tipoIndice)
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
	void ArchivoIndiceArbol::copiarClaveHoja(Clave* clave, char* &puntero)
	{		
		//Copio el valor de la clave
		memcpy(puntero, clave->getValor(), clave->getTamanioValor());
		puntero += clave->getTamanioValor();
		
		//Copia de la referencia a registro del archivo de datos. 
		unsigned int referencia = clave->getReferencia();
		memcpy(puntero, &referencia, Tamanios::TAMANIO_REFERENCIA);
	} 
		
	void ArchivoIndiceArbol::copiarClaveNoHoja(Clave* clave, char* &puntero)
	{
		unsigned int referencia = 0;				
		
		//Copia del valor de la clave
		memcpy(puntero, clave->getValor(), clave->getTamanioValor());
		puntero += clave->getTamanioValor();
		
		if (this->getTipoIndice() == ARBOL_BS){
			//Si el arbol es B*, copiar referencia al registro de datos.			
			referencia = clave->getReferencia();
			memcpy(puntero, &referencia, Tamanios::TAMANIO_REFERENCIA);
			puntero += Tamanios::TAMANIO_REFERENCIA;
		}
		
		//Copia de la referencia al hijo derecho.
		referencia = clave->getHijoDer();
		memcpy(puntero, &referencia, Tamanios::TAMANIO_REFERENCIA);
	}
			
	void ArchivoIndiceArbol::leerBloque(unsigned int numBloque, BloqueIndice* bloqueLeido)
	{		
		Nodo* nodoLeido = static_cast<Nodo*> bloqueLeido;
		string buffer;
		
		//Variables de interpretacion del nodo
		Header headerNodo;		
		Clave* claveNueva;
		SetClaves* set = new SetClaves();
		
		//Instancia del pipe
		ComuDatos* pipe = instanciarPipe(NombreCapas::CAPA_FISICA);
		pipe->agregarParametro(OperacionesCapas::FISICA_LEER, 0);
		pipe->agregarParametro(this->nombreArchivo, 1);
		pipe->agregarParametro(numBloque*(this->getTamanioBloque()), 2);
	
		//Se lanza el proceso de la capa fisica. 
		//Se obtiene en buffer el contenido del Nodo solicitado.
		pipe->lanzar();
		buffer = pipe->leerString(this->getTamanioBloque());
		
		char* data = buffer.data();
		//Castear el header
		memcpy(&headerNodo, data, sizeof(Header));
		data += sizeof(Header);
		
		//Setear el espacio libre, si es hoja y el HijoIzq
		nodoLeido->setNivel(headerNodo.nivel);
		nodoLeido->setEspacioLibre(headerNodo.espacioLibre); 
		nodoLeido->setHijoIzq(headerNodo.hijoIzq);
		
		//Recorrer el buffer desde donde quedo hasta que supere
		//el espacio libre, interpretando clave por clave.			
		char* punteroFinal = data + (this->getTamanioBloque() - headerNodo.espacioLibre);
		
		if(nodoLeido->getNivel() == 0){
			while(data < punteroFinal){	
				//Leer la clave	
				claveNueva = leerClaveHoja(data);
				//Agregarla a la lista	
				set->insert(claveNueva);
			}	 
		}else{
			while(data < punteroFinal){	
				//Leer la clave	
				claveNueva = leerClaveNoHoja(data);
				//Agregarla a la lista	
				lista->agregar(claveNueva);
			}
		}
		
		//Agregar el setClaves al nodo
		nodoLeido->setClaves(set);
		//Setear la posicion del nodo en el archivo
		nodoLeido->setPosicionEnArchivo(numBloque);	
		pipe->liberarRecursos();
	}
	
	void ArchivoIndiceArbol::escribirBloque(BloqueIndice* bloqueNuevo)
	{	
		//TODO Revisar implementacion
		
		Nodo* nodoNuevo = (Nodo*) bloqueNuevo;
		
		//Variables de escritura del buffer
		string buffer;
		char* data = new char[this->getTamanioBloque()];
		
		//Variables de interpretacion del nodo
		Header headerNodo;
		SetClaves* set;
		
		nodoNuevo->actualizarEspacioLibre(this); //NADIE SABIA.
		
		//Instancia del pipe
		ComuDatos* pipe = instanciarPipe(NombreCapas::CAPA_FISICA);
		pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR, 0);
		pipe->agregarParametro(this->nombreArchivo, 1);
		pipe->agregarParametro(this->nombreArchivoEL, 2);
		pipe->agregarParametro(numBloque*(this->getTamanioBloque()), 3);				
		
		//Se lanza el proceso de la capa fisica. 
		//Se obtiene en buffer el contenido del Nodo solicitado.
		pipe->lanzar();		
		buffer = pipe->escribir(this->getTamanioBloque());
		
		//TODO Buscar en archivo de espacio libre.
		//Buscar una posicion en el archivo a partir del archivo de nodos liberados
		 // (buscar el ultimo agregado a la lista)
		/*
		this->archivoLiberados->posicionarse(0);
		
		if(posicion!=-1){ //Si se encontro un nodo liberado
			//Posicionarse en el numeroRegistro
			this->archivoIndex->posicionarse(posicion);
			
			//Grabar un -1 en esa posicion indicando que ese registro liberados dejo de existir
			if(numero==-1){
				//grabar dos posiciones antes
				this->archivoLiberados->posicionarse(archivoLiberados->posicion()-2);
				this->archivoLiberados->escribir(&registroUsado); //Escribir un -1
				this->cantEscriturasControl++;
			}
			else{
				//grabar una posicion antes
				this->archivoLiberados->posicionarse(archivoLiberados->posicion()-1);
				this->archivoLiberados->escribir(&registroUsado); //Escribir un -1
				this->cantEscriturasControl++;
			}
		
		} //fin if(posicion!=-1) 
		else{
			this->archivoIndex->posicionarseFin(); // Si no hay ninguno libre me posicion al final
			posicion = this->archivoIndex->posicion();
			}
		*/
		
		//Copiar el header al buffer
		headerNodo.nivel = nodoNuevo->getNivel();  
		headerNodo.hijoIzq = nodoNuevo->getHijoIzq();
		headerNodo.espacioLibre = nodoNuevo->getEspacioLibre();
		
		memcpy(data, &headerNodo, sizeof(Header));
		data += sizeof(Header);
		
		//Obtener la lista de claves
		set = nodoNuevo->getClaves();
		
		//Recorrer la lista de claves copiando cada clave al buffer
		if(nodoNuevo->getNivel()==0){
			for(SetClaves::iterator iterClaves = set->begin(); 
				iterClaves != set->end(); ++iterClaves)
				copiarClaveHoja((Clave*)(*iter), data);
			
		}else{
			for(SetClaves::iterator iterClaves = set->begin(); 
				iterClaves != set->end(); ++iterClaves)
				copiarClaveNoHoja((Clave*)(*iter), data);
		}
		
		//Grabar el buffer en el archivo 
		this->archivoIndex->escribir(buffer);
		this->cantEscrituras++;
		 
		//Setear en el nodo la posicion donde se grabo el nodo
		nodoNuevo->setPosicionEnArchivo(posicion);
		
		delete[] buffer;
		*/
		
	}
	
	void ArchivoIndiceArbol::sobreEscribirBloque(BloqueIndice* bloqueModif)
	{	
		//TODO Revisar implementacion.
		/*
		Nodo* nodoModif = (Nodo*) nodoModif;
		Header headerNodo;
		char* buffer = new char[this->tamanioNodo];
		char* puntero = buffer;
		SetClaves* set;
		
		//Actualizar el espacio libre
		nodoModif->actualizarEspacioLibre(this);
		
		//Copiar el header al buffer
		headerNodo.nivel = nodoModif->getNivel();  
		headerNodo.hijoIzq = nodoModif->getHijoIzq();
		headerNodo.espacioLibre = nodoModif->getEspacioLibre();
		
		memcpy(buffer,&headerNodo,sizeof(Header));
		puntero += sizeof(Header);
		
		//Obtener la lista de claves
		lista = nodoModif->obtenerClaves();
		
		//Recorrer la lista de claves copiando cada clave al buffer
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
			
		//Grabar el buffer en el archivo
		this->archivoIndex->posicionarse(nodoModif->obtenerPosicionEnArchivo()); 
		this->archivoIndex->escribir(buffer);
		this->cantEscrituras++;
		
		delete[] buffer;	
		*/
	}

	void ArchivoIndiceArbol::eliminarNodo(unsigned int posicion)
	{
		//TODO Revisar implementacion
		/*
		//Setear en el archivo de nodos liberados la posicion al final
		bool ubicado = false;
		int numero   = 0;
		
		//TODO Usar ComuDato para recorrer el archivo de espacio liberado.
		//Buscar el fin del archivo o un -1 para sobreescribir
		this->archivoLiberados->posicionarse(0);
		while( (!this->archivoLiberados->fin()) && (!ubicado) ){	
			this->archivoLiberados->leer(&numero);
			this->cantLecturasControl++;
			if(numero==-1)
				ubicado = true;
		}
		
		if(numero == -1){
			//grabar una posicion antes (sobre el -1 encontrado)
			this->archivoLiberados->posicionarse(archivoLiberados->posicion()-1);
			this->archivoLiberados->escribir(&posicion); //Escribir un -1
			this->cantEscriturasControl++;
		}
		else{
			//grabar en esta posicion (se llego al fin sin ningun -1)
			this->archivoLiberados->escribir(&posicion);
			this->cantEscriturasControl++;
		}
		*/
	}
	
	void ArchivoIndiceArbol::exportar(ostream &archivoTexto,int posicion)
	{	
		//TODO Revisar implementacion
		/*
		SetClaves* listaClaves;
		Clave* claveLeer;
		//Leo el nodo
		
		Nodo* nodo = new Nodo(this,posicion);
	
		//Imprime el header del nodo	
		archivoTexto<<"Pos. de Archivo: "<<posicion<<endl;
		listaClaves = nodo->obtenerClaves();
		archivoTexto<<"Hijo Izquierdo: "<<nodo->getHijoIzq()<<endl;
		archivoTexto<<"Nivel: "<<nodo->getNivel()<<endl;
		
		//Recorro la lista de claves
		listaClaves->primero();
		while(!listaClaves->fin()){
			claveLeer = static_cast<Clave*>(listaClaves->obtenerDato());
			claveLeer->imprimir(archivoTexto);
			listaClaves->siguiente();	
		}
		
		archivoTexto<<endl;
		delete nodo;
		*/
	
	}
	
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceHash
//			(Abstracta. Clase que sirve de abstraccion de la capa 
//			fisica para los indices de dispersion de la capa de indices).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoIndiceHash::ArchivoIndiceHash(unsigned int tamBucket, string nombreArchivo):
			ArchivoIndice(tamBucket, nombreArchivo, HASH)
		{
			this->nombreArchivoTabla = nombreArchivo + ".tbl";
		}
		
		ArchivoIndiceHash::~ArchivoIndiceHash() { }
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		void ArchivoIndiceHash::leerBloque(unsigned int numeroBloque, BloqueIndice* bloqueLeido)
		{
			//TODO Revisar implementacion.
			
			Bucket* bucketLeido = (Bucket*) bloqueLeido;
						
			char* bloqueArchivo;
			//TODO: Levantar de archivo
			
			// Se obtiene el offset a espacio libre.
			unsigned short espLibre;
			memcpy(&espLibre,bloqueArchivo,Tamanios::TAMANIO_ESPACIO_LIBRE);
			bucketLeido->setEspLibre(espLibre);
			
			// Se obtiene la cantidad de registros.
			unsigned short cantRegs;
			memcpy(&cantRegs,&bloqueArchivo[Tamanios::TAMANIO_ESPACIO_LIBRE],Tamanios::TAMANIO_CANTIDAD_REGISTROS);
			bucketLeido->setCantRegs(cantRegs);
			
			// Se obtiene el tamanio de dispersión del bucket.
			unsigned short tamDisp;
			memcpy(&tamDisp,&bloqueArchivo[Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS],Tamanios::TAMANIO_DISPERSION);
			bucketLeido->setTamDispersion(tamDisp);
						
		}
		
		void ArchivoIndiceHash::escribirBloque(BloqueIndice* nuevoBloque)
		{
			// TODO: pedir a la capa fisica q escriba datos a partir de offset en el archivo.
		}
		
		void ArchivoIndiceHash::sobreEscribirBloque(BloqueIndice* bloqueModif)
		{
			//TODO: Sobre-escribir datos del bloque usando el ComuDatos
			//para acceder a la capa fisica.
		}

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
	ArchivoIndiceSecundario::ArchivoIndiceSecundario(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, t_indice tipoIndice):
		ArchivoIndiceArbol(tamNodo, nombreArchivo, tipoIndice)
	{			 
			this->tamanioArray = tamBloqueLista/sizeof(int); 
			this->nombreArchivoLista = nombreArchivo + ".list";
	}
	
	ArchivoIndiceSecundario::~ArchivoIndiceSecundario(){ }
	
	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////
	void ArchivoIndiceSecundario::sobreEscribirListaClavesP(unsigned int posicion, SetClaves* setClaves)
	{
		/*
		//Variables
		int arrayRef[this->tamanioArray]; 
		int arrayAnt[this->tamanioArray];
		int i = 0;
		int parciales = 0;
		bool fin = false;
		bool haySiguiente;
		int posicionSiguiente = posicion;
		
		//Alamceno la cantidad de claves
		int totales = listaClaves->getCantidadNodos();
		 
		listaClaves->primero();
		while(!fin){
				//Se posiciona en el primer registro de la lista a sobreescribir
				this->archivoLista->posicionarse(posicionSiguiente);
				
				while( (i<tamanioArray-2) && (!listaClaves->fin()) ){
					i++;
					parciales++;
					arrayRef[i] = *((int*)listaClaves->obtenerDato());
					listaClaves->siguiente();
				}
				//Almaceno la cantidad de claves de la lista
				arrayRef[0] = i;
				
				//Chequeo si tengo que seguir escribiendo en otro bloque
				if(parciales<totales){
					
					//Obtener la proxima posicion donde leer en el archivo, si es que
					// hay definida una
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
						//Caso en el que ya esta definido donde debe seguir la lista
						posicionSiguiente = arrayAnt[tamanioArray-1];
					
					else{
						//Si no hay otro bloque definido sigo al final del archivo
						int posBck = this->archivoLista->posicion();
						this->archivoLista->posicionarseFin();
						posicionSiguiente = this->archivoLista->posicion();
						this->archivoLista->posicionarse(posBck); 			
					}								
				}		
				else{
					//Almaceno que no hay mas bloques de la lista
					posicionSiguiente = -1; 
					fin = true;
				}
				//Almaceno el bloque donde se va a continuar escribiendo
				arrayRef[tamanioArray-1] = posicionSiguiente;
				//Escribo el bloque en el archivo
				this->archivoLista->escribir(arrayRef);
				this->cantEscriturasLista++;
				//Se posiciona en el proximo registro donde escribir lo que falta de lista
				i = 0;	
		}
		*/
	}
	
	SetClaves* ArchivoIndiceSecundario::obtenerLstClavesP(unsigned int posicion)
	{
		//TODO Revisar implementacion.
		
		/*
		//Variables
		Lista* listaClaves = new Lista;
		int arrayRef[this->tamanioArray];
		int cantClaves;
		bool fin = false;
		
		//Se posiciona al final del archivo
		this->archivoLista->posicionarse(posicion);
		
		while(!fin){
			
			//Obtengo la cantidad de claves
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
		*/
		
		return NULL;
	}

	unsigned int ArchivoIndiceSecundario::grabarNuevaLstClavesP(SetClaves* setClaves)
	{
		/*
		//Variables
		int arrayRef[this->tamanioArray]; 
		int i = 0;
		int parciales = 0;
		bool fin = false;
		
		//Me posiciono al final del archivo
		this->archivoLista->posicionarseFin();
		
		//Almaceno la posicion donde se grabo para devolverla
		int posicion = this->archivoLista->posicion();
		
		//Almaceno la cantidad de claves
		int totales = listaClaves->getCantidadNodos();
		 
		listaClaves->primero();
		while(!fin){
				
				while( (i<tamanioArray-2) && (!listaClaves->fin()) ){
					i++;
					parciales++;
					arrayRef[i] = *((int*)listaClaves->obtenerDato());
					listaClaves->siguiente();
				}
				//Almaceno la cantidad de claves de la lista		
				arrayRef[0] = i;
				
				//Chequeo si tengo que seguir escribiendo en otro bloque
				if(parciales<totales){
					//Almaceno el bloque donde se va a continuar escribiendo
					arrayRef[tamanioArray-1] = this->archivoLista->posicion()+1;	
				}		
				else{
					//Almaceno que no hay un bloque donde continuar
					arrayRef[tamanioArray-1] = -1; 
					fin = true;
				}
				//Escribo el bloque en el archivo
				this->archivoLista->escribir(arrayRef);
				this->cantEscriturasLista++;
				i = 0;
		}
		
		return posicion;
		*/
		
		return 0;
	}

	
void ArchivoIndiceSecundario::exportar(ostream &archivoTexto,int posicion){
	/*
	ListaClaves* listaClaves;
	Lista* listaClavesPrim;
	ClaveCadena* claveLeer; 
	//Leo el nodo
	
	Nodo* nodo = new Nodo(this,posicion);

	//Imprime el header del nodo
	archivoTexto<<"Pos. de Archivo: "<<posicion<<endl;
	listaClaves = nodo->obtenerClaves();
	archivoTexto<<"Hijo Izquierdo: "<<nodo->getHijoIzq()<<endl;
	archivoTexto<<"Nivel: "<<nodo->getNivel()<<endl;
	
	//Recorro la lista de claves
	listaClaves->primero();
	while(!listaClaves->fin()){
		claveLeer = static_cast<ClaveCadena*>(listaClaves->obtenerDato());
		claveLeer->imprimir(archivoTexto);
		
		archivoTexto<<"Lista de Claves Primarias:"<<endl;
		//Leer la lista de claves primarias del nombre
		listaClavesPrim = this->obtenerLstClavesP(claveLeer->getRefListaPrim());
		
		//Recorrer la lista de claves primarias imprimiendo
		listaClavesPrim->primero();
		while(!listaClavesPrim->fin()){
			archivoTexto<<*(int*)listaClavesPrim->obtenerDato()<<endl;
			listaClavesPrim->siguiente();	
		}
		
		//Borrar la lista
		listaClavesPrim->primero();
		while(!listaClavesPrim->fin()){
			delete static_cast<int*>(listaClavesPrim->obtenerDato());
			listaClavesPrim->siguiente();	
		}
		delete listaClavesPrim;
		
		
		listaClaves->siguiente();	
	}
	archivoTexto<<"------------------------------"<<endl;
	delete nodo;
	
	*/
}