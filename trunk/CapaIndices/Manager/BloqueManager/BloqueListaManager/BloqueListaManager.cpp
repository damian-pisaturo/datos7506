///////////////////////////////////////////////////////////////////////////
//	Archivo   : BloqueListaManager.cpp
//  Namespace : CapaIndices
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clase BloqueListaManager.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#include "BloqueListaManager.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: BloqueListaManager
//			(Permite la escritura, lectura, modificacion y eliminacion de
//			bloques de listas de claves primarias indexados en indices 
//			secundarios, accediendo remotamente a la Capa Fisica).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		BloqueListaManager::BloqueListaManager(unsigned int tamanioBloqueLista, string nombreArchivo):
			BloqueManager(tamanioBloqueLista, nombreArchivo + ".lst")
		{ }
		
		BloqueListaManager::~BloqueListaManager()
		{ }
		
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////

		int BloqueListaManager::leerBloqueDatos(unsigned int numLista, void* listaLeida)
		{
			unsigned int cantClaves = 0; //Cantidad de claves contenidas en la lista.

			// Instancia del pipe		
			ComuDatos* pipe = this->instanciarPipe();

			// Parametros de inicializacion de la Capa Fisica para
			// leer un nodo de disco.
			pipe->agregarParametro(OperacionesCapas::FISICA_LEER_LISTA, 0); // Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); // Nombre de archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); // Tamaño del bloque en disco.
			pipe->agregarParametro(numLista, 3); // Numero de nodo a leer dentro del archivo.

			// Se lanza el proceso de la capa fisica. 
			pipe->lanzar();
			
			// Obtener tamaño de la lista
			pipe->leer(&cantClaves);
			
			*(unsigned int*)listaLeida = cantClaves;
			
			if (cantClaves > 0)
				//Se obtiene la lista solicitada.
				pipe->leer(this->getTamanioBloque() - sizeof(unsigned int), (char*)(listaLeida + sizeof(unsigned int));

			if (pipe)
				delete pipe;

			return cantClaves;
		}

		int BloqueListaManager::escribirBloqueDatos(const void* listaNueva)
		{
			int numLista = 0; // Numero de bloque donde fue almacenada la lista en disco.
			
			//Instancia del pipe
			ComuDatos* pipe = this->instanciarPipe();

			//Parametros de inicializacion de la Capa Fisica para
			//escribir un nodo en disco.		
			pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR_LISTA, 0); //Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre del archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bloque en disco.
		
			//Se lanza el proceso de la capa fisica. 
			pipe->lanzar();
			
			//Grabar la cantidad de claves en el archivo.
			pipe->escribir(*(unsigned int*)listaNueva);
			
			//Grabar el bloque en el archivo.
			pipe->escribir((char*)(listaNueva + sizeof(unsigned int)), this->getTamanioBloque() - sizeof(unsigned int));

			//Obtener nueva posicion del bloque en el archivo. 
			pipe->leer(&numLista);

			if (pipe)
				delete pipe;
			
			return numLista;
		}

		
		int BloqueListaManager::escribirBloqueDatos(unsigned short numLista, const void* listaModif)
		{
			int resultado = 0;

			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe();

			//Parametros de inicializacion de la Capa Fisica.
			pipe->agregarParametro(OperacionesCapas::FISICA_MODIFICAR_LISTA, 0); //Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.
			pipe->agregarParametro(numLista, 3); //Numero de lista a sobre-escribir.
				
			//Se lanza el proceso de la capa fisica. 
			pipe->lanzar();
	
			//Grabar la cantidad de claves en el archivo.
			pipe->escribir(*(unsigned int*)listaModif);
			
			// Grabar la lista a disco.
			pipe->escribir((char*)(listaModif + sizeof(unsigned int)), this->getTamanioBloque() - sizeof(unsigned int));

			//Solicitar resultado de la comunicacion con la 
			//capa fisica.
			pipe->leer(&resultado);

			if (pipe)
				delete pipe;

			return resultado;
		}

		int BloqueListaManager::eliminarBloqueDatos(unsigned int numLista)
		{
			return 0;
		}
		

		int BloqueListaManager::buscarEspacioLibre(void* bloque, unsigned int espLibre)
		{
			return 0;
		}
