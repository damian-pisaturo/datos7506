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
	
	for (ListaNodos::iterator it = ++(this->getListaNodos()->begin()); it != this->getListaNodos()->end(); ++it)
		listaTipos->push_back(it->tipo);
	
	return listaTipos;
	
}


ListaTipos* Indice::getListaTiposClavePrimaria() const {
	
	ListaTipos* listaTipos = new ListaTipos();
	ListaNodos::iterator it = this->getListaNodos()->begin();
	unsigned short cantClaves = it->cantClaves, i = 0;
	
	for (++it; (i < cantClaves) && (it != this->getListaNodos()->end()); ++it) {
		if (it->pk == "true") {
			listaTipos->push_back(it->tipo);
			++i;
		}
	}
	
	return listaTipos;
	
}


ListaNodos* Indice::getListaNodosClavePrimaria() const {
	
	ListaNodos* listaTipos = new ListaNodos();
	ListaNodos::iterator it = this->getListaNodos()->begin();
	unsigned short i = 0;
	
	nodoLista nodo;
	nodo.cantClaves = it->cantClaves;
	nodo.pk = "";
	nodo.tipo = TipoDatos::TIPO_VARIABLE;
	
	listaTipos->push_back(nodo);
	
	for (++it; (i < nodo.cantClaves) && (it != this->getListaNodos()->end()); ++it) {
		if (it->pk == "true") {
			listaTipos->push_back(*it);
			++i;
		}
	}
	
	return listaTipos;
	
}

