#include "Vista.h"

Vista::Vista()
{ }

Vista::~Vista()
{ }


/*
 * Muestra por consola el contenido de un registro
 * */
void Vista::showRegister(char *registro, ListaTiposAtributos  *listaTiposAtributos){
	ListaTiposAtributos ::const_iterator it = listaTiposAtributos->begin();
	unsigned char tipo;
	// Si el registro pasado por parametro es variable e incluye los dos bytes de long se debera inicializar el offset en 2
	unsigned short offset = 0; 
	
	if(it->tipoDato == TipoDatos::TIPO_VARIABLE)
		offset += Tamanios::TAMANIO_LONGITUD;
	
	for (++it; it != listaTiposAtributos->end(); ++it) {
		tipo = it->tipoDato;
		
		switch (tipo){
		
		case TipoDatos::TIPO_ENTERO:
			int entero;
			memcpy(&entero, (registro + offset), sizeof(int));
			offset += sizeof(int);
			
			cout << entero << endl;
			break;
		
		case TipoDatos::TIPO_BOOL:
			bool booleano;
			memcpy(&booleano, (registro + offset), sizeof(bool));
			offset += sizeof(bool);
			
			cout << booleano << endl;
			break;
		
		case TipoDatos::TIPO_CHAR:
			char caracter;
			memcpy(&caracter, (registro + offset), sizeof(char));
			offset += sizeof(char);
			
			cout << caracter << endl;
			break;
		
		case TipoDatos::TIPO_FECHA:
			unsigned short anio;
			unsigned char mes;
			unsigned char dia;
			
			memcpy(&anio, (registro + offset), sizeof(unsigned short));
			offset += sizeof(unsigned short);
			
			memcpy(&mes, (registro + offset), sizeof(unsigned char));
			offset += sizeof(unsigned char);
			
			memcpy(&dia, (registro + offset), sizeof(unsigned char));
			offset += sizeof(unsigned char);
			
			cout << (int)dia; cout << "/"; cout << (int)mes; cout << "/"; cout << anio << endl;
			break;
		
		case TipoDatos::TIPO_STRING:
			unsigned char longReg;
			char *campoVariable;
			// Obtengo la longitud del campo variable
			memcpy(&longReg, (registro + offset), Tamanios::TAMANIO_LONGITUD_CADENA);
			offset +=  Tamanios::TAMANIO_LONGITUD_CADENA;
			
			campoVariable = new char [longReg + 1];
			memcpy(campoVariable, (registro + offset), longReg);
			*(campoVariable + longReg) = 0;
			offset += longReg;
			
			cout << campoVariable <<endl;
			delete[] campoVariable;
			break;
		
		case TipoDatos::TIPO_SHORT:
			short campoShort;
			memcpy(&campoShort, (registro + offset), sizeof(short));
			offset += sizeof(short);
			
			cout << campoShort << endl;
			break;
			
		case TipoDatos::TIPO_FLOAT:
			float campoFloat;
			memcpy(&campoFloat, (registro + offset), sizeof(float));
			offset += sizeof(float);
			
			cout << campoFloat << endl;
			break;
		}
		
		
	}
	
}

/*
 * Muestra de un registro los campos indicados en la listaNombresCampos, la misma contiene 
 * los nombres de los campos a mostrar
 * */
void Vista::showRegister(char *registro, ListaTiposAtributos* listaTiposAtributos, ListaNombresAtributos *listaNombresAtributos,
						 ListaStrings& listaNombresCampos){
	
	ListaNombresAtributos::const_iterator itNombresAtributos;
	ListaTiposAtributos ::const_iterator itTiposAtributos = listaTiposAtributos->begin();
	ListaStrings::const_iterator itNombresCampos;
	bool seguirBuscando;
	unsigned short offsetToCampo = 0;
	
	// Itero la lista de nombres de campos a mostrar
	for(itNombresCampos = listaNombresCampos.begin(); itNombresCampos != listaNombresCampos.end(); ++itNombresCampos){
		itTiposAtributos = listaTiposAtributos->begin();
		seguirBuscando = true;
		offsetToCampo = 0;
		if(itTiposAtributos->tipoDato == TipoDatos::TIPO_VARIABLE)
			offsetToCampo += Tamanios::TAMANIO_LONGITUD;
		
		++itTiposAtributos;
		
		// Busco la ubicacion del campo, obtengo el tipo del mismo y muestro el campo
		for(itNombresAtributos = listaNombresAtributos->begin(); seguirBuscando; ++itNombresAtributos, ++itTiposAtributos){
			// Comparo el nombre del campo a mostrar
			if(*itNombresCampos == *itNombresAtributos){
				this->showCampo(offsetToCampo, registro, itTiposAtributos->tipoDato);
				seguirBuscando = false;
			}
			else {
				switch(itTiposAtributos->tipoDato){
					case TipoDatos::TIPO_ENTERO:
						offsetToCampo += sizeof(int);
						break;
					case TipoDatos::TIPO_BOOL:
						offsetToCampo += sizeof(bool);
						break;
					case TipoDatos::TIPO_CHAR:
						offsetToCampo += sizeof(char);
						break;
					case TipoDatos::TIPO_FECHA:
						offsetToCampo += Tamanios::TAMANIO_FECHA;
						break;
					case TipoDatos::TIPO_FLOAT:
						offsetToCampo += sizeof(float);
						break;
					case TipoDatos::TIPO_SHORT:
						offsetToCampo += sizeof(short);
						break;
					case TipoDatos::TIPO_STRING:
						unsigned char longReg;
						// Obtengo la longitud del campo variable
						memcpy(&longReg, (registro + offsetToCampo), Tamanios::TAMANIO_LONGITUD_CADENA);
						offsetToCampo +=  Tamanios::TAMANIO_LONGITUD_CADENA + longReg;
						break;
				}
			}
		}
	}
	
}

/*
 * Este método imprime los campos de registro seleccionados en la listaCamposSeleccionados 
 * */
void Vista::mostrarRegistro(char *registro, ListaTiposAtributos  *listaTiposAtributos,  ListaNombresAtributos *listaNombresAtributos, 
			ListaStrings& listaCamposSeleccionados) {
	
	ListaStrings::iterator itCampos;
	bool muestroTodosLosCampos = false;
	
	for(itCampos = listaCamposSeleccionados.begin(); itCampos != listaCamposSeleccionados.end(); ++itCampos) {
		if ( (*itCampos) == TODOS_LOS_CAMPOS )
			muestroTodosLosCampos = true;
	}

	// Si hay que mostrar todos los campos, se llama al método que imprime el
	// registro completo.
	if (muestroTodosLosCampos)
		showRegister(registro, listaTiposAtributos);
	// Sino, solo se imprimen los campos que están en la listaCamposSeleccionados.
	else{ 
		showRegister(registro, listaTiposAtributos, listaNombresAtributos, listaCamposSeleccionados);			
	}
	
}

void Vista::showTipoContent(ListaStrings& listaNombresCampos){
	ListaStrings::const_iterator it;
	
	for(it = listaNombresCampos.begin(); it != listaNombresCampos.end(); ++it)
		cout << *it << endl;
}

void Vista::showCampo(unsigned short offset, char *registro, int tipoCampo){

	switch (tipoCampo){
			
	case TipoDatos::TIPO_ENTERO:
		int entero;
		memcpy(&entero, (registro + offset), sizeof(int));
		cout << entero << endl;
		break;
	
	case TipoDatos::TIPO_BOOL:
		bool booleano;
		memcpy(&booleano, (registro + offset), sizeof(bool));
		cout << booleano << endl;
		break;
	
	case TipoDatos::TIPO_CHAR:
		char caracter;
		memcpy(&caracter, (registro + offset), sizeof(char));
		cout << caracter << endl;
		break;
	
	case TipoDatos::TIPO_FECHA:
		unsigned short anio;
		unsigned char mes;
		unsigned char dia;
		
		memcpy(&anio, (registro + offset), sizeof(unsigned short));
		offset += sizeof(unsigned short);
		
		memcpy(&mes, (registro + offset), sizeof(unsigned char));
		offset += sizeof(unsigned char);
		memcpy(&dia, (registro + offset), sizeof(unsigned char));
		cout << (int)dia; cout << "/"; cout << (int)mes; cout << "/"; cout << anio << endl;
		break;
	
	case TipoDatos::TIPO_STRING:
		unsigned char longReg;
		char *campoVariable;
		// Obtengo la longitud del campo variable
		memcpy(&longReg, (registro + offset), Tamanios::TAMANIO_LONGITUD_CADENA);
		offset +=  Tamanios::TAMANIO_LONGITUD_CADENA;
		
		campoVariable = new char [longReg + 1];
		memcpy(campoVariable, (registro + offset), longReg);
		*(campoVariable + longReg) = 0;
		cout << campoVariable <<endl;
		delete[] campoVariable;
		break;
	
	case TipoDatos::TIPO_SHORT:
		short campoShort;
		memcpy(&campoShort, (registro + offset), sizeof(short));
		cout << campoShort << endl;
		break;
		
	case TipoDatos::TIPO_FLOAT:
		float campoFloat;
		memcpy(&campoFloat, (registro + offset), sizeof(float));
		cout << campoFloat << endl;
		break;
		
	}

}
