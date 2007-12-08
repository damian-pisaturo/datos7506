///////////////////////////////////////////////////////////////////////////
//	Archivo   : ArchivoMaestro.cpp
//  Namespace : Common
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clase ArchivoMaestro.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#include "ArchivoMaestro.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoMaestro
//			(Permite el manejo del archivo de control maestro de la 
//			aplicacion. El archivo maestro almacena los datos de todos 
//			los tipos definidos, sus definiciones y estructuras).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	ArchivoMaestro::ArchivoMaestro():
		archivo(NOM_ARCHIVO_MAESTRO, *(int*)MASTER_MAGIC_NUMBER)
	{
		this->magicNumber      = 97;
		this->infoRegistros    = NULL;
		this->nombresAtributos = NULL;
		this->nombresClaves    = NULL;
		this->tiposIndices     = NULL;
	}
	
	ArchivoMaestro::~ArchivoMaestro()
	{
		if (this->infoRegistros) 
			delete this->infoRegistros;
		
		if (this->nombresAtributos)
			delete this->nombresAtributos;
		
		if (this->nombresClaves)
			delete this->nombresClaves;
		
		if (this->tiposIndices)
			delete this->tiposIndices;
	}
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		char ArchivoMaestro::buscarTipo(const string& nombreTipo)
		{
			int idTipo = this->hashNombreTipo(nombreTipo);
			return this->buscarTipo(idTipo);
		}
		
		char ArchivoMaestro::agregarTipo(const DefinicionTipo& tipo)
		{
			char resultado              = ArchivoMaestro::OK;
			unsigned int offsetProxTipo = 0;
			streampos posAuxOffset      = 0;
			
			// Estructuras que seran persistidas en el
			// archivo maestro.
			ListaInfoRegistro infoRegistro;
			ListaTiposIndices tiposIndices;
			
			// ID unico del tipo a insertar.
			int idTipo = this->hashNombreTipo(tipo.nombreTipo);
			
			// Busqueda del tipo por su numero de ID.
			resultado = this->buscarTipo(idTipo);
			
			// Si el tipo no se encontraba definido, se lo inserta
			if (resultado == ArchivoMaestro::TIPO_INEXISTENTE){
				
				// Se valida la estructura del tipo a insertar
				resultado = this->validarTipo(tipo);
				if (resultado == ArchivoMaestro::OK){
					this->archivo.clear();
					this->archivo.end();
					
					// Header tipo
					this->archivo << idTipo;
					posAuxOffset = this->archivo.tellg();
					
					this->archivo << offsetProxTipo;
					this->archivo << tipo.nombreTipo;
					
					offsetProxTipo += tipo.nombreTipo.size() + 1;
	
					// ListaInfoRegistro
					this->armarListaInfoRegistro(tipo, infoRegistro);
					offsetProxTipo += this->almacenarListaInfoRegistro(infoRegistro);
					
					// ListaTipoIndices
					this->armarListaTiposIndices(tipo, tiposIndices);
					offsetProxTipo += this->almacenarListaTiposIndices(tiposIndices);
					
					// ListaNombresAtributos
					offsetProxTipo += this->almacenarListaNombresAtributos(*tipo.nombresAtributos);

					// Offset al proximo tipo.
					this->archivo.seekg(posAuxOffset, ios_base::beg);
					this->archivo << offsetProxTipo;
				}
			}
		
			return resultado;
		}
		
		char ArchivoMaestro::agregarTipo(const ListaDefinicionesTipos& tipos)
		{
			char resultado = ArchivoMaestro::OK;
			ListaDefinicionesTipos::const_iterator iter;
			
			for (iter = tipos.begin(); (iter != tipos.end()) && (resultado == ArchivoMaestro::OK); ++iter)
				resultado = agregarTipo(*iter);
			
			return resultado;			
		}
				
		char ArchivoMaestro::agregarTipo(const string& nomArchivoDefiniciones)
		{
			ParserDefiniciones parser(nomArchivoDefiniciones);
			parser.parsearDefiniciones();
			
			return (this->agregarTipo(parser.getDefinicionesTipos()));
		}
		
		char ArchivoMaestro::recuperarTipo(const string& nombreTipo)
		{
			streampos offsetTipo = 0;
			
			// Busqueda del tipo por su nombre.
			char resultado = this->buscarTipo(nombreTipo);
						
			// Si el tipo no existe, se retorna TIPO_INEXISTENTE
			if (resultado == ArchivoMaestro::TIPO_EXISTENTE){
				offsetTipo = sizeof(int) + nombreTipo.size() + 1;
				this->archivo.seekg(offsetTipo, ios_base::cur);
				
				this->recuperarListaInfoRegistro();
				this->recuperarListaTiposIndices();
				this->recuperarListaNombresAtributos();
				
				resultado = ArchivoMaestro::OK;
			}
			
			return resultado;
		}
		
	///////////////////////////////////////////////////////////////////////
	// Metodos privados
	///////////////////////////////////////////////////////////////////////
		unsigned long ArchivoMaestro::hashNombreTipo(const string& nombreTipo) const
		{
			unsigned long hash = 5381;
	        string::const_iterator iter = nombreTipo.begin();
	        
	        while (iter++ != nombreTipo.end())
	            hash = ((hash << 5) + hash) + (int)*iter; 

	        return hash;			
		}
		
		char ArchivoMaestro::buscarTipo(int idTipo)
		{
			bool encontrado = false;
			int id = 0, offsetProxTipo = 0;
			char resultado = ArchivoMaestro::TIPO_INEXISTENTE;
			
			if (this->archivo.size() > sizeof(int)){
				this->archivo.rewind();
				
				while ( (!encontrado) && (!this->archivo.eof()) ){
					this->archivo >> id;
					
					if (id == idTipo) encontrado = true;
					else{
						this->archivo >> offsetProxTipo;
						this->archivo.seekg(offsetProxTipo, ios_base::cur);
					}
				}				
			}
			
			if (encontrado) resultado = ArchivoMaestro::TIPO_EXISTENTE;
			
			return resultado;			
		}
		
		string ArchivoMaestro::generarNombreArchivo(string nombreTipo)
		{
			stringstream nombreArchivo;
			time_t rawtime;
			struct tm * timeinfo = NULL;
			char buffer[16];
			
			time (&rawtime);
			timeinfo = localtime(&rawtime);
			strftime(buffer, 16, "%d%m%Y", timeinfo);
			
			transform(nombreTipo.begin(), nombreTipo.end(), nombreTipo.begin(), (int (*)(int))std::tolower);
			
			nombreArchivo << nombreTipo;
			nombreArchivo << "FP7506_";
			nombreArchivo << buffer;
			nombreArchivo << this->magicNumber++;
			
			return nombreArchivo.str();
		}
		
		void ArchivoMaestro::armarListaInfoRegistro(const DefinicionTipo& tipo, ListaInfoRegistro& listaInfoRegistro) const
		{
			short tamRegistro = 0;			
			
			// Iteradores para las estructuras.
			VectorTiposAtributos::const_iterator iterVTA; 
			ListaNombresAtributos::const_iterator iterLNA;
			ListaNombresClaves::const_iterator iterLNC; 
			
			// Puntero auxiliar a lista de los nombres de las claves 
			// primarias.
			ListaNombresClaves* nombresClaves = NULL;
			
			// Nodo de la lista a devolver.
			NodoInfoRegistro nodoIR;
			
			// Construccion del primer nodo
			nodoIR.tipoDato = tipo.tipoOrganizacion; // Organizacion de los registros (RVB o RF)
			
			if (tipo.tipoOrganizacion == TipoDatos::TIPO_FIJO){
				// Si la organizacion es RF, se calcula el tamaño del registro.
				tamRegistro = this->calcularTamRegistro(*tipo.tiposAtributos);
				nodoIR.tamRegistro = tamRegistro;
			}else nodoIR.tamRegistro = 0;
		
			// Cantidad de campos que componen la clave primaria.
			nodoIR.cantClaves  = tipo.datosIndicesGriegos->vNomClaves->size();
			
			listaInfoRegistro.push_back(nodoIR);			
			nombresClaves = *(tipo.datosIndicesGriegos->vNomClaves->begin());
			
			for (iterVTA = tipo.tiposAtributos->begin(), iterLNA = tipo.nombresAtributos->begin(); 
					iterVTA != tipo.tiposAtributos->end(); ++iterVTA, ++iterLNA){
				nodoIR.tipoDato = (*iterVTA);
				nodoIR.esPk = false;
				
				for (iterLNC = nombresClaves->begin(); iterLNC != nombresClaves->end(); ++iterLNC)						
					if (*iterLNC == *iterLNA) nodoIR.esPk = true;
				
				listaInfoRegistro.push_back(nodoIR);
			}	
				
		}
		
		streampos ArchivoMaestro::almacenarListaInfoRegistro(const ListaInfoRegistro& listaInfoRegistro)
		{
			streampos offsetInicial = this->archivo.tellg();
			unsigned char cantNodos =  listaInfoRegistro.size();
			ListaInfoRegistro::const_iterator iter;
			NodoInfoRegistro* nodoInfoReg = NULL;
			
			// Se almacena la cantidad de nodos.
			this->archivo << cantNodos;
			
			// Se almacenan todos los nodos de la lista en el archivo.
			for (iter = listaInfoRegistro.begin(); iter != listaInfoRegistro.end(); ++iter)
			{
				nodoInfoReg = new NodoInfoRegistro(*iter);
				
				this->archivo << nodoInfoReg->tipoDato;
				this->archivo << nodoInfoReg->tamRegistro;
				this->archivo << nodoInfoReg->esPk;
				this->archivo << nodoInfoReg->cantClaves;
				
				delete nodoInfoReg;
			}
			
			return (this->archivo.tellg() - offsetInicial);
		}
		
		void ArchivoMaestro::armarListaTiposIndices(const DefinicionTipo& tipo, ListaTiposIndices& listaTiposIndices)
		{
			// Nodo de la lista a crear.
			NodoListaIndices* nodoLI = NULL;
			
			bool nombreEncontrado = false;
			
			// Iteradores a las estructuras contenidas en la 
			// ListaTiposIndices a crear.
			ListaNombresClaves::const_iterator iterLNC;
			ListaNombresAtributos::const_iterator iterLNA;
			VectorTiposAtributos::const_iterator iterVTA;
			VectorNombresClaves::const_iterator iterVNC;
			VectorTamaniosBloques::const_iterator iterVTB;
			VectorTiposIndices::const_iterator iterVTI;
			
			// Puntero auxiliar  lista de nombres de los campos
			// que constituyen la clave primaria del indice griego.
			ListaNombresClaves* nombresClaves = NULL;
			
			// Indice griego (primario)
			nodoLI = new NodoListaIndices();
			nodoLI->infoTipoIndice.tipoIndice     = TipoIndices::GRIEGO;
			nodoLI->infoTipoIndice.tipoEstructura = *tipo.datosIndicesGriegos->vTiposIndices->begin();
			nodoLI->infoTipoIndice.tamanioBloque  = *tipo.datosIndicesGriegos->vTamBloques->begin();
			
			nombresClaves = *tipo.datosIndicesGriegos->vNomClaves->begin();
			if (nombresClaves->size() > 1)
				nodoLI->infoTipoIndice.tipoClave = TipoDatos::TIPO_COMPUESTO;
			else{
				nombreEncontrado = false;
				for (iterLNA = tipo.nombresAtributos->begin(), iterVTA = tipo.tiposAtributos->begin();
					(!nombreEncontrado) && (iterLNA != tipo.nombresAtributos->end()); ++iterLNA, ++iterVTA){
					
					if (*nombresClaves->begin() == *iterLNA){
						nombreEncontrado = true;
						nodoLI->infoTipoIndice.tipoClave = *iterVTA;
					}					
				}
			}
			
			nodoLI->infoTipoIndice.nombreArchivo = this->generarNombreArchivo(tipo.nombreTipo);
			*nodoLI->listaNombresClaves = *nombresClaves;
			
			for (iterLNC = nombresClaves->begin(); iterLNC != nombresClaves->end(); ++iterLNC){
				for (iterLNA = tipo.nombresAtributos->begin(), iterVTA = tipo.tiposAtributos->begin();
					iterLNA != tipo.nombresAtributos->end(); ++iterLNA, ++iterVTA){
					
					if (*iterLNC == *iterLNA) nodoLI->listaTipos->push_back(*iterVTA);
				}
			}
			
			listaTiposIndices.push_back(*nodoLI);
			delete nodoLI;
			nodoLI = NULL;
			
			// Indices romanos (secundarios)
			for (iterVNC = tipo.datosIndicesRomanos->vNomClaves->begin(), 
				iterVTB  = tipo.datosIndicesRomanos->vTamBloques->begin(), 
				iterVTI  = tipo.datosIndicesRomanos->vTiposIndices->begin();
				iterVTB != tipo.datosIndicesRomanos->vTamBloques->end(); ++iterVNC, ++iterVTB, ++iterVTI){
				
				nodoLI = new NodoListaIndices();
				nodoLI->infoTipoIndice.tipoIndice = TipoIndices::ROMANO;
				nodoLI->infoTipoIndice.tipoEstructura = *iterVTI;
				nodoLI->infoTipoIndice.tamanioBloque  = *iterVTB;				
				
				nombresClaves = (*iterVNC);
				if (nombresClaves->size() > 1)
					nodoLI->infoTipoIndice.tipoClave = TipoDatos::TIPO_COMPUESTO;
				else{
					nombreEncontrado = false;
					for (iterLNA = tipo.nombresAtributos->begin(), iterVTA = tipo.tiposAtributos->begin();
						(!nombreEncontrado) && (iterLNA != tipo.nombresAtributos->end()); ++iterLNA, ++iterVTA){

						if (*nombresClaves->begin() == *iterLNA){
							nombreEncontrado = true;
							nodoLI->infoTipoIndice.tipoClave = *iterVTA;
						}						
					}
				}
				
				*nodoLI->listaNombresClaves = *nombresClaves;
				nodoLI->infoTipoIndice.nombreArchivo = this->generarNombreArchivo(tipo.nombreTipo);
				
				for (iterLNC = nombresClaves->begin(); iterLNC != nombresClaves->end(); ++iterLNC){
					for (iterLNA = tipo.nombresAtributos->begin(), iterVTA = tipo.tiposAtributos->begin();
						iterLNA != tipo.nombresAtributos->end(); ++iterLNA, ++iterVTA){
						
						if (*iterLNC == *iterLNA) nodoLI->listaTipos->push_back(*iterVTA);
					}
				}
				
				listaTiposIndices.push_back(*nodoLI);
				delete nodoLI;
				nodoLI = NULL;
			}
		}
		
		streampos ArchivoMaestro::almacenarListaTiposIndices(const ListaTiposIndices& listaTiposIndices)
		{
			streampos offsetInicial = this->archivo.tellg();
			unsigned char cantNodos =  listaTiposIndices.size();
			ListaTiposIndices::const_iterator iter;
			NodoListaIndices* nodoLI = NULL;
			
			// Se almacena la cantidad de nodos.
			this->archivo << cantNodos;
			
			// Se almacenan los nodos de la lista
			for (iter = listaTiposIndices.begin(); iter != listaTiposIndices.end(); ++iter){
				nodoLI = new NodoListaIndices(*iter);
				
				this->archivo << nodoLI->infoTipoIndice.tipoIndice;
				this->archivo << nodoLI->infoTipoIndice.tipoEstructura;
				this->archivo << nodoLI->infoTipoIndice.tamanioBloque;
				this->archivo << nodoLI->infoTipoIndice.tipoClave;
				this->archivo << nodoLI->infoTipoIndice.nombreArchivo;				
				
				this->almacenarListaNombresClaves(*nodoLI->listaNombresClaves);
				this->almacenarListaTipos(*nodoLI->listaTipos);
				
				delete nodoLI;
				nodoLI = NULL;
			}
				
			return (this->archivo.tellg() - offsetInicial);
		}
		
		streampos ArchivoMaestro::almacenarListaNombresAtributos(const ListaNombresAtributos& listaNombresAtributos)
		{
			streampos offsetInicial = this->archivo.tellg();
			unsigned char cantNodos =  listaNombresAtributos.size();
			ListaNombresAtributos::const_iterator iter;
			
			// Se almacena la cantidad de nodos.
			this->archivo << cantNodos;
			
			// Se almacenan todos los nodos de la lista en el archivo.
			for (iter = listaNombresAtributos.begin(); iter != listaNombresAtributos.end(); ++iter)
				this->archivo << (*iter);
			
			return (this->archivo.tellg() - offsetInicial);
		}
		
		streampos ArchivoMaestro::almacenarListaNombresClaves(const ListaNombresClaves& listaNombresClaves)
		{
			streampos offsetInicial = this->archivo.tellg();
			unsigned char cantNodos =  listaNombresClaves.size();
			ListaNombresClaves::const_iterator iter;
			
			// Se almacena la cantidad de nodos.
			this->archivo << cantNodos;
			// Se almacenan todos los nodos de la lista en el archivo.
			for (iter = listaNombresClaves.begin(); iter != listaNombresClaves.end(); ++iter)
				this->archivo << (*iter);
			
			return (this->archivo.tellg() - offsetInicial);
		}
		
		streampos ArchivoMaestro::almacenarListaTipos(const ListaTipos& listaTipos)
		{
			streampos offsetInicial = this->archivo.tellg();
			unsigned char cantNodos =  listaTipos.size();
			ListaTipos::const_iterator iter;
			
			// Se almacena la cantidad de nodos.
			this->archivo << cantNodos;
			
			// Se almacenan todos los nodos de la lista en el archivo.
			for (iter = listaTipos.begin(); iter != listaTipos.end(); ++iter)
				this->archivo << (*iter);
			
			return (this->archivo.tellg() - offsetInicial);
		}
		
		void ArchivoMaestro::recuperarListaInfoRegistro()
		{
			unsigned char cantNodos       = 0;
			NodoInfoRegistro* nodoInfoReg = NULL;
			
			// Se crea una nueva lista, eliminando la existente.
			if (this->infoRegistros){
				delete this->infoRegistros;
				this->infoRegistros = NULL;
			}			
			this->infoRegistros  = new ListaInfoRegistro(); 
			
			// Se obtiene la cantidad de nodos de la lista.
			this->archivo >> cantNodos;
			
			for (unsigned char i = 0; i < cantNodos; i++){
				nodoInfoReg = new NodoInfoRegistro();
				
				this->archivo >> nodoInfoReg->tipoDato;
				this->archivo >> nodoInfoReg->tamRegistro;
				this->archivo >> nodoInfoReg->esPk;
				this->archivo >> nodoInfoReg->cantClaves;
				
				this->infoRegistros->push_back(*nodoInfoReg);
				delete nodoInfoReg;				
			}
		}
		
		void ArchivoMaestro::recuperarListaTiposIndices()
		{
			unsigned char cantNodos = 0;
			NodoListaIndices* nodoLI = NULL;
			
			if  (this->tiposIndices){
				delete this->tiposIndices;
				this->tiposIndices = NULL;
			}
			
			this->tiposIndices = new ListaTiposIndices();
			
			// Se obtiene la cantidad de nodos.
			this->archivo >> cantNodos;
			
			// Se obtienen todos los nodos de la lista
			for (unsigned char i = 0; i < cantNodos; i++){
				nodoLI = new NodoListaIndices();
				
				this->archivo >> nodoLI->infoTipoIndice.tipoIndice;
				this->archivo >> nodoLI->infoTipoIndice.tipoEstructura;
				this->archivo >> nodoLI->infoTipoIndice.tamanioBloque;
				this->archivo >> nodoLI->infoTipoIndice.tipoClave;
				this->archivo >> nodoLI->infoTipoIndice.nombreArchivo;				
				
				this->recuperarListaNombresClaves(*nodoLI->listaNombresClaves);
				this->recuperarListaTipos(*nodoLI->listaTipos);
				
				if (nodoLI->infoTipoIndice.tipoIndice == TipoIndices::GRIEGO){
					if  (this->nombresClaves){
						delete this->nombresClaves;
						this->nombresClaves = NULL;
					}
					
					this->nombresClaves = new ListaNombresClaves(*nodoLI->listaNombresClaves);
				}
				
				this->tiposIndices->push_back(*nodoLI);
				delete nodoLI;
			}
		}
		
		void ArchivoMaestro::recuperarListaNombresClaves(ListaNombresClaves& nombresClaves)
		{
			unsigned char cantNodos = 0;
			string tipo;
			
			// Se obtiene la cantidad de nodos.
			this->archivo >> cantNodos;
			
			// Se obtienen todos los nodos de la lista en el archivo.
			for (unsigned char i = 0; i < cantNodos; i++){
				this->archivo >> tipo;
				nombresClaves.push_back(tipo);
			}
			
		}
		
		void ArchivoMaestro::recuperarListaNombresAtributos()
		{
			unsigned char cantNodos = 0;
			string nombreAtributo;
			
			if  (this->nombresAtributos){
				delete this->nombresAtributos;
				this->nombresAtributos = NULL;
			}
			
			this->nombresAtributos = new ListaNombresAtributos();
			
						
			// Se obtiene la cantidad de nodos.
			this->archivo >> cantNodos;
			
			// Se obtienen todos los nodos de la lista en el archivo.
			for (unsigned char i = 0; i < cantNodos; i++){
				this->archivo >> nombreAtributo;
				this->nombresAtributos->push_back(nombreAtributo);
			}			
		}
		
		void ArchivoMaestro::recuperarListaTipos(ListaTipos& listaTipos)
		{
			unsigned char cantNodos = 0;
			unsigned char tipo = 0;
			
			// Se obtiene la cantidad de nodos.
			this->archivo >> cantNodos;
			
			// Se obtienen todos los nodos de la lista en el archivo.
			for (unsigned char i = 0; i < cantNodos; i++){
				this->archivo >> tipo;
				listaTipos.push_back(tipo);
			}
		}
		
		unsigned short ArchivoMaestro::calcularTamRegistro(const VectorTiposAtributos& tiposAtributos) const
		{
			unsigned short tamReg = 0;
			
			for (VectorTiposAtributos::const_iterator iter = tiposAtributos.begin(); 
				(iter != tiposAtributos.end());  ++iter){
				
				switch (*iter){
					case TipoDatos::TIPO_BOOL : tamReg += sizeof(bool); break;
					case TipoDatos::TIPO_ENTERO:tamReg += sizeof(int); break;
					case TipoDatos::TIPO_SHORT: tamReg += sizeof(short); break;
					case TipoDatos::TIPO_FLOAT: tamReg += sizeof(float); break;
					case TipoDatos::TIPO_FECHA: tamReg += Tamanios::TAMANIO_FECHA; break;
					case TipoDatos::TIPO_CHAR:  tamReg += sizeof(char); break;
				}
			}
			
			return tamReg;
		}
		
		char ArchivoMaestro::validarTamBloque(const unsigned short tamBloque) const
		{
			char resultado = ArchivoMaestro::OK;
			
			if (tamBloque % 512 != 0)
				resultado = ArchivoMaestro::TAMANIO_BLOQUE_INVALIDO;
			
			return resultado;
		}
		
		char ArchivoMaestro::validarEstructuraRF(const VectorTiposAtributos& tiposAtributos) const
		{	
			char resultado = ArchivoMaestro::OK;
			
			for (VectorTiposAtributos::const_iterator iter = tiposAtributos.begin(); 
				(iter != tiposAtributos.end()) && (resultado == ArchivoMaestro::OK);  ++iter)
				if (*iter == TipoDatos::TIPO_STRING) resultado = ArchivoMaestro::ESTRUCTURA_RF_INVALIDA;
			
			return resultado;
		}
		
		char ArchivoMaestro::validarDefinicionAtributos(const ListaNombresClaves& nombresClaves, const ListaNombresAtributos& nombresAtributos) const
		{
			bool esAtributo = true;
			char resultado = ArchivoMaestro::OK;
			ListaNombresAtributos::const_iterator iterLNA;
			ListaNombresClaves::const_iterator iterLNC;
			
			for (iterLNC = nombresClaves.begin(); (iterLNC != nombresClaves.end()) && (esAtributo);
				++iterLNC){
				
				esAtributo = false;
				for (iterLNA = nombresAtributos.begin(); (iterLNA != nombresAtributos.end()) && (!esAtributo);
				++iterLNA)
					if (*iterLNA == *iterLNC) esAtributo = true;						
			}
			
			if (!esAtributo) resultado = ArchivoMaestro::ATRIBUTO_NO_DEFINIDO;
			
			return resultado;
		}
		
		char ArchivoMaestro::validarIndiceGriego(const DatosIndices& datosGriego, const ListaNombresAtributos& nombresAtributos) const
		{
			char resultado  = ArchivoMaestro::OK;
				
			ListaNombresClaves* nombresClaves = NULL;
			
			if ( (datosGriego.vNomClaves->size() != 1) || (datosGriego.vTamBloques->size() != 1) ||
				 (datosGriego.vTiposIndices->size() != 1))
				resultado = ArchivoMaestro::INDICE_GRIEGO_AMBIGUO;
			else{
				resultado = this->validarTamBloque(*datosGriego.vTamBloques->begin());
				
				if (resultado == ArchivoMaestro::OK){
					nombresClaves = *(datosGriego.vNomClaves->begin());
					resultado = this->validarDefinicionAtributos(*nombresClaves, nombresAtributos);
				}
			}
			
			return resultado;
		}
		
		char ArchivoMaestro::validarIndicesRomanos(const DatosIndices& datosRomanos, const ListaNombresAtributos& nombresAtributos) const
		{
			char resultado  = ArchivoMaestro::OK;
			VectorNombresClaves::const_iterator iterVNC;
			VectorTamaniosBloques::const_iterator iterVTB;
			
			for (iterVNC = datosRomanos.vNomClaves->begin(), iterVTB = datosRomanos.vTamBloques->begin();
				(iterVTB != datosRomanos.vTamBloques->end()) && (resultado == ArchivoMaestro::OK); ++iterVNC, ++iterVTB){
				resultado = this->validarTamBloque(*iterVTB);
				
				if (resultado == ArchivoMaestro::OK)
					resultado = this->validarDefinicionAtributos(*(*iterVNC), nombresAtributos);
			}
			
			return resultado;
		}
		
		char ArchivoMaestro::validarTipo(const DefinicionTipo& tipo) const
		{
			char resultado = ArchivoMaestro::OK;
			
			if (tipo.tipoOrganizacion == TipoDatos::TIPO_FIJO)
				resultado = this->validarEstructuraRF(*tipo.tiposAtributos);
			
			if (resultado == ArchivoMaestro::OK){
				resultado = this->validarIndiceGriego(*tipo.datosIndicesGriegos, *tipo.nombresAtributos);
				if (resultado == ArchivoMaestro::OK)
					resultado = this->validarIndicesRomanos(*tipo.datosIndicesRomanos, *tipo.nombresAtributos);
			}
			
			return resultado;
		}

