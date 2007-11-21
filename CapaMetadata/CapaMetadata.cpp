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


int main(int argc, char* argv[]) 
{
	Error errorOut;
	int resultado = ResultadosMetadata::OK;
	
	if (argc < 2){
		
		ComuDatos* pipe = instanciarPipe(); //Instancia del pipe de comunicacion		
		
		unsigned char operacion      = 0;    // Tipo de operacion a ejecutar
		unsigned short tamRegistro   = 0;	// Tamano de un registro.
		unsigned short cantRegistros = 0;  //Cant. de registros que responden a la consulta dada.
		char* registro               = NULL; // Buffer contenedor de un registro. 
		string valoresClaves;
		
		// Lista de los valores de las claves y sus nombres dentro de un registro.
		DefinitionsManager::ListaClaves* listaClaves = NULL; 
		
		// Mapa contenedor de los valores de los atributos, indexados por sus nombres.
		DefinitionsManager::MapaValoresAtributos* mapaValoresAtributos = NULL; 
		
		// Se instancia el DefinitionsManager (conocedor absoluto del universo).
		//DefinitionsManager& defManager = DefinitionsManager::getInstance();
		
		// Controlador de registros de datos
		DataManager dataManager;
		
		// Parseador del archivo de operaciones.
		ParserOperaciones parserOperaciones("../operaciones.txt");		
		
		while ( (parserOperaciones.proximaOperacion()) && (resultado == ResultadosIndices::OK) ){
			operacion   = parserOperaciones.getTipoOperacion();
			listaClaves = parserOperaciones.getListaClaves();
			serializarListaClaves(valoresClaves, listaClaves);
	
			switch(operacion){
				case OperacionesCapas::METADATA_CONSULTA:
				{
					// Codigo de operacion de consulta para la Capa de Indices
					pipe->agregarParametro(OperacionesCapas::INDICES_CONSULTAR, 0); 
					// Nombre del tipo de dato a ser dado de alta (Persona/Pelicula)
					pipe->agregarParametro(parserOperaciones.getNombreTipo(), 1);
					
					// Se lanza el proceso de la Capa Indices.
					resultado = pipe->lanzar();
					
					if (resultado == ComuDatos::OK){						
						// Envio de los valores de las claves a consultar por el pipe
						pipe->escribir(valoresClaves);
						
						// Obtencion del resultado de la operacion
						pipe->leer(&resultado);
						
						if (resultado == ResultadosIndices::OK){
							
							// Se obtiene la cantidad de registros que responden 
							// a la consulta realizada.
							pipe->leer(&cantRegistros);
							
							for (unsigned short i = 0; i < cantRegistros; i++){
								// Se obtiene el tamano del registro a levantar
								pipe->leer(&tamRegistro);
								
								registro = new char[tamRegistro*sizeof(char)];
								
								// Se obtiene el registro de datos consultado.
								pipe->leer(tamRegistro*sizeof(char), registro);
								
								//TODO Usar la vista para mostrar el resultado.
								
								delete[] registro;
							}
						}
					}
				}break;
				
				case OperacionesCapas::METADATA_BAJA:
				{
					// Codigo de operacion de consulta para la Capa de Indices
					pipe->agregarParametro(OperacionesCapas::INDICES_ELIMINAR, 0); 
					// Nombre del tipo de dato a ser dado de alta (Persona/Pelicula)
					pipe->agregarParametro(parserOperaciones.getNombreTipo(), 1);
					
					// Se lanza el proceso de la Capa Indices.
					resultado = pipe->lanzar();
					
					if (resultado == ComuDatos::OK){
						// Envio de los valores de las claves de los registros
						// a eliminar por el pipe.
						pipe->escribir(valoresClaves);
						
						// Se obtiene el resultado de la operacion
						pipe->leer(&resultado);
					
					}
				}break;
				
				case OperacionesCapas::METADATA_MODIFICACION:
				{
					// Codigo de operacion de consulta para la Capa de Indices
					pipe->agregarParametro(OperacionesCapas::INDICES_MODIFICAR, 0); 
					// Nombre del tipo de dato a ser dado de alta (Persona/Pelicula)
					pipe->agregarParametro(parserOperaciones.getNombreTipo(), 1);
					
					// Se lanza el proceso de la Capa Indices.
					resultado = pipe->lanzar();
					
					if (resultado == ComuDatos::OK){
						// Envio de los valores de las claves de los registros
						// a modificar por el pipe.
						pipe->escribir(valoresClaves);
						
						// Se obtiene la cantidad de registros a
						// ser modificados.
						pipe->leer(&cantRegistros);
						
						for (unsigned short i = 0; i < cantRegistros; i++){
							// Se obtiene el tamano del registro original a 
							// a modificar.
							pipe->leer(&tamRegistro);
							registro = new char[tamRegistro*sizeof(char)];
							
							pipe->leer(tamRegistro*sizeof(char), registro);
							
						//dataManager.crearRegistroModificacion(registro);
						}
						
					}
					
				}break;
				
				case OperacionesCapas::METADATA_ALTA:
				{
					//TODO
				}break;
				
				default:
					resultado = ResultadosMetadata::OPERACION_INVALIDA;
			}
			
			pipe->liberarRecursos();
			
		//	errorOut << resultado; 
		}
		
		
		
		if (registro)
			delete[] registro;
		
		if (mapaValoresAtributos)
			delete mapaValoresAtributos;
		
		if (listaClaves)
			delete listaClaves;
		
		if (pipe)
			delete pipe;
	}
	
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

