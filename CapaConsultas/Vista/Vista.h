#ifndef VISTA_H_
#define VISTA_H_

#include "../../Common/DefManager/DefinitionsManager.h"
#include "../../Common/Bloque/Bloque.h"

#define TODOS_LOS_CAMPOS 		"*"

class Vista
{
public:
	Vista();
	virtual ~Vista();
	/*
	 * Muestra por consola el contenido de un registro
	 * */
	void showRegister(char *registro, ListaTiposAtributos *listaTiposAtributos);

	void showRegister(char *registro, ListaTiposAtributos  *listaTiposAtributos,  ListaNombresAtributos *listaNombresAtributos, 
			ListaStrings& listaNombresCampos);
	
	/*
	 * Este método imprime los campos de registro seleccionados en la listaCamposSeleccionados 
	 * */
	void mostrarRegistro(char *registro, ListaTiposAtributos *listaTiposAtributos, ListaNombresAtributos *listaNombresAtributos, 
						 ListaStrings& listaNombresCampos);
	
	/*
	 * Muestra por consola los campos de un tipo correspondiente
	 * */
	void showTipoContent(ListaStrings& listaNombresCampos);
	
private:
	
	void showCampo(unsigned short offsetToCampo, char *registro, int tipoCampo);

};

#endif /*VISTA_H_*/
