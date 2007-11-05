#include "ParserOperaciones.h"

ParserOperaciones::ParserOperaciones(const string &nombreArchivo, VectorIndices &vectorIndicesPersonas,
									 VectorIndices &vectorIndicesPeliculas)
									: indicesPersonas(vectorIndicesPersonas),
									indicesPeliculas(vectorIndicesPeliculas){
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
			switch(lineaStr[0]) {
				case 'A':
					//Llamada al indice correspondiente
					//dataManager.insertar(nombreTipo, defManager.getListaValoresAtributos(nombreTipo, mapValoresAtributos), defManager.getListaTiposAtributos(nombreTipo) );
					break;
				case 'B':
					//llamada al indice correspondiente
					//dataManager.eliminar(nombreTipo, &listaClaves);
					break;
				case 'M':
					//Llamada al indice correspondiente
					//dataManager.modificar(nombreTipo, defManager.getListaValoresAtributos(nombreTipo, mapValoresAtributos), defManager.getListaTiposAtributos(nombreTipo), &listaClaves);
					break;
				default:
					return false;
			}
		}
	}
	
	return true;
}
