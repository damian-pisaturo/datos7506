///////////////////////////////////////////////////////////////////////////
//	Archivo   : DefinitionsManager.cpp
//  Namespace : Common
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Definicion de los metodos de la DefinitionsManager.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#include "DefinitionsManager.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: DefinitionsManager 
//	(Contiene las definiciones de los tipos de datos declarados en el 
// 	archivo de definiciones, almacenadas en estructuras (mapas, listas)
// 	que pueden ser consultadas)
///////////////////////////////////////////////////////////////////////////

//Se inicializa la instancia estática de esta clase
DefinitionsManager DefinitionsManager::instance;

///////////////////////////////////////////////////////////////////////////
// Constructor/Destructor
///////////////////////////////////////////////////////////////////////////
DefinitionsManager::DefinitionsManager()
{ }

DefinitionsManager::~DefinitionsManager() 
{
	for (MapaTiposAtributos::iterator iter = this->mapaTiposAtributos.begin();
		iter != this->mapaTiposAtributos.end(); ++iter)
		delete iter->second;
		
	for (MapaNombresAtributos::iterator iter = this->mapaNombresAtributos.begin();
		iter != this->mapaNombresAtributos.end(); ++iter)
		delete iter->second;
	
	for (MapaTiposIndices::iterator iter = this->mapaTiposIndices.begin();
		iter != this->mapaTiposIndices.end(); ++iter)
		delete iter->second;	
	
	for (MapaNombresClavesPrimarias::iterator iter = this->mapaNombresClavesPrimarias.begin();
		iter != this->mapaNombresClavesPrimarias.end(); ++iter)
		delete iter->second;
}

///////////////////////////////////////////////////////////////////////////
// Metodos publicos
///////////////////////////////////////////////////////////////////////////
ListaValoresAtributos* DefinitionsManager::getListaValoresAtributos(const string &nombreTipo,
		const MapaValoresAtributos &mapaValoresAtributos) 
{	
	//Lista a retornar
	ListaValoresAtributos* listaValoresAtributos = NULL;
	ListaNombresAtributos* listaNombresAtributos = this->getListaNombresAtributos(nombreTipo);
	
	if (listaNombresAtributos){
		listaValoresAtributos = new ListaValoresAtributos();
		MapaValoresAtributos::const_iterator iterMap;
		
		string valorAtributo("");
		
		for (ListaNombresAtributos::iterator iter = listaNombresAtributos->begin();
			iter != listaNombresAtributos->end(); ++iter) {
			
			if ( (iterMap = mapaValoresAtributos.find(*iter)) != mapaValoresAtributos.end() )
				valorAtributo = iterMap->second;
			else valorAtributo = "";
			
			listaValoresAtributos->push_back(valorAtributo);
			
		}
	}
	
	return listaValoresAtributos;
}
 
ListaTipos* DefinitionsManager::getListaTipos(const string &nombreTipo) 
{	
	ListaTipos *listaTipos = NULL;
	
	ListaTiposAtributos *listaTiposAtributos = this->getListaTiposAtributos(nombreTipo);
	
	if (listaTiposAtributos){	
		listaTipos = new ListaTipos();
		ListaTiposAtributos::const_iterator iter;
		
		for (iter = ++(listaTiposAtributos->begin()); iter != listaTiposAtributos->end(); ++iter)
			listaTipos->push_back(iter->tipoDato);
	}
	
	return listaTipos;
}

ListaTiposAtributos* DefinitionsManager::getListaTiposAtributos(const string &nombreTipo, const ListaNombresClaves &listaNombresClaves) {
	
	// Estructuras necesarias para armar la lista a retornar.
	ListaTiposAtributos* listaTiposAtributos = this->getListaTiposAtributos(nombreTipo);
	ListaTiposAtributos* listaARetornar = NULL;
	
	if (listaTiposAtributos){
		ListaNombresAtributos* listaNombresAtributos = this->getListaNombresAtributos(nombreTipo);
		
		if (listaNombresAtributos){
			// Estructura a retornar
			listaARetornar = new ListaTiposAtributos();
			NodoInfoRegistro nodoIR;
			
			ListaTiposAtributos::const_iterator itTA;
			ListaNombresAtributos::const_iterator itNA;
			
			// Copio el primer nodo de la lista
			nodoIR = *(listaTiposAtributos->begin());
			nodoIR.cantClaves = listaNombresClaves.size();
			
			listaARetornar->push_back(nodoIR);
			
			for (itTA = ++(listaTiposAtributos->begin()), itNA = listaNombresAtributos->begin();
				 (itTA != listaTiposAtributos->end()) && (itNA != listaNombresAtributos->end());
				 ++itTA, ++itNA) {
				
				nodoIR.tipoDato = itTA->tipoDato;
				
				if (this->buscarNombre(*itNA, listaNombresClaves))
					nodoIR.esPk = true;
				else
					nodoIR.esPk = false;
				
				listaARetornar->push_back(nodoIR);
			}
		}
	}
	
	return listaARetornar;
	
}

ListaTipos* DefinitionsManager::getListaTiposClaves(const string &nombreTipo, const ListaNombresClaves &listaNombresClaves) {
	
	ListaTipos* listaTipos = NULL;
	
	// Listas necesarias para armar la listaTipos a retornar
	ListaNombresAtributos* listaNombresAtributos = NULL;
	ListaTiposAtributos* listaTiposAtributos = this->getListaTiposAtributos(nombreTipo);
	
	if (listaTiposAtributos){
		listaNombresAtributos = this->getListaNombresAtributos(nombreTipo);
		
		if (listaNombresAtributos){			
			listaTipos = new ListaTipos();
			// Iterador de la lista recibida por parametro
			ListaNombresClaves::const_iterator iterNomClaves;
			
			// Iteradores de las listas del DefinitionsManager
			ListaNombresAtributos::const_iterator iterNomAtributos;
			ListaTiposAtributos::const_iterator iterTiposAtributos;
			
			for (iterNomClaves = listaNombresClaves.begin(); iterNomClaves != listaNombresClaves.end();
				++iterNomClaves) {
				
				for (iterNomAtributos = listaNombresAtributos->begin(),
					 iterTiposAtributos = ++(listaTiposAtributos->begin());
					 iterNomAtributos != listaNombresAtributos->end(); 
					 ++iterNomAtributos, ++iterTiposAtributos) {
					
					if (*iterNomAtributos == *iterNomClaves) {
						listaTipos->push_back(iterTiposAtributos->tipoDato);
						break;
					}
					
				}
				
			}
		}
	}
	
	return listaTipos;
}

ListaTiposAtributos* DefinitionsManager::getListaTiposAtributos(const string &nombreTipo)
{
	char resultado = ArchivoMaestro::OK;
	ListaTiposAtributos* lta = NULL;
	MapaTiposAtributos::iterator iterMapaTA = this->mapaTiposAtributos.find(nombreTipo);
	
	if (iterMapaTA == this->mapaTiposAtributos.end())	
		resultado = this->recuperarTipo(nombreTipo);
	
	if (resultado == ArchivoMaestro::OK) 
		lta = this->mapaTiposAtributos[nombreTipo];
	
	return lta;
}

unsigned char DefinitionsManager::getTipoOrgRegistro(const string &nombreTipo)
{
	unsigned char tipoOrgRegistro = 0;
	char resultado = ArchivoMaestro::OK;
	MapaTiposAtributos::iterator iterMapaTA = this->mapaTiposAtributos.find(nombreTipo);
	
	if (iterMapaTA == this->mapaTiposAtributos.end())	
		resultado = this->recuperarTipo(nombreTipo);
	
	if (resultado == ArchivoMaestro::OK)
		tipoOrgRegistro = this->mapaTiposAtributos[nombreTipo]->begin()->tipoDato;
	
	return tipoOrgRegistro;	
}

ListaNombresClaves* DefinitionsManager::getListaNombresClavesPrimarias(const string &nombreTipo)
{
	char resultado = ArchivoMaestro::OK;
	ListaNombresClaves* lnc = NULL;
	MapaNombresClavesPrimarias::iterator iterMapaNCP = this->mapaNombresClavesPrimarias.find(nombreTipo);
	
	if (iterMapaNCP == this->mapaNombresClavesPrimarias.end())	
		resultado = this->recuperarTipo(nombreTipo);
	
	if (resultado == ArchivoMaestro::OK)
		lnc = this->mapaNombresClavesPrimarias[nombreTipo];
	
	return lnc;
}

ListaNombresAtributos* DefinitionsManager::getListaNombresAtributos(const string &nombreTipo)
{
	char resultado = ArchivoMaestro::OK;
	ListaNombresAtributos* lna = NULL;
	MapaNombresAtributos::iterator iterMapaNA = this->mapaNombresAtributos.find(nombreTipo);
	
	if (iterMapaNA == this->mapaNombresAtributos.end())	
		resultado = this->recuperarTipo(nombreTipo);
	
	if (resultado == ArchivoMaestro::OK)
		lna = this->mapaNombresAtributos[nombreTipo];
	
	return lna;
}

ListaTiposIndices* DefinitionsManager::getListaTiposIndices(const string &nombreTipo)
{
	char resultado = ArchivoMaestro::OK;
	ListaTiposIndices* lti = NULL;
	MapaTiposIndices::iterator iterMapaTI = this->mapaTiposIndices.find(nombreTipo);
	
	if (iterMapaTI == this->mapaTiposIndices.end())	
		resultado = this->recuperarTipo(nombreTipo);
	
	if (resultado == ArchivoMaestro::OK)
		lti = this->mapaTiposIndices[nombreTipo];
	
	return lti;	
}

unsigned short DefinitionsManager::getTamBloqueDatos(const string& nombreTipo)
{
	unsigned short tamBloqueDatos = 0;
	char resultado = ArchivoMaestro::OK;
	ListaTiposAtributos* lta = NULL;
	MapaTiposAtributos::iterator iterMapaTA = this->mapaTiposAtributos.find(nombreTipo);
	
	if (iterMapaTA == this->mapaTiposAtributos.end())	
		resultado = this->recuperarTipo(nombreTipo);
	
	if (resultado == ArchivoMaestro::OK){
		lta = this->mapaTiposAtributos[nombreTipo];
		
		if (lta->begin()->tipoDato == TipoDatos::TIPO_VARIABLE) tamBloqueDatos = Tamanios::TAMANIO_BLOQUE_DATO;
		else tamBloqueDatos = lta->begin()->tamRegistro;
	}
	
	return tamBloqueDatos;
	
}

unsigned short DefinitionsManager::getTamBloqueLista(const string& nombreTipo) {
	
	return Tamanios::TAMANIO_BLOQUE_DATO;;
	
}

///////////////////////////////////////////////////////////////////////////
// Metodos privados
///////////////////////////////////////////////////////////////////////////

bool DefinitionsManager::buscarNombre(const string &nombre, const ListaNombresClaves &listaNombresClaves) const {
	
	bool encontrado = false;
	
	for (ListaNombresClaves::const_iterator it = listaNombresClaves.begin();
		 (it != listaNombresClaves.end()) && !encontrado; ++it)
		encontrado = (nombre == *it); 
	
	return encontrado;
}

char DefinitionsManager::recuperarTipo(const string& nombreTipo)
{
	ListaInfoRegistro* listaIR     = NULL;
	ListaTiposIndices* listaTI     = NULL;
	ListaNombresAtributos* listaNA = NULL;
	ListaNombresClaves* listaNC    = NULL;
	
	char resultado = this->archivoMaestro.recuperarTipo(nombreTipo);
	
	if (resultado == ArchivoMaestro::OK){
		listaIR = this->archivoMaestro.getListaInfoRegistro();
		listaTI = this->archivoMaestro.getListaTiposIndices();
		listaNA = this->archivoMaestro.getListaNombresAtributos();
		listaNC = this->archivoMaestro.getListaNombresClaves();
		
		this->mapaTiposAtributos[nombreTipo] = listaIR;
		this->mapaTiposIndices[nombreTipo] = listaTI;
		this->mapaNombresAtributos[nombreTipo] = listaNA; 
		this->mapaNombresClavesPrimarias[nombreTipo] = listaNC;
	}
	
	return resultado;
}
