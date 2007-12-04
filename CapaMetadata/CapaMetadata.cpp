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

void armarListaClaves(DefinitionsManager::EstructuraCampos &estructuraCampos, DefinitionsManager::ListaClaves &listaClaves)
{
	DefinitionsManager::ListaCampos::iterator iter;
	DefinitionsManager::NodoListaClaves nodoListaClaves;
	
	for(iter = estructuraCampos.listaCampos.begin(); iter != estructuraCampos.listaCampos.end(); ++iter) {
		nodoListaClaves.nombreClave = iter->nombreCampo;
		nodoListaClaves.valorClave = iter->valorCampo;
		
		listaClaves.push_back(nodoListaClaves);
	}
	
}


int baja(string nombreTipo,  DefinitionsManager::EstructuraCampos &estructuraCampos)
{
	//TODO: FALTA VER EL TEMA DE LAS RESTRICCIONES!!!
	ComuDatos * pipe = instanciarPipe();
		
	// Codigo de operacion de consulta para la Capa de Indices
	pipe->agregarParametro((unsigned char)OperacionesCapas::INDICES_ELIMINAR, 0); 
	// Nombre del tipo de dato a ser dado de alta (Persona/Pelicula)
	pipe->agregarParametro(nombreTipo, 1);
	
	// Se lanza el proceso de la Capa Indices.
	int pipeResult = pipe->lanzar();
	
	if (pipeResult == ComuDatos::OK){
		
		// Envio de los valores de las claves de los registros
		// a eliminar por el pipe.
		
		string valoresClaves;
		DefinitionsManager::ListaClaves listaClaves;
		
		armarListaClaves(estructuraCampos, listaClaves);
		
		serializarListaClaves(valoresClaves,&listaClaves);
		pipe->escribir((unsigned short)valoresClaves.length());
		pipe->escribir(valoresClaves);
		
		// Se obtiene el resultado de la operacion
		pipe->leer(&pipeResult);
		
	}
	
	delete pipe;
	
	return pipeResult; 
}

int modificacion(string nombreTipo,DefinitionsManager::MapaValoresAtributos &mapaValoresAtributos, 
				 DefinitionsManager::EstructuraCampos &estructuraCampos)
{
	ComuDatos *pipe = instanciarPipe();
	
	// Codigo de operacion de consulta para la Capa de Indices
	pipe->agregarParametro((unsigned char)OperacionesCapas::INDICES_MODIFICAR, 0); 
	// Nombre del tipo de dato a ser dado de alta (Persona/Pelicula)
	pipe->agregarParametro(nombreTipo, 1);
	
	// Se lanza el proceso de la Capa Indices.
	int pipeResult = pipe->lanzar();
	
	if (pipeResult == ComuDatos::OK){
	
		unsigned short cantRegistros;
		// Envio de los valores de las claves de los registros
		// a modificar por el pipe.
		string valoresClaves;
		DefinitionsManager::ListaClaves listaClaves;
		
		armarListaClaves(estructuraCampos, listaClaves);
		
		serializarListaClaves(valoresClaves,&listaClaves);
		pipe->escribir((unsigned short)valoresClaves.length());
		pipe->escribir(valoresClaves);
		
		// Se obtiene la cantidad de registros a ser modificados.
		pipe->leer(&cantRegistros);
		
		if (cantRegistros > 0) {
			
			DefinitionsManager& defManager = DefinitionsManager::getInstance();
	
			// Lista de los tipos de todos los atributos dentro de un registro.
			DefinitionsManager::ListaTiposAtributos* listaTiposAtributos = NULL;
	
			// Lista de valores de todos los atributos dentro de un registro.
			DefinitionsManager::ListaValoresAtributos* listaValAtributos = NULL;
					
			unsigned short tamRegistro = 0;
			char* registro = NULL;
			
			// Controlador de registros de datos
			DataManager dataManager;
			
			listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
			listaValAtributos   = defManager.getListaValoresAtributos(nombreTipo, mapaValoresAtributos);
			pipeResult = ResultadosIndices::OK;
			
			for (unsigned short i = 0; (i < cantRegistros) && (pipeResult == ResultadosIndices::OK); i++){
				
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
			delete listaValAtributos;
			}
			
			registro = NULL;
			
		} else pipeResult = cantRegistros; //Se produjo un error en CapaIndices
	}
	
	return pipeResult;
}

int alta(string nombreTipo, DefinitionsManager::MapaValoresAtributos &mapaValoresAtributos)
{
	ComuDatos *pipe = instanciarPipe();

	// Se instancia el DefinitionsManager (conocedor absoluto del universo).
	DefinitionsManager& defManager = DefinitionsManager::getInstance();
	
	DefinitionsManager::ListaNombresClaves *listaNombres = defManager.getListaNombresClavesPrimarias(nombreTipo);
	
	pipe->agregarParametro((unsigned char)OperacionesCapas::INDICES_INSERTAR, 0); 
	// Nombre del tipo de dato a ser dado de alta (Persona/Pelicula)
	pipe->agregarParametro(nombreTipo, 1);
	
	// Se lanza el proceso de la Capa Indices.
	int pipeResult = pipe->lanzar();

	if (pipeResult == ComuDatos::OK){
		// Envio de los valores de las claves de los registros
		// a modificar por el pipe.
		string valoresClaves;		
		
		serializarListaClaves(valoresClaves, &mapaValoresAtributos, listaNombres);
		
		pipe->escribir((unsigned short)valoresClaves.length());
		pipe->escribir(valoresClaves);
		
		// Se obtiene el resultado de la operacion.
		pipe->leer(&pipeResult);
		
		if (pipeResult == ResultadosIndices::CLAVE_NO_ENCONTRADA){
	
			// Lista de valores de todos los atributos dentro de un registro.
			DefinitionsManager::ListaValoresAtributos* listaValAtributos = NULL;
			
			// Lista de los tipos de todos los atributos dentro de un registro.
			DefinitionsManager::ListaTiposAtributos* listaTiposAtributos = NULL;
	
			// Controlador de registros de datos
			DataManager dataManager;
			
			listaValAtributos   = defManager.getListaValoresAtributos(nombreTipo, mapaValoresAtributos);
			listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
			
			// Se crea el registro a dar de alta y se obtiene su longitud
			unsigned short tamRegistro = dataManager.crearRegistroAlta(*listaValAtributos, *listaTiposAtributos);
			
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

	return pipeResult;
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
		
		
		// Lista de los tipos de todos los atributos dentro de un registro.
		DefinitionsManager::ListaTiposAtributos* listaTiposAtributos = NULL;
		
		// Lista de los valores de las claves y sus nombres dentro de un registro.
		DefinitionsManager::ListaClaves* listaClaves = NULL; 
		
		// Se instancia el DefinitionsManager (conocedor absoluto del universo).
		DefinitionsManager& defManager = DefinitionsManager::getInstance();
		
		// Controlador de registros de datos
		DataManager dataManager;
		
		//pipe = instanciarPipe();
		pipe->parametro(0,operacion);
	
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
				unsigned int cantidadElementos;
				unsigned int longitudCadena;
				
				// Obtiene el nombre del tipo sobre el cual se hace la baja.
				pipe->parametro(1,nombreTipo);
				
				DefinitionsManager::EstructuraCampos estructuraCampos;
				
				pipe->leer(&estructuraCampos.operacion);
	
				pipe->leer(&cantidadElementos);
				
				DefinitionsManager::NodoListaCampos nodoListaCampos;
				
				// Se itera para recibir cada nodo de la lista de campos.
				for (unsigned int i = 0; i < cantidadElementos; ++i) {
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nodoListaCampos.nombreCampo);
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nodoListaCampos.valorCampo);
					pipe->leer(&nodoListaCampos.operacion);
					
					estructuraCampos.listaCampos.push_back(nodoListaCampos);
				}
				
				int resultadoBaja = baja(nombreTipo, estructuraCampos);
				
				pipe->escribir(resultadoBaja);
				
			}break;
			
			case OperacionesCapas::METADATA_MODIFICACION:
			{		
				unsigned int cantidadElementos;
				unsigned int longitudCadena;
				string nombreAtributo, valorAtributo;
				pipe->parametro(1, nombreTipo);
				
				// Se recibe el mapaValoresAtributos.
				DefinitionsManager::MapaValoresAtributos mapaValoresAtributos;
				
				pipe->leer(&cantidadElementos);
				
				for (unsigned int i = 0; i < cantidadElementos; ++i) {
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nombreAtributo);
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, valorAtributo);
					
					mapaValoresAtributos[nombreAtributo] = valorAtributo;
				}
				
				// Se recibe la información sobre los registros a modificar,
				// dentro de una estructuraCampos.
				DefinitionsManager::EstructuraCampos estructuraCampos;
				
				pipe->leer(&estructuraCampos.operacion);
				
				// Se obtiene la cantidad de elementos en la lista de campos.
				pipe->leer(&cantidadElementos);
				
				DefinitionsManager::NodoListaCampos nodoListaCampos;
				
				for (unsigned int i = 0; i < cantidadElementos; ++i) {
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nodoListaCampos.nombreCampo);
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nodoListaCampos.valorCampo);
					pipe->leer(&nodoListaCampos.operacion);
					
					estructuraCampos.listaCampos.push_back(nodoListaCampos);
				}

				int resultadoModificacion = modificacion(nombreTipo, mapaValoresAtributos, estructuraCampos);
				
				pipe->escribir(resultadoModificacion);
				
			}break;
			
			case OperacionesCapas::METADATA_ALTA:
			{	
				unsigned int cantidadElementos;
				unsigned int longitudCadena;
				string nombreAtributo, valorAtributo;
				pipe->parametro(1,nombreTipo);

				// Se recibe el mapaValoresAtributos.
				DefinitionsManager::MapaValoresAtributos mapaValoresAtributos;
				
				pipe->leer(&cantidadElementos);
				
				for (unsigned int i = 0; i < cantidadElementos; ++i) {
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, nombreAtributo);
					pipe->leer(&longitudCadena);
					pipe->leer(longitudCadena, valorAtributo);
					
					mapaValoresAtributos[nombreAtributo] = valorAtributo;
				}

				int resultadoAlta = alta (nombreTipo, mapaValoresAtributos);
				
				pipe->escribir(resultadoAlta);
									
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

