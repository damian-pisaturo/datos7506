#include "CapaMetadata.h"


int main(int argc, char* argv[]) {
	
	if (argc < 2) return ERROR;
		
	// Se instancia el DefinitionsManager. Esta clase tiene todas las definiciones.
	DefinitionsManager defManager;
	
	//Para probar esta capa hay cargadas definiciones de dos tipos (Persona y Pelicula)
	
	//Se crean los indices para los registros de tipo persona.
	
	MapaMapaIndices mapaMapaIndices;
	
	const DefinitionsManager::MapaTiposIndices &mapaTiposIndices = defManager.getMapaTiposIndices();
	DefinitionsManager::ListaTiposIndices* listaTiposIndices;
	MapaIndices* mapaIndices;
	string nombreTipo;
	DefinitionsManager::NodoListaIndices nodoListaIndices;			
	unsigned char tipoIndice;
			
	for (DefinitionsManager::MapaTiposIndices::const_iterator iterMapa = mapaTiposIndices.begin();
		iterMapa != mapaTiposIndices.end(); ++iterMapa) {
		
		mapaIndices = new MapaIndices();
		nombreTipo = iterMapa->first;
		listaTiposIndices = iterMapa->second;
		
		for (DefinitionsManager::ListaTiposIndices::const_iterator iter = listaTiposIndices->begin();
			iter != listaTiposIndices->end(); ++iter) {
			
			nodoListaIndices = *iter;
			
			tipoIndice = nodoListaIndices.estructTipoIndice.tipoEstructura;
				
			crearIndice(nombreTipo, *mapaIndices, nodoListaIndices, defManager);
			
		}
		
		mapaMapaIndices[nombreTipo] = mapaIndices;
		
	}
	
	ParserOperaciones parserOperaciones(argv[1], mapaMapaIndices);
	
	//TODO: modificar parser
	
	destruirMapaMapaIndices();	
	
}