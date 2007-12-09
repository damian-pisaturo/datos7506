////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Funcion principal de la Capa Metadata.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#include "CapaMetadata.h"

// MÉTODOS PARA RESOLVER LAS CONSULTAS CON JOINS

char invertirOperadorLogico(char operador) {
	
	switch (operador) {
	
		case ExpresionesLogicas::MAYOR:
			return ExpresionesLogicas::MENOR;
		case ExpresionesLogicas::MAYOR_O_IGUAL:
			return ExpresionesLogicas::MENOR_O_IGUAL;
		case ExpresionesLogicas::MENOR:
			return ExpresionesLogicas::MAYOR;
		case ExpresionesLogicas::MENOR_O_IGUAL:
			return ExpresionesLogicas::MAYOR_O_IGUAL;
		default:
			return operador;
	
	}
	
}

void invertirOperacionLogica(const string &nombreTipo, ListaOperaciones &lista) {
	
	EstructuraNombres estructuraAuxiliar;
	
	for (ListaOperaciones::iterator it = lista.begin();
		 it != lista.end(); ++it) {
		
		if (it->estructuraNombresDer.nombreTipo == nombreTipo) {
			
			estructuraAuxiliar = it->estructuraNombresIzq;
			it->estructuraNombresIzq = it->estructuraNombresDer;
			it->estructuraNombresDer = estructuraAuxiliar;
			it->operacion = invertirOperadorLogico(it->operacion);
			
		}	
		
	}	
	
}

void cargarMapasRestricciones(ListaOperaciones &lista, MapaRestricciones &mapa) {
	
	ListaOperaciones* listaOp = NULL;
	MapaRestricciones::iterator iterMapa;
	string nombreTipo("");
	
	for (ListaOperaciones::iterator it = lista.begin();
		 it != lista.end(); ++it) {
		
		nombreTipo = it->estructuraNombresIzq.nombreTipo;
		
		iterMapa = mapa.find(nombreTipo);
		
		if (iterMapa != mapa.end()) // Se encontró el nombre del tipo en el mapa	
			iterMapa->second->push_back(*it);
		else {	
			listaOp = new ListaOperaciones();
			listaOp->push_back(*it);
			mapa[nombreTipo] = listaOp;
		}
		
	}
	
}


void destruirMapaRestricciones(MapaRestricciones &mapa) {
	
	for (MapaRestricciones::iterator it = mapa.begin();
		 it != mapa.end(); ++it)
		delete it->second;
	
}


void destruirMapaExtensiones(MapaExtensiones &mapa) {
	
	for (MapaExtensiones::iterator it = mapa.begin();
		 it != mapa.end(); ++it)
		delete it->second;
	
}


bool compararRegistros(char* registro1, char* registro2, unsigned short longitudRegistros)
{
	bool iguales = true;
	
	for (unsigned int i=0; (i<longitudRegistros) && iguales; ++i ) {
		if (registro1[i] != registro2[i])
			iguales = false;
	}
	return iguales;
}


bool compararClaves(Clave *clave, char operacion, Clave* claveAComparar)
{
	bool comparacion = false;
	switch (operacion) {
		case ExpresionesLogicas::IGUAL:
			comparacion = (*clave == *claveAComparar);
			break;
		case ExpresionesLogicas::MAYOR:
			comparacion = (*claveAComparar < *clave);
			break;
		case ExpresionesLogicas::MAYOR_O_IGUAL:
			comparacion = ((*claveAComparar < *clave) || (*clave == *claveAComparar));
			break;
		case ExpresionesLogicas::MENOR:
			comparacion = (*clave < *claveAComparar);
			break;
		case ExpresionesLogicas::MENOR_O_IGUAL:
			comparacion = ((*clave < *claveAComparar) || (*clave == *claveAComparar));
			break;
	}
	return comparacion;
}

char* obtenerCampo(unsigned int numeroCampo, char* registro, ListaTiposAtributos &listaTiposAtributos) {

	unsigned short offset = 0;
	
	ListaTiposAtributos::iterator itLTA = listaTiposAtributos.begin();
	
	if (numeroCampo < listaTiposAtributos.size()) {
		
		if (itLTA->tipoDato == TipoDatos::TIPO_VARIABLE) 
			offset = Tamanios::TAMANIO_LONGITUD;
	
		++itLTA;
		
		for (unsigned int i=1; i<numeroCampo; i++) {
			
			switch (itLTA->tipoDato) {
				case TipoDatos::TIPO_BOOL:
					offset += sizeof(bool);
					break;
				case TipoDatos::TIPO_CHAR:
					offset += sizeof(char);
					break;
				case TipoDatos::TIPO_ENTERO:
					offset += sizeof(int);
					break;
				case TipoDatos::TIPO_FECHA:
					offset += Tamanios::TAMANIO_FECHA;
					break;
				case TipoDatos::TIPO_FLOAT:
					offset += sizeof(float);
					break;
				case TipoDatos::TIPO_SHORT:
					offset += sizeof(short);
					break;
				case TipoDatos::TIPO_STRING:
					unsigned char tamanioCadena;
					memcpy(&tamanioCadena, registro + offset, Tamanios::TAMANIO_LONGITUD_CADENA);
					offset += (tamanioCadena + Tamanios::TAMANIO_LONGITUD_CADENA);
					break;
			}
			++itLTA;
		}
		return (registro + offset);
	
	}else
		return NULL;
}

int compararCamposRegistros(ListaNombresAtributos &listaNombresAtributos1, ListaNombresAtributos
							 &listaNombresAtributos2, ListaTiposAtributos &listaTiposAtributos1,
							ListaTiposAtributos &listaTiposAtributos2, string nombre1, string nombre2,
							 char* registro1, char* registro2, char operacion)
{
	ListaNombresAtributos::iterator itNombres = listaNombresAtributos1.begin();
 
	// Obtiene el campo pedido del primer registro.
	unsigned int numeroCampo = 1;

	while ((nombre1 != *itNombres) && (itNombres != listaNombresAtributos1.end())) {
		++numeroCampo;
		++itNombres;
	}
	
	char* campo1 = obtenerCampo(numeroCampo, registro1, listaTiposAtributos1);
	
	ListaTiposAtributos::iterator itTipos = listaTiposAtributos1.begin();

	// itera la lista de tipos para obtener el tipo del campo "campo1".
	for (unsigned int i = 0; i < numeroCampo; ++i)
		++itTipos;
	
	unsigned char tipoCampo1 = 0;
	Clave * clave1 = NULL;
	
	// Se genera una clave que contenga el campo1 para usarla luego en la comparación con el campo2
	switch (itTipos->tipoDato) {
		case TipoDatos::TIPO_BOOL: {	
			bool valorCampo;
			memcpy(&valorCampo, campo1, sizeof(bool));
			clave1 = new ClaveBoolean(valorCampo);
			tipoCampo1 = TipoDatos::TIPO_BOOL;
			break;
		}
		case TipoDatos::TIPO_CHAR: {	
			char valorCampo;
			memcpy(&valorCampo, campo1, sizeof(char));
			clave1 = new ClaveChar(valorCampo);
			tipoCampo1 = TipoDatos::TIPO_CHAR;
			break;
		}
		case TipoDatos::TIPO_SHORT: {
			short valorCampo;
			memcpy(&valorCampo, campo1, sizeof(short));
			clave1 = new ClaveShort(valorCampo);
			tipoCampo1 = TipoDatos::TIPO_SHORT;
			break;
		}
		case TipoDatos::TIPO_ENTERO: {
			int valorCampo;
			memcpy(&valorCampo, campo1, sizeof(int));
			clave1 = new ClaveEntera(valorCampo);
			tipoCampo1 = TipoDatos::TIPO_ENTERO;
			break;
		}
		case TipoDatos::TIPO_FLOAT: {
			float valorCampo;
			memcpy(&valorCampo, campo1, sizeof(float));
			clave1 = new ClaveReal(valorCampo);
			tipoCampo1 = TipoDatos::TIPO_FLOAT;
			break;
		}
		case TipoDatos::TIPO_FECHA: {
			ClaveFecha::TFECHA valorCampo;
			unsigned short anio, dia, mes;
			
			memcpy(&anio, campo1, sizeof(short));
			memcpy(&mes, campo1 + sizeof(short), sizeof(char));
			memcpy(&dia, campo1 + sizeof(short) + sizeof(char), sizeof(char));
			valorCampo.crear(dia, mes, anio);
			clave1 = new ClaveFecha(valorCampo);
			tipoCampo1 = TipoDatos::TIPO_FECHA;
			break;
		}
		case TipoDatos::TIPO_STRING: {
			unsigned char longCadena;
			memcpy(&longCadena, campo1, Tamanios::TAMANIO_LONGITUD_CADENA);
			char * valorCampo = new char[longCadena + 1];
			memcpy(valorCampo, campo1 + Tamanios::TAMANIO_LONGITUD_CADENA, longCadena);
			*(valorCampo + longCadena) = 0;
			
			clave1 = new ClaveVariable(valorCampo);
			tipoCampo1 = TipoDatos::TIPO_STRING;
			break;
		}
			
	}
	
	// Obtiene el campo pedido del segundo registro.
	numeroCampo = 1;
	
	while ((nombre2 != *itNombres) && (itNombres != listaNombresAtributos2.end())) {
		++numeroCampo;
		++itNombres;
	}
	
	char* campo2 = obtenerCampo(numeroCampo, registro2, listaTiposAtributos2);


	itTipos = listaTiposAtributos2.begin();

	// itera la lista de tipos para obtener el tipo del campo "campo2".
	for (unsigned int i = 0; i < numeroCampo; ++i)
		++itTipos;
	
	Clave* clave2 = NULL;
	
	// Comprueba que el campo2 sea del mismo tipo que el campo1.
	// Y genera con el una clave para usar en la comparacion.
	switch (itTipos->tipoDato) {
		case TipoDatos::TIPO_BOOL: {
			if (tipoCampo1 != TipoDatos::TIPO_BOOL)
				return ResultadosMetadata::ERROR_DISTINTO_TIPO_DATO;
			bool valorCampo;
			memcpy(&valorCampo, campo2, sizeof(bool));
			clave2 = new ClaveBoolean(valorCampo);
			break;
		}
		case TipoDatos::TIPO_CHAR: {	
			if (tipoCampo1 != TipoDatos::TIPO_CHAR)
				return ResultadosMetadata::ERROR_DISTINTO_TIPO_DATO;
			char valorCampo;
			memcpy(&valorCampo, campo2, sizeof(char));
			clave2 = new ClaveChar(valorCampo);
			break;
		}
		case TipoDatos::TIPO_SHORT: {
			if (tipoCampo1 != TipoDatos::TIPO_SHORT)
				return ResultadosMetadata::ERROR_DISTINTO_TIPO_DATO;
			short valorCampo;
			memcpy(&valorCampo, campo2, sizeof(short));
			clave2 = new ClaveShort(valorCampo);
			break;
		}
		case TipoDatos::TIPO_ENTERO: {
			if (tipoCampo1 != TipoDatos::TIPO_ENTERO)
				return ResultadosMetadata::ERROR_DISTINTO_TIPO_DATO;
			int valorCampo;
			memcpy(&valorCampo, campo2, sizeof(int));
			clave2 = new ClaveEntera(valorCampo);
			break;
		}
		case TipoDatos::TIPO_FLOAT: {
			if (tipoCampo1 != TipoDatos::TIPO_FLOAT)
				return ResultadosMetadata::ERROR_DISTINTO_TIPO_DATO;
			float valorCampo;
			memcpy(&valorCampo, campo2, sizeof(float));
			clave2 = new ClaveReal(valorCampo);
			break;
		}
		case TipoDatos::TIPO_FECHA: {
			if (tipoCampo1 != TipoDatos::TIPO_FECHA)
				return ResultadosMetadata::ERROR_DISTINTO_TIPO_DATO;
			ClaveFecha::TFECHA valorCampo;
			unsigned short anio, dia, mes;
			
			memcpy(&anio, campo2, sizeof(short));
			memcpy(&mes, campo2 + sizeof(short), sizeof(char));
			memcpy(&dia, campo2 + sizeof(short) + sizeof(char), sizeof(char));
			valorCampo.crear(dia, mes, anio);
			clave2 = new ClaveFecha(valorCampo);
			break;
		}
		case TipoDatos::TIPO_STRING: {
			if (tipoCampo1 != TipoDatos::TIPO_STRING)
				return ResultadosMetadata::ERROR_DISTINTO_TIPO_DATO;
			
			unsigned char longCadena;
			memcpy(&longCadena, campo2, Tamanios::TAMANIO_LONGITUD_CADENA);
			char * valorCampo = new char[longCadena + 1];
			memcpy(valorCampo, campo2 + Tamanios::TAMANIO_LONGITUD_CADENA, longCadena);
			*(valorCampo + longCadena) = 0;
			
			clave2 = new ClaveVariable(valorCampo);
			break;
		}
			
	}
	
	if (compararClaves(clave1 ,operacion, clave2))
		return ResultadosMetadata::CAMPOS_IGUALES;
	return ResultadosMetadata::CAMPOS_DISTINTOS;
}

bool cumpleRestriccion(char *registro, ListaNombresAtributos &listaNombresAtributos, 
				  	  ListaTiposAtributos &listaTiposAtributos, 
				  	   NodoListaOperaciones &nodoListaOperaciones)
{
	ListaNombresAtributos::iterator itNombres = listaNombresAtributos.begin();
	unsigned int numeroCampo = 1;
	
	while ((nodoListaOperaciones.estructuraNombresIzq.nombreCampo != *itNombres) && (itNombres != listaNombresAtributos.end())) {
		++numeroCampo;
		++itNombres;
	}
	
	char* campo = obtenerCampo(numeroCampo, registro, listaTiposAtributos);
	
	ListaTiposAtributos::iterator itTipos = listaTiposAtributos.begin();

	// itera la lista de tipos para obtener el tipo del campo "campo".
	for (unsigned int i = 0; i < numeroCampo; ++i)
		++itTipos;
	
	stringstream conversorCampoAComparar;
	Clave* clave = NULL;
	Clave* claveAComparar = NULL;
	
	// En "nodoListaOperaciones.estructuraNombresDer.nombreCampo" se encuentra el valor constante 
	// con el cual hay que hacer la comparación.
	conversorCampoAComparar << nodoListaOperaciones.estructuraNombresDer.nombreCampo;
	
	switch (itTipos->tipoDato) {
		case TipoDatos::TIPO_BOOL: {	
			bool valorCampo;
			memcpy(&valorCampo, campo, sizeof(bool));
			clave = new ClaveBoolean(valorCampo);
			
			bool valorAComparar;
			conversorCampoAComparar >> valorAComparar;
			claveAComparar = new ClaveBoolean(valorAComparar);
			break;
		}
		case TipoDatos::TIPO_CHAR: {	
			char valorCampo;
			memcpy(&valorCampo, campo, sizeof(char));
			clave = new ClaveChar(valorCampo);
			
			char valorAComparar;
			conversorCampoAComparar >> valorAComparar;
			claveAComparar = new ClaveChar(valorAComparar);
			break;
		}
		case TipoDatos::TIPO_SHORT: {
			short valorCampo;
			memcpy(&valorCampo, campo, sizeof(short));
			clave = new ClaveShort(valorCampo);
			
			short valorAComparar;
			conversorCampoAComparar >> valorAComparar;
			claveAComparar = new ClaveShort(valorAComparar);
			break;
		}
		case TipoDatos::TIPO_ENTERO: {
			int valorCampo;
			memcpy(&valorCampo, campo, sizeof(int));
			clave = new ClaveEntera(valorCampo);
			
			int valorAComparar;
			conversorCampoAComparar >> valorAComparar;
			claveAComparar = new ClaveEntera(valorAComparar);
			break;
		}
		case TipoDatos::TIPO_FLOAT: {
			float valorCampo;
			memcpy(&valorCampo, campo, sizeof(float));
			clave = new ClaveReal(valorCampo);
			
			float valorAComparar;
			conversorCampoAComparar >> valorAComparar;
			claveAComparar = new ClaveReal(valorAComparar);
			break;
		}
		case TipoDatos::TIPO_FECHA: {
			ClaveFecha::TFECHA valorCampo;
			unsigned short anio, dia, mes;
			
			memcpy(&anio, campo, sizeof(short));
			memcpy(&mes, campo + sizeof(short), sizeof(char));
			memcpy(&dia, campo + sizeof(short) + sizeof(char), sizeof(char));
			valorCampo.crear(dia, mes, anio);
			clave = new ClaveFecha(valorCampo);
			
			conversorCampoAComparar.clear();
			conversorCampoAComparar.str("");
			conversorCampoAComparar << nodoListaOperaciones.estructuraNombresDer.nombreCampo.substr(0, 4);
			conversorCampoAComparar >> anio;
			conversorCampoAComparar.clear();
			conversorCampoAComparar.str("");
			conversorCampoAComparar << nodoListaOperaciones.estructuraNombresDer.nombreCampo.substr(4, 2);
			conversorCampoAComparar >> mes;
			conversorCampoAComparar.clear();
			conversorCampoAComparar.str("");
			conversorCampoAComparar << nodoListaOperaciones.estructuraNombresDer.nombreCampo.substr(6, 2);
			conversorCampoAComparar >> dia;
			valorCampo.crear(dia, mes, anio);
			claveAComparar = new ClaveFecha(valorCampo);
			break;
		}
		case TipoDatos::TIPO_STRING: {
			unsigned char longCadena;
			memcpy(&longCadena, campo, Tamanios::TAMANIO_LONGITUD_CADENA);
			char * valorCampo = new char[longCadena + 1];
			memcpy(valorCampo, campo + Tamanios::TAMANIO_LONGITUD_CADENA, longCadena);
			*(valorCampo + longCadena) = 0;
			
			clave = new ClaveVariable(valorCampo);
			
			claveAComparar = new ClaveVariable(nodoListaOperaciones.estructuraNombresDer.nombreCampo);
			break;
		}
			
	}
	
	bool cumple = compararClaves(clave, nodoListaOperaciones.operacion, claveAComparar);
	
	delete clave;
	delete claveAComparar;
	
	return cumple;
}

/*
 * Este metodo se fija si el registro cumple con las restricciones que tiene la lista de operaciones.
 * Si es asi, devuelve true; de lo contrario false.
 * Solo comprobara que cumpla con las restricciones numéricas, no con las que comparan contra otro tipo.
 * Ademas, sacara de la lista de operaciones, aquellas que haya comprobado, dejando solo las que comparan 
 * con otro tipo.
 * Se supone que en la listaOperaciones vienen primero todas las restricciones con valores constantes, 
 * y luego las que comparan contra campos de una tabla.
 * */
bool cumpleRestricciones(char* registro, ListaOperaciones &listaOperaciones, 
						 char operacion, ListaNombresAtributos &listaNombresAtributos, 
						 ListaTiposAtributos &listaTiposAtributos)
{
	bool cumple = true;
	bool continuar = true;
	
	ListaOperaciones::iterator itLO = listaOperaciones.begin();
	NodoListaOperaciones nodo;
	
	// Itera las restricciones a verificar
	while (continuar &&  itLO != listaOperaciones.end()) {
		
		// Si las comparaciones dejan de ser numericas, corta el ciclo.
		if (itLO->estructuraNombresDer.nombreTipo != "")
			continuar = false;
		else {
			
			nodo = *itLO;
			// Se comprueba si se cumple con la restricción que hay en el iterador de la lista de operaciones.
			cumple = cumpleRestriccion(registro, listaNombresAtributos, listaTiposAtributos, nodo);
			
			// Si la operación es AND y hay una que no cumple, ya no cumple con las restricciones.
			if ((!cumple) && (operacion == ExpresionesLogicas::AND))
				continuar = false;
			
			// Si la operación es OR con que se cumpla con una sola restricción, ya se cumple.
			else if ((cumple) && (operacion == ExpresionesLogicas::OR))
				continuar = false;
			
			// Saca el primer elemento de la lista, que es el que acaba de revisar.
			listaOperaciones.pop_front();
			itLO = listaOperaciones.begin();
		}
	
	}
	
	return cumple;
}


string generarSiguienteExtension(const string &extension) {
	
	stringstream conversor;
	unsigned short numero;
	
	conversor << extension.substr(3);
	conversor >> numero;
	conversor.clear();
	conversor.str("");
	conversor << extension.substr(0, 3) << ++numero;
	
	return conversor.str();
	
}


int guardarRegistro(const string &nombreTipo, char* registro, unsigned short tamRegistro) {
	
	DefinitionsManager& defManager = DefinitionsManager::getInstance();
	string nombreArchivo(nombreTipo + EXTENSION_TEMPORAL_INICIAL);
	unsigned short tamBloque = defManager.getTamBloqueDatos(nombreTipo);
	FileManager fileManager(nombreArchivo, tamBloque,
							defManager.getTipoOrgRegistro(nombreTipo),
							defManager.getListaTiposAtributos(nombreTipo));
	
	return fileManager.escribirRegistro(registro, tamRegistro);
}


int join(const MapaRestricciones &mapaOp, MapaExtensiones &mapaExt) {
	
	int resultado = ResultadosMetadata::OK;
	string nombreTipo1, nombreTipo2, nombreTipoAux, nombreArchivo;
	string extensionVieja1, extensionVieja2, extensionNueva1, extensionNueva2;
	string extensionViejaAux, extensionNuevaAux;
	DefinitionsManager& defManager = DefinitionsManager::getInstance();
	ListaOperaciones* listaOp = NULL;
	char *registro1 = NULL, *registro2 = NULL, *auxReg = NULL;
	unsigned short tamRegistro1 = 0, tamRegistro2 = 0, tamRegAux = 0;
	bool cumpleRestriccion = false, registrosJoineados = false;
	unsigned short tamBloque = 0, nroRegistro = 0;
	unsigned char tipoOrg = 0;
	ListaTiposAtributos* listaTiposAtributos = NULL;
	ListaStrings* listaExtensiones = NULL;
	
	for (MapaRestricciones::const_iterator it = mapaOp.begin(); it != mapaOp.end(); ++it) {
		
		nombreTipo1 = it->first;
		listaOp = it->second; // Lista de restricciones del join
		
		for (ListaOperaciones::iterator itTipo1 = listaOp->begin();
			 itTipo1 != listaOp->end(); ++itTipo1) {
			
			extensionVieja1 = *(mapaExt[nombreTipo1]->rbegin());
			extensionNueva1 = generarSiguienteExtension(extensionVieja1);
			
			nombreArchivo = nombreTipo1 + "." + extensionVieja1;
			tamBloque = defManager.getTamBloqueDatos(nombreTipo1);
			tipoOrg = defManager.getTipoOrgRegistro(nombreTipo1);
			listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo1);
			
			FileManager* archivoViejo1 = new FileManager(nombreArchivo, tamBloque, tipoOrg, listaTiposAtributos);
			
			nombreArchivo = nombreTipo1 + "." + extensionNueva1;
			FileManager* archivoNuevo1 = new FileManager(nombreArchivo, tamBloque, tipoOrg, listaTiposAtributos);
			
			nroRegistro = 0;
			
			while (archivoViejo1->siguienteRegistro(registro1, tamRegistro1) == ResultadosMetadata::OK) {
				
				nombreTipo2 = itTipo1->estructuraNombresDer.nombreTipo;
				
				extensionVieja2 = *(mapaExt[nombreTipo2]->rbegin());
				extensionNueva2 = generarSiguienteExtension(extensionVieja2);
				
				nombreArchivo = nombreTipo2 + "." + extensionVieja2;
				tamBloque = defManager.getTamBloqueDatos(nombreTipo2);
				tipoOrg = defManager.getTipoOrgRegistro(nombreTipo2);
				listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo2);
				
				FileManager* archivoViejo2 = new FileManager(nombreArchivo, tamBloque, tipoOrg, listaTiposAtributos);
				
				nombreArchivo = nombreTipo2 + "." + extensionNueva2;
				FileManager* archivoNuevo2 = new FileManager(nombreArchivo, tamBloque, tipoOrg, listaTiposAtributos);
				
				while (archivoViejo2->siguienteRegistro(registro2, tamRegistro2) == ResultadosMetadata::OK) {
					
					cumpleRestriccion = compararCamposRegistros(*defManager.getListaNombresAtributos(nombreTipo1),
																*defManager.getListaNombresAtributos(nombreTipo2),
																*defManager.getListaTiposAtributos(nombreTipo1),
																*defManager.getListaTiposAtributos(nombreTipo2),
																itTipo1->estructuraNombresIzq.nombreCampo,
																itTipo1->estructuraNombresDer.nombreCampo,
																registro1, registro2, itTipo1->operacion);
					if (cumpleRestriccion) {
						archivoNuevo1->escribirRegistro(registro1, tamRegistro1);
						archivoNuevo2->escribirRegistro(registro2, tamRegistro2);
						
						// Se escriben los registros que habian joineado anteriormente con nombreTipo1
						for (ListaOperaciones::iterator itLO = listaOp->begin();
							 itLO != itTipo1; ++itLO) {
							
							nombreTipoAux = itLO->estructuraNombresDer.nombreTipo;
							
							extensionViejaAux = *(mapaExt[nombreTipoAux]->rbegin());
							extensionNuevaAux = generarSiguienteExtension(extensionViejaAux);
							
							nombreArchivo = nombreTipoAux + "." + extensionViejaAux;
							tamBloque = defManager.getTamBloqueDatos(nombreTipoAux);
							tipoOrg = defManager.getTipoOrgRegistro(nombreTipoAux);
							listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipoAux);
							
							FileManager* archivoViejoAux = new FileManager(nombreArchivo, tamBloque, tipoOrg, listaTiposAtributos);
							
							nombreArchivo = nombreTipoAux + "." + extensionNuevaAux;
							FileManager* archivoNuevoAux = new FileManager(nombreArchivo, tamBloque, tipoOrg, listaTiposAtributos);
							
							archivoViejoAux->leerRegistro(auxReg, nroRegistro, tamRegAux);
							archivoNuevoAux->escribirRegistro(auxReg, tamRegAux);
							
							listaExtensiones = mapaExt[nombreTipoAux];
							listaExtensiones->push_back(extensionNuevaAux);
							if (listaExtensiones->size() > 2) {
								FileManager::eliminarArchivoTemporal(nombreTipoAux, listaExtensiones->front());
								listaExtensiones->pop_front();
							}
							
							delete[] auxReg;
							auxReg = NULL;
							delete archivoViejoAux;
							delete archivoNuevoAux;
						}
						
						registrosJoineados = true;
					}
				
				}
				
				// Guardo las extensiones de los nuevos archivos
				if (registrosJoineados) {
					listaExtensiones = mapaExt[nombreTipo2];
					listaExtensiones->push_back(extensionNueva2);
					if (listaExtensiones->size() > 2) {
						FileManager::eliminarArchivoTemporal(nombreTipo2, listaExtensiones->front());
						listaExtensiones->pop_front();
					}
				}
				
				delete archivoViejo2;
				delete archivoNuevo2;
				
				++nroRegistro;
			
			}
			
			// Guardo las extensiones de los nuevos archivos
			if (registrosJoineados) {
				listaExtensiones = mapaExt[nombreTipo1];
				listaExtensiones->push_back(extensionNueva1);
				if (listaExtensiones->size() > 2) {
					FileManager::eliminarArchivoTemporal(nombreTipo1, listaExtensiones->front());
					listaExtensiones->pop_front();
				}
			}
			
			delete archivoViejo1;
			delete archivoNuevo1;
			
			registrosJoineados = false;
		}
		
	}
	
	return resultado;
	
}


int resolverJoins(const MapaRestricciones &mapaJoins, const MapaRestricciones &mapaWhere, MapaExtensiones &mapaExt) {
	
	int resultado = 0;
	
	// El mapa de extensiones no puede venir vacio ya que por lo menos debe
	// existir el archivo con los registros que pasaron las restricciones numéricas.
	resultado = join(mapaWhere, mapaExt);
	
	if (resultado == ResultadosMetadata::OK)
		resultado = join(mapaJoins, mapaExt);
	
	return resultado;
}


// FIN MÉTODOS PARA JOINS


void serializarListaClaves(string& s,MapaValoresAtributos* mapaValores, 
		ListaNombresClaves* listaNombres)
{
	ListaNombresClaves::const_iterator iter;
	
	for (iter = listaNombres->begin(); iter != listaNombres->end(); ++iter){
		s += *iter; 
		s += CodigosPipe::COMIENZO_OPERADOR;
		s += ExpresionesLogicas::IGUAL;
		s += CodigosPipe::FIN_OPERADOR;
		s += (*mapaValores)[*iter];
		s += CodigosPipe::COD_FIN_CLAVE;
	}
}

void serializarListaClaves(string& s, ListaInfoClave* listaClaves)
{
	ListaInfoClave::const_iterator iter;
	
	for (iter = listaClaves->begin(); iter != listaClaves->end(); ++iter)
	{
		s += (*iter).nombreClave;
		s += CodigosPipe::COMIENZO_OPERADOR ;
		s += ExpresionesLogicas::IGUAL;
		s += CodigosPipe::FIN_OPERADOR;
		s += (*iter).valorClave;
		s += CodigosPipe::COD_FIN_CLAVE;
	}	
}

// Serializa una clave para hacer una consulta ordenada por algún campo.
void serializarListaClaves(string &s, const ListaEstructuraNombres &listaNombres)
{
	ListaEstructuraNombres::const_iterator iter;
	
	for (iter = listaNombres.begin(); iter != listaNombres.end(); ++iter)
	{
		s += iter->nombreCampo;
		s += CodigosPipe::COMIENZO_OPERADOR;
		s += ExpresionesLogicas::IGUAL;
		s += CodigosPipe::FIN_OPERADOR;
		s += CodigosPipe::COD_FIN_CLAVE;
	}	
}

// La ListaOperaciones debe contener todas las operaciones relacionadas a un tipo/tabla.
// En esta lista llegan primero las restriccines numéricas y luego las restricciones
// referidas a campos de otras tablas.
// El string 's' se arma con los nombres y los valores (numéricos) de los campos.
// Si se encuentra un <, >, <= o >=, la clave serializada contendrá sólo ese campo.
// NOTA: esta método debería llamarse sólo si la operación del WHERE es una AND.
void serializarListaClaves(string &s, const ListaOperaciones &listaOp)
{
	ListaOperaciones::const_iterator iter;
	bool continuar = true;
	
	for (iter = listaOp.begin(); (iter != listaOp.end()) && continuar; ++iter)
	{
		if (iter->estructuraNombresDer.nombreTipo == "") { // El campo contiene un valor numérico
			
			if (iter->operacion == ExpresionesLogicas::IGUAL)
				s += iter->estructuraNombresIzq.nombreCampo;
			else {
				s = iter->estructuraNombresIzq.nombreCampo;
				continuar = false;
			}
			
			s += CodigosPipe::COMIENZO_OPERADOR + iter->operacion + CodigosPipe::FIN_OPERADOR;
			s += iter->estructuraNombresDer.nombreCampo;
			s += CodigosPipe::COD_FIN_CLAVE;
			
		} else continuar = false; 	// Se terminaron de recorrer los campos que
									// estaban igualados a un valor numérico
	}	
}

ComuDatos* instanciarPipe()
{
	string path= RUTA_CAPA_INDICES;
	path += NOMBRE_CAPA_INDICES;
	
	return new ComuDatos(path);
}

void armarListaClaves(EstructuraCampos &estructuraCampos, ListaInfoClave &listaClaves)
{
	ListaCampos::iterator iter;
	NodoListaClaves nodoListaClaves;
	
	for(iter = estructuraCampos.listaCampos.begin(); iter != estructuraCampos.listaCampos.end(); ++iter) {
		nodoListaClaves.nombreClave = iter->nombreCampo;
		nodoListaClaves.valorClave = iter->valorCampo;
		
		listaClaves.push_back(nodoListaClaves);
	}
	
}


int baja(const string& nombreTipo,  EstructuraCampos &estructuraCampos)
{
	int resultado = ResultadosIndices::OK;	
	unsigned short tamRegistro = 0;
	char* registro = NULL;
	char operacion = 0;
	
	ComuDatos * pipe = NULL;
	
	string valoresClaves("");
	ListaInfoClave listaClaves;
	
	// Validador de nombres de claves a 
	// dar de baja.
	ValidadorDatos validador;
	
	ListaOperaciones listaOperaciones;
	NodoListaOperaciones nodoListaOperaciones;
	
	ListaTiposAtributos *listaTiposAtributos     = NULL;
	ListaNombresAtributos *listaNombresAtributos = NULL;
	ListaNombresClaves* listaNombresClaves       = NULL;
	
	DefinitionsManager& defManager = DefinitionsManager::getInstance();
	
	armarListaClaves(estructuraCampos, listaClaves);
	
	listaNombresAtributos = defManager.getListaNombresAtributos(nombreTipo);
	
	if (listaNombresAtributos){
		listaNombresClaves = defManager.getListaNombresClaves(listaClaves);		

		// Se validan los nombres de las claves por las que se esta 
		// dando de baja.
		resultado = validador.validarNombresClaves(*listaNombresAtributos, *listaNombresClaves);
		
		if (resultado == ResultadosMetadata::OK){
			pipe = instanciarPipe();
			// Codigo de operacion de consulta para la Capa de Indices
			pipe->agregarParametro((unsigned char)OperacionesCapas::INDICES_ELIMINAR, 0); 
			
			// Nombre del tipo de dato a ser dado de baja (Persona/Pelicula)
			pipe->agregarParametro(nombreTipo, 1);
			
			// Se lanza el proceso de la Capa Indices.
			resultado = pipe->lanzar();
			
			if (resultado == ComuDatos::OK){
				
				// Envio de los valores de las claves de los registros
				// a eliminar por el pipe.		
				serializarListaClaves(valoresClaves,&listaClaves);
				
				pipe->escribir(valoresClaves);				
				
				listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
				
				// CapaIndices hace una consulta no indexada y retorna todos los registros para 
				// Comprobar la cláusula WHERE.
				pipe->leer(&resultado);
				
				// Mientras haya registros que mandar
				while (resultado == ResultadosIndices::OK) {
					
					pipe->leer(&tamRegistro);
					
					if (tamRegistro == 0)
						resultado = ResultadosIndices::ERROR_ELIMINACION;
					else{
					
						registro = new char[tamRegistro];
						
						// Se obtiene el registro de datos consultado.
						pipe->leer(tamRegistro, registro);
			
						for (ListaCampos::iterator iter = estructuraCampos.listaCampos.begin(); iter != estructuraCampos.listaCampos.end(); ++iter){
							nodoListaOperaciones.estructuraNombresIzq.nombreTipo = nombreTipo;
							nodoListaOperaciones.estructuraNombresIzq.nombreCampo = iter->nombreCampo;
							nodoListaOperaciones.estructuraNombresDer.nombreTipo = "";
							nodoListaOperaciones.estructuraNombresDer.nombreCampo = iter->valorCampo;
							nodoListaOperaciones.operacion = iter->operacion;
							listaOperaciones.push_back(nodoListaOperaciones);
						}
						
						if(cumpleRestricciones(registro, listaOperaciones, estructuraCampos.operacion, *listaNombresAtributos, *listaTiposAtributos)) { 
							// Si el registro debe ser eliminado, se le avisa a capa indices para que lo haga.
							operacion = OperacionesCapas::INDICES_ELIMINAR;
							pipe->escribir(operacion);
							pipe->leer(&resultado);
						}else{
							// Si el registro no debe ser eliminado, se le avisa a indices que no lo haga.
							operacion = OperacionesCapas::INDICES_NO_OPERACION;
							pipe->escribir(operacion);
						}
						
						if (registro){
							delete[] registro;
							registro = NULL;
						}
						
						// Leo el resultado que indica si se van a recibir más registros
						pipe->leer(&resultado);
					}										
				}
			}
		}
	}else resultado = ResultadosMetadata::TIPO_NO_DEFINIDO;
	
	if (pipe)
		delete pipe;
	
	if (listaNombresClaves)
		delete listaNombresClaves;
	
	if (resultado == ResultadosFisica::FIN_BLOQUES)
		resultado = ResultadosMetadata::OK;
	
	return resultado; 
}

int modificacion(const string& nombreTipo, MapaValoresAtributos &mapaValoresAtributos, 
				 EstructuraCampos &estructuraCampos)
{
	int resultadoModificacion = ResultadosIndices::OK;
	char operacion = 0;
	int resultado = ResultadosIndices::OK;	
	ListaValoresAtributos* listaValAtributos = NULL;
	ListaTipos* listaTipos = NULL;
	
	// Controlador de registros de datos
	DataManager dataManager;
	
	DefinitionsManager& defManager = DefinitionsManager::getInstance();

	//Validador de datos a modificar
	ValidadorDatos validador;
	
	listaTipos = defManager.getListaTipos(nombreTipo);
	
	if(listaTipos){
		
		listaValAtributos = defManager.getListaValoresAtributos(nombreTipo, mapaValoresAtributos);
	
		// Se valida el contenido del registro modificado.
		resultado = validador.validarRegistro(*listaTipos, *listaValAtributos);
			
		ComuDatos * pipe = instanciarPipe();
					
		// Codigo de operacion de consulta para la Capa de Indices
		pipe->agregarParametro((unsigned char)OperacionesCapas::INDICES_MODIFICAR, 0); 
		
		// Nombre del tipo de dato a ser modificado (Persona/Pelicula)
		pipe->agregarParametro(nombreTipo, 1);
		
		// Se lanza el proceso de la Capa Indices.
		int pipeResult = pipe->lanzar();
		
		if (pipeResult == ComuDatos::OK){
			
			// Envio de los valores de las claves de los registros
			// a modificar por el pipe.	
			string valoresClaves("");
			ListaInfoClave listaClaves;
			
			armarListaClaves(estructuraCampos, listaClaves);
			serializarListaClaves(valoresClaves,&listaClaves);
			
			pipe->escribir(valoresClaves);
		
			ListaOperaciones listaOperaciones;
			NodoListaOperaciones nodoListaOperaciones;
			ListaTiposAtributos *listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
			ListaNombresAtributos *listaNombresAtributos = defManager.getListaNombresAtributos(nombreTipo);
			
			unsigned short tamRegistro;
			char* registro = NULL;
			
			// CapaIndices hace una consulta no indexada y retorna todos los registros para 
			// Comprobar la cláusula WHERE.
			pipe->leer(&pipeResult);
			
			// Mientras haya resgistros que mandar
			while (pipeResult == ResultadosIndices::OK) {
				
				pipe->leer(&tamRegistro);
				
				if (tamRegistro == 0) {
					resultadoModificacion = ResultadosIndices::ERROR_MODIFICACION;
					break;
				}
				
				registro = new char[tamRegistro];
				
				// Se obtiene el registro de datos consultado.
				pipe->leer(tamRegistro, registro);

				// Se arma una lista con todas las cláusulas que deben cumplir los registros para ser modificados.
				for (ListaCampos::iterator iter = estructuraCampos.listaCampos.begin(); iter != estructuraCampos.listaCampos.end(); ++iter){
					nodoListaOperaciones.estructuraNombresIzq.nombreTipo = nombreTipo;
					nodoListaOperaciones.estructuraNombresIzq.nombreCampo = iter->nombreCampo;
					nodoListaOperaciones.estructuraNombresDer.nombreTipo = "";
					nodoListaOperaciones.estructuraNombresDer.nombreCampo = iter->valorCampo;
					nodoListaOperaciones.operacion = iter->operacion;
					listaOperaciones.push_back(nodoListaOperaciones);
				}
				
		
				if(cumpleRestricciones(registro, listaOperaciones, estructuraCampos.operacion, *listaNombresAtributos, *listaTiposAtributos)) { 
		
					// Si el registro debe ser eliminado, se le avisa a capa indices para que lo haga.
					operacion = OperacionesCapas::INDICES_MODIFICAR;
					// Avisa a CapaIndices que tiene que modificar el registro.
		
					pipe->escribir(operacion);
		
				
					// Lee si encontro la clave del registro a modificar en el indice.
					pipe->leer(&resultadoModificacion);
		
					if (resultadoModificacion == ResultadosIndices::OK) {
						
						pipe->leer(&tamRegistro);
						pipe->leer(tamRegistro, registro);
						
						// Crea un nuevo registro con las modificaciones pertinentes. Devuelve su nueva longitud.
						tamRegistro = dataManager.crearRegistroModificacion(*listaTiposAtributos, *listaValAtributos, registro);
						
						// Se envia el tamano del registro modificado por el pipe.
						pipe->escribir(tamRegistro);
						
						// Se envia el nuevo registro con la modificacion.
						pipe->escribir(dataManager.getRegistro(), tamRegistro);
					}
	
					// Lee el resultado de la modificación.
					pipe->leer(&resultadoModificacion);
					
		
				}
				else {
					// Si el registro no debe ser eliminado, se le avisa a indices que no lo haga.
					operacion = OperacionesCapas::INDICES_NO_OPERACION;
		
					pipe->escribir(operacion);
				}
				
				delete[] registro;
				registro = NULL;
				
				// Leo el resultado que indica si se van a recibir más registros
				pipe->leer(&pipeResult);
								
			}
		}	
		delete pipe;
	}	
	return resultadoModificacion; 	
}


int alta(const string& nombreTipo, MapaValoresAtributos& mapaValoresAtributos)
{
	int resultado              = ResultadosIndices::OK;
	unsigned short tamRegistro = 0;
	ComuDatos *pipe            = NULL;
	
	// Validador de datos a dar de alta.
	ValidadorDatos validador;
	
	// Controlador de registros de datos
	DataManager dataManager;
	
	string valoresClaves;
	// Lista de valores de todos los atributos dentro de un registro.
	ListaValoresAtributos* listaValAtributos = NULL;	
	// Lista de los tipos de todos los atributos dentro de un registro.
	ListaTiposAtributos* listaTiposAtributos = NULL;
	ListaTipos* listaTipos                   = NULL;
	ListaNombresClaves *listaNomClaves		 = NULL;
	ListaNombresAtributos *listaNomAtributos = NULL;
	
	// Se instancia el DefinitionsManager (conocedor absoluto del universo).
	DefinitionsManager& defManager = DefinitionsManager::getInstance();
	
	listaValAtributos = defManager.getListaValoresAtributos(nombreTipo, mapaValoresAtributos);
	
	if (listaValAtributos){
		listaTipos = defManager.getListaTipos(nombreTipo);
		
		// Se valida el registro a insertar.
		resultado = validador.validarRegistro(*listaTipos, *listaValAtributos);
		
		if (resultado == ResultadosMetadata::OK){
			
			listaNomAtributos = defManager.getListaNombresAtributos(nombreTipo);
			// Se validan los nombres de los campos del registro a dar de alta
			resultado = validador.validarNombresAtributos(*listaNomAtributos, mapaValoresAtributos);

			if (resultado == ResultadosMetadata::OK){
				pipe = instanciarPipe();
				
				pipe->agregarParametro((unsigned char)OperacionesCapas::INDICES_INSERTAR, 0); 
				// Nombre del tipo de dato a ser dado de alta.
				pipe->agregarParametro(nombreTipo, 1);
				
				// Se lanza el proceso de la Capa Indices.
				resultado = pipe->lanzar();
			
				if (resultado == ComuDatos::OK){
	
					// Envio de los valores de las claves de los registros
					// a modificar por el pipe.		
					listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
					listaNomClaves      = defManager.getListaNombresClavesPrimarias(nombreTipo);
			
					serializarListaClaves(valoresClaves, &mapaValoresAtributos, listaNomClaves);
					
					pipe->escribir(valoresClaves);
					
					// Se obtiene el resultado de la operacion.
					pipe->leer(&resultado);
	
					if (resultado == ResultadosIndices::CLAVE_NO_ENCONTRADA){				
	
						// Se crea el registro a dar de alta y se obtiene su longitud
						tamRegistro = dataManager.crearRegistroAlta(*listaValAtributos, *listaTiposAtributos);
						
						pipe->escribir(tamRegistro);
						// Se envia el registro a dar de alta por el pipe.
						pipe->escribir(dataManager.getRegistro(), tamRegistro);
						
						// Se obtiene el resultado de la insercion
						pipe->leer(&resultado);
					}
		
				}
			}
		}
	}else resultado = ResultadosMetadata::TIPO_NO_DEFINIDO;

	if (pipe)
		delete pipe;	
	
	if (listaValAtributos)
		delete listaValAtributos;		

	return resultado;
}


void eliminarArchivosTemporales(const MapaExtensiones &mapaExt) {
	
	ListaStrings* listaExtensiones = NULL;
	
	for (MapaExtensiones::const_iterator iterMapa = mapaExt.begin();
		 iterMapa != mapaExt.end(); ++iterMapa) {
		
		listaExtensiones = iterMapa->second;
		
		for (ListaStrings::const_iterator iterLista = listaExtensiones->begin();
			 iterLista != listaExtensiones->end(); ++iterLista)
			FileManager::eliminarArchivosTemporales(*iterLista);
		
	}
	
}


int consulta(EstructuraConsulta &estructura, ComuDatos &pipeCapaConsultas) {
	
	char* registro = NULL;
	unsigned short tamRegistro   = 0; 	 // Tamaño de un registro.
	unsigned short cantRegistros = 0;    // Cant. de registros que responden a la consulta dada.
	string valoresClaves("");
	string nombreTipo("");
	int pipeResult;
	unsigned char operacionCapaIndices = 0;
	bool seGuardoRegistro = true;
	
	// Objeto validador de los datos de entrada
	ValidadorDatos validador;
	
	// Se instancia el DefinitionsManager (conocedor absoluto del universo).
	DefinitionsManager& defManager = DefinitionsManager::getInstance();
	
	// Mapas que contienen listas de restricciones para cada tipo/clase.
	MapaRestricciones mapaJoins, mapaWhere;
	MapaExtensiones mapaExt;
	
	// Se verifica si la consulta se debe devolver ordenada
	if (estructura.listaOrderBy.size() > 0) {
		// Se invierten los campos y los operadores lógicos de las restricciones
		// para que siempre quede como primer tabla la tabla por la que se debe ordenar.
		invertirOperacionLogica(estructura.listaOrderBy.begin()->nombreTipo, estructura.estructuraWhere.listaOperaciones);
		invertirOperacionLogica(estructura.listaOrderBy.begin()->nombreTipo, estructura.estructuraJoins.listaOperaciones);
		// Cargo los mapas de restricciones con las listas ordenadas por la tabla
		// por la que se debe ordenar la consulta.
		cargarMapasRestricciones(estructura.estructuraJoins.listaOperaciones, mapaJoins);
		cargarMapasRestricciones(estructura.estructuraWhere.listaOperaciones, mapaWhere);
	} else {
		// Cargo las restricciones sin ordenarlas
		cargarMapasRestricciones(estructura.estructuraJoins.listaOperaciones, mapaJoins);
		cargarMapasRestricciones(estructura.estructuraWhere.listaOperaciones, mapaWhere);
	}
	
	// Itero la lista con los nombres de los distintos tipos (tablas) que
	// intervienen en la consulta.
	ListaStrings& listaNombresTipos = estructura.listaNombresTipos;
	
	for (ListaStrings::iterator it = listaNombresTipos.begin();
		 (it != listaNombresTipos.end()) && (seGuardoRegistro)
		 && (pipeResult != ResultadosIndices::ERROR_NO_HAY_INDICE);
		 ++it) {
	
		nombreTipo = *it;
		seGuardoRegistro = false;
		
		// Se verifica si la consulta se debe devolver ordenada
		if ((estructura.listaOrderBy.size() > 0) && (estructura.listaOrderBy.begin()->nombreTipo == nombreTipo)) {
			// Se serializa la clave para indicarle a la capa de índices los campos por los
			// que debe ordenar. Si no se encuentra un índice para ordenar por esos campos,
			// se devuelve un error.
			serializarListaClaves(valoresClaves, estructura.listaOrderBy);
			operacionCapaIndices = OperacionesCapas::INDICES_CONSULTAR;
		} else {
			if (estructura.estructuraWhere.operacion == ExpresionesLogicas::AND) {
				// Se construye la clave para enviar a la capa de índices
				serializarListaClaves(valoresClaves, estructura.estructuraWhere.listaOperaciones);
				// Codigo de operacion de consulta para la Capa de Indices
				if (valoresClaves.size() > 0)
					operacionCapaIndices = OperacionesCapas::INDICES_CONSULTAR;
				else
					operacionCapaIndices = OperacionesCapas::INDICES_CONSULTAR_TODO;
			} else
				operacionCapaIndices = OperacionesCapas::INDICES_CONSULTAR_TODO;
		}
		
		ComuDatos* pipe = instanciarPipe();
		
		// Codigo de operacion de consulta para la Capa de Indices
		pipe->agregarParametro(operacionCapaIndices, 0); 
		// Nombre del tipo de dato a ser dado de alta
		pipe->agregarParametro(nombreTipo, 1);
		
		// Se lanza el proceso de la Capa Indices.
		pipeResult = pipe->lanzar();
		
		if (pipeResult == ComuDatos::OK){						
			// Envio de los valores de las claves a consultar por el pipe
			pipe->escribir(valoresClaves);
	
			// Obtencion del resultado de la operacion
			pipe->leer(&pipeResult);
			
			if (pipeResult == ResultadosIndices::OK) {
				
				bool cumpleRestriccion = false;
				
				do {
					// Se obtiene la cantidad de registros que responden 
					// a la consulta realizada.
					pipe->leer(&cantRegistros); 
					
					if (cantRegistros > 0) {
						
						for (unsigned short i = 0; i < cantRegistros; i++){
							// Se obtiene el tamaño del registro a levantar
							pipe->leer(&tamRegistro);
						
							if (tamRegistro == 0) {
								pipeResult = ResultadosIndices::ERROR_CONSULTA;
								break;
							}
							
							registro = new char[tamRegistro];
							
							// Se obtiene el registro de datos consultado.
							pipe->leer(tamRegistro, registro);
							
							cumpleRestriccion = cumpleRestricciones(registro, *mapaWhere[nombreTipo],
																	estructura.estructuraWhere.operacion,
																	*defManager.getListaNombresAtributos(nombreTipo),
																	*defManager.getListaTiposAtributos(nombreTipo));
							if (cumpleRestriccion) {
								guardarRegistro(nombreTipo, registro, tamRegistro);
								seGuardoRegistro = true;
							}
							
							delete[] registro;
							
							cout << endl;
						}
						
						registro = NULL;
					}
					
					// Leo el resultado que indica si se van a recibir más bloques
					pipe->leer(&pipeResult);
					
				} while (pipeResult == ResultadosIndices::OK);
				
				if (seGuardoRegistro) {
					// Se agrega la extensión del primer archivo temporal
					ListaStrings* listaExtensiones = new ListaStrings();
					listaExtensiones->push_back(EXTENSION_TEMPORAL_INICIAL);
					mapaExt[nombreTipo] = listaExtensiones;
				}
				
			}
			
		}
		
		delete pipe;
		
	}
	
	// Una vez que se terminaron de aplicar los filtros/restricciones a los registros
	// de las distintas tablas, se resuelven los joins (en caso que haya).
	if (seGuardoRegistro) {
		pipeResult = resolverJoins(mapaJoins, mapaWhere, mapaExt);
		pipeCapaConsultas.escribir(pipeResult);
		
		if (pipeResult == ResultadosMetadata::OK) {
			
			// Se envian los registros a la capa de consultas
			ListaStrings& listaNombresTipos = estructura.listaNombresTipos;
			ListaTiposAtributos* listaTiposAtributos = NULL;
			unsigned short nroRegistro = 0, tamBloque = 0, tamRegistro = 0;
			string nombreArchivo, extension;
			char* registro = NULL;
			unsigned char tipoOrg;
			
			while (pipeResult == ResultadosMetadata::OK) {
				
				// Se envía la cantidad de registros
				pipeCapaConsultas.escribir((unsigned short)listaNombresTipos.size());
				
				for (ListaStrings::const_iterator it = listaNombresTipos.begin();
					 (it != listaNombresTipos.end()) && (pipeResult == ResultadosMetadata::OK); ++it) {
					
					nombreTipo = *it;
					// Obetengo la última extensión del archivo temporal
					extension = mapaExt[nombreTipo]->back();
					nombreArchivo = nombreTipo + "." + extension;
					tamBloque = defManager.getTamBloqueDatos(nombreTipo);
					tipoOrg = defManager.getTipoOrgRegistro(nombreTipo);
					listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
					
					FileManager* archivoResultados = new FileManager(nombreArchivo, tamBloque, tipoOrg, listaTiposAtributos);
					pipeResult = archivoResultados->leerRegistro(registro, nroRegistro, tamRegistro);
					
					// Si se produce un error tamRegistro es 0 y la capa superior lo interpreta
					pipeCapaConsultas.escribir(tamRegistro);
					
					if (pipeResult == ResultadosMetadata::OK) {
						pipeCapaConsultas.escribir(registro, tamRegistro);
						delete[] registro;
						registro = NULL;
					}
					
					delete archivoResultados;
					
				}
				
				// Se indica a la capa superior si debe seguir recibiendo bloques
				pipeCapaConsultas.escribir(pipeResult);
				
				++nroRegistro;
				
			}
			
		}
	}
	
	// Se destruyen los mapas de las restricciones
	destruirMapaRestricciones(mapaWhere);
	destruirMapaRestricciones(mapaJoins);
	
	//Se eliminan los archivos temporales
	eliminarArchivosTemporales(mapaExt);
	
	// Se destruye el mapa de las extensiones de los archivos temporales
	destruirMapaExtensiones(mapaExt);
	
	return pipeResult;
}



int main(int argc, char* argv[]) 
{
	Resultado resultado;
	int pipeResult = ResultadosIndices::OK;
	
	if (argc > 1){
		
		ComuDatos* pipe = new ComuDatos(argv); //Instancia del pipe de comunicacion		
		
		unsigned char operacion = 0;    // Tipo de operacion a ejecutar
		string nombreTipo("");
		
		// Controlador de registros de datos
		DataManager dataManager;
		
		pipe->parametro(0, operacion);
	
		switch(operacion){
			case OperacionesCapas::METADATA_CONSULTA:
			{	
				EstructuraConsulta estructuraConsulta;
				
				// Se recibe el contenido de estructuraConsulta, pero sin incluir la 
				// lista de campos seleccionados.
				
				// Se recibe la listaNombresTipos.
				unsigned int cantidadElementos;
				pipe->leer(&cantidadElementos);
				
				//unsigned int longitudCadena;
				string cadena;
				
				for (unsigned int i = 0; i < cantidadElementos; i++) {
					pipe->leer(cadena);
					estructuraConsulta.listaNombresTipos.push_back(cadena);
				}
				
				// Se recibe la estructuraJoins
				pipe->leer(&estructuraConsulta.estructuraJoins.operacion);
				pipe->leer(&cantidadElementos);
				
				NodoListaOperaciones nodoListaOperaciones;
				// Se itera para obtener cada nodo de la lista de operaciones.
				for (unsigned int i=0; i<cantidadElementos; i++) {
					pipe->leer(nodoListaOperaciones.estructuraNombresIzq.nombreTipo);
					pipe->leer(nodoListaOperaciones.estructuraNombresIzq.nombreCampo);
					pipe->leer(nodoListaOperaciones.estructuraNombresDer.nombreTipo);
					pipe->leer(nodoListaOperaciones.estructuraNombresDer.nombreCampo);
					
					pipe->leer(&nodoListaOperaciones.operacion);
					
					estructuraConsulta.estructuraJoins.listaOperaciones.push_back(nodoListaOperaciones);
				}
				
				// Se recibe la estructuraWhere
				pipe->leer(&estructuraConsulta.estructuraWhere.operacion);
				pipe->leer(&cantidadElementos);
				
				for (unsigned int i = 0; i < cantidadElementos; ++i) {
					pipe->leer(nodoListaOperaciones.estructuraNombresIzq.nombreTipo);
					pipe->leer(nodoListaOperaciones.estructuraNombresIzq.nombreCampo);
					pipe->leer(nodoListaOperaciones.estructuraNombresDer.nombreTipo);
					pipe->leer(nodoListaOperaciones.estructuraNombresDer.nombreCampo);
					
					pipe->leer(&nodoListaOperaciones.operacion);
					
					estructuraConsulta.estructuraWhere.listaOperaciones.push_back(nodoListaOperaciones);
				}
				
				// Se recibe el orderBy
				pipe->leer(&cantidadElementos);
				
				EstructuraNombres estructuraNombres;
				
				for (unsigned int i = 0; i < cantidadElementos; ++i) {
					pipe->leer(estructuraNombres.nombreTipo);
					pipe->leer(estructuraNombres.nombreCampo);
					
					estructuraConsulta.listaOrderBy.push_back(estructuraNombres);
				}
				
				// CONSULTAS LLAMANDO A CAPA INDICES
				pipeResult = consulta(estructuraConsulta, *pipe);
				
			}break;
			
			case OperacionesCapas::METADATA_BAJA:
			{
				unsigned int cantidadElementos;
				
				// Obtiene el nombre del tipo sobre el cual se hace la baja.
				pipe->parametro(1,nombreTipo);
				
				EstructuraCampos estructuraCampos;
				
				pipe->leer(&estructuraCampos.operacion);
	
				pipe->leer(&cantidadElementos);
				
				NodoListaCampos nodoListaCampos;
				
				// Se itera para recibir cada nodo de la lista de campos.
				for (unsigned int i = 0; i < cantidadElementos; ++i) {
					pipe->leer(nodoListaCampos.nombreCampo);
					pipe->leer(nodoListaCampos.valorCampo);
					pipe->leer(&nodoListaCampos.operacion);
					
					estructuraCampos.listaCampos.push_back(nodoListaCampos);
				}
				
				pipeResult = baja(nombreTipo, estructuraCampos);
				
				pipe->escribir(pipeResult);
				
			}break;
			
			case OperacionesCapas::METADATA_MODIFICACION:
			{		
				unsigned int cantidadElementos;
				string nombreAtributo, valorAtributo;
				pipe->parametro(1, nombreTipo);
				
				// Se recibe el mapaValoresAtributos.
				MapaValoresAtributos mapaValoresAtributos;
				
				pipe->leer(&cantidadElementos);
				
				for (unsigned int i = 0; i < cantidadElementos; ++i) {
					pipe->leer(nombreAtributo);
					pipe->leer(valorAtributo);
					
					mapaValoresAtributos[nombreAtributo] = valorAtributo;
				}
				
				// Se recibe la información sobre los registros a modificar,
				// dentro de una estructuraCampos.
				EstructuraCampos estructuraCampos;
				
				pipe->leer(&estructuraCampos.operacion);
				
				// Se obtiene la cantidad de elementos en la lista de campos.
				pipe->leer(&cantidadElementos);
				
				NodoListaCampos nodoListaCampos;
				
				for (unsigned int i = 0; i < cantidadElementos; ++i) {
					pipe->leer(nodoListaCampos.nombreCampo);
					pipe->leer(nodoListaCampos.valorCampo);
					pipe->leer(&nodoListaCampos.operacion);
					
					estructuraCampos.listaCampos.push_back(nodoListaCampos);
				}

				pipeResult = modificacion(nombreTipo, mapaValoresAtributos, estructuraCampos);
				
				pipe->escribir(pipeResult);
				
			}break;
			
			case OperacionesCapas::METADATA_ALTA:
			{	
				unsigned int cantidadElementos;
				//unsigned int longitudCadena;
				string nombreAtributo, valorAtributo;
				pipe->parametro(1,nombreTipo);

				// Se recibe el mapaValoresAtributos.
				MapaValoresAtributos mapaValoresAtributos;
				
				pipe->leer(&cantidadElementos);
				
				for (unsigned int i = 0; i < cantidadElementos; ++i) {
					pipe->leer(nombreAtributo);
					pipe->leer(valorAtributo);
					
					mapaValoresAtributos[nombreAtributo] = valorAtributo;
				}

				pipeResult = alta(nombreTipo, mapaValoresAtributos);
				
				pipe->escribir(pipeResult);
									
			}break;
			
			default:
				pipeResult = ResultadosMetadata::OPERACION_INVALIDA;
		}
		
		if (pipe)
			delete pipe;
	}
	
	cout.flush();

	cout << "Fin Capa Metadata" << endl;
	cout << endl;
	
	return pipeResult;	
}

