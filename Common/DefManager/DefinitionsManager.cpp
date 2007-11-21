#include "DefinitionsManager.h"

//se inicializa la instancia estática de esta clase
DefinitionsManager DefinitionsManager::instance;

DefinitionsManager::DefinitionsManager() {
	
	this->cargarDefiniciones();
	
}

DefinitionsManager::~DefinitionsManager() {
	
	for (MapaTiposAtributos::iterator iter = this->mapaTiposAtributos.begin();
		iter != this->mapaTiposAtributos.end(); ++iter)
		delete iter->second;
	
	for (MapaNombresAtributos::iterator iter = this->mapaNombresAtributos.begin();
		iter != this->mapaNombresAtributos.end(); ++iter)
		delete iter->second;
	
	ListaTiposIndices* listaTiposIndices;
	
	for (MapaTiposIndices::iterator iter = this->mapaTiposIndices.begin();
		iter != this->mapaTiposIndices.end(); ++iter) {
		listaTiposIndices = iter->second;
		for (ListaTiposIndices::iterator iterLista = listaTiposIndices->begin();
			iterLista != listaTiposIndices->end(); ++iterLista) {
			delete (*iterLista).listaNombresClaves;
			delete (*iterLista).listaTipos;
		}
		delete listaTiposIndices;
	}

}


void DefinitionsManager::cargarDefiniciones() {

	this->cargarNombresAtributos();
	
	this->cargarTiposAtributos();
	
	this->cargarTiposIndices();

}


void DefinitionsManager::cargarNombresAtributos() {
	
	//CARGO LOS NOMBRES DE LOS ATRIBUTOS DEL TIPO "PERSONA"
	
	ListaNombresAtributos* listaNombresAtributos = new ListaNombresAtributos();
	
	listaNombresAtributos->push_back("DNI");
	listaNombresAtributos->push_back("NOMBRE");
	listaNombresAtributos->push_back("FECHANACIMIENTO");
	
	this->mapaNombresAtributos["PERSONA"] = listaNombresAtributos;
	
	
	//CARGO LOS NOMBRES DE LOS ATRIBUTOS DEL TIPO "PELICULA"
		
	listaNombresAtributos = new ListaNombresAtributos();
	
	listaNombresAtributos->push_back("TITULO");
	listaNombresAtributos->push_back("DIRECTOR");
	listaNombresAtributos->push_back("GENERO");
	
	this->mapaNombresAtributos["PELICULA"] = listaNombresAtributos;
}


void DefinitionsManager::cargarTiposAtributos() {

	ListaTiposAtributos* listaTiposAtributos = NULL;
	nodoLista nodo;
	
	//CARGO LAS DEFINICIONES DEL TIPO "PERSONA"
	
	listaTiposAtributos = new ListaTiposAtributos();
	
	//El primer nodo de la lista indica si el registro es variable o no.
	nodo.tipo = TipoDatos::TIPO_VARIABLE;
	nodo.pk = "0";
	nodo.cantClaves = 1;
	
	listaTiposAtributos->push_back(nodo);
	
	//DNI
	nodo.tipo = TipoDatos::TIPO_ENTERO;
	nodo.pk = "true";
	listaTiposAtributos->push_back(nodo);
	
	//Nombre
	nodo.tipo = TipoDatos::TIPO_STRING;
	nodo.pk = "false";
	listaTiposAtributos->push_back(nodo);
	
	//FechaNacimiento
	nodo.tipo = TipoDatos::TIPO_FECHA;
	nodo.pk = "false";
	listaTiposAtributos->push_back(nodo);
	
	this->mapaTiposAtributos["PERSONA"] = listaTiposAtributos;
	
	
	//CARGO LAS DEFINICIONES DEL TIPO "PELICULA"

	listaTiposAtributos = new ListaTiposAtributos();
	
	//El primer nodo de la lista indica si el registro es variable o no.
	nodo.tipo = TipoDatos::TIPO_FIJO;
	nodo.pk = "34";
	nodo.cantClaves = 2;
	
	listaTiposAtributos->push_back(nodo);
	
	//Titulo (cadena de 15 carecteres)
	nodo.tipo = TipoDatos::TIPO_STRING;
	nodo.pk = "true";
	listaTiposAtributos->push_back(nodo);
	
	//Director (cadena de 10 caracteres)
	nodo.tipo = TipoDatos::TIPO_STRING;
	nodo.pk = "true";
	listaTiposAtributos->push_back(nodo);
	
	//Genero
	nodo.tipo = TipoDatos::TIPO_CHAR;
	nodo.pk = "false";
	listaTiposAtributos->push_back(nodo);
	
	this->mapaTiposAtributos["PELICULA"] = listaTiposAtributos;

	
}


void DefinitionsManager::cargarTiposIndices() {

	EstructTipoIndice estructTipoIndice;
	ListaNombresClaves* listaNombresClaves = NULL;
	ListaTipos* listaTipos = NULL;
	NodoListaIndices nodoListaIndices;
	ListaTiposIndices* listaTiposIndices = NULL;

	//CARGO LAS DEFINICIONES DE LOS INDICES DEL TIPO "PERSONA"
	
	//INDICE GRIEGO (HASH)
	estructTipoIndice.tipoIndice = TipoIndices::GRIEGO;
	estructTipoIndice.tipoEstructura = TipoIndices::HASH;
	estructTipoIndice.tamanioBloque = 4096;
	estructTipoIndice.tipoClave = TipoDatos::TIPO_ENTERO;
	estructTipoIndice.nombreArchivo = "persona1.hash";
	
	listaNombresClaves = new ListaNombresClaves();
	listaNombresClaves->push_back("DNI");
	
	//Se carga la lista de los nombres de los campos de la clave primaria
	this->mapaNombresClavesPrimarias["PERSONA"] = listaNombresClaves;
	
	listaTipos = new ListaTipos();
	int tipo = TipoDatos::TIPO_ENTERO;
	listaTipos->push_back(tipo);
	
	nodoListaIndices.estructTipoIndice = estructTipoIndice;
	nodoListaIndices.listaNombresClaves = listaNombresClaves;
	nodoListaIndices.listaTipos = listaTipos;
	
	listaTiposIndices = new ListaTiposIndices();
	listaTiposIndices->push_back(nodoListaIndices);
	
	
	//INDICE ROMANO (ARBOLB*)
	estructTipoIndice.tipoIndice = TipoIndices::ROMANO;
	estructTipoIndice.tipoEstructura = TipoIndices::ARBOL_BS;
	estructTipoIndice.tamanioBloque = 1024;
	estructTipoIndice.tipoClave = TipoDatos::TIPO_COMPUESTO;
	estructTipoIndice.nombreArchivo = "persona1.bstar";
	
	listaNombresClaves = new ListaNombresClaves();
	listaNombresClaves->push_back("FECHANACIMIENTO");
	listaNombresClaves->push_back("NOMBRE");
	
	listaTipos = new ListaTipos();
	tipo = TipoDatos::TIPO_FECHA;
	listaTipos->push_back(tipo);
	tipo = TipoDatos::TIPO_STRING;
	listaTipos->push_back(tipo);
	
	nodoListaIndices.estructTipoIndice = estructTipoIndice;
	nodoListaIndices.listaNombresClaves = listaNombresClaves;
	nodoListaIndices.listaTipos = listaTipos;
	
	listaTiposIndices->push_back(nodoListaIndices);
	
	
	//INDICE ROMANO (ARBOLB+)
	estructTipoIndice.tipoIndice = TipoIndices::ROMANO;
	estructTipoIndice.tipoEstructura = TipoIndices::ARBOL_BP;
	estructTipoIndice.tamanioBloque = 2048;
	estructTipoIndice.tipoClave = TipoDatos::TIPO_COMPUESTO;
	estructTipoIndice.nombreArchivo = "persona1.bplus";
	
	listaNombresClaves = new ListaNombresClaves();
	listaNombresClaves->push_back("NOMBRE");
	listaNombresClaves->push_back("FECHANACIMIENTO");
	
	listaTipos = new ListaTipos();
	tipo = TipoDatos::TIPO_STRING;
	listaTipos->push_back(tipo);
	tipo = TipoDatos::TIPO_FECHA;
	listaTipos->push_back(tipo);
	
	nodoListaIndices.estructTipoIndice = estructTipoIndice;
	nodoListaIndices.listaNombresClaves = listaNombresClaves;
	nodoListaIndices.listaTipos = listaTipos;
	
	listaTiposIndices->push_back(nodoListaIndices);
	
	//Se inserta la lista de tipos de indices en el mapa de tipos de indices
	this->mapaTiposIndices["PERSONA"] = listaTiposIndices;

	
	//CARGO LAS DEFINICIONES DE LOS INDICES DEL TIPO "PELICULA"
	
	//INDICE GRIEGO (ARBOLB*)
	estructTipoIndice.tipoIndice = TipoIndices::GRIEGO;
	estructTipoIndice.tipoEstructura = TipoIndices::ARBOL_BS;
	estructTipoIndice.tamanioBloque = 2048;
	estructTipoIndice.tipoClave = TipoDatos::TIPO_COMPUESTO;
	estructTipoIndice.nombreArchivo = "pelicula1.bstar";
	
	listaNombresClaves = new ListaNombresClaves();
	listaNombresClaves->push_back("TITULO");
	listaNombresClaves->push_back("DIRECTOR");
	
	this->mapaNombresClavesPrimarias["PELICULA"] = listaNombresClaves;
	
	listaTipos = new ListaTipos();
	tipo = TipoDatos::TIPO_STRING;
	listaTipos->push_back(tipo);
	listaTipos->push_back(tipo);
	
	nodoListaIndices.estructTipoIndice = estructTipoIndice;
	nodoListaIndices.listaNombresClaves = listaNombresClaves;
	nodoListaIndices.listaTipos = listaTipos;
	
	listaTiposIndices = new ListaTiposIndices();
	listaTiposIndices->push_back(nodoListaIndices);
	
	
	//INDICE ROMANO (ARBOLB+)
	estructTipoIndice.tipoIndice = TipoIndices::ROMANO;
	estructTipoIndice.tipoEstructura = TipoIndices::ARBOL_BP;
	estructTipoIndice.tamanioBloque = 1024;
	estructTipoIndice.tipoClave = TipoDatos::TIPO_CHAR;
	estructTipoIndice.nombreArchivo = "pelicula1.bplus";
	
	listaNombresClaves = new ListaNombresClaves();
	listaNombresClaves->push_back("GENERO");
	
	listaTipos = new ListaTipos();
	tipo = TipoDatos::TIPO_CHAR;
	listaTipos->push_back(tipo);
	
	nodoListaIndices.estructTipoIndice = estructTipoIndice;
	nodoListaIndices.listaNombresClaves = listaNombresClaves;
	nodoListaIndices.listaTipos = listaTipos;
	
	listaTiposIndices->push_back(nodoListaIndices);
	
	//Se inserta la lista de tipos de indices en el mapa de tipos de indices
	this->mapaTiposIndices["PELICULA"] = listaTiposIndices;

}


DefinitionsManager::ListaTiposAtributos* DefinitionsManager::getListaTiposAtributos(const string &nombreTipo) {
	
	return this->mapaTiposAtributos[nombreTipo];
	
}


DefinitionsManager::ListaValoresAtributos* DefinitionsManager::getListaValoresAtributos(const string &nombreTipo,
																	const MapaValoresAtributos &mapaValoresAtributos) {
	
	//Lista a retornar
	ListaValoresAtributos* listaValoresAtributos = new ListaValoresAtributos();
	
	ListaNombresAtributos* listaNombresAtributos = this->mapaNombresAtributos[nombreTipo];
	
	MapaValoresAtributos::const_iterator iterMap;
	
	string valorAtributo("");
	
	for (ListaNombresAtributos::iterator iter = listaNombresAtributos->begin();
		iter != listaNombresAtributos->end(); ++iter) {
		
		if ( (iterMap = mapaValoresAtributos.find(*iter)) != mapaValoresAtributos.end() )
			valorAtributo = iterMap->second;
		else valorAtributo = "";
		
		listaValoresAtributos->push_back(valorAtributo);
		
	}
	
	return listaValoresAtributos;
	
}
 
ListaTipos* DefinitionsManager::getListaTipos(const string &nombreTipo) {
	
	ListaTipos *listaTipos = new ListaTipos();
	
	ListaTiposAtributos *listaTiposAtributos = this->mapaTiposAtributos[nombreTipo];
	
	ListaTiposAtributos::const_iterator iter;
	
	for (iter = listaTiposAtributos->begin(); iter != listaTiposAtributos->end(); ++iter)
		listaTipos->push_back(iter->tipo);
	
	return listaTipos;
}


DefinitionsManager::ListaTiposIndices* DefinitionsManager::getListaTiposIndices(const string &nombreTipo) {
	
	return this->mapaTiposIndices[nombreTipo];
	
}


ListaTipos* DefinitionsManager::getListaTiposClaves(const string &nombreTipo, const ListaNombresClaves &listaNombresClaves) {
	
	ListaTiposIndices *listaTiposIndices = this->mapaTiposIndices[nombreTipo];
	
	for (ListaTiposIndices::iterator iter = listaTiposIndices->begin(); iter != listaTiposIndices->end(); ++iter) {
		
		if (*((*iter).listaNombresClaves) == listaNombresClaves)
			return (*iter).listaTipos;
		
	}
	
	return NULL;
}
