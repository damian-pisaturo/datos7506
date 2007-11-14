#ifndef VISTA_H_
#define VISTA_H_

#include <string>
#include <iostream>
#include <fstream>
#include <list>

#include "../../Common/DefManager/DefinitionsManager.h"
#include "../DataManager/DataManager.h"
#include "../../Common/Bloque/Bloque.h"
#include "../../Common/Tamanios.h"


class Vista
{
public:
	Vista();
	virtual ~Vista();
	/*
	 * Muestra por consola el contenido de un registro
	 * */
	void showRegister(char *registro, ListaNodos *listaTiposAtributos);
	
	
	/*
	 * Muestra por consola los campos de un tipo correspondiente
	 * */
	void showTipoContent(list<string>& listaNombresCampos);
	


};

#endif /*VISTA_H_*/
