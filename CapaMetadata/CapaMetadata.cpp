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

// MÉTODOS PARA RESOLVER LAS CONSULTAS CON JOINS

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

	return pipeResult;
}

bool compararRegistros(char* registro1, char* registro2, unsigned short longitudRegistros)
{
	bool iguales = true;
	
	for (unsigned int i=0; i<longitudRegistros; ++i ) {
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

string obtenerCampo(unsigned int numeroCampo, char* registro, DefinitionsManager::ListaTiposAtributos &listaTiposAtributos) {

	unsigned short offset = 0;
	string campo;
	
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
		
		// itLTA tiene ahora el tipo del campo pedido. Se pone dicho campo en un string.
		switch (itLTA->tipo) {
			case TipoDatos::TIPO_BOOL:
				campo.assign(registro + offset, sizeof(bool));
				break;
			case TipoDatos::TIPO_CHAR:
				campo.assign(registro + offset, sizeof(char));
				break;
			case TipoDatos::TIPO_ENTERO:
				campo.assign(registro + offset, sizeof(int));
				break;
			case TipoDatos::TIPO_FECHA:
				//TODO: revisar esto q no se si funciona asi con las fechas!
				campo.assign(registro + offset, Tamanios::TAMANIO_FECHA);
				break;
			case TipoDatos::TIPO_FLOAT:
				campo.assign(registro + offset, sizeof(float));
				break;
			case TipoDatos::TIPO_SHORT:
				campo.assign(registro + offset, sizeof(short));
				break;
			case TipoDatos::TIPO_STRING:
				unsigned char tamanioCadena;
				memcpy(&tamanioCadena, registro + offset, Tamanios::TAMANIO_LONGITUD_CADENA);
				offset += Tamanios::TAMANIO_LONGITUD_CADENA;
				campo.assign(registro + offset, sizeof(tamanioCadena));
				break;
		}
		
		return campo;
	
	}else
		return "";
}

bool cumpleRestriccion(char *registro, DefinitionsManager::ListaNombresAtributos &listaNombresAtributos, 
				  	   DefinitionsManager::ListaTiposAtributos listaTiposAtributos, 
				  	   DefinitionsManager::NodoListaOperaciones &nodoListaOperaciones)
{
	DefinitionsManager::ListaNombresAtributos::iterator itNombres = listaNombresAtributos.begin();
	unsigned int numeroCampo = 1;
 
	while ((nodoListaOperaciones.estructuraNombresIzq.nombreCampo != *itNombres) && (itNombres != listaNombresAtributos.end())) {
		++numeroCampo;
		++itNombres;
	}
	
	string campo = obtenerCampo(numeroCampo, registro, listaTiposAtributos);
	
	DefinitionsManager::ListaTiposAtributos::iterator itTipos = listaTiposAtributos.begin();

	// itera la lista de tipos para obtener el tipo del campo "campo".
	for (unsigned int i = 0; i < numeroCampo; ++i)
		++itTipos;
	
	stringstream conversorCampo, conversorCampoAComparar;
	Clave* clave;
	Clave* claveAComparar;
	
	conversorCampo << campo;
	// En "nodoListaOperaciones.estructuraNombresDer.nombreCampo" se encuentra el valor constante 
	// con el cual hay que hacer la comparación.
	conversorCampoAComparar << nodoListaOperaciones.estructuraNombresDer.nombreCampo;
	
	switch (itTipos->tipo) {
		case TipoDatos::TIPO_BOOL: {	
			bool valorCampo;
			conversorCampo >> valorCampo;
			clave = new ClaveBoolean(valorCampo);
			
			bool valorAComparar;
			conversorCampoAComparar >> valorAComparar;
			claveAComparar = new ClaveBoolean(valorAComparar);
			break;
		}
		case TipoDatos::TIPO_CHAR: {	
			char valorCampo;
			conversorCampo >> valorCampo;
			clave = new ClaveChar(valorCampo);
			
			char valorAComparar;
			conversorCampoAComparar >> valorAComparar;
			claveAComparar = new ClaveChar(valorAComparar);
			break;
		}
		case TipoDatos::TIPO_SHORT: {
			short valorCampo;
			conversorCampo >> valorCampo;
			clave = new ClaveShort(valorCampo);
			
			short valorAComparar;
			conversorCampoAComparar >> valorAComparar;
			claveAComparar = new ClaveShort(valorAComparar);
			break;
		}
		case TipoDatos::TIPO_ENTERO: {
			int valorCampo;
			conversorCampo >> valorCampo;
			clave = new ClaveEntera(valorCampo);
			
			int valorAComparar;
			conversorCampoAComparar >> valorAComparar;
			claveAComparar = new ClaveEntera(valorAComparar);
			break;
		}
		case TipoDatos::TIPO_FLOAT: {
			float valorCampo;
			conversorCampo >> valorCampo;
			clave = new ClaveReal(valorCampo);
			
			float valorAComparar;
			conversorCampoAComparar >> valorAComparar;
			claveAComparar = new ClaveReal(valorAComparar);
			break;
		}
		case TipoDatos::TIPO_FECHA: {
			ClaveFecha::TFECHA valorCampo;
			unsigned short anio, dia, mes;
			
			conversorCampo.clear();
			conversorCampo.str("");
			conversorCampo << campo.substr(0, 4);
			conversorCampo >> anio;
			conversorCampo.clear();
			conversorCampo.str("");
			conversorCampo << campo.substr(4, 2);
			conversorCampo >> mes;
			conversorCampo.clear();
			conversorCampo.str("");
			conversorCampo << campo.substr(6, 2);
			conversorCampo >> dia;
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
			clave = new ClaveVariable(campo);
			
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
			
			++itLO;
			// Saca el primer elemento de la lista, que es el que acaba de revisar.
			listaOperaciones.pop_front();
		}
	
	}
	return cumple;
}

int main(int argc, char* argv[]) 
{
	Resultado resultado;
	int pipeResult = ResultadosIndices::OK;
	
	if (argc > 1){
		
		ComuDatos* pipe = new ComuDatos(argv); //Instancia del pipe de comunicacion		
		
		unsigned char operacion      = 0;    // Tipo de operacion a ejecutar
		unsigned short tamRegistro   = 0; 	 // Tamano de un registro.
		unsigned short cantRegistros = 0;    // Cant. de registros que responden a la consulta dada.
		char* registro               = NULL; // Buffer contenedor de un registro. 
		string valoresClaves("");
		string nombreTipo("");
		
		
		// Lista de los tipos de todos los atributos dentro de un registro.
		DefinitionsManager::ListaTiposAtributos* listaTiposAtributos = NULL;
		
		// Lista de los valores de las claves y sus nombres dentro de un registro.
		DefinitionsManager::ListaClaves* listaClaves = NULL; 
		
		// Se instancia el DefinitionsManager (conocedor absoluto del universo).
		DefinitionsManager& defManager = DefinitionsManager::getInstance();
		
		// Controlador de registros de datos
		DataManager dataManager;
		
		//pipe = instanciarPipe();
		pipe->parametro(0,operacion);
	
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
				
				
				
//---------------------ACA HAY Q HACER LAS CONSULTAS LLAMANDO A CAPA INDICES.----
				//TODO: ver el tema de como se comunica con capaConsultas para pasar el resultado..
				// Codigo de operacion de consulta para la Capa de Indices
				pipe->agregarParametro((unsigned char)OperacionesCapas::INDICES_CONSULTAR, 0); 
				// Nombre del tipo de dato a ser dado de alta (Persona/Pelicula)
				pipe->agregarParametro(nombreTipo, 1);
				
				// Se lanza el proceso de la Capa Indices.
				pipeResult = pipe->lanzar();
				
				if (pipeResult == ComuDatos::OK){						
					// Envio de los valores de las claves a consultar por el pipe
					serializarListaClaves(valoresClaves, listaClaves);
					pipe->escribir((unsigned short)valoresClaves.length());
					pipe->escribir(valoresClaves);

					// Obtencion del resultado de la operacion
					pipe->leer(&pipeResult);
					
					if (pipeResult == ResultadosIndices::OK) {
						
						do {
							// Se obtiene la cantidad de registros que responden 
							// a la consulta realizada.
							pipe->leer(&cantRegistros); 
							
							if (cantRegistros > 0) {
								
								listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
								
								cout << "======= Resultado de la consulta =======" << endl << endl;
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
									
				//					vista.showRegister(registro, listaTiposAtributos);
									
									delete[] registro;
									
									cout << endl;
								}
								cout << "========================================" << endl;
								
								registro = NULL;
							}
							
							// Leo el resultado que indica si se van a recibir más bloques
							pipe->leer(&pipeResult);
							
						} while (pipeResult == ResultadosIndices::OK);
					}
				}
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
				
				int resultadoBaja = baja(nombreTipo, estructuraCampos);
				
				pipe->escribir(resultadoBaja);
				
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

				int resultadoModificacion = modificacion(nombreTipo, mapaValoresAtributos, estructuraCampos);
				
				pipe->escribir(resultadoModificacion);
				
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

				int resultadoAlta = alta (nombreTipo, mapaValoresAtributos);
				
				pipe->escribir(resultadoAlta);
									
			}break;
			
			default:
				pipeResult = ResultadosMetadata::OPERACION_INVALIDA;
		}
			
		resultado << pipeResult;
		
		cout << endl << resultado << endl;
		
		delete pipe;
		pipe = NULL;
		
		usleep(100); //Recibe microsegundos
		

		
		if (registro)
			delete[] registro;
		
		if (pipe)
			delete pipe;
	}
	
	cout << "Fin Capa Metadata" << endl;
	
	return pipeResult;
	
}

