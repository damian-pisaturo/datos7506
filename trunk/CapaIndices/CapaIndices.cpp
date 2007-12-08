#include "CapaIndices.h"

/*
 * Crea todos los indices correspondientes a un tipo, y carga su mapa de indices.
 **/
void crearIndices(const string &nombreTipo, MapaIndices &mapaIndices,
				  DefinitionsManager &defManager) {
	
	Indice* indice = NULL;
	unsigned char tipoIndice = 0;
	InfoIndice estructura;
	NodoListaIndices nodoListaIndices;
	ListaTiposIndices *listaTiposIndices = defManager.getListaTiposIndices(nombreTipo);
	ListaTiposAtributos *listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
	char tipoOrg = defManager.getTipoOrgRegistro(nombreTipo);
	
	unsigned short tamBloqueDatos = defManager.getTamBloqueDatos(nombreTipo);
	unsigned short tamBloqueLista = defManager.getTamBloqueLista(nombreTipo);	
	
	for (ListaTiposIndices::const_iterator iter = listaTiposIndices->begin();
		iter != listaTiposIndices->end(); ++iter) {
		
		nodoListaIndices = *iter;
		estructura = nodoListaIndices.infoTipoIndice;
		tipoIndice = estructura.tipoEstructura;
	
		switch (tipoIndice) {
			
			case TipoIndices::ARBOL_BP:
			case TipoIndices::ARBOL_BS:
				indice = new IndiceArbol(estructura.tipoIndice, estructura.tipoClave,
										nodoListaIndices.listaTipos, listaTiposAtributos,
										estructura.tipoEstructura, tamBloqueDatos,
										estructura.tamanioBloque, tamBloqueLista,
										estructura.nombreArchivo, tipoOrg);				
				break;
				
			case TipoIndices::HASH:
				indice = new IndiceHash(estructura.tipoIndice, nodoListaIndices.listaTipos,
										listaTiposAtributos, tamBloqueLista,
										estructura.tamanioBloque, estructura.nombreArchivo);
				break;
		}
		
		mapaIndices[*(nodoListaIndices.listaNombresClaves)] = indice;
		
	}
	
}


Indice* obtenerIndiceParaOrderBy(const MapaIndices &mapaIndices, const ListaNombresClaves &listaNombresClaves) {
	Indice* indice = NULL;
	bool encontrado = false;
	ListaNombresClaves::const_iterator itLNC, itLNCMapa;
	
	for (MapaIndices::const_iterator it = mapaIndices.begin();
		 (it != mapaIndices.end()) && (!encontrado); ++it) {
		
		if (it->first.size() >= listaNombresClaves.size()) {
			
			for (itLNC = listaNombresClaves.begin(), itLNCMapa = it->first.begin();
				 (itLNC != listaNombresClaves.end()) && (*itLNCMapa == *itLNC);
				 ++itLNC, ++itLNCMapa);
			
			if ( (itLNC != listaNombresClaves.end()) && (it->second->getTipoEstructura() != TipoIndices::HASH) ) {
				indice = it->second;
				encontrado = true;
			}
		}
		
	}
	
	return indice;
}


void destruirIndices(MapaIndices &mapaIndices) {

	for (MapaIndices::iterator iter = mapaIndices.begin();
		iter != mapaIndices.end(); ++iter)
		delete iter->second;
	
}


void destruirListaClaves(ListaClaves* &listaClaves) {

	for (ListaClaves::iterator iter = listaClaves->begin();
		iter != listaClaves->end(); ++iter)
		delete *iter;
	
	delete listaClaves;
	
	listaClaves = NULL;
	
}

bool compararClaves(Clave *claveMenor, Clave *claveMayor, Clave *clave)
{	
	if ((claveMenor == NULL) && (claveMayor == NULL))
		return true;
	if (claveMenor == NULL)
		return (*clave < *claveMayor);
	
	if (claveMayor == NULL)
		return (*claveMenor < *clave);
	
	return ((*claveMenor < *clave) && (*clave < *claveMayor));
}

void consultaNoIndexadaPorRango(const string &nombreTipo, MapaIndices &mapaIndices,
								Clave *claveMenor, Clave * claveMayor, DefinitionsManager &defManager, ComuDatos &pipe) {
	
	// Obtiene el indice primario para poder obtener luego los bloques de datos.
	Indice* indice = mapaIndices[*defManager.getListaNombresClavesPrimarias(nombreTipo)];
	
	// Se obtiene la lista de los tipos de datos de los atributos del tipo 'nombreTipo', con
	// los campos pk en "true" para los atributos cuyos nombres figuran en la listaNombresClaves.
	ListaTiposAtributos *listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
	
	Bloque* bloque = NULL;
	char* registro = NULL;
	unsigned short tamanioRegistro = 0;
	unsigned short cantidadRegistros = 0;
	Clave* clave = NULL;
	Clave* claveDelRegistro = NULL;
	// Obtiene un bloque de datos.
	int resultado = indice->siguienteBloque(bloque);
	char operacion = 0;
	
	// Se indica a la capa superior si hay un bloque siguiente.
	pipe.escribir(resultado);	
	
	 while (resultado == ResultadosIndices::OK) {
		
	
		// Se obtiene un registro.
		registro = bloque->getNextRegister();
	
		while (registro) {
			
			// Obtiene la clave del registro y la compara dentro del rango.
			claveDelRegistro = bloque->getClavePrimaria(listaTiposAtributos, registro);
			
			if (compararClaves(claveMenor,claveMayor,claveDelRegistro)) {
				
				// Si el registro se encuentra en el rango pedido, lo envío por el pipe.
				cantidadRegistros = 1;
				pipe.escribir(cantidadRegistros);
				
				// Obtiene el tamaño del registro a enviar.
				tamanioRegistro = bloque->getTamanioRegistroConPrefijo(listaTiposAtributos, registro);
				pipe.escribir(tamanioRegistro);
				pipe.escribir(registro, tamanioRegistro);
				
				pipe.leer(&operacion);
				
				if (operacion == OperacionesCapas::INDICES_ELIMINAR) {
					clave = bloque->getClavePrimaria(listaTiposAtributos,registro);
					eliminar(nombreTipo, mapaIndices, indice, clave, defManager, pipe);	
					delete clave;				
				}
				
				// Vuelve a mandar el resultado, para que se esperen mas registros.
				pipe.escribir(resultado);
			}
			else {
				cantidadRegistros = 0;
				pipe.escribir(cantidadRegistros);
				pipe.escribir(resultado);
			}
			
			delete claveDelRegistro;
			delete[] registro;
			registro = bloque->getNextRegister();	
		}	
		cantidadRegistros = 0;
		pipe.escribir(cantidadRegistros);

		delete bloque;
		bloque = NULL;
		resultado = indice->siguienteBloque(bloque);
		
		// Se indica a la capa superior si hay un bloque siguiente.
		pipe.escribir(resultado);
	
	}
}

void eliminacionNoIndexadaPorRango(const string &nombreTipo, MapaIndices &mapaIndices,
									DefinitionsManager &defManager, ComuDatos &pipe) {
	
	// Obtiene el indice primario para poder obtener luego los bloques de datos.
	Indice* indice = mapaIndices[*defManager.getListaNombresClavesPrimarias(nombreTipo)];
	
	ListaTiposAtributos *listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
	
	Clave* clave = NULL;
	Bloque* bloque = NULL;
	char* registro = NULL;
	unsigned short tamanioRegistro = 0;
	char operacion = 0;
	// Obtiene un bloque de datos.
	int resultado = indice->siguienteBloque(bloque);
	
	 while (resultado == ResultadosIndices::OK) {
		cout << "CI: entre en el while de q hay bloques" << endl;
		// Se obtiene un registro.
		registro = bloque->getNextRegister();
	
		while (registro) {
			cout << "CI: entre el el while de q hay registros" << endl;
			// Indica a metadata que tiene un registro para enviarle.
			pipe.escribir(resultado);
				
			cout << "CI: escribi que hay mas registros:" << resultado << endl;
			// Obtiene el tamaño del registro a enviar.
			tamanioRegistro = bloque->getTamanioRegistroConPrefijo(listaTiposAtributos, registro);
			
			cout << "CI: mando el tamaño: "<< tamanioRegistro << endl;
			pipe.escribir(tamanioRegistro);
			
			cout << "CI: mando el registro."<< endl;
			pipe.escribir(registro, tamanioRegistro);
			cout << "CI: ya mande el registro."<< endl;
			
			cout << "CI: voy a leer la operacion."<< endl;
			pipe.leer(&operacion);
			
			if (operacion == OperacionesCapas::INDICES_ELIMINAR) {		
				cout << "CI: lei la operacion, tengo q eliminar."<< endl;
				clave = bloque->getClavePrimaria(listaTiposAtributos,registro);
				cout << "CI: busque la clave primaria." << endl;
				eliminar(nombreTipo, mapaIndices, indice, clave, defManager, pipe);
				delete clave;
			}else
				cout << "CI: lei la operacion, es no op: "<< (int)operacion << endl;
			
			delete[] registro;
			registro = bloque->getNextRegister();	
		}	

		delete bloque;
		bloque = NULL;
		resultado = indice->siguienteBloque(bloque);
	}
	 // Indica que no hay mas registros.
	 pipe.escribir(resultado);
	 
}


void consultaNoIndexada(const string &nombreTipo, MapaIndices &mapaIndices,
						Clave *clave, DefinitionsManager &defManager, ComuDatos &pipe) {
	
	// Obtiene el indice primario para poder obtener luego los bloques de datos.
	Indice* indice = mapaIndices[*defManager.getListaNombresClavesPrimarias(nombreTipo)];
	
	Bloque* bloque = NULL;
	char *registro = NULL;
	unsigned short offsetToReg = 0;
	unsigned short longReg = 0;
	bool seguirBuscando = false;
	unsigned short cantidadRegistros = 0;
	
	// Se obtiene la lista de los tipos de datos de los atributos del tipo 'nombreTipo', con
	// los campos pk en "true" para los atributos cuyos nombres figuran en la listaNombresClaves.
	ListaTiposAtributos *listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo, listaNombresClaves);
	
	int resultado = indice->siguienteBloque(bloque);
	pipe.escribir(resultado);

	while (resultado == ResultadosIndices::OK) {
		seguirBuscando = true;
		
		// Sigue buscando mientras encuentra registros, ya que si encuentra uno es probable que mas adelante
		// haya otro. Si ya no encuentra un registro dentro del bloque se cesa la busqueda dentro del mismo.
		while(seguirBuscando){

			if(bloque->buscarRegistro(listaTiposAtributos, *clave, &offsetToReg)){
				
				cantidadRegistros = 1;
				pipe.escribir(cantidadRegistros);
				
				
				// Obtengo la longitud del registro corriente
				longReg = bloque->getTamanioRegistroConPrefijo(listaTiposAtributos, bloque->getDatos() + offsetToReg);				

				// Envío su longitud
				pipe.escribir(longReg);
				
				// Actualizo el offset a datos
				bloque->setOffsetADatos(offsetToReg + longReg);
				
				// Obtengo el registro
				registro = bloque->getRegistro(longReg, offsetToReg);
				
				// Envío el registro
				pipe.escribir(registro, longReg);
				
				delete[] registro;
				
				pipe.escribir(resultado);
			}
			// Si no encontro mas registros ceso la busqueda dentro del bloque
			else {
				seguirBuscando = false;
				cantidadRegistros = 0;
				pipe.escribir(cantidadRegistros);
			}	
				
		}
		
		delete bloque;
		
		resultado = indice->siguienteBloque(bloque);
		pipe.escribir(resultado);
	}
	
	delete listaTiposAtributos;
	
}

void enviarClavesMayoresOIguales(const string &nombreTipo, MapaIndices &mapaIndices,
		 						 Indice *indice, DefinitionsManager &defManager,
		 						 ComuDatos &pipe) {
	
	Clave* claveSiguiente = NULL;
	char* registro = NULL;
	unsigned short tamRegistro = 0, cantRegistros = 1;
	int resultado = ResultadosIndices::OK;
	
	claveSiguiente = indice->siguiente();
	if (!claveSiguiente) resultado = ResultadosIndices::FIN_BLOQUES;
	// Se le indica a la capa de metadata si va a recibir un bloque de datos
	pipe.escribir(resultado);
	
	while (claveSiguiente) {
		
		if (indice->getTipoIndice() == TipoIndices::GRIEGO) { // Índice Primario
			
			resultado = indice->buscar(claveSiguiente, registro, tamRegistro);
			delete claveSiguiente;
			
			if (resultado == ResultadosIndices::OK) {
				
				cantRegistros = 1;
				pipe.escribir(cantRegistros);
				
				// Envío su longitud del registro
				pipe.escribir(tamRegistro);
				
				// Envío el registro
				pipe.escribir(registro, tamRegistro);
				
				delete[] registro;
				registro = NULL;
				
				claveSiguiente = indice->siguiente();
				
				if (!claveSiguiente) resultado = ResultadosIndices::FIN_BLOQUES;
				
			} else {
				claveSiguiente = NULL;
				cantRegistros = 0;
				pipe.escribir(cantRegistros);
			}
			
		} else { // Índice Secundario
			
			SetClaves* setClaves = NULL;
			resultado = indice->buscar(claveSiguiente, setClaves);
			delete claveSiguiente;
			
			if (resultado == ResultadosIndices::OK) {
				
				//Obtengo el índice primario
				ListaNombresClaves* listaNombresClaves = defManager.getListaNombresClavesPrimarias(nombreTipo);
				indice = mapaIndices[*listaNombresClaves];
				
				//Envío la cantidad de registros (Un registro por cada clave primaria)
				cantRegistros = setClaves->size();
				pipe.escribir(cantRegistros);
				
				//Consulto el registro de datos de cada clave primaria
				for (SetClaves::iterator it = setClaves->begin(); it != setClaves->end(); ++it) {
					
					resultado = indice->buscar(*it, registro, tamRegistro);
					
					//Si el tamaño del registro es 0, la capa de metadata sabe que se produjo un error. 
					pipe.escribir(tamRegistro);
					
					if (resultado == ResultadosIndices::OK)
						pipe.escribir(registro, tamRegistro);
					else break;
				}
				
				delete setClaves;
				
				claveSiguiente = indice->siguiente();
								
				if (!claveSiguiente) resultado = ResultadosIndices::FIN_BLOQUES;
				
			} else {
				claveSiguiente = NULL;
				cantRegistros = 0;
				pipe.escribir(cantRegistros);
			}
			
		}
		
		pipe.escribir(resultado);
		
	}
	
}


void enviarClavesMenoresOIguales(const string &nombreTipo, MapaIndices &mapaIndices,
								 Indice *indice, Clave *clave, char operador,
								 DefinitionsManager &defManager, ComuDatos &pipe) {
	
	Clave* claveSiguiente = NULL;
	char* registro = NULL;
	unsigned short tamRegistro = 0, cantRegistros = 1;
	int resultado = ResultadosIndices::OK;
	bool continuar = false;
	
	indice->primero();
	claveSiguiente = indice->siguiente();
	
	if (!claveSiguiente)
		resultado = ResultadosIndices::FIN_BLOQUES;
	else {
		if (operador == ExpresionesLogicas::MENOR)
			continuar = (*claveSiguiente < *clave);
		else
			continuar = ((*claveSiguiente < *clave) || (*claveSiguiente == *clave));
		
		if (!continuar) resultado = ResultadosIndices::FIN_BLOQUES;
	}
	
	// Se le indica a la capa de metadata si va a recibir un bloque de datos
	pipe.escribir(resultado);
	
	while (claveSiguiente && continuar) {
		
		if (indice->getTipoIndice() == TipoIndices::GRIEGO) { // Índice Primario
			
			resultado = indice->buscar(claveSiguiente, registro, tamRegistro);
			delete claveSiguiente;
			
			if (resultado == ResultadosIndices::OK) {
				
				cantRegistros = 1;
				pipe.escribir(cantRegistros);
				
				// Envío su longitud del registro
				pipe.escribir(tamRegistro);
				
				// Envío el registro
				pipe.escribir(registro, tamRegistro);
				
				delete[] registro;
				registro = NULL;
				
				claveSiguiente = indice->siguiente();
				
				if (!claveSiguiente)
					resultado = ResultadosIndices::FIN_BLOQUES;
				else {
					if (operador == ExpresionesLogicas::MENOR)
						continuar = (*claveSiguiente < *clave);
					else
						continuar = ((*claveSiguiente < *clave) || (*claveSiguiente == *clave));
					
					if (!continuar) resultado = ResultadosIndices::FIN_BLOQUES;
				}
				
			} else {
				claveSiguiente = NULL;
				cantRegistros = 0;
				pipe.escribir(cantRegistros);
			}
			
		} else { // Índice Secundario
			
			SetClaves* setClaves = NULL;
			resultado = indice->buscar(claveSiguiente, setClaves);
			delete claveSiguiente;
			
			if (resultado == ResultadosIndices::OK) {
				
				//Obtengo el índice primario
				ListaNombresClaves* listaNombresClaves = defManager.getListaNombresClavesPrimarias(nombreTipo);
				indice = mapaIndices[*listaNombresClaves];
				
				//Envío la cantidad de registros (Un registro por cada clave primaria)
				cantRegistros = setClaves->size();
				pipe.escribir(cantRegistros);
				
				//Consulto el registro de datos de cada clave primaria
				for (SetClaves::iterator it = setClaves->begin(); it != setClaves->end(); ++it) {
					
					resultado = indice->buscar(*it, registro, tamRegistro);
					
					//Si el tamaño del registro es 0, la capa de metadata sabe que se produjo un error. 
					pipe.escribir(tamRegistro);
					
					if (resultado == ResultadosIndices::OK)
						pipe.escribir(registro, tamRegistro);
					else break;
				}
				
				delete setClaves;
				
				claveSiguiente = indice->siguiente();
								
				if (!claveSiguiente)
					resultado = ResultadosIndices::FIN_BLOQUES;
				else {
					if (operador == ExpresionesLogicas::MENOR)
						continuar = (*claveSiguiente < *clave);
					else
						continuar = ((*claveSiguiente < *clave) || (*claveSiguiente == *clave));
					
					if (!continuar) resultado = ResultadosIndices::FIN_BLOQUES;
				}
				
			} else {
				claveSiguiente = NULL;
				cantRegistros = 0;
				pipe.escribir(cantRegistros);
			}
			
		}
		
		pipe.escribir(resultado);
		
	}
	
}


void enviarTodasLasClavesPrimarias(const string &nombreTipo, MapaIndices &mapaIndices,
								   Indice *indice, DefinitionsManager &defManager,
								   ComuDatos &pipe) {
	
	// Se devuelven todos los registros almacenados en el indice.
	indice->primero();
	Clave* primeraClave = indice->siguiente();
	
	if (primeraClave) {
		indice->mayorOIgual(primeraClave);
		enviarClavesMayoresOIguales(nombreTipo, mapaIndices, indice, defManager, pipe);
		delete primeraClave;
	} else {
		// Le indico a la capa superior que no va a recibir mas bloques
		int resultado = ResultadosIndices::FIN_BLOQUES;
		pipe.escribir(resultado);
	}
	
}

void consultaIndexada(const string &nombreTipo, MapaIndices &mapaIndices,
			   		  Indice *indice, Clave *clave,
			   		  DefinitionsManager &defManager, ComuDatos &pipe) {
	
	unsigned short cantRegistros = 1, tamRegistro = 0;
	int resultado = 0;
	char *registroDatos = NULL;
	
	if (indice->getTipoIndice() == TipoIndices::GRIEGO) {
		
		if (clave)
		{
			resultado = indice->buscar(clave, registroDatos, tamRegistro);
			pipe.escribir(resultado);
			
			if (resultado == ResultadosIndices::OK) {
				// Envío la cantidad de registros
				pipe.escribir(cantRegistros);
				pipe.escribir(tamRegistro);
				pipe.escribir(registroDatos, tamRegistro);
				// Le indico a la capa superior que no va a recibir mas bloques
				resultado = ResultadosIndices::FIN_BLOQUES;
				pipe.escribir(resultado);
			}
			
		}
		else enviarTodasLasClavesPrimarias(nombreTipo, mapaIndices, indice, defManager, pipe);
	}
	else { //Indice Secundario
		
		if (clave) {
			
			SetClaves* setClaves = NULL;
			resultado = indice->buscar(clave, setClaves);
			pipe.escribir(resultado);
			
			if (resultado == ResultadosIndices::OK) {
				
				//Obtengo el índice primario
				ListaNombresClaves* listaNombresClaves = defManager.getListaNombresClavesPrimarias(nombreTipo);
				indice = mapaIndices[*listaNombresClaves];
				
				//Envío la cantidad de registros (Un registro por cada clave primaria)
				cantRegistros = setClaves->size();
				pipe.escribir(cantRegistros);
				
				//Consulto el registro de datos de cada clave primaria
				for (SetClaves::iterator it = setClaves->begin(); it != setClaves->end(); ++it) {
					
					resultado = indice->buscar(*it, registroDatos, tamRegistro);
					
					//Si el tamaño del registro es 0, la capa de metadata sabe que se produjo un error. 
					pipe.escribir(tamRegistro);
					
					if (resultado == ResultadosIndices::OK)
						pipe.escribir(registroDatos, tamRegistro);
					else break;
				}
				
				// Le indico a la capa superior que no va a recibir mas bloques
				resultado = ResultadosIndices::FIN_BLOQUES;
				pipe.escribir(resultado);
				
				delete setClaves;
			}
			
		} else enviarTodasLasClavesPrimarias(nombreTipo, mapaIndices, indice, defManager, pipe);
		
	}
	
}


void consultaIndexadaPorRango(const string &nombreTipo, MapaIndices &mapaIndices,
			   		  		  Indice *indice, Clave *clave, char operador,
			   		  		  DefinitionsManager &defManager, ComuDatos &pipe) {
	
	if (operador == ExpresionesLogicas::MAYOR) {
		
		indice->mayor(clave);
		enviarClavesMayoresOIguales(nombreTipo, mapaIndices, indice, defManager, pipe);
		
	} else if (operador == ExpresionesLogicas::MAYOR_O_IGUAL) {
		
		indice->mayorOIgual(clave);
		enviarClavesMayoresOIguales(nombreTipo, mapaIndices, indice, defManager, pipe);
		
	} else enviarClavesMenoresOIguales(nombreTipo, mapaIndices, indice, clave, operador, defManager, pipe);
	
}


void insertar(const string &nombreTipo, MapaIndices &mapaIndices,
		   	  Indice *indice, Clave *clave,
		   	  DefinitionsManager &defManager, ComuDatos &pipe) {
	
	unsigned short tamRegistro = 0;
	char * registroDatos       = NULL;
	Clave* claveSecundaria 	   = NULL;

	int resultado = indice->buscar(clave);
	pipe.escribir(resultado);
	
	if (resultado == ResultadosIndices::CLAVE_NO_ENCONTRADA) {
		// Recibe el tamaño del registro a insertar.
		pipe.leer(&tamRegistro);
		
		registroDatos = new char[tamRegistro];
		memset(registroDatos, 0, tamRegistro);
		
		// Recibe el registro de datos.
		pipe.leer(tamRegistro, registroDatos);
		
		resultado = indice->insertar(clave, registroDatos, tamRegistro);
		
		if (resultado == ResultadosIndices::OK) {
			
			//Actualizo los indices secundarios
			
			//Saco el índice primario para no volver a insertar.
			MapaIndices::iterator it = mapaIndices.find(*defManager.getListaNombresClavesPrimarias(nombreTipo));
			delete it->second;
			mapaIndices.erase(it);			
			
			Registro registro(defManager.getTipoOrgRegistro(nombreTipo),
							  registroDatos, defManager.getListaTipos(nombreTipo),
							  defManager.getListaNombresAtributos(nombreTipo));
			
			for (MapaIndices::iterator iter = mapaIndices.begin();
				(iter != mapaIndices.end()) && (resultado == ResultadosIndices::OK); ++iter) {
				
				claveSecundaria = registro.getClave(iter->first);
				indice = iter->second;
				
				resultado = indice->insertar(claveSecundaria, clave);				
				
				delete claveSecundaria;
			}
		}		
	}
	
	if (registroDatos)
		delete[] registroDatos;

	pipe.escribir(resultado);
}


int eliminarClavePrimaria(const string &nombreTipo, MapaIndices &mapaIndices,
 	  	  				   Indice *indicePrimario, Clave *clave,
 	  	  				   DefinitionsManager &defManager) {
	
	int resultado = 0;
	char* registroDatos = NULL;
	unsigned short tamRegistro;
	
	//Antes de eliminar la clave primaria obtengo su registro de datos
	//para luego armar las claves secundarias.
	cout << "CI: Busco la clave primaria." << endl;
	resultado = indicePrimario->buscar(clave, registroDatos, tamRegistro);
	cout << "resultado de la busqueda: " << resultado << endl;
	if (resultado == ResultadosIndices::OK) {
		
		cout << "CI: resultado = OK, mando a eliminar..." << endl;
		//Elimino la clave primaria
		resultado = indicePrimario->eliminar(clave);
		cout << "CI: elimine la primaria. Resultado: " << resultado << endl;
		
		if (resultado == ResultadosIndices::OK) {
			
			//Actualizo los indices secundarios
			Indice* indiceSecundario = NULL;
			Clave* claveSecundaria   = NULL;
			
			//Se instancia un objeto Registro, el cual es el encargado de
			//administrar el registro de los datos y de proporcionar las claves
			//secundarias creadas con los valores correspondientes.
			Registro registro(defManager.getTipoOrgRegistro(nombreTipo),
							  registroDatos, defManager.getListaTipos(nombreTipo),
							  defManager.getListaNombresAtributos(nombreTipo));
			
			for (MapaIndices::iterator iter = mapaIndices.begin();
				iter != mapaIndices.end(); ++iter) {
				
				//Obtengo la clave secundaria
				claveSecundaria = registro.getClave(iter->first);
				//Obtengo el índice secundario
				indiceSecundario = iter->second;
		
				indiceSecundario->eliminar(claveSecundaria, clave);
		
				delete claveSecundaria;
				
			}
			
		}
		
		delete[] registroDatos;
	}
	
	return resultado;
	
}


void eliminar(const string &nombreTipo, MapaIndices &mapaIndices,
	   	  	  Indice *indice, Clave *clave,
	   	  	  DefinitionsManager &defManager, ComuDatos &pipe) {
	
	int resultado = ResultadosIndices::OK;
	
	if (indice->getTipoIndice() == TipoIndices::GRIEGO) {
	
		cout << "CI: es un indice primario." << endl;
		//Saco el índice primario para no volver a eliminar.
		mapaIndices.erase(*defManager.getListaNombresClavesPrimarias(nombreTipo));
		//Elimino la clave primaria y actualizo los índices secundarios.
		cout << "CI: voy a eliminar la clave primaria." << endl;
		resultado = eliminarClavePrimaria(nombreTipo, mapaIndices, indice, clave, defManager);
		cout << "CI: elimine la clave primaria." << endl;
		// Vuelve a insertar el indice que saco del mapa.
		mapaIndices[*defManager.getListaNombresClavesPrimarias(nombreTipo)] = indice;
		cout << "CI: reinserte el indice primario en el mapa." << endl;
		
	} else { //Indice Secundario
		cout << "CI: es un indice secundario." << endl;
		SetClaves* setClavesPrimarias = NULL;
		int resultado = indice->buscar(clave, setClavesPrimarias);
		
		if (resultado == ResultadosIndices::OK) {
			
			SetClaves::iterator iterClaves = setClavesPrimarias->begin();
			
			//Obtengo el índice primario
			MapaIndices::iterator it = mapaIndices.find(*defManager.getListaNombresClavesPrimarias(nombreTipo));
			Indice* indicePrimario = it->second;
			//Saco el índice primario del mapa para que no sea encontrado cuando se busquen los
			//índices secundarios en 'eliminarClavePrimaria'
			mapaIndices.erase(it);
			
			//Elimino todas las claves primarias vinculadas a esta clave secundaria
			for (unsigned i = 0; i < setClavesPrimarias->size(); ++i, ++iterClaves)
				resultado = eliminarClavePrimaria(nombreTipo, mapaIndices, indicePrimario, *iterClaves, defManager); 
			
			mapaIndices[*defManager.getListaNombresClavesPrimarias(nombreTipo)] = indicePrimario;
			
			delete setClavesPrimarias;
			
		}
		
	}
	
	//Envía el resultado de la eliminación a la capa de metadata
	cout << "CI: Termine de eliminar. Resultado: "<< resultado << ". lo mando." << endl;
	pipe.escribir(resultado);
	
}


void eliminarPorRango(const string &nombreTipo, MapaIndices &mapaIndices,
	   	  	  Indice *indice, Clave *clave,
	   	  	  DefinitionsManager &defManager, ComuDatos &pipe, char operacion) {
	/*
	int resultado = ResultadosIndices::OK;
	
	if (indice->getTipoIndice() == TipoIndices::GRIEGO) {
		
		//Saco el índice primario para no volver a eliminar.
		mapaIndices.erase(*defManager.getListaNombresClavesPrimarias(nombreTipo));
		//Elimino la clave primaria y actualizo los índices secundarios.
		resultado = eliminarClavePrimariaPorRango(nombreTipo, mapaIndices, indice, clave, defManager, operacion);
		//Elimino el índice primario que saqué del mapa
		delete indice;
		
	} else { //Indice Secundario
		
		ListaClaves* listaClavesPrimarias = NULL;
		
		
		int resultado = indice->buscar(clave, listaClavesPrimarias);
		
		if (resultado == ResultadosIndices::OK) {
			
			ListaClaves::iterator iterClaves = listaClavesPrimarias->begin();
			
			//Obtengo el índice primario
			MapaIndices::iterator it = mapaIndices.find(*defManager.getListaNombresClavesPrimarias(nombreTipo));
			Indice* indicePrimario = it->second;
			//Saco el índice primario del mapa para que no sea encontrado cuando se busquen los
			//índices secundarios en 'eliminarClavePrimaria'
			mapaIndices.erase(it);
			
			//Elimino todas las claves primarias vinculadas a esta clave secundaria
			for (unsigned i = 0; i < listaClavesPrimarias->size(); ++i, ++iterClaves)
				resultado = eliminarClavePrimaria(nombreTipo, mapaIndices, indicePrimario, *iterClaves, defManager); //TODO verlo bien
			
			delete indicePrimario;
			
			destruirListaClaves(listaClavesPrimarias);
			
		}
		
	}
	
	//Envío el resultado a la capa de metadata
	pipe.escribir(resultado);
	*/
}


int modificarClavePrimaria(const string &nombreTipo, MapaIndices &mapaIndices,
 	  	  	   			   Indice *indicePrimario, Clave *clavePrimariaVieja,
 	  	  	   			   DefinitionsManager &defManager, ComuDatos &pipe) {
	
	int resultado = ResultadosIndices::OK;
	unsigned short tamRegistro = 0;
	char *registroViejo = NULL;
	
	// Busca el registro viejo.
	resultado = indicePrimario->buscar(clavePrimariaVieja, registroViejo, tamRegistro);
	pipe.escribir(resultado);
	
	if (resultado == ResultadosIndices::OK) {
		// Se envía el tamaño del registro a modificar.
		pipe.escribir(tamRegistro);
		//	Se envía el registro viejo
		pipe.escribir(registroViejo, tamRegistro);
		
		// Se recibe el tamaño del registro modificado.
		pipe.leer(&tamRegistro);
		
		//Creo un buffer con el tamaño del nuevo registro
		char* registroNuevo = new char[tamRegistro];
		
		// Se recibe el registro con los atributos modificados
		pipe.leer(tamRegistro, registroNuevo);

		// Se instancia un objeto Registro, el cual es el encargado de
		// administrar el registro de los datos y de proporcionar las claves
		// creadas con los valores correspondientes.
		Registro rv(defManager.getTipoOrgRegistro(nombreTipo),
					registroViejo, defManager.getListaTipos(nombreTipo),
					defManager.getListaNombresAtributos(nombreTipo));
		
		Registro rn(defManager.getTipoOrgRegistro(nombreTipo),
					registroNuevo, defManager.getListaTipos(nombreTipo),
					defManager.getListaNombresAtributos(nombreTipo));
		
		Clave* clavePrimariaNueva = rn.getClave(*defManager.getListaNombresClavesPrimarias(nombreTipo));
		
		resultado = indicePrimario->modificar(clavePrimariaVieja, clavePrimariaNueva, registroNuevo, tamRegistro);
		
		if (resultado == ResultadosIndices::OK) {
			
			//Actualizo los indices secundarios
			Indice* indiceSecundario = NULL;
			Clave* claveSecundariaVieja = NULL;
			Clave* claveSecundariaNueva = NULL;
			
			for (MapaIndices::iterator iter = mapaIndices.begin();
				iter != mapaIndices.end(); ++iter) {
				
				//Obtengo la clave secundaria vieja
				claveSecundariaVieja = rv.getClave(iter->first);
				//Obtengo la clave secundaria nueva
				claveSecundariaNueva = rn.getClave(iter->first);
				//Obtengo el índice secundario
				indiceSecundario = iter->second;
				
				indiceSecundario->modificar(claveSecundariaVieja, claveSecundariaNueva, clavePrimariaVieja, clavePrimariaNueva);
				
				delete claveSecundariaVieja;
				delete claveSecundariaNueva;
				
			}
		}
	
		delete[] registroViejo;
		delete[] registroNuevo;
	}
	
	//Envío el resultado a la capa de metadata
	pipe.escribir(resultado);
	
	return resultado;
	
}
void modificacionNoIndexadaPorRango(const string &nombreTipo, MapaIndices &mapaIndices,
									DefinitionsManager &defManager, ComuDatos &pipe) {
	
	// Obtiene el indice primario para poder obtener luego los bloques de datos.
	Indice* indice = mapaIndices[*defManager.getListaNombresClavesPrimarias(nombreTipo)];
	
	ListaTiposAtributos *listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
	
	Clave* claveVieja = NULL;
	Bloque* bloque = NULL;
	char* registroViejo = NULL;
	char* registroNuevo = NULL;
	unsigned short tamanioRegistroViejo = 0;
	unsigned short tamanioRegistroNuevo = 0;

	// Obtiene un bloque de datos.
	int resultado = indice->siguienteBloque(bloque);
	
	char operacion = 0;
	
	// Se itera mientras haya registros.
	 while (resultado == ResultadosIndices::OK) {
		
		// Se obtiene un registro.
		registroViejo = bloque->getNextRegister();
	
		while (registroViejo) {
			
			// Indica a metadata que tiene un registro para enviarle.
			pipe.escribir(resultado);
				
			// Obtiene el tamaño del registro a enviar.
			tamanioRegistroViejo = bloque->getTamanioRegistroConPrefijo(listaTiposAtributos, registroViejo);
			pipe.escribir(tamanioRegistroViejo);
			pipe.escribir(registroViejo, tamanioRegistroViejo);
			
			pipe.leer(&operacion);
			
			// Si el registro debe ser modificado, llega por el pipe, el nuevo registro;
			if (operacion == OperacionesCapas::INDICES_MODIFICAR) {

				pipe.leer(&tamanioRegistroNuevo);
				registroNuevo = new char[tamanioRegistroNuevo];
				pipe.leer(tamanioRegistroNuevo, registroNuevo);
				
				claveVieja = bloque->getClavePrimaria(listaTiposAtributos,registroViejo);
				modificar(nombreTipo,mapaIndices,indice,claveVieja,defManager,pipe);
				
				delete claveVieja;
			}
			
			delete[] registroViejo;
			registroViejo = bloque->getNextRegister();	
		}	

		delete bloque;
		bloque = NULL;
		resultado = indice->siguienteBloque(bloque);
	}
}

void modificar(const string &nombreTipo, MapaIndices &mapaIndices,
 	  	  	   Indice *indice, Clave *clave,
 	  	  	   DefinitionsManager &defManager, ComuDatos &pipe) {
	
	int resultado = ResultadosIndices::OK;
	unsigned short cantRegistros = 1;
		
	if (indice->getTipoIndice() == TipoIndices::GRIEGO) {
		
		//Se envía la cantidad de registros que se deben modificar
		pipe.escribir(cantRegistros);
		
		//Saco el índice primario para no volver a modificar.
		mapaIndices.erase(*defManager.getListaNombresClavesPrimarias(nombreTipo));
		//Modifico la clave primaria y actualizo los índices secundarios.
		resultado = modificarClavePrimaria(nombreTipo, mapaIndices, indice, clave, defManager, pipe);
		// Vuelvo a insertar el índice primario que saqué del mapa
		mapaIndices[*defManager.getListaNombresClavesPrimarias(nombreTipo)] = indice;
		
	} else { //Indice Secundario
		
		SetClaves* setClavesPrimarias = NULL;
		int resultado = indice->buscar(clave, setClavesPrimarias);
		
		if (resultado == ResultadosIndices::OK) {
			
			//Se envía la cantidad de registros que se deben modificar
			cantRegistros = setClavesPrimarias->size();
			pipe.escribir(cantRegistros);
			
			SetClaves::iterator iterClaves = setClavesPrimarias->begin();
			
			//Obtengo el índice primario
			MapaIndices::iterator it = mapaIndices.find(*defManager.getListaNombresClavesPrimarias(nombreTipo));
			Indice* indicePrimario = it->second;
			//Saco el índice primario del mapa para que no sea encontrado cuando se busquen los
			//índices secundarios en 'modificarClavePrimaria'
			mapaIndices.erase(it);
			
			//Modifico todas las claves primarias vinculadas a esta clave secundaria
			for (unsigned i = 0; i < setClavesPrimarias->size(); ++i, ++iterClaves)
				resultado = modificarClavePrimaria(nombreTipo, mapaIndices, indicePrimario, *iterClaves, defManager, pipe);
			
			mapaIndices[*defManager.getListaNombresClavesPrimarias(nombreTipo)] = indicePrimario;
			
			delete setClavesPrimarias;
			
		}
		
	}
	
}


int procesarOperacion(unsigned char codOp, const string &nombreTipo, ComuDatos &pipe) {
	int resultado = ResultadosIndices::OK;
	string buffer(""), auxStr(""), valorClave("");
	DefinitionsManager& defManager = DefinitionsManager::getInstance();
	ListaValoresClaves listaValoresClaves;
	string::size_type posAnterior = 0, posActual = 0, posSeparador = 0;
	char operador = 0;
	
	MapaIndices mapaIndices;
	Clave *clave           = NULL;
	Indice* indice         = NULL;
	ListaTipos* listaTipos = NULL;
	
	//Si la operación es una inserción, recibo la clave o la lista de claves del índice primario
		
	//Leo el buffer con los nombres y los valores de cada campo de la variable
	pipe.leer(buffer);
	
	//Se crean los indices correspondientes al tipo 'nombreTipo'
	crearIndices(nombreTipo, mapaIndices, defManager);
	
	if (buffer.size() > 5) {
		
		if (codOp == OperacionesCapas::INDICES_CONSULTAR_TODO)
			indice = mapaIndices[*defManager.getListaNombresClavesPrimarias(nombreTipo)];
		else {
			// Se parsea el buffer obteniendo los nombres de la claves y sus valores correspondientes
			// (NOMBRE_CLAVE-COMIENZO_OPERADOR-OPERADOR-FIN_OPERADOR-VALOR_CLAVE)
			posActual = buffer.find(CodigosPipe::COD_FIN_CLAVE, posAnterior);
			while ( (posActual != string::npos) ) {
				auxStr = buffer.substr(posAnterior, posActual - posAnterior);
				// En auxStr tengo (NOMBRE_CLAVE-COMIENZO_OPERADOR-OPERADOR-FIN_OPERADOR-VALOR_CLAVE)
				// sin los guiones.
				posSeparador = auxStr.find(CodigosPipe::COMIENZO_OPERADOR);
				operador = auxStr[posSeparador + 1];
				listaNombresClaves.push_back(auxStr.substr(0, posSeparador));
				valorClave = auxStr.substr(posSeparador+3);
				if (valorClave.size() > 0) listaValoresClaves.push_back(valorClave);
				posAnterior = posActual + 1;
				posActual = buffer.find(CodigosPipe::COD_FIN_CLAVE, posAnterior);
			}
			
			// Busco el índice correspondiente a los nombres de los campos que componen la clave.
			// Si no se encuentra el índice, significa que esos campos no están indexados, por lo
			// que habrá que hacer una búsqueda secuencial para resolver la operación.
			// Además, si la lista de nombres de claves contiene elementos y la lista
			// de valores de claves está vacia, significa que la operación es un order by
			// por los campos cuyos nombres figuran el 'listaNombresClaves'
			
			if ( (listaNombresClaves.size() > 0) && (listaValoresClaves.size() == 0) ) {
				// La operación es una consulta con un OrderBy
				indice = obtenerIndiceParaOrderBy(mapaIndices, listaNombresClaves);
				if (!indice) {
					// No se encontró un índice para devolver los registros ordenados
					// por los campos indicados. Se devuelve el error correspondiente.
					resultado = ResultadosIndices::ERROR_NO_HAY_INDICE;
					pipe.escribir(resultado);
				}
				clave = NULL;				
			} else {
				MapaIndices::iterator it = mapaIndices.find(listaNombresClaves);
				if (it != mapaIndices.end()) indice = it->second;

				listaTipos = defManager.getListaTiposClaves(nombreTipo, listaNombresClaves);
				clave = ClaveFactory::getInstance().getClave(listaValoresClaves, *listaTipos);

				delete listaTipos;
			}

		}
		
	} else {
		
		resultado = ResultadosIndices::ERROR_VALORES_CLAVES;
		pipe.escribir(resultado);
		
	}
	
	if (resultado == ResultadosIndices::OK) {
		
		switch(codOp) {
			case OperacionesCapas::INDICES_CONSULTAR:
				// Si la consulta se hace a un índice primario, se envía el registro
				// pedido, sino se envían todos los registros correspondiente a las
				// claves primarias de la lista del indice secundario.
				// Si no se encuentra ningún índice por el campo solicitado,
				// se realiza una búsqueda secuencial en el archivo de datos,
				// y se envían todos los registros que coincidan con el campo solicitado.
				if (indice) {
					
					if (operador == ExpresionesLogicas::IGUAL)
						consultaIndexada(nombreTipo, mapaIndices, indice, clave, defManager, pipe);
					else {
						
						if (indice->getTipoEstructura() != TipoIndices::HASH)
							consultaIndexadaPorRango(nombreTipo, mapaIndices, indice, clave, operador, defManager, pipe);
						else {
							// No se encontró un índice para devolver los registros ordenados
							// por los campos indicados. Se devuelve el error correspondiente.
							resultado = ResultadosIndices::ERROR_NO_HAY_INDICE;
							pipe.escribir(resultado);
						}
						
					}
					
				} else
					consultaNoIndexada(nombreTipo, mapaIndices, clave, defManager, pipe);
				break;
			case OperacionesCapas::INDICES_CONSULTAR_TODO:
				// Se devuelven todos los registros ordenados según el índice primario
				consultaIndexada(nombreTipo, mapaIndices, indice, clave, defManager, pipe);
				break;
			case OperacionesCapas::INDICES_INSERTAR:
				// Siempre se inserta un registro a partir de su clave primaria,
				// y luego se actualizan los índices secundarios.
				insertar(nombreTipo, mapaIndices, indice, clave, defManager, pipe);
				break;
			case OperacionesCapas::INDICES_ELIMINAR:
				// Si la eliminación se hace a partir de una clave primaria, se elimina
				// dicha clave y su correspondiente registro de datos, y se actualizan
				// todos los indices secundarios.
				// Si la eliminación es a partir de una clave secundaria, se obtienen
				// todas las claves primarias y se lleva a cabo la eliminación para cada
				// una de ellas.
				eliminacionNoIndexadaPorRango(nombreTipo, mapaIndices, defManager, pipe);			
				break;
			case OperacionesCapas::INDICES_MODIFICAR:
				// Si la modificación se hace a partir de una clave primaria, se modifica
				// dicha clave y su correspondiente registro de datos, y se actualizan
				// todos los indices secundarios.
				// Si la modificación es a partir de una clave secundaria, se obtienen
				// todas las claves primarias y se lleva a cabo la modificación para cada
				// una de ellas.
				modificacionNoIndexadaPorRango(nombreTipo, mapaIndices, defManager, pipe);
				break;
			default:
				//No se reconoce la operación
				resultado = ResultadosIndices::OPERACION_INVALIDA;
				pipe.escribir(resultado);
		}
		
	}
	
	destruirIndices(mapaIndices);
	
	if (clave) delete clave;
	
	return resultado;
	
}

int main(int argc, char* argv[]) {
	
	ComuDatos pipe(argv);
	unsigned char codOp = 0;
	string nombreTipo;
	
	pipe.parametro(0, codOp);
	pipe.parametro(1, nombreTipo);
	
	procesarOperacion(codOp, nombreTipo, pipe);
		
	cout << "Fin Capa Indices" << endl;
}

