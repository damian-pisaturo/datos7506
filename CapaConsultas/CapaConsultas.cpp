#include "CapaConsultas.h"


/*Imprime la ayuda del programa*/
void imprimirAyuda() {
	cout << endl << "Uso:" << endl;
	cout << "\tcapaConsultas -h" << endl;
	cout << "\tcapaConsultas -v" << endl;
	cout << "\tcapaConsultas -d <nombre_archivo>" << endl;
	cout << "\tcapaConsultas -o <nombre_archivo>" << endl;
	cout << "\tcapaConsultas -i <nombre_archivo>" << endl << endl;
	cout << "\tcapaConsultas [opciones]" << endl << endl;
	cout << "Opciones:" << endl;
	cout << "\t-h, --help\tImprime esta informacion" << endl;
	cout << "\t-v, --version\tImprime informacion de la version" << endl;
	cout << "\t-d, --definitions\tPath o archivo de definiciones" << endl;
	cout << "\t-o, --output\tPath o archivo de salida" << endl;
	cout << "\t-i, --input\tPath o archivo de operaciones" << endl << endl;
	cout << "Ejemplos:" << endl;
	cout << "\tcapaConsultas -i operaciones.sql" << endl;
	cout << "\tcapaConsultas < operaciones.sql > salida.txt" << endl << endl;
}


/*Funcion que procesa los parametros de entrada y ejecuta las
operaciones correspondientes.*/
int parsearParametros(int argc, char* argv[], string &nombreArchivoOperaciones, 
					   string &nombreArchivoSalida, string &nombreArchivoDefiniciones) {

	if ( strcmp(argv[1],"-h")==0 || strcmp(argv[1],"--help")==0 ) {
		imprimirAyuda();
		return ResultadosConsultas::FINALIZAR_PROGRAMA;
	}
	else if ( strcmp(argv[1],"-v")==0 || strcmp(argv[1],"--version")==0 ) {
		cout << endl << "Trabajo Practico Organizacion de Datos" << endl;
		cout << "Framework de Persistencia 2007" << endl << "Version 1.0" << endl << endl;
		return ResultadosConsultas::FINALIZAR_PROGRAMA;
	}
	else if (argc > 2) {
		
		if (strcmp(argv[1],"-o")==0 || strcmp(argv[1],"--output")==0) {
			nombreArchivoSalida = argv[2];
			if (argc > 4) {
				if (strcmp(argv[3],"-i")==0 || strcmp(argv[3],"--input")==0) {
					nombreArchivoOperaciones = argv[4];
					if ( (argc > 6) && (strcmp(argv[5],"-d")==0 || strcmp(argv[5],"--definitions")==0) )
						nombreArchivoDefiniciones = argv[6];
				}
				else if (strcmp(argv[3],"-d")==0 || strcmp(argv[3],"--definitions")==0) {
					nombreArchivoDefiniciones = argv[4];
					if ( (argc > 6) && (strcmp(argv[5],"-i")==0 || strcmp(argv[5],"--input")==0) )
						nombreArchivoOperaciones = argv[6];
				}
			}
		}
		else if (strcmp(argv[1],"-i")==0 || strcmp(argv[1],"--input")==0) {
			nombreArchivoOperaciones = argv[2];
			if (argc > 4) {
				if (strcmp(argv[3],"-o")==0 || strcmp(argv[3],"--output")==0) {
					nombreArchivoSalida = argv[4];
					if ( (argc > 6) && (strcmp(argv[5],"-d")==0 || strcmp(argv[5],"--definitions")==0) )
						nombreArchivoDefiniciones = argv[6];
				}
				else if (strcmp(argv[3],"-d")==0 || strcmp(argv[3],"--definitions")==0) {
					nombreArchivoDefiniciones = argv[4];
					if ( (argc > 6) && (strcmp(argv[5],"-o")==0 || strcmp(argv[5],"--output")==0) )
						nombreArchivoSalida = argv[6];
				}
			}
		}
		else if (strcmp(argv[1],"-d")==0 || strcmp(argv[1],"--definitions")==0) {
			nombreArchivoDefiniciones = argv[2];
			if (argc > 4) {
				if (strcmp(argv[3],"-i")==0 || strcmp(argv[3],"--input")==0) {
					nombreArchivoOperaciones = argv[4];
					if ( (argc > 6) && (strcmp(argv[5],"-o")==0 || strcmp(argv[5],"--output")==0) )
						nombreArchivoSalida = argv[6];
				}
				else if (strcmp(argv[3],"-o")==0 || strcmp(argv[3],"--output")==0) {
					nombreArchivoSalida = argv[4];
					if ( (argc > 6) && (strcmp(argv[5],"-i")==0 || strcmp(argv[5],"--input")==0) )
						nombreArchivoOperaciones = argv[6];
				}
			}
		}
	}
	return ResultadosConsultas::OK;
}


ComuDatos* instanciarPipe()
{
	string path = RUTA_CAPA_METADATA;
	path += NOMBRE_CAPA_METADATA;
	
	return new ComuDatos(path);
}



int main(int argc, char* argv[]) {
	
	Resultado resultado;
	int pipeResult = ResultadosConsultas::OK;
	string nombreArchivoOperaciones(""), nombreArchivoSalida(""), nombreArchivoDefiniciones("");
	
	if (argc > 1)
		pipeResult = parsearParametros(argc, argv, nombreArchivoOperaciones, nombreArchivoSalida, nombreArchivoDefiniciones);
	
	if (pipeResult == ResultadosConsultas::OK) {
		
		ComuDatos* pipe = NULL; // Instancia del pipe de comunicacion		
		
		unsigned char operacion      = 0;    // Tipo de operacion a ejecutar
		unsigned short tamRegistro   = 0; 	 // Tamano de un registro.
		unsigned short cantRegistros = 0;    // Cant. de registros que responden a la consulta dada.
		char* registro               = NULL; // Buffer contenedor de un registro. 
		string valoresClaves("");
		string nombreTipo("");
		
		// Lista de valores de todos los atributos dentro de un registro.
		DefinitionsManager::ListaValoresAtributos* listaValAtributos = NULL;
		
		// Lista de los tipos de todos los atributos dentro de un registro.
		DefinitionsManager::ListaTiposAtributos* listaTiposAtributos = NULL;
		
		// Lista de los valores de las claves y sus nombres dentro de un registro.
		DefinitionsManager::ListaClaves* listaClaves = NULL; 
		
		// Mapa contenedor de los valores de los atributos, indexados por sus nombres.
		DefinitionsManager::MapaValoresAtributos* mapaValoresAtributos = NULL; 
		
		// Se instancia el DefinitionsManager (conocedor absoluto del universo).
		DefinitionsManager& defManager = DefinitionsManager::getInstance();
		
		if (nombreArchivoDefiniciones != "") {
			// TODO Llamar al metodo del DM que parsea las definiciones
		}
		
		// Parser del archivo de operaciones.
		ParserOperaciones parserOperaciones(nombreArchivoOperaciones);
		
		// Clase de impresion de registros.
		Vista vista;
		
		// TODO Luego verificar si el codigo de retorno del parser
		// es distinto de FIN_ARCHIVO
		while (parserOperaciones.proximaOperacion()) {
		
	//		for (unsigned i = 0; i < 100; ++i) {
		
			pipe = instanciarPipe();
			operacion   = parserOperaciones.getTipoOperacion();
	//			operacion   = OperacionesCapas::METADATA_ALTA;
			listaClaves = parserOperaciones.getListaClaves();
			nombreTipo  = parserOperaciones.getNombreTipo();			
	//			nombreTipo  = "PERSONA";
			mapaValoresAtributos = parserOperaciones.getMapaValoresAtributos();
			valoresClaves.clear();
			
			switch(operacion){
				case OperacionesCapas::METADATA_CONSULTA:
				{					
					// Codigo de operacion de consulta para la Capa de Indices
					pipe->agregarParametro((unsigned char)OperacionesCapas::INDICES_CONSULTAR, 0); 
					// Nombre del tipo de dato a ser dado de alta (Persona/Pelicula)
					pipe->agregarParametro(nombreTipo, 1);
					
					// Se lanza el proceso de la Capa Indices.
					pipeResult = pipe->lanzar();
					
					if (pipeResult == ComuDatos::OK){						
						// Envio de los valores de las claves a consultar por el pipe
//						serializarListaClaves(valoresClaves, listaClaves);
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
										
										vista.showRegister(registro, listaTiposAtributos);
										
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
					// Codigo de operacion de consulta para la Capa de Indices
					pipe->agregarParametro((unsigned char)OperacionesCapas::INDICES_ELIMINAR, 0); 
					// Nombre del tipo de dato a ser dado de alta (Persona/Pelicula)
					pipe->agregarParametro(nombreTipo, 1);
					
					// Se lanza el proceso de la Capa Indices.
					pipeResult = pipe->lanzar();
					
					if (pipeResult == ComuDatos::OK){
						// Envio de los valores de las claves de los registros
						// a eliminar por el pipe.
//						serializarListaClaves(valoresClaves, listaClaves);
						pipe->escribir((unsigned short)valoresClaves.length());
						pipe->escribir(valoresClaves);
						
						// Se obtiene el resultado de la operacion
						pipe->leer(&pipeResult);
					}
				}break;
				
				case OperacionesCapas::METADATA_MODIFICACION:
				{				
					// Codigo de operacion de consulta para la Capa de Indices
					pipe->agregarParametro((unsigned char)OperacionesCapas::INDICES_MODIFICAR, 0); 
					// Nombre del tipo de dato a ser dado de alta (Persona/Pelicula)
					pipe->agregarParametro(nombreTipo, 1);
					
					// Se lanza el proceso de la Capa Indices.
					pipeResult = pipe->lanzar();
					
					if (pipeResult == ComuDatos::OK){
						// Envio de los valores de las claves de los registros
						// a modificar por el pipe.
//						serializarListaClaves(valoresClaves, listaClaves);
						pipe->escribir((unsigned short)valoresClaves.length());
						pipe->escribir(valoresClaves);
						
						// Se obtiene la cantidad de registros a ser modificados.
						pipe->leer(&cantRegistros);
						
						if (cantRegistros > 0) {
							
							listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
							listaValAtributos   = defManager.getListaValoresAtributos(nombreTipo, *mapaValoresAtributos);
							pipeResult = ResultadosIndices::OK;
							
							for (unsigned short i = 0; (i < cantRegistros) && (pipeResult == ResultadosIndices::OK);
								i++){
								
								// Se obtiene el resultado de la búsqueda de la clave
								pipe->leer(&pipeResult);
								
								if (pipeResult == ResultadosIndices::OK) {
										
									// Se obtiene el tamano del registro original a modificar.
									pipe->leer(&tamRegistro);
									registro = new char[tamRegistro];
									
									pipe->leer(tamRegistro, registro);
									
									// Crea un nuevo registro con las modificaciones pertinentes. Devuelve su nueva longitud.
//									tamRegistro = dataManager.crearRegistroModificacion(*listaTiposAtributos, *listaValAtributos, registro);
									
									// Se envia el tamano del registro modificado por el pipe.
									pipe->escribir(tamRegistro);
									
									// Se envia el nuevo registro con la modificacion.
//									pipe->escribir(dataManager.getRegistro(), tamRegistro);
									
									// Se obtiene el resultado de la modificacion.
									pipe->leer(&pipeResult);
									
									delete[] registro;
								}
							}
							
							delete listaValAtributos;
							
							registro = NULL;
							
						} else pipeResult = cantRegistros; //Se produjo un error en CapaIndices
					}
					
				}break;
				
				case OperacionesCapas::METADATA_ALTA:
				{					
//					DefinitionsManager::ListaNombresClaves *listaNombres = defManager.getListaNombresClavesPrimarias(nombreTipo);
					
					pipe->agregarParametro((unsigned char)OperacionesCapas::INDICES_INSERTAR, 0); 
					// Nombre del tipo de dato a ser dado de alta (Persona/Pelicula)
					pipe->agregarParametro(nombreTipo, 1);
					
					// Se lanza el proceso de la Capa Indices.
					pipeResult = pipe->lanzar();
	
					if (pipeResult == ComuDatos::OK){
						// Envio de los valores de las claves de los registros
						// a modificar por el pipe.
//						serializarListaClaves(valoresClaves, mapaValoresAtributos, listaNombres);
						
						//LINEAS PARA PROBAR ALTAS MASIVAS
						/*********/
	//						cout << endl << "ALTA NRO: " << i << endl;
	//						tamRegistro = dataManager.crearRegistroAltaRandom(valoresClaves, *defManager.getListaTiposAtributos(nombreTipo));
						/*********/
						
						pipe->escribir((unsigned short)valoresClaves.length());
						pipe->escribir(valoresClaves);
						
						// Se obtiene el resultado de la operacion.
						pipe->leer(&pipeResult);
						
						if (pipeResult == ResultadosIndices::CLAVE_NO_ENCONTRADA){
						
							listaValAtributos   = defManager.getListaValoresAtributos(nombreTipo, *mapaValoresAtributos);
							listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
							
							// Se crea el registro a dar de alta y se obtiene su longitud
//							tamRegistro = dataManager.crearRegistroAlta(*listaValAtributos, *listaTiposAtributos);
							
							pipe->escribir(tamRegistro);
							
							if (tamRegistro > 0) {
								
//								vista.showRegister(dataManager.getRegistro(), listaTiposAtributos);
								
								// Se envia el registro a dar de alta por el pipe.
//								pipe->escribir(dataManager.getRegistro(), tamRegistro);
								
								// Se obtiene el resultado de la insercion
								pipe->leer(&pipeResult);
								
							} else pipeResult = ResultadosMetadata::ERROR_FORMATO_FECHA;
							
							delete listaValAtributos;
							
						}
					}
					
				}break;
				
				default:
					pipeResult = ResultadosMetadata::OPERACION_INVALIDA;
			}
			
			resultado << pipeResult;
			
			cout << endl << resultado << endl;
			
			delete pipe;
			pipe = NULL;
			
			usleep(100); //Recibe microsegundos
			
		}
		
		if (registro)
			delete[] registro;
		
		if (pipe)
			delete pipe;
		
		cout << "Fin Capa Consultas" << endl;
	}
		
	return pipeResult;

	
}

