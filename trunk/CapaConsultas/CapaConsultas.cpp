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


/*
 * Funcion que procesa los parametros de entrada.
 * */
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

/*
 * Instancia un pipe con la Capa Metadata
 * */
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
	ofstream archivo; // Archivo de salida en caso de ser necesario.
	
	if (argc > 1) {
		pipeResult = parsearParametros(argc, argv, nombreArchivoOperaciones, nombreArchivoSalida, nombreArchivoDefiniciones);
		
		// Si se indico un archivo de salido se redirige el buffer de cout.
		if (nombreArchivoSalida.size() != 0){ 
			archivo.open(nombreArchivoSalida.c_str());
			cout.rdbuf(archivo.rdbuf());
		}		
	}
	
	if (pipeResult == ResultadosConsultas::OK) {
		
		ComuDatos* pipe = NULL; // Instancia del pipe de comunicacion		
		
		unsigned char operacion = 0;    // Tipo de operacion a ejecutar
		string valoresClaves("");
		string nombreTipo("");
		
		// Mapa contenedor de los valores de los atributos, indexados por sus nombres.
		MapaValoresAtributos* mapaValoresAtributos = NULL; 
		
		DefinitionsManager& defManager = DefinitionsManager::getInstance();
		
		if (nombreArchivoDefiniciones != "")
			defManager.cargarArchivoMaestro(nombreArchivoDefiniciones);
		
		// Parser del archivo de operaciones.
		ParserOperaciones parserOperaciones(nombreArchivoOperaciones);
		
		// Clase de impresion de registros.
		Vista vista;
		
		int resultadoProxOp = 0;
		
		while ((resultadoProxOp = parserOperaciones.proximaOperacion()) != ResultadosParserOperaciones::FIN_ARCHIVO) {
			if (resultadoProxOp != ResultadosParserOperaciones::SINTAXIS_CORRECTA){
				resultado << resultadoProxOp;
				cout << resultado << endl;
			}
			else{
				pipe = instanciarPipe();
				operacion   = parserOperaciones.getTipoOperacion();
				nombreTipo  = parserOperaciones.getNombreTipo();			
	
				switch(operacion){
					case OperacionesCapas::CONSULTAS_CONSULTA:
					{					
						// Codigo de operacion de consulta para la Capa de Metadata
						pipe->agregarParametro((unsigned char)OperacionesCapas::METADATA_CONSULTA, 0); 
						
						// Se lanza el proceso de la Capa Indices.
						pipeResult = pipe->lanzar();
						
						if (pipeResult == ComuDatos::OK){						
							
							EstructuraConsulta estructuraConsulta;
							
							estructuraConsulta = parserOperaciones.getEstructuraConsulta();
							
							// Envia el contenido de la estructuraConsulta por el pipe.
							
							// Envia la listaNombresTipos
							unsigned int cantidadElementos = estructuraConsulta.listaNombresTipos.size();
							pipe->escribir(cantidadElementos);
							
							ListaStrings::iterator itLNT;
							
							for (itLNT = estructuraConsulta.listaNombresTipos.begin(); 
								 itLNT != estructuraConsulta.listaNombresTipos.end(); ++itLNT) {
								pipe->escribir(*itLNT);
							}
												
							// Envia la estructuraJoins
							pipe->escribir(estructuraConsulta.estructuraJoins.operacion);
							cantidadElementos = estructuraConsulta.estructuraJoins.listaOperaciones.size();
							pipe->escribir(cantidadElementos);
							
							ListaOperaciones::iterator itLOJoins;
							
							for(itLOJoins = estructuraConsulta.estructuraJoins.listaOperaciones.begin();
								itLOJoins != estructuraConsulta.estructuraJoins.listaOperaciones.end(); ++itLOJoins) {
								pipe->escribir(itLOJoins->estructuraNombresIzq.nombreTipo);
								pipe->escribir(itLOJoins->estructuraNombresIzq.nombreCampo);
								pipe->escribir(itLOJoins->estructuraNombresDer.nombreTipo);
								pipe->escribir(itLOJoins->estructuraNombresDer.nombreCampo);
								
								pipe->escribir(itLOJoins->operacion);
							}
							
							// Envia la estructuraWhere
							pipe->escribir(estructuraConsulta.estructuraWhere.operacion);
							cantidadElementos = estructuraConsulta.estructuraWhere.listaOperaciones.size();
							pipe->escribir(cantidadElementos);
							
							ListaOperaciones::iterator itLOWhere;
							
							for(itLOWhere = estructuraConsulta.estructuraWhere.listaOperaciones.begin();
								itLOWhere != estructuraConsulta.estructuraWhere.listaOperaciones.end(); ++itLOWhere) {
								pipe->escribir(itLOWhere->estructuraNombresIzq.nombreTipo);
								pipe->escribir(itLOWhere->estructuraNombresIzq.nombreCampo);
								pipe->escribir(itLOWhere->estructuraNombresDer.nombreTipo);
								pipe->escribir(itLOWhere->estructuraNombresDer.nombreCampo);
								
								pipe->escribir(itLOWhere->operacion);
							}
							
							// Envia el orderBy
							cantidadElementos = estructuraConsulta.listaOrderBy.size();
							pipe->escribir(cantidadElementos);
							
							ListaEstructuraNombres::iterator itLEN;
							
							for (itLEN = estructuraConsulta.listaOrderBy.begin();
								 itLEN != estructuraConsulta.listaOrderBy.end(); ++itLEN) {
								pipe->escribir(itLEN->nombreTipo);
								pipe->escribir(itLEN->nombreCampo);				
							}
								
							// Obtencion del resultado de la operacion
							pipe->leer(&pipeResult);
							
							if (pipeResult == ResultadosMetadata::OK) {
								
								// Obtiene la cantidad de tipos involucrados en la consulta.
								cantidadElementos = estructuraConsulta.listaNombresTipos.size(); 
								
								// Crea una lista de nombres de los campos a mostrar por cada tipo.
								ListaStrings* listasNombresCamposAMostrarPorTipo;
								listasNombresCamposAMostrarPorTipo = new ListaStrings[cantidadElementos];
	
								// Se crea un mapa por tipo y su lista de nombres de campos a mostrar.
								std::map<std::string, ListaStrings*> mapaNombresCamposAImprimir;
								unsigned int i = 0;
								for (itLNT = estructuraConsulta.listaNombresTipos.begin(); 
									 itLNT != estructuraConsulta.listaNombresTipos.end(); ++itLNT, i++) 
									mapaNombresCamposAImprimir[*itLNT] = &listasNombresCamposAMostrarPorTipo[i];
								
								
								// Se itera la lista de campos seleccionados, y se van agregando dichos campos a las 
								// listas del mapa.
								ListaStrings *lista;
								for (itLEN = estructuraConsulta.listaCamposSeleccionados.begin();
									 itLEN != estructuraConsulta.listaCamposSeleccionados.end(); ++itLEN) {
		
									lista = mapaNombresCamposAImprimir[itLEN->nombreTipo];
									lista->push_back(itLEN->nombreCampo);
								}
							// TODO: ver como se mostrarian los resultados.
							}
								
							
	//						if (pipeResult == ResultadosIndices::OK) {
	//							
	//							do {
	//								// Se obtiene la cantidad de registros que responden 
	//								// a la consulta realizada.
	//								pipe->leer(&cantRegistros); 
	//								
	//								if (cantRegistros > 0) {
	//									
	//									listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
	//									
	//									cout << "======= Resultado de la consulta =======" << endl << endl;
	//									for (unsigned short i = 0; i < cantRegistros; i++){
	//										// Se obtiene el tamaño del registro a levantar
	//										pipe->leer(&tamRegistro);
	//									
	//										if (tamRegistro == 0) {
	//											pipeResult = ResultadosIndices::ERROR_CONSULTA;
	//											break;
	//										}
	//										
	//										registro = new char[tamRegistro];
	//										
	//										// Se obtiene el registro de datos consultado.
	//										pipe->leer(tamRegistro, registro);
	//										
	//										vista.showRegister(registro, listaTiposAtributos);
	//										
	//										delete[] registro;
	//										
	//										cout << endl;
	//									}
	//									cout << "========================================" << endl;
	//									
	//									registro = NULL;
	//								}
	//								
	//								// Leo el resultado que indica si se van a recibir más bloques
	//								pipe->leer(&pipeResult);
	//								
	//							} while (pipeResult == ResultadosIndices::OK);
	//						}
						}
					}break;
					
					case OperacionesCapas::CONSULTAS_BAJA:
					{
						// Codigo de operacion de consulta para la Capa de Metadata
						pipe->agregarParametro((unsigned char)OperacionesCapas::METADATA_BAJA, 0); 
						// Nombre del tipo de dato a ser dado de baja
						pipe->agregarParametro(nombreTipo, 1);
						
						// Se lanza el proceso de la capa Metadata
						pipeResult = pipe->lanzar();
						
						if (pipeResult == ComuDatos::OK){
							
							// Se obtiene la información sobre los registros a eliminar, y se la envia por el pipe.
							EstructuraCampos estructuraCampos;
							
							estructuraCampos = parserOperaciones.getEstructuraCampos();
							
							pipe->escribir(estructuraCampos.operacion);
							
							// Obtiene la cantidad de elementos en la lista de campos y la manda por el pipe.
							unsigned int numeroElementosLista = estructuraCampos.listaCampos.size();
							pipe->escribir(numeroElementosLista); 
							
							ListaCampos::iterator iter; 
							
							// Manda cada nodo de la lista.
							for (iter = estructuraCampos.listaCampos.begin(); iter != estructuraCampos.listaCampos.end(); ++iter) {
								pipe->escribir(iter->nombreCampo);
								pipe->escribir(iter->valorCampo);
								pipe->escribir(iter->operacion);
							}
							
							// Obtiene el resultado de la operacion.
							pipe->leer(&pipeResult);
						}
					}break;
					
					case OperacionesCapas::CONSULTAS_MODIFICACION:
					{				
						// Codigo de operacion de consulta para la Capa de Metadata
						pipe->agregarParametro((unsigned char)OperacionesCapas::METADATA_MODIFICACION, 0); 
						// Nombre del tipo de dato a ser modificado
						pipe->agregarParametro(nombreTipo, 1);
						
						// Se lanza el proceso de la Capa Metadata.
						pipeResult = pipe->lanzar();
						
						if (pipeResult == ComuDatos::OK){
							
							// Se obtiene el mapa de valores atributos con los valores nuevos para los atributos
							mapaValoresAtributos =  parserOperaciones.getMapaValoresAtributos();
							
							// Se envia el contenido del mapa por el pipe.
							MapaValoresAtributos::iterator itMapa;
							unsigned int cantidadElementos = mapaValoresAtributos->size();
							pipe->escribir(cantidadElementos);
							
							for (itMapa = mapaValoresAtributos->begin(); itMapa != mapaValoresAtributos->end(); ++itMapa) {
								pipe->escribir(itMapa->first);
								pipe->escribir(itMapa->second);
							}
							
							// Se obtiene la información sobre los registros a modificar, y se la envia por el pipe.
							EstructuraCampos estructuraCampos;
			
							estructuraCampos = parserOperaciones.getEstructuraCampos();
							
							pipe->escribir(estructuraCampos.operacion);
							
							// Obtiene la cantidad de elementos en la lista de campos y la manda por el pipe.
							cantidadElementos = estructuraCampos.listaCampos.size();
							pipe->escribir(cantidadElementos); 
							
							ListaCampos::iterator iter; 
							
							// Manda cada nodo de la lista.
							for (iter = estructuraCampos.listaCampos.begin(); iter != estructuraCampos.listaCampos.end(); ++iter) {
								pipe->escribir(iter->nombreCampo);
								pipe->escribir(iter->valorCampo);
								pipe->escribir(iter->operacion);
							}
							
							// Se obtiene el resultado de la modificacion.
							pipe->leer(&pipeResult);
						}
						
					}break;
					
					case OperacionesCapas::CONSULTAS_ALTA:
					{					
						pipe->agregarParametro((unsigned char)OperacionesCapas::METADATA_ALTA, 0); 
						// Nombre del tipo de dato a ser dado de alta.
						pipe->agregarParametro(nombreTipo, 1);
						
						// Se lanza el proceso de la Capa Indices.
						pipeResult = pipe->lanzar();
		
						if (pipeResult == ComuDatos::OK){
							
							// Se obtiene el mapa de valores atributos con los valores nuevos para los atributos
							mapaValoresAtributos =  parserOperaciones.getMapaValoresAtributos();
							
							// Se envia el contenido del mapa por el pipe.
							MapaValoresAtributos::iterator itMapa;
							unsigned int cantidadElementos = mapaValoresAtributos->size();
							pipe->escribir(cantidadElementos);
							
							for (itMapa = mapaValoresAtributos->begin(); itMapa != mapaValoresAtributos->end(); ++itMapa) {
								pipe->escribir(itMapa->first);
								pipe->escribir(itMapa->second);
							}
							
							// Se obtiene el resultado de la operacion.
							pipe->leer(&pipeResult);
							
						}	
					}break;
					
					default:
						pipeResult = ResultadosMetadata::OPERACION_INVALIDA;
				}
				
				resultado << pipeResult;
				
				cout << endl << resultado << endl;
				
				delete pipe;
				pipe = NULL;
			}
				
		}
		
		if (pipe)
			delete pipe;
		
		if (archivo.is_open())
			archivo.close();
		
		cout << "Fin Capa Consultas" << endl;
	}
		
	return pipeResult;

	
}

