#include "ParserOperaciones.h"

string ParserOperaciones::generarPrototipoClave(DefinitionsManager::ListaClaves &listaClaves){
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

ParserOperaciones::ParserOperaciones(const string &nombreArchivo, MapaMapaIndices mapaMapaIndices)
									: mapaMapaIndices(mapaMapaIndices) {
	archivo.open(nombreArchivo.c_str());
}

ParserOperaciones::~ParserOperaciones() {
	if (archivo.is_open()) archivo.close();
}

bool ParserOperaciones::ejecutarOperaciones(){
	string lineaStr;
	DefinitionsManager defManager;
	DataManager dataManager;
	bool leyendoClaves; // es true si se trata de una modificacion y esta leyendo la parte que contiene la clave o lista de claves 
	 
	while (!archivo.fail()){
		leyendoClaves = false;
		getline(archivo, lineaStr);
		
		if (lineaStr.size() != 0){
		
			DefinitionsManager::MapaValoresAtributos mapValoresAtributos;
			DefinitionsManager::ListaClaves listaClaves;
			
			size_t separatorPos, nextSeparatorPos, proximoSeparatorCampos;
			string nombreTipo;
			string nombreAtributo, valorAtributo;
			
			separatorPos = lineaStr.find(SEPARATOR_CAMPOS);
			nextSeparatorPos = lineaStr.find(SEPARATOR_CAMPOS, separatorPos + 1);
			
			nombreTipo = lineaStr.substr(separatorPos + 1, nextSeparatorPos - separatorPos - 1);
			
			
			proximoSeparatorCampos = lineaStr.find(SEPARATOR_CAMPOS, lineaStr.find(SEPARATOR_CAMPOS, nextSeparatorPos + 1));
			
			while (nextSeparatorPos != string::npos){
				separatorPos = nextSeparatorPos;
				nextSeparatorPos = lineaStr.find(ASIGNACION_ATRIBUTOS, separatorPos + 1);
				nombreAtributo = lineaStr.substr(separatorPos + 1, nextSeparatorPos - separatorPos - 1);
				
				separatorPos = nextSeparatorPos;
				nextSeparatorPos = lineaStr.find(SEPARATOR_ATRIBUTOS, separatorPos + 1);
				
				if ( (!leyendoClaves) && (lineaStr[0] == 'M') && ((nextSeparatorPos == string::npos) || (nextSeparatorPos > proximoSeparatorCampos)) ){
					nextSeparatorPos = proximoSeparatorCampos;
				}
				valorAtributo = lineaStr.substr(separatorPos + 1, nextSeparatorPos - separatorPos - 1);

				switch(lineaStr[0]){
					case 'A':
						mapValoresAtributos[nombreAtributo] = valorAtributo;
						break;
					case 'B': {
						DefinitionsManager::NodoListaClaves nodo;
						nodo.nombreClave = nombreAtributo;
						nodo.valorClave = valorAtributo;
						listaClaves.push_back(nodo);
						break;
					}
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
					default:
						return false;
				}
			}
			
			// Instancio comuDatos para la comunicacion con la capa de indices
			ComuDatos comuDatos(NOMBRE_CAPA_INDICES); 
			
			switch(lineaStr[0]) {
				case 'A':{
					int tamanioRegistro;
					int nroBloque;
					char *datos = new char[Tamanios::TAMANIO_BLOQUE_DATO];
					ListaNodos *listaTiposAtributos;
					DefinitionsManager::ListaValoresAtributos *listaValoresAtributos;
					
					// Obtengo las listas necesarias con la informacion de los registros
					DataManager *dataManager = new DataManager();
					listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
					listaValoresAtributos = defManager.getListaValoresAtributos(nombreTipo, mapValoresAtributos);
					
					// Obtengo el tamanio que va ocupar el nuevo registro
					tamanioRegistro = dataManager->getTamanioRegistro(*listaTiposAtributos,*listaValoresAtributos);
					
					comuDatos.agregarParametro(OperacionesCapas::INDICES_INSERTAR, 0);
					comuDatos.agregarParametro(nombreTipo, 1);
					comuDatos.lanzar();
					comuDatos.escribir(tamanioRegistro);
					comuDatos.leer(&nroBloque);
					comuDatos.leer(Tamanios::TAMANIO_BLOQUE_DATO,datos);
					
					// Creo el bloque
					Bloque *bloque = new Bloque(nroBloque,Tamanios::TAMANIO_BLOQUE_DATO);
					bloque->setDatos(datos);
					
					// TODO: VER NUMERO DE RETORNO PARA VALIDACIONES
					dataManager->insertar(listaValoresAtributos,listaTiposAtributos,bloque);
					
					break;
				}

				case 'B':{
					int nroBloque;
					char *datos = new char[Tamanios::TAMANIO_BLOQUE_DATO];
					ListaNodos *listaTiposAtributos;
					DefinitionsManager::ListaValoresAtributos *listaValoresAtributos;
					string clave;
					
					// Obtengo las listas necesarias con la informacion de los registros
					DataManager *dataManager = new DataManager();
					listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
					listaValoresAtributos = defManager.getListaValoresAtributos(nombreTipo, mapValoresAtributos);
					
					comuDatos.agregarParametro(OperacionesCapas::INDICES_ELIMINAR, 0);
					comuDatos.agregarParametro(nombreTipo, 1);
					comuDatos.lanzar();
					
					clave = this->generarPrototipoClave(listaClaves);
					comuDatos.escribir(clave.c_str(),clave.size());
					comuDatos.leer(&nroBloque);
					comuDatos.leer(Tamanios::TAMANIO_BLOQUE_DATO,datos);
					
					// Creo el bloque
					Bloque *bloque = new Bloque(nroBloque,Tamanios::TAMANIO_BLOQUE_DATO);
					bloque->setDatos(datos);
					
					// TODO: VER NUMERO DE RETORNO PARA VALIDACIONES
					dataManager->eliminar(&listaClaves,listaTiposAtributos,bloque);
					
					break;
				}
					
				case 'M':{
					int nroBloque;
					char *datos = new char[Tamanios::TAMANIO_BLOQUE_DATO];
					ListaNodos *listaTiposAtributos;
					DefinitionsManager::ListaValoresAtributos *listaValoresAtributos;
					string clave;
					char *registroViejo;
					
					// Obtengo las listas necesarias con la informacion de los registros
					DataManager *dataManager = new DataManager();
					listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
					listaValoresAtributos = defManager.getListaValoresAtributos(nombreTipo, mapValoresAtributos);
					
					comuDatos.agregarParametro(OperacionesCapas::INDICES_MODIFICAR, 0);
					comuDatos.agregarParametro(nombreTipo, 1);
					comuDatos.lanzar();
					
					clave = this->generarPrototipoClave(listaClaves);
					comuDatos.escribir(clave.c_str(),clave.size());
					comuDatos.leer(&nroBloque);
					comuDatos.leer(Tamanios::TAMANIO_BLOQUE_DATO,datos);
					
					// Creo el bloque
					Bloque *bloque = new Bloque(nroBloque, Tamanios::TAMANIO_BLOQUE_DATO);
					bloque->setDatos(datos);
					
					// TODO: VER NUMERO DE RETORNO PARA VALIDACIONES
					dataManager->modificar(listaValoresAtributos, listaTiposAtributos, &listaClaves, bloque, registroViejo);
					
					break;
				}
				default:
					return false;
			}
		}
	}
	
	return true;
}
