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

void serializarListaClaves(string& s, DefinitionsManager::MapaValoresAtributos* mapaValores, 
		DefinitionsManager::ListaNombresClaves* listaNombres)
{
	DefinitionsManager::ListaNombresClaves::const_iterator iter;
	
	for (iter = listaNombres->begin(); iter != listaNombres->end(); ++iter){
		s += *iter; 
		s += "=";
		s += (*mapaValores)[*iter];
		s += CodigosPipe::COD_FIN_CLAVE;
	}
}

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
	Resultado resultado;
	int pipeResult = ResultadosIndices::OK;
	
	if (argc > 1){
		
		ComuDatos* pipe = new ComuDatos(argv); //Instancia del pipe de comunicacion		
		
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
		
		// Controlador de registros de datos
		DataManager dataManager;
		
		//pipe = instanciarPipe();
		pipe->parametro(0,operacion);

//			listaClaves = parserOperaciones.getListaClaves();
		
//		nombreTipo  = parserOperaciones.getNombreTipo();			
////			nombreTipo  = "PERSONA";
//			mapaValoresAtributos = parserOperaciones.getMapaValoresAtributos();
//			valoresClaves.clear();
			
			switch(operacion){
				case OperacionesCapas::METADATA_CONSULTA:
				{	
					DefinitionsManager::EstructuraConsulta estructuraConsulta;
					
					// Se recibe el contenido de estructuraConsulta, pero sin incluir la 
					// lista de campos seleccionados.
					
					// Se recibe la listaNombresTipos.
					unsigned int cantidadElementos;
					pipe->leer(&cantidadElementos);
					
					unsigned int longitudCadena;
					string cadena;
					
					for (unsigned int i = 0; i<cantidadElementos; i++) {
						pipe->leer(&longitudCadena);
						pipe->leer(longitudCadena, cadena);
						estructuraConsulta.listaNombresTipos.push_back(cadena);
					}
					
					// Se recibe la estructuraJoins
					pipe->leer(&estructuraConsulta.estructuraJoins.operacion);
					pipe->leer(&cantidadElementos);
					
					DefinitionsManager::NodoListaOperaciones nodoListaOperaciones;
					// Se itera para obtener cada nodo de la lista de operaciones.
					for (unsigned int i=0; i<cantidadElementos; i++) {
						pipe->leer(&longitudCadena);
						pipe->leer(longitudCadena, nodoListaOperaciones.estructuraNombresIzq.nombreTipo);
						pipe->leer(&longitudCadena);
						pipe->leer(longitudCadena, nodoListaOperaciones.estructuraNombresIzq.nombreCampo);
						
						pipe->leer(&longitudCadena);
						pipe->leer(longitudCadena, nodoListaOperaciones.estructuraNombresDer.nombreTipo);
						pipe->leer(&longitudCadena);
						pipe->leer(longitudCadena, nodoListaOperaciones.estructuraNombresDer.nombreCampo);
						
						pipe->leer(&nodoListaOperaciones.operacion);
						
						estructuraConsulta.estructuraJoins.listaOperaciones.push_back(nodoListaOperaciones);
					}
					
					// Se recibe la estructuraWhere
					pipe->leer(&estructuraConsulta.estructuraWhere.operacion);
					pipe->leer(&cantidadElementos);
					
					for (unsigned int i = 0; i < cantidadElementos; ++i) {
						pipe->leer(&longitudCadena);
						pipe->leer(longitudCadena, nodoListaOperaciones.estructuraNombresIzq.nombreTipo);
						pipe->leer(&longitudCadena);
						pipe->leer(longitudCadena, nodoListaOperaciones.estructuraNombresIzq.nombreCampo);
						
						pipe->leer(&longitudCadena);
						pipe->leer(longitudCadena, nodoListaOperaciones.estructuraNombresDer.nombreTipo);
						pipe->leer(&longitudCadena);
						pipe->leer(longitudCadena, nodoListaOperaciones.estructuraNombresDer.nombreCampo);
						
						pipe->leer(&nodoListaOperaciones.operacion);
						
						estructuraConsulta.estructuraWhere.listaOperaciones.push_back(nodoListaOperaciones);
					}
					
					// Se recibe el orderBy
					pipe->leer(&cantidadElementos);
					
					DefinitionsManager::EstructuraNombres estructuraNombres;
					
					for (unsigned int i = 0; i < cantidadElementos; ++i) {
						pipe->leer(&longitudCadena);
						pipe->leer(longitudCadena, estructuraNombres.nombreTipo);
						pipe->leer(&longitudCadena);
						pipe->leer(longitudCadena, estructuraNombres.nombreCampo);
						
						estructuraConsulta.listaOrderBy.push_back(estructuraNombres);
					}
					
//---------------------ACA HAY Q HACER LAS CONSULTAS LLAMANDO A CAPA INDICES.----
					//TODO: ver el tema de como se comunica con capaConsultas para pasar el resultado..
					// Codigo de operacion de consulta para la Capa de Indices
					pipe->agregarParametro((unsigned char)OperacionesCapas::INDICES_CONSULTAR, 0); 
					// Nombre del tipo de dato a ser dado de alta (Persona/Pelicula)
					pipe->agregarParametro(nombreTipo, 1);
					
					// Se lanza el proceso de la Capa Indices.
					pipeResult = pipe->lanzar();
					
					if (pipeResult == ComuDatos::OK){						
						// Envio de los valores de las claves a consultar por el pipe
						serializarListaClaves(valoresClaves, listaClaves);
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
										
					//					vista.showRegister(registro, listaTiposAtributos);
										
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
					// Obtiene el nombre del tipo sobre el cual se hace la baja.
					pipe->parametro(1,nombreTipo);
					
					DefinitionsManager::EstructuraCampos estructuraCampos;
					
					pipe->leer(&estructuraCampos.operacion);
					
					
// ---------ESTA PARTE LLAMA A LA CAPA DE INDICES --------------
					// Codigo de operacion de consulta para la Capa de Indices
					pipe->agregarParametro((unsigned char)OperacionesCapas::INDICES_ELIMINAR, 0); 
					// Nombre del tipo de dato a ser dado de alta (Persona/Pelicula)
					pipe->agregarParametro(nombreTipo, 1);
					
					// Se lanza el proceso de la Capa Indices.
					pipeResult = pipe->lanzar();
					
					if (pipeResult == ComuDatos::OK){
						// Envio de los valores de las claves de los registros
						// a eliminar por el pipe.
						serializarListaClaves(valoresClaves, listaClaves);
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
						serializarListaClaves(valoresClaves, listaClaves);
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
									tamRegistro = dataManager.crearRegistroModificacion(*listaTiposAtributos, *listaValAtributos, registro);
									
									// Se envia el tamano del registro modificado por el pipe.
									pipe->escribir(tamRegistro);
									
									// Se envia el nuevo registro con la modificacion.
									pipe->escribir(dataManager.getRegistro(), tamRegistro);
									
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
					DefinitionsManager::ListaNombresClaves *listaNombres = defManager.getListaNombresClavesPrimarias(nombreTipo);
					
					pipe->agregarParametro((unsigned char)OperacionesCapas::INDICES_INSERTAR, 0); 
					// Nombre del tipo de dato a ser dado de alta (Persona/Pelicula)
					pipe->agregarParametro(nombreTipo, 1);
					
					// Se lanza el proceso de la Capa Indices.
					pipeResult = pipe->lanzar();

					if (pipeResult == ComuDatos::OK){
						// Envio de los valores de las claves de los registros
						// a modificar por el pipe.
						serializarListaClaves(valoresClaves, mapaValoresAtributos, listaNombres);
						
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
							tamRegistro = dataManager.crearRegistroAlta(*listaValAtributos, *listaTiposAtributos);
							
							pipe->escribir(tamRegistro);
							
							if (tamRegistro > 0) {
								
			//					vista.showRegister(dataManager.getRegistro(), listaTiposAtributos);
								
								// Se envia el registro a dar de alta por el pipe.
								pipe->escribir(dataManager.getRegistro(), tamRegistro);
								
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
			
	
		
		if (registro)
			delete[] registro;
		
		if (pipe)
			delete pipe;
	}
	
	cout << "Fin Capa Metadata" << endl;
	
	return pipeResult;
	
}

