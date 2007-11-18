#include "ParserOperaciones.h"
/*
string ParserOperaciones::generarPrototipoClave(DefinitionsManager::ListaClaves &listaClaves)
{
	string clave = NULL;
	DefinitionsManager::ListaClaves::const_iterator it = listaClaves.begin();
	
	clave =  it->nombreClave;
	clave += "=";
	clave += it->valorClave;
	clave += CodigosPipe::COD_FIN_CLAVE;
	
	for (++it; (it != listaClaves.end()); ++it){
		clave += it->nombreClave;
		clave += "=";
		clave += it->valorClave;
		clave += CodigosPipe::COD_FIN_CLAVE;
	}
	
	return clave;
}
*/


ParserOperaciones::ParserOperaciones(const string &nombreArchivo)
{
	archivo.open(nombreArchivo.c_str());
	numOperacion = 0;
	mapValoresAtributos = NULL;
	listaClaves = NULL;
}

ParserOperaciones::~ParserOperaciones()
{
	if (archivo.is_open()) archivo.close();
	if (mapValoresAtributos){
		delete mapValoresAtributos;
		mapValoresAtributos = NULL;
	}
	if (listaClaves){
		delete listaClaves;
		listaClaves = NULL;
	}
	
}

bool ParserOperaciones::proximaOperacion()
{	
	string linea;
	bool leyendoClaves; // es true si se trata de una modificacion y esta leyendo la parte que contiene la clave o lista de claves 
	bool procesado = false;
	
	while ( (!archivo.fail()) && (!procesado) ){
		
		leyendoClaves = false;
		getline(archivo, linea);
		
		if (linea.size() != 0){
			if (mapValoresAtributos){
				delete mapValoresAtributos;
				mapValoresAtributos = NULL;
			}
			mapValoresAtributos = new DefinitionsManager::MapaValoresAtributos();
			
			if (listaClaves){
				delete listaClaves;
				listaClaves = NULL;
			}
			listaClaves = new DefinitionsManager::ListaClaves();
			
			procesado = true;
			numOperacion++;
									
			size_t separatorPos, nextSeparatorPos, proximoSeparatorCampos;
			string nombreAtributo, valorAtributo;
			
			separatorPos = linea.find(SEPARATOR_CAMPOS);
			nextSeparatorPos = linea.find(SEPARATOR_CAMPOS, separatorPos + 1);
			
			nombreTipo = linea.substr(separatorPos + 1, nextSeparatorPos - separatorPos - 1);
			
			
			proximoSeparatorCampos = linea.find(SEPARATOR_CAMPOS, linea.find(SEPARATOR_CAMPOS, nextSeparatorPos + 1));
			
			while (nextSeparatorPos != string::npos){
				separatorPos = nextSeparatorPos;
				nextSeparatorPos = linea.find(ASIGNACION_ATRIBUTOS, separatorPos + 1);
				nombreAtributo = linea.substr(separatorPos + 1, nextSeparatorPos - separatorPos - 1);
				
				separatorPos = nextSeparatorPos;
				nextSeparatorPos = linea.find(SEPARATOR_ATRIBUTOS, separatorPos + 1);
				
				if ( (!leyendoClaves) && (linea[0] == MODIFICACION) && ((nextSeparatorPos == string::npos) || (nextSeparatorPos > proximoSeparatorCampos)) ){
					nextSeparatorPos = proximoSeparatorCampos;
				}
				valorAtributo = linea.substr(separatorPos + 1, nextSeparatorPos - separatorPos - 1);

				switch(linea[0]){
					case ALTA:
						tipoOperacion = OperacionesCapas::METADATA_ALTA;
						(*mapValoresAtributos)[nombreAtributo] = valorAtributo;
						break;
					
					case BAJA: {
						tipoOperacion = OperacionesCapas::METADATA_BAJA;
						DefinitionsManager::NodoListaClaves nodo;
						nodo.nombreClave = nombreAtributo;
						nodo.valorClave = valorAtributo;
						listaClaves->push_back(nodo);
						
					}break;
					
					case MODIFICACION:
						tipoOperacion = OperacionesCapas::METADATA_MODIFICACION;
						if (leyendoClaves){
							DefinitionsManager::NodoListaClaves nodo;
							nodo.nombreClave = nombreAtributo;
							nodo.valorClave = valorAtributo;
							listaClaves->push_back(nodo);
						}
						else{
							(*mapValoresAtributos)[nombreAtributo] = valorAtributo;
							if (nextSeparatorPos == proximoSeparatorCampos) leyendoClaves = true;
						}
						break;
						
					case CONSULTA: {
						tipoOperacion = OperacionesCapas::METADATA_CONSULTA;
						DefinitionsManager::NodoListaClaves nodo;
						nodo.nombreClave = nombreAtributo;
						nodo.valorClave = valorAtributo;
						listaClaves->push_back(nodo);
					
					}break;
						
				}
			}		


		}
	}
	
	return procesado;
	
}

/*
char ParserOperaciones::ejecutarOperaciones()
{
	char resultado = c
	string linea;
	DefinitionsManager defManager;
	DataManager dataManager;
	bool leyendoClaves; // es true si se trata de una modificacion y esta leyendo la parte que contiene la clave o lista de claves 
	 
	while (!archivo.fail()){
		
		leyendoClaves = false;
		getline(archivo, linea);
		
		if (linea.size() != 0){
		
			DefinitionsManager::MapaValoresAtributos mapValoresAtributos;
			
			
			size_t separatorPos, nextSeparatorPos, proximoSeparatorCampos;
			string nombreTipo;
			string nombreAtributo, valorAtributo;
			
			separatorPos = linea.find(SEPARATOR_CAMPOS);
			nextSeparatorPos = linea.find(SEPARATOR_CAMPOS, separatorPos + 1);
			
			nombreTipo = linea.substr(separatorPos + 1, nextSeparatorPos - separatorPos - 1);
			
			
			proximoSeparatorCampos = linea.find(SEPARATOR_CAMPOS, linea.find(SEPARATOR_CAMPOS, nextSeparatorPos + 1));
			
			while (nextSeparatorPos != string::npos){
				separatorPos = nextSeparatorPos;
				nextSeparatorPos = linea.find(ASIGNACION_ATRIBUTOS, separatorPos + 1);
				nombreAtributo = linea.substr(separatorPos + 1, nextSeparatorPos - separatorPos - 1);
				
				separatorPos = nextSeparatorPos;
				nextSeparatorPos = linea.find(SEPARATOR_ATRIBUTOS, separatorPos + 1);
				
				if ( (!leyendoClaves) && (linea[0] == 'M') && ((nextSeparatorPos == string::npos) || (nextSeparatorPos > proximoSeparatorCampos)) ){
					nextSeparatorPos = proximoSeparatorCampos;
				}
				valorAtributo = linea.substr(separatorPos + 1, nextSeparatorPos - separatorPos - 1);

				switch(linea[0]){
					case 'A':
						mapValoresAtributos[nombreAtributo] = valorAtributo;
						break;
					
					case 'B': {
						DefinitionsManager::NodoListaClaves nodo;
						nodo.nombreClave = nombreAtributo;
						nodo.valorClave = valorAtributo;
						listaClaves.push_back(nodo);
						
					}break;
					
					case 'M':
						if (leyendoClaves){
							DefinitionsManager::NodoListaClaves nodo;
							nodo.nombreClave = nombreAtributo;
							nodo.valorClave = valorAtributo;
							listaClaves.push_back(nodo);
						}
						else{
							mapValoresAtributos[nombreAtributo] = valorAtributo;
							if (nextSeparatorPos == proximoSeparatorCampos) leyendoClaves = true;
						}
						break;
				}
			}		
*/			
			/*// Instancio comuDatos para la comunicacion con la capa de indices
			ComuDatos comuDatos(NOMBRE_CAPA_INDICES);
			
			int nroBloque                                                    = 0;			
			ListaNodos *listaTiposAtributos                                  = NULL;
			DefinitionsManager::ListaValoresAtributos *listaValoresAtributos = NULL;
			
			char* datos = new char[Tamanios::TAMANIO_BLOQUE_DATO];
			DataManager* dataManager = new DataManager();
			string clave;
			
			// Obtengo las listas necesarias con la informacion de los registros;
			listaTiposAtributos   = defManager.getListaTiposAtributos(nombreTipo);
			listaValoresAtributos = defManager.getListaValoresAtributos(nombreTipo, mapValoresAtributos);
			
			Bloque *bloque = NULL;
			
			switch(linea[0]) {
				case 'A':{
					int tamanioRegistro = 0;					
					
					// Obtengo el tamanio que va ocupar el nuevo registro
					tamanioRegistro = dataManager->getTamanioRegistro(*listaTiposAtributos,*listaValoresAtributos);
					
					comuDatos.agregarParametro(OperacionesCapas::INDICES_INSERTAR, 0);
					comuDatos.agregarParametro(nombreTipo, 1);
					
					comuDatos.lanzar();
					
					comuDatos.escribir(tamanioRegistro);
					comuDatos.leer(&nroBloque);
					
					// Obtengo los datos del bloque
					comuDatos.leer(Tamanios::TAMANIO_BLOQUE_DATO, datos);
					
					// Creo el bloque	
					bloque = new Bloque(nroBloque,Tamanios::TAMANIO_BLOQUE_DATO);
					bloque->setDatos(datos);
					
					// TODO: VER NUMERO DE RETORNO PARA VALIDACIONES
					dataManager->insertar(listaValoresAtributos,listaTiposAtributos, bloque);
					
					// Escritura del bloque modificado a disco.
					comuDatos.escribir(bloque->getDatos(), Tamanios::TAMANIO_BLOQUE_DATO);
					
					// Obtener resultado de la operacion.
					comuDatos.leer(&resultado);
					
				}break;

				case 'B':{									
					comuDatos.agregarParametro(OperacionesCapas::INDICES_ELIMINAR, 0);
					comuDatos.agregarParametro(nombreTipo, 1);
					
					comuDatos.lanzar();
					
					clave = this->generarPrototipoClave(listaClaves);
					comuDatos.escribir(clave.c_str(),clave.size());
					comuDatos.leer(&nroBloque);
					comuDatos.leer(Tamanios::TAMANIO_BLOQUE_DATO, datos);
					
					// Creo el bloque
					bloque = new Bloque(nroBloque,Tamanios::TAMANIO_BLOQUE_DATO);
					bloque->setDatos(datos);
					
					// TODO: VER NUMERO DE RETORNO PARA VALIDACIONES
					dataManager->eliminar(&listaClaves,listaTiposAtributos,bloque);
					
					// Escritura del bloque modificado a disco.
					comuDatos.escribir(bloque->getDatos(), Tamanios::TAMANIO_BLOQUE_DATO);
					
					// Obtener resultado de la operacion.
					comuDatos.leer(&resultado);
				
				}break;
					
				case 'M':{
					char *registroViejo = NULL;
					
					// TODO: DAMI TIENE QUE PASARME EL REGISTRO VIEJO
								
					comuDatos.agregarParametro(OperacionesCapas::INDICES_MODIFICAR, 0);
					comuDatos.agregarParametro(nombreTipo, 1);
					
					comuDatos.lanzar();
					
					clave = this->generarPrototipoClave(listaClaves);
					comuDatos.escribir(clave.c_str(), clave.size());
					
					comuDatos.leer(&nroBloque);
					comuDatos.leer(Tamanios::TAMANIO_BLOQUE_DATO, datos);
					
					// Creo el bloque
					bloque = new Bloque(nroBloque, Tamanios::TAMANIO_BLOQUE_DATO);
					bloque->setDatos(datos);
					
					// TODO: VER NUMERO DE RETORNO PARA VALIDACIONES
					dataManager->modificar(listaValoresAtributos, listaTiposAtributos, &listaClaves, bloque, registroViejo);
					
					// Escritura del bloque modificado a disco.
					comuDatos.escribir(bloque->getDatos(), Tamanios::TAMANIO_BLOQUE_DATO);
					
					// Obtener resultado de la operacion.
					comuDatos.leer(&resultado);
					
				}break;
				
				case 'C':{
			
					// Agregar parametros para lanzar la Capa de Indices y consultar
					comuDatos.agregarParametro(OperacionesCapas::INDICES_CONSULTAR, 0);
					comuDatos.agregarParametro(nombreTipo, 1);
					
					// Se lanza la Capa de Indices
					comuDatos.lanzar();
					
					clave = this->generarPrototipoClave(listaClaves);
					comuDatos.escribir(clave.c_str(), clave.size());
					
					comuDatos.leer(&resultado);
					
					if (resultado == ResultadosIndices::OK){
						comuDatos.leer(&nroBloque);
					
						// Se obtiene el bloque de datos contenedor 
						comuDatos.leer(Tamanios::TAMANIO_BLOQUE_DATO, datos);
										
						// Creo el bloque
						bloque = new Bloque(nroBloque, Tamanios::TAMANIO_BLOQUE_DATO);
						bloque->setDatos(datos);
					}
					
				}break;
				
				default:
					resultado = ResultadosMetadata::OPERACION_INVALIDA;
			}

			
			if (bloque)
				delete bloque;
								
			delete dataManager;
			delete listaTiposAtributos;
			delete listaValoresAtributos;
			
		}
	}
	
	return resultado;
*///}

