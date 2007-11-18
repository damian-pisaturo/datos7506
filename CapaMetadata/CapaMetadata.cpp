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

int main(int argc, char* argv[]) 
{
	
	
	//if (argc < 2) return 1;
		
	// Se instancia el DefinitionsManager. Esta clase tiene todas las definiciones.
	//DefinitionsManager defManager;
	
	// Para probar esta capa hay cargadas definiciones de dos tipos (Persona y Pelicula)
	
	//ParserOperaciones parserOperaciones("../operaciones.txt");
	//parserOperaciones.ejecutarOperaciones();
	
	/*cout << "Datos del registro" << endl;
	
	char *registro = new char [30];
	int entero = 789;
	int offset = 2;
	string campoVariable = "Maria";
	unsigned char longCampoVariable;
	ListaNodos lista;
	nodoLista nodo;
	list<string> listaValoresAtributos;
	listaValoresAtributos.push_back("");
	listaValoresAtributos.push_back("Rodriguez");
	listaValoresAtributos.push_back("20");
	listaValoresAtributos.push_back("21");
	listaValoresAtributos.push_back("");
	listaValoresAtributos.push_back("20081212");
	listaValoresAtributos.push_back("");
	listaValoresAtributos.push_back("");
	unsigned short longR = 28; 
	nodo.tipo = TipoDatos::TIPO_VARIABLE;
	lista.push_back(nodo);
	short campoShort = 76;
	nodo.pk = "fruta";
	nodo.tipo  = TipoDatos::TIPO_STRING;
	lista.push_back(nodo);
	lista.push_back(nodo);
	memcpy(registro,&longR,2);
	**************** PRIMER CAMPO VARIABLE *********************

	longCampoVariable = campoVariable.size();
	
	memcpy((registro + offset), &longCampoVariable, Tamanios::TAMANIO_LONGITUD_CADENA);
	offset += Tamanios::TAMANIO_LONGITUD_CADENA;
	
	memcpy((registro + offset), campoVariable.c_str(), longCampoVariable);
	offset += longCampoVariable;
	
	**************** SEGUNDO CAMPO VARIABLE *********************
	
	campoVariable = "Laura";

	longCampoVariable = campoVariable.size();
	
	memcpy((registro + offset), &longCampoVariable, Tamanios::TAMANIO_LONGITUD_CADENA);
	offset += Tamanios::TAMANIO_LONGITUD_CADENA;
	
	memcpy((registro + offset), campoVariable.c_str(), longCampoVariable);
	offset += longCampoVariable;
	
	nodo.tipo = TipoDatos::TIPO_SHORT;
	lista.push_back(nodo);
	nodo.tipo = TipoDatos::TIPO_ENTERO;
	memcpy((registro + offset), &campoShort, sizeof(short));
	offset += sizeof(short);
	
	lista.push_back(nodo);
	memcpy((registro + offset), &entero, sizeof(int));
	offset += sizeof(int);
	entero = 2;
	lista.push_back(nodo);
	memcpy((registro + offset), &entero, sizeof(int));
	offset += sizeof(int);
	
	nodo.tipo = TipoDatos::TIPO_FECHA;
	lista.push_back(nodo);
	unsigned short anio = 2007;
	unsigned char mes = 11;
	unsigned char dia = 14;
	
	memcpy((registro + offset), &anio, sizeof(unsigned short));
	offset += sizeof(unsigned short);
	
	memcpy((registro + offset), &mes, sizeof(unsigned char));
	offset += sizeof(unsigned char);
	
	memcpy((registro + offset), &dia, sizeof(unsigned char));
	offset += sizeof(unsigned char);
	
	nodo.tipo = TipoDatos::TIPO_BOOL;
	lista.push_back(nodo);
	lista.push_back(nodo);
	
	bool pepe = true;
	memcpy((registro + offset), &pepe, sizeof(bool));
	offset += sizeof(bool);
	
	pepe = false;
	memcpy((registro + offset), &pepe, sizeof(bool));
	offset += sizeof(bool);
	Vista *vista = new Vista();
	
	vista->showRegister(registro,&lista);
	
	DataManager *dataManager = new DataManager();
	dataManager->crearRegistroModificacion(lista, listaValoresAtributos, registro);
	
	cout << "El modificado" <<endl;
	
	vista->showRegister(dataManager->getRegistro(),&lista);
	
	cout << "Pruebo como genero un registro para un alta" <<endl;
	list<string> listaValoresA;
	    listaValoresA.push_back("Juan");
		listaValoresA.push_back("Ramon");
		listaValoresA.push_back("67");
		listaValoresA.push_back("68");
		listaValoresA.push_back("25");
		listaValoresA.push_back("20071212");
		listaValoresA.push_back("1");
		listaValoresA.push_back("1");
		
		dataManager->crearRegistroAlta(listaValoresA,lista);
		
		vista->showRegister(dataManager->getRegistro(),&lista);
	return 0;
*/	
	
}

