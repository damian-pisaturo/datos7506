#include "CapaMetadata.h"


int main(int argc, char* argv[]) {
	
	if (argc < 2) return ERROR;
		
	// Se instancia el DefinitionsManager. Esta clase tiene todas las definiciones.
	DefinitionsManager defManager;
	
	//Para probar esta capa hay cargadas definiciones de dos tipos (Persona y Pelicula)
	
	ParserOperaciones parserOperaciones("../operaciones.txt");
	parserOperaciones.ejecutarOperaciones();
	
	return 0;
	
}

