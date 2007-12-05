#include "Indice.h"

/*
 * Este método modifica claves secundarias y sus correspondientes listas de claves primarias.
 */
int Indice::modificar(Clave* claveSecundariaVieja, Clave* claveSecundariaNueva,
			  Clave* clavePrimariaVieja, Clave* clavePrimariaNueva) {
	
	int resultado = ResultadosIndices::OK;

	if ( (*claveSecundariaVieja == *claveSecundariaNueva) && !(*clavePrimariaVieja == *clavePrimariaNueva) ) {
		
		if (this->eliminar(claveSecundariaVieja, clavePrimariaVieja) == ResultadosIndices::OK)
			resultado = this->insertar(claveSecundariaVieja, clavePrimariaNueva);
		else resultado = ResultadosIndices::ERROR_MODIFICACION;
		
	} else if (!(*claveSecundariaVieja == *claveSecundariaNueva)) {
		
		if (this->eliminar(claveSecundariaVieja, clavePrimariaVieja) == ResultadosIndices::OK)
			resultado = this->insertar(claveSecundariaNueva, clavePrimariaNueva);
		else resultado = ResultadosIndices::ERROR_MODIFICACION;
		
	}
	
	return resultado;
	
}



ListaTipos* Indice::getListaTipos() const {
	
	ListaTipos* listaTipos = new ListaTipos();
	
	for (ListaInfoRegistro::iterator it = ++(this->getListaInfoReg()->begin()); it != this->getListaInfoReg()->end(); ++it)
		listaTipos->push_back(it->tipoDato);
	
	return listaTipos;
	
}


ListaTipos* Indice::getListaTiposClavePrimaria() const {
	
	ListaTipos* listaTipos = new ListaTipos();
	ListaInfoRegistro::iterator it = this->getListaInfoReg()->begin();
	unsigned short cantClaves = it->cantClaves, i = 0;
	
	for (++it; (i < cantClaves) && (it != this->getListaInfoReg()->end()); ++it) {
		if (it->esPk) {
			listaTipos->push_back(it->tipoDato);
			++i;
		}
	}
	
	return listaTipos;
	
}


ListaInfoRegistro* Indice::getListaInfoRegClavePrimaria() const {
	
	ListaInfoRegistro* listaTipos = new ListaInfoRegistro();
	ListaInfoRegistro::iterator it = this->getListaInfoReg()->begin();
	unsigned short i = 0;
	
	NodoInfoRegistro nodoIR;
	nodoIR.cantClaves = it->cantClaves;
	nodoIR.esPk = false;
	nodoIR.tipoDato = TipoDatos::TIPO_VARIABLE;
	
	listaTipos->push_back(nodoIR);
	
	for (++it; (i < nodoIR.cantClaves) && (it != this->getListaInfoReg()->end()); ++it) {
		if (it->esPk) {
			listaTipos->push_back(*it);
			++i;
		}
	}
	
	return listaTipos;
	
}

