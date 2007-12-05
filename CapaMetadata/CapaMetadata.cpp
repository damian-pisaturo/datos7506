////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcionr
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

typedef map<string, DefinitionsManager::ListaOperaciones*> MapaRestricciones;

typedef map<string, DefinitionsManager::ListaStrings*> MapaExtensiones;

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

void invertirOperacionLogica(const string &nombreTipo, DefinitionsManager::ListaOperaciones &lista) {
	
	DefinitionsManager::EstructuraNombres estructuraAuxiliar;
	
	for (DefinitionsManager::ListaOperaciones::iterator it = lista.begin();
		 it != lista.end(); ++it) {
		
		if (it->estructuraNombresDer.nombreTipo == nombreTipo) {
			
			estructuraAuxiliar = it->estructuraNombresIzq;
			it->estructuraNombresIzq = it->estructuraNombresDer;
			it->estructuraNombresDer = estructuraAuxiliar;
			it->operacion = invertirOperadorLogico(it->operacion);
			
		}	
		
	}	
	
}

void cargarMapasRestricciones(DefinitionsManager::ListaOperaciones &lista, MapaRestricciones &mapa) {
	
	DefinitionsManager::ListaOperaciones* listaOp = NULL;
	MapaRestricciones::iterator iterMapa;
	string nombreTipo("");
	
	for (DefinitionsManager::ListaOperaciones::iterator it = lista.begin();
		 it != lista.end(); ++it) {
		
		nombreTipo = it->estructuraNombresIzq.nombreTipo;
		
		iterMapa = mapa.find(nombreTipo);
		
		if (iterMapa != mapa.end()) // Se encontró el nombre del tipo en el mapa	
			iterMapa->second->push_back(*it);
		else {	
			listaOp = new DefinitionsManager::ListaOperaciones();
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


bool compararClaves(Clave *clave, unsigned char operacion, Clave* claveAComparar)
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

char* obtenerCampo(unsigned int numeroCampo, char* registro, DefinitionsManager::ListaTiposAtributos &listaTiposAtributos) {

	unsigned short offset = 0;
	
	DefinitionsManager::ListaTiposAtributos::iterator itLTA = listaTiposAtributos.begin();
	
	if (numeroCampo < listaTiposAtributos.size()) {
		
		if (itLTA->tipo == TipoDatos::TIPO_VARIABLE) 
			offset = Tamanios::TAMANIO_LONGITUD;
	
		++itLTA;
		
		for (unsigned int i=1; i<numeroCampo; i++) {
			
			switch (itLTA->tipo) {
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

int compararCamposRegistros(DefinitionsManager::ListaNombresAtributos &listaNombresAtributos1, DefinitionsManager::ListaNombresAtributos
							 &listaNombresAtributos2, DefinitionsManager::ListaTiposAtributos &listaTiposAtributos1,
							 DefinitionsManager::ListaTiposAtributos &listaTiposAtributos2, string nombre1, string nombre2,
							 char* registro1, char* registro2, char operacion)
{
	DefinitionsManager::ListaNombresAtributos::iterator itNombres = listaNombresAtributos1.begin();
 
	// Obtiene el campo pedido del primer registro.
	unsigned int numeroCampo = 1;

	while ((nombre1 != *itNombres) && (itNombres != listaNombresAtributos1.end())) {
		++numeroCampo;
		++itNombres;
	}
	
	char* campo1 = obtenerCampo(numeroCampo, registro1, listaTiposAtributos1);
	
	DefinitionsManager::ListaTiposAtributos::iterator itTipos = listaTiposAtributos1.begin();

	// itera la lista de tipos para obtener el tipo del campo "campo1".
	for (unsigned int i = 0; i < numeroCampo; ++i)
		++itTipos;
	
	int tipoCampo1;
	Clave * clave1;
	
	// Se genera una clave que contenga el campo1 para usarla luego en la comparación con el campo2
	switch (itTipos->tipo) {
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
	
	Clave* clave2;
	
	// Comprueba que el campo2 sea del mismo tipo que el campo1.
	// Y genera con el una clave para usar en la comparacion.
	switch (itTipos->tipo) {
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

bool cumpleRestriccion(char *registro, DefinitionsManager::ListaNombresAtributos &listaNombresAtributos, 
				  	   DefinitionsManager::ListaTiposAtributos &listaTiposAtributos, 
				  	   DefinitionsManager::NodoListaOperaciones &nodoListaOperaciones)
{
	DefinitionsManager::ListaNombresAtributos::iterator itNombres = listaNombresAtributos.begin();
	unsigned int numeroCampo = 1;
	
	while ((nodoListaOperaciones.estructuraNombresIzq.nombreCampo != *itNombres) && (itNombres != listaNombresAtributos.end())) {
		++numeroCampo;
		++itNombres;
	}
	
	char* campo = obtenerCampo(numeroCampo, registro, listaTiposAtributos);
	
	DefinitionsManager::ListaTiposAtributos::iterator itTipos = listaTiposAtributos.begin();

	// itera la lista de tipos para obtener el tipo del campo "campo".
	for (unsigned int i = 0; i < numeroCampo; ++i)
		++itTipos;
	
	stringstream conversorCampoAComparar;
	Clave* clave;
	Clave* claveAComparar;
	
	// En "nodoListaOperaciones.estructuraNombresDer.nombreCampo" se encuentra el valor constante 
	// con el cual hay que hacer la comparación.
	conversorCampoAComparar << nodoListaOperaciones.estructuraNombresDer.nombreCampo;
	
	switch (itTipos->tipo) {
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
bool cumpleRestricciones(char* registro, DefinitionsManager::ListaOperaciones &listaOperaciones, 
						 char operacion, DefinitionsManager::ListaNombresAtributos &listaNombresAtributos, 
						 DefinitionsManager::ListaTiposAtributos &listaTiposAtributos)
{
	bool cumple = true;
	bool continuar = true;
	
	DefinitionsManager::ListaOperaciones::iterator itLO = listaOperaciones.begin();
	DefinitionsManager::NodoListaOperaciones nodo;
	
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
	cout << "cumple restricciones: " << cumple << endl;
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
	
	// TODO Pedirle el tamaño del bloque de datos al DM
	FileManager fileManager(nombreArchivo, Tamanios::TAMANIO_BLOQUE_DATO,
							defManager.getTipoOrgRegistro(nombreTipo),
							defManager.getListaTiposAtributos(nombreTipo));
	
	return fileManager.escribirRegistro(registro, tamRegistro);
}

/*
int join(const MapaRestricciones &mapaOp, MapaExtensiones &mapaExt) {
	
	int resultado = 0;
	string nombreTipo1, nombreTipo2, nombreArchivo;
	string extensionVieja1, extensionVieja2, extensionNueva1, extensionNueva2;
	DefinitionsManager& defManager = DefinitionsManager::getInstance();
	DefinitionsManager::ListaOperaciones* listaOp = NULL;
	char *registro1 = NULL, *registro2 = NULL; *auxReg = NULL;
	unsigned short tamRegistro1 = 0, tamRegistro2 = 0;
	bool cumpleRestriccion = false;
	
	// TODO Pedir el tamaño del bloque al DM
	
	for (MapaRestricciones::const_iterator it = mapaOp.begin(); it != mapaOp.end(); ++it) {
		
		nombreTipo1 = it->first;
		listaOp = it->second; // Lista de restricciones del join
		
		extensionVieja1 = *(mapaExt[nombreTipo1]->rbegin());
		extensionNueva1 = generarSiguienteExtension(extensionVieja1);
		
		nombreArchivo = nombreTipo1 + "." + extensionVieja1;
		FileManager* archivoViejo1 = new FileManager(nombreArchivo, Tamanios::TAMANIO_BLOQUE_DATO,
								  					defManager.getTipoOrgRegistro(nombreTipo1),
								  					defManager.getListaTiposAtributos(nombreTipo1));
		
		nombreArchivo = nombreTipo1 + "." + extensionNueva1;
		FileManager* archivoNuevo1 = new FileManager(nombreArchivo, Tamanios::TAMANIO_BLOQUE_DATO,
								  					defManager.getTipoOrgRegistro(nombreTipo1),
								  					defManager.getListaTiposAtributos(nombreTipo1));
		
		for (DefinitionsManager::ListaOperaciones::iterator itTipo1 = listaOp->begin();
			 itTipo1 != listaOp->end(); ++itTipo1) {
			
			resultado = archivoViejo1->siguienteRegistro(registro1, tamRegistro1);
			
			// TODO Verificar que registro1 no sea igual al registro anterior
			
			if (resultado == ResultadosMetadata::OK) {
				
				nombreTipo2 = it->estructuraNombresDer.nombreTipo;
				
				extensionVieja2 = *(mapaExt[nombreTipo2]->rbegin());
				extensionNueva2 = generarSiguienteExtension(extensionVieja2);
				
				nombreArchivo = nombreTipo2 + "." + extensionVieja2;
				FileManager* archivoViejo2 = new FileManager(nombreArchivo, Tamanios::TAMANIO_BLOQUE_DATO,
										  					defManager.getTipoOrgRegistro(nombreTipo2),
										  					defManager.getListaTiposAtributos(nombreTipo2));
				
				nombreArchivo = nombreTipo2 + "." + extensionNueva2;
				FileManager* archivoNuevo2 = new FileManager(nombreArchivo, Tamanios::TAMANIO_BLOQUE_DATO,
										  					defManager.getTipoOrgRegistro(nombreTipo2),
										  					defManager.getListaTiposAtributos(nombreTipo2));
				
				while (archivoViejo2->siguienteRegistro(registro2, tamRegistro2) == ResultadosMetadata::OK) {
					
					cumpleRestriccion = compararCamposRegistros(*defManager.getListaNombresAtributos(nombreTipo1),
																*defManager.getListaNombresAtributos(nombreTipo2),
																*defManager.getListaTiposAtributos(nombreTipo1),
																*defManager.getListaTiposAtributos(nombreTipo2),
																itTipo1->estructuraNombresIzq.nombreCampo,
																itTipo1->estructuraNombresDer.nombreCampo,
																registro1, registro2, itTipo1->operacion);
				}
				
				delete archivoViejo2;
				delete archivoViejo1;
			
			}
			
		}
		
	}
	
	return resultado;
	
}
*/

int resolverJoins(const MapaRestricciones &mapaJoins, const MapaRestricciones &mapaWhere, MapaExtensiones &mapaExt) {
	
	int resultado = 0;
	
	// El mapa de extensiones no puede venir vacio ya que por lo menos debe
	// existir el archivo con los registros que pasaron las restricciones numéricas.
	
	return resultado;
}


// FIN MÉTODOS PARA JOINS


void serializarListaClaves(string& s, DefinitionsManager::MapaValoresAtributos* mapaValores, 
		DefinitionsManager::ListaNombresClaves* listaNombres)
{
	DefinitionsManager::ListaNombresClaves::const_iterator iter;
	
	for (iter = listaNombres->begin(); iter != listaNombres->end(); ++iter){
		s += *iter; 
		s += "=";
		s += (*mapaValores)[*iter];
		s += CodigosPipe::COD_FIN_CLAVE;
	}
}

void serializarListaClaves(string& s, DefinitionsManager::ListaClaves* listaClaves)
{
	DefinitionsManager::ListaClaves::const_iterator iter;
	
	for (iter = listaClaves->begin(); iter != listaClaves->end(); ++iter)
	{
		s += (*iter).nombreClave;
		s += "=";
		s += (*iter).valorClave;
		s += CodigosPipe::COD_FIN_CLAVE;
	}	
}

void serializarListaClaves(string &s, const DefinitionsManager::ListaEstructuraNombres &listaNombres)
{
	DefinitionsManager::ListaEstructuraNombres::const_iterator iter;
	
	for (iter = listaNombres.begin(); iter != listaNombres.end(); ++iter)
	{
		s += iter->nombreCampo;
		s += "=";
		s += CodigosPipe::COD_FIN_CLAVE;
	}	
}

ComuDatos* instanciarPipe()
{
	string path= RUTA_CAPA_INDICES;
	path += NOMBRE_CAPA_INDICES;
	
	return new ComuDatos(path);
}

void armarListaClaves(DefinitionsManager::EstructuraCampos &estructuraCampos, DefinitionsManager::ListaClaves &listaClaves)
{
	DefinitionsManager::ListaCampos::iterator iter;
	DefinitionsManager::NodoListaClaves nodoListaClaves;
	
	for(iter = estructuraCampos.listaCampos.begin(); iter != estructuraCampos.listaCampos.end(); ++iter) {
		nodoListaClaves.nombreClave = iter->nombreCampo;
		nodoListaClaves.valorClave = iter->valorCampo;
		
		listaClaves.push_back(nodoListaClaves);
	}
	
}


int baja(string nombreTipo,  DefinitionsManager::EstructuraCampos &estructuraCampos)
{
	//TODO: FALTA VER EL TEMA DE LAS RESTRICCIONES!!!
	ComuDatos * pipe = instanciarPipe();
		
	// Codigo de operacion de consulta para la Capa de Indices
	pipe->agregarParametro((unsigned char)OperacionesCapas::INDICES_ELIMINAR, 0); 
	// Nombre del tipo de dato a ser dado de alta (Persona/Pelicula)
	pipe->agregarParametro(nombreTipo, 1);
	
	// Se lanza el proceso de la Capa Indices.
	int pipeResult = pipe->lanzar();
	
	if (pipeResult == ComuDatos::OK){
		
		// Envio de los valores de las claves de los registros
		// a eliminar por el pipe.
		
		string valoresClaves;
		DefinitionsManager::ListaClaves listaClaves;
		
		armarListaClaves(estructuraCampos, listaClaves);
		
		serializarListaClaves(valoresClaves,&listaClaves);
		pipe->escribir((unsigned short)valoresClaves.length());
		pipe->escribir(valoresClaves);
		
		// Se obtiene el resultado de la operacion
		pipe->leer(&pipeResult);
		
	}
	
	delete pipe;
	
	return pipeResult; 
}

int modificacion(string nombreTipo,DefinitionsManager::MapaValoresAtributos &mapaValoresAtributos, 
				 DefinitionsManager::EstructuraCampos &estructuraCampos)
{
	ComuDatos *pipe = instanciarPipe();
	
	// Codigo de operacion de consulta para la Capa de Indices
	pipe->agregarParametro((unsigned char)OperacionesCapas::INDICES_MODIFICAR, 0); 
	// Nombre del tipo de dato a ser dado de alta (Persona/Pelicula)
	pipe->agregarParametro(nombreTipo, 1);
	
	// Se lanza el proceso de la Capa Indices.
	int pipeResult = pipe->lanzar();
	
	if (pipeResult == ComuDatos::OK){
	
		unsigned short cantRegistros;
		// Envio de los valores de las claves de los registros
		// a modificar por el pipe.
		string valoresClaves;
		DefinitionsManager::ListaClaves listaClaves;
		
		armarListaClaves(estructuraCampos, listaClaves);
		
		serializarListaClaves(valoresClaves,&listaClaves);
		pipe->escribir((unsigned short)valoresClaves.length());
		pipe->escribir(valoresClaves);
		
		// Se obtiene la cantidad de registros a ser modificados.
		pipe->leer(&cantRegistros);
		
		if (cantRegistros > 0) {
			
			DefinitionsManager& defManager = DefinitionsManager::getInstance();
	
			// Lista de los tipos de todos los atributos dentro de un registro.
			DefinitionsManager::ListaTiposAtributos* listaTiposAtributos = NULL;
	
			// Lista de valores de todos los atributos dentro de un registro.
			DefinitionsManager::ListaValoresAtributos* listaValAtributos = NULL;
					
			unsigned short tamRegistro = 0;
			char* registro = NULL;
			
			// Controlador de registros de datos
			DataManager dataManager;
			
			listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
			listaValAtributos   = defManager.getListaValoresAtributos(nombreTipo, mapaValoresAtributos);
			pipeResult = ResultadosIndices::OK;
			
			for (unsigned short i = 0; (i < cantRegistros) && (pipeResult == ResultadosIndices::OK); i++){
				
				// Se obtiene el resultado de la búsqueda de la clave
				pipe->leer(&pipeResult);
				
				if (pipeResult == ResultadosIndices::OK) {
							
					// Se obtiene el tamano del registro original a modificar.
					pipe->leer(&tamRegistro);
					registro = new char[tamRegistro];
					
					pipe->leer(tamRegistro, registro);
					
					// Crea un nuevo registro con las modificaciones pertinentes. Devuelve su nueva longitud.
					tamRegistro = dataManager.crearRegistroModificacion(*listaTiposAtributos, *listaValAtributos, registro);
					
					// Se envia el tamano del registro modificado por el pipe.
					pipe->escribir(tamRegistro);
					
					// Se envia el nuevo registro con la modificacion.
					pipe->escribir(dataManager.getRegistro(), tamRegistro);
					
					// Se obtiene el resultado de la modificacion.
					pipe->leer(&pipeResult);
					
					delete[] registro;
				}
			delete listaValAtributos;
			}
			
			registro = NULL;
			
		} else pipeResult = cantRegistros; //Se produjo un error en CapaIndices
	}
	
	delete pipe;
	
	return pipeResult;
}

int alta(string nombreTipo, DefinitionsManager::MapaValoresAtributos &mapaValoresAtributos)
{
	ComuDatos *pipe = instanciarPipe();

	// Se instancia el DefinitionsManager (conocedor absoluto del universo).
	DefinitionsManager& defManager = DefinitionsManager::getInstance();
	
	DefinitionsManager::ListaNombresClaves *listaNombres = defManager.getListaNombresClavesPrimarias(nombreTipo);
	
	pipe->agregarParametro((unsigned char)OperacionesCapas::INDICES_INSERTAR, 0); 
	// Nombre del tipo de dato a ser dado de alta (Persona/Pelicula)
	pipe->agregarParametro(nombreTipo, 1);
	
	// Se lanza el proceso de la Capa Indices.
	int pipeResult = pipe->lanzar();

	if (pipeResult == ComuDatos::OK){
		// Envio de los valores de las claves de los registros
		// a modificar por el pipe.
		string valoresClaves;		
		
		serializarListaClaves(valoresClaves, &mapaValoresAtributos, listaNombres);
		
		pipe->escribir((unsigned short)valoresClaves.length());
		pipe->escribir(valoresClaves);
		
		// Se obtiene el resultado de la operacion.
		pipe->leer(&pipeResult);
		
		if (pipeResult == ResultadosIndices::CLAVE_NO_ENCONTRADA){
	
			// Lista de valores de todos los atributos dentro de un registro.
			DefinitionsManager::ListaValoresAtributos* listaValAtributos = NULL;
			
			// Lista de los tipos de todos los atributos dentro de un registro.
			DefinitionsManager::ListaTiposAtributos* listaTiposAtributos = NULL;
	
			// Controlador de registros de datos
			DataManager dataManager;
			
			listaValAtributos   = defManager.getListaValoresAtributos(nombreTipo, mapaValoresAtributos);
			listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
			
			// Se crea el registro a dar de alta y se obtiene su longitud
			unsigned short tamRegistro = dataManager.crearRegistroAlta(*listaValAtributos, *listaTiposAtributos);
			
			pipe->escribir(tamRegistro);
			
			if (tamRegistro > 0) {
				
//					vista.showRegister(dataManager.getRegistro(), listaTiposAtributos);
				
				// Se envia el registro a dar de alta por el pipe.
				pipe->escribir(dataManager.getRegistro(), tamRegistro);
				
				// Se obtiene el resultado de la insercion
				pipe->leer(&pipeResult);
				
			} else pipeResult = ResultadosMetadata::ERROR_FORMATO_FECHA;
			
			delete listaValAtributos;
			
		}
	}

	delete pipe;
	
	return pipeResult;
}


int consulta(DefinitionsManager::EstructuraConsulta &estructura) {
	
	//TODO: ver el tema de como se comunica con capaConsultas para pasar el resultado..
	
	char* registro = NULL;
	unsigned short tamRegistro   = 0; 	 // Tamaño de un registro.
	unsigned short cantRegistros = 0;    // Cant. de registros que responden a la consulta dada.
	string valoresClaves("");
	string nombreTipo("");
	int pipeResult;
	
	// Se instancia el DefinitionsManager (conocedor absoluto del universo).
	DefinitionsManager& defManager = DefinitionsManager::getInstance();
	
	// Mapas que contienen listas de restricciones para cada tipo/clase.
	MapaRestricciones mapaJoins, mapaWhere;
	MapaExtensiones mapaExt;
	
	cargarMapasRestricciones(estructura.estructuraJoins.listaOperaciones, mapaJoins);
	cargarMapasRestricciones(estructura.estructuraWhere.listaOperaciones, mapaWhere);	
	
	// Itero la lista con los nombres de los distintos tipos (tablas) que
	// intervienen en la consulta.
	DefinitionsManager::ListaStrings& listaNombresTipos = estructura.listaNombresTipos;
	
	for (DefinitionsManager::ListaStrings::iterator it = listaNombresTipos.begin();
		 it != listaNombresTipos.end(); ++it) {
	
		nombreTipo = *it;
		
		// Se verifica si la consulta se debe devolver ordenada
		if ((estructura.listaOrderBy.size() > 0) && (estructura.listaOrderBy.begin()->nombreTipo == nombreTipo))
			serializarListaClaves(valoresClaves, estructura.listaOrderBy);
		else
			valoresClaves = "*=*" + CodigosPipe::COD_FIN_CLAVE;
		
		ComuDatos* pipe = instanciarPipe();
		
		// Codigo de operacion de consulta para la Capa de Indices
		pipe->agregarParametro((unsigned char)OperacionesCapas::INDICES_CONSULTAR, 0); 
		// Nombre del tipo de dato a ser dado de alta (Persona/Pelicula/etc)
		pipe->agregarParametro(nombreTipo, 1);
		
		// Se lanza el proceso de la Capa Indices.
		pipeResult = pipe->lanzar();
		
		if (pipeResult == ComuDatos::OK){						
			// Envio de los valores de las claves a consultar por el pipe
			pipe->escribir((unsigned short)valoresClaves.length());
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
							if (cumpleRestriccion)
								guardarRegistro(nombreTipo, registro, tamRegistro);
							
							delete[] registro;
							
							cout << endl;
						}
						
						registro = NULL;
					}
					
					// Leo el resultado que indica si se van a recibir más bloques
					pipe->leer(&pipeResult);
					
				} while (pipeResult == ResultadosIndices::OK);
				
				// Se agrega la extensión del primer archivo temporal
				DefinitionsManager::ListaStrings* listaExtensiones = new DefinitionsManager::ListaStrings();
				listaExtensiones->push_back(EXTENSION_TEMPORAL_INICIAL);
				mapaExt[nombreTipo] = listaExtensiones;
				
			}
			
		}
		
		delete pipe;
		
	}
	
	// Una vez que se terminaron de aplicar los filtros/restricciones a los registros
	// de las distintas tablas, se resuelven los joins (en caso que haya).
	pipeResult = resolverJoins(mapaJoins, mapaWhere, mapaExt);
	
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
				DefinitionsManager::EstructuraConsulta estructuraConsulta;
				
				// Se recibe el contenido de estructuraConsulta, pero sin incluir la 
				// lista de campos seleccionados.
				
				// Se recibe la listaNombresTipos.
				unsigned int cantidadElementos;
				pipe->leer(&cantidadElementos);
				
				unsigned int longitudCadena;
				string cadena;
				
				for (unsigned int i = 0; i<cantidadElementos; i++) {
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, cadena);
					estructuraConsulta.listaNombresTipos.push_back(cadena);
				}
				
				// Se recibe la estructuraJoins
				pipe->leer(&estructuraConsulta.estructuraJoins.operacion);
				pipe->leer(&cantidadElementos);
				
				DefinitionsManager::NodoListaOperaciones nodoListaOperaciones;
				// Se itera para obtener cada nodo de la lista de operaciones.
				for (unsigned int i=0; i<cantidadElementos; i++) {
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nodoListaOperaciones.estructuraNombresIzq.nombreTipo);
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nodoListaOperaciones.estructuraNombresIzq.nombreCampo);
					
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nodoListaOperaciones.estructuraNombresDer.nombreTipo);
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nodoListaOperaciones.estructuraNombresDer.nombreCampo);
					
					pipe->leer(&nodoListaOperaciones.operacion);
					
					estructuraConsulta.estructuraJoins.listaOperaciones.push_back(nodoListaOperaciones);
				}
				
				// Se recibe la estructuraWhere
				pipe->leer(&estructuraConsulta.estructuraWhere.operacion);
				pipe->leer(&cantidadElementos);
				
				for (unsigned int i = 0; i < cantidadElementos; ++i) {
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nodoListaOperaciones.estructuraNombresIzq.nombreTipo);
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nodoListaOperaciones.estructuraNombresIzq.nombreCampo);
					
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nodoListaOperaciones.estructuraNombresDer.nombreTipo);
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nodoListaOperaciones.estructuraNombresDer.nombreCampo);
					
					pipe->leer(&nodoListaOperaciones.operacion);
					
					estructuraConsulta.estructuraWhere.listaOperaciones.push_back(nodoListaOperaciones);
				}
				
				// Se recibe el orderBy
				pipe->leer(&cantidadElementos);
				
				DefinitionsManager::EstructuraNombres estructuraNombres;
				
				for (unsigned int i = 0; i < cantidadElementos; ++i) {
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, estructuraNombres.nombreTipo);
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, estructuraNombres.nombreCampo);
					
					estructuraConsulta.listaOrderBy.push_back(estructuraNombres);
				}
				
				// CONSULTAS LLAMANDO A CAPA INDICES
				pipeResult = consulta(estructuraConsulta);
				
				// TODO Ver si hay que enviar un resultado a CapaConsultas
				pipe->escribir(pipeResult);
				
			}break;
			
			case OperacionesCapas::METADATA_BAJA:
			{
				unsigned int cantidadElementos;
				unsigned int longitudCadena;
				
				// Obtiene el nombre del tipo sobre el cual se hace la baja.
				pipe->parametro(1,nombreTipo);
				
				DefinitionsManager::EstructuraCampos estructuraCampos;
				
				pipe->leer(&estructuraCampos.operacion);
	
				pipe->leer(&cantidadElementos);
				
				DefinitionsManager::NodoListaCampos nodoListaCampos;
				
				// Se itera para recibir cada nodo de la lista de campos.
				for (unsigned int i = 0; i < cantidadElementos; ++i) {
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nodoListaCampos.nombreCampo);
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nodoListaCampos.valorCampo);
					pipe->leer(&nodoListaCampos.operacion);
					
					estructuraCampos.listaCampos.push_back(nodoListaCampos);
				}
				
				pipeResult = baja(nombreTipo, estructuraCampos);
				
				pipe->escribir(pipeResult);
				
			}break;
			
			case OperacionesCapas::METADATA_MODIFICACION:
			{		
				unsigned int cantidadElementos;
				unsigned int longitudCadena;
				string nombreAtributo, valorAtributo;
				pipe->parametro(1, nombreTipo);
				
				// Se recibe el mapaValoresAtributos.
				DefinitionsManager::MapaValoresAtributos mapaValoresAtributos;
				
				pipe->leer(&cantidadElementos);
				
				for (unsigned int i = 0; i < cantidadElementos; ++i) {
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nombreAtributo);
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, valorAtributo);
					
					mapaValoresAtributos[nombreAtributo] = valorAtributo;
				}
				
				// Se recibe la información sobre los registros a modificar,
				// dentro de una estructuraCampos.
				DefinitionsManager::EstructuraCampos estructuraCampos;
				
				pipe->leer(&estructuraCampos.operacion);
				
				// Se obtiene la cantidad de elementos en la lista de campos.
				pipe->leer(&cantidadElementos);
				
				DefinitionsManager::NodoListaCampos nodoListaCampos;
				
				for (unsigned int i = 0; i < cantidadElementos; ++i) {
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nodoListaCampos.nombreCampo);
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nodoListaCampos.valorCampo);
					pipe->leer(&nodoListaCampos.operacion);
					
					estructuraCampos.listaCampos.push_back(nodoListaCampos);
				}

				pipeResult = modificacion(nombreTipo, mapaValoresAtributos, estructuraCampos);
				
				pipe->escribir(pipeResult);
				
			}break;
			
			case OperacionesCapas::METADATA_ALTA:
			{	
				unsigned int cantidadElementos;
				unsigned int longitudCadena;
				string nombreAtributo, valorAtributo;
				pipe->parametro(1,nombreTipo);

				// Se recibe el mapaValoresAtributos.
				DefinitionsManager::MapaValoresAtributos mapaValoresAtributos;
				
				pipe->leer(&cantidadElementos);
				
				for (unsigned int i = 0; i < cantidadElementos; ++i) {
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nombreAtributo);
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, valorAtributo);
					
					mapaValoresAtributos[nombreAtributo] = valorAtributo;
				}

				pipeResult = alta (nombreTipo, mapaValoresAtributos);
				
				pipe->escribir(pipeResult);
									
			}break;
			
			default:
				pipeResult = ResultadosMetadata::OPERACION_INVALIDA;
		}
			
		resultado << pipeResult;
		
		cout << endl << resultado << endl;
		
		usleep(100); //Recibe microsegundos
		
		if (pipe)
			delete pipe;
	}
	
	cout << "Fin Capa Metadata" << endl;
	
	return pipeResult;
	
}

