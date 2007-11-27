#include "CapaIndices.h"

/*
 * Crea todos los indices correspondientes a un tipo, y carga su mapa de indices.
 **/
void crearIndices(const string &nombreTipo, MapaIndices &mapaIndices,
				  DefinitionsManager &defManager) {
	
	Indice* indice;
	unsigned char tipoIndice;
	DefinitionsManager::EstructTipoIndice estructura;
	DefinitionsManager::NodoListaIndices nodoListaIndices;
	DefinitionsManager::ListaTiposIndices *listaTiposIndices = defManager.getListaTiposIndices(nombreTipo);
	DefinitionsManager::ListaTiposAtributos *listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
	char tipoOrg = defManager.getTipoOrgRegistro(nombreTipo);
	
	for (DefinitionsManager::ListaTiposIndices::const_iterator iter = listaTiposIndices->begin();
		iter != listaTiposIndices->end(); ++iter) {
		
		nodoListaIndices = *iter;
		estructura = nodoListaIndices.estructTipoIndice;
		tipoIndice = estructura.tipoEstructura;
	
		switch (tipoIndice) {
			
			case TipoIndices::ARBOL_BP:
			case TipoIndices::ARBOL_BS:
				indice = new IndiceArbol(estructura.tipoIndice, estructura.tipoClave,
										defManager.getListaTiposAtributos(nombreTipo),
										estructura.tipoEstructura, estructura.tamanioBloque,
										Tamanios::TAMANIO_BLOQUE_DATO, estructura.nombreArchivo,
										tipoOrg);
				break;
				
			case TipoIndices::HASH:
			
				indice = new IndiceHash(listaTiposAtributos, estructura.tamanioBloque, estructura.nombreArchivo);
				break;
		}
		
		mapaIndices[*(nodoListaIndices.listaNombresClaves)] = indice;
		
	}
	
}


void destruirIndices(MapaIndices &mapaIndices) {

	for (MapaIndices::iterator iter = mapaIndices.begin();
		iter != mapaIndices.end(); ++iter)
		delete iter->second;
}


void consultar(const string &nombreTipo, MapaIndices &mapaIndices,
			   Indice *indice, Clave *clave,
			   DefinitionsManager &defManager, ComuDatos &pipe) {
	
	unsigned short cantRegistros = 1, tamRegistro = 0;
	int resultado = 0;
	char *registroDatos = NULL;
	unsigned short cantidadBloques = 1;
	
	if (indice->getTipo() == TipoIndices::GRIEGO) {
		
		if (clave)
		{
			resultado = indice->buscar(clave, registroDatos, tamRegistro);
			pipe.escribir(resultado);
			
			// Se manda 1, para que metadata sepa que tiene que iterar solo una vez.
			pipe.escribir(cantidadBloques);
			
			if (resultado == ResultadosIndices::OK) {
				//Envío la cantidad de registros
				pipe.escribir(cantRegistros);
				pipe.escribir(tamRegistro);
				pipe.escribir(registroDatos, tamRegistro);
			}
			
		}
		else
		{
			// Se devuelven todos los registros almacenados en el indice.
			
			SetEnteros conjuntoBloques = indice->getConjuntoBloques();
			SetEnteros::iterator it;
			
			// Metadata esta esperando el resultado de la busqueda de clave. 
			// Como en este caso no se manda una clave en particular, no es necesario hacer dicha búsqueda
			// y siempre se retorna ResultadosIndices::OK.
			resultado = ResultadosIndices::OK;
			pipe.escribir(resultado);
			
			Bloque* bloque;
			unsigned short cantidadBloques = conjuntoBloques.size();
			
			// Envio la cantidad de bloques a la capa de arriba para saber la cant de iteraciones 
			pipe.escribir(cantidadBloques);
			
			for (it = conjuntoBloques.begin(); it != conjuntoBloques.end(); ++it) {
				bloque = indice->leerBloque(*it);
			
				cout << "nro bloque (del set): " << *it << endl;
				cout << "cant de reg del bloque: " << bloque->getCantidadRegistros() << endl;
				cout << "cant de bloques dentro del conj: " << conjuntoBloques.size() << endl;
				
				pipe.escribir(bloque->getCantidadRegistros());
				
				for(unsigned short i = 0; i<bloque->getCantidadRegistros(); i++){
					tamRegistro = bloque->getTamanioRegistros();
					pipe.escribir(tamRegistro);
					registroDatos = bloque->getNextRegister();					
					pipe.escribir(registroDatos, tamRegistro);
				}
			}
			delete bloque;
		}
	}
	else{ //Indice Secundario
		
		ListaClaves* listaClaves = NULL;
		resultado = indice->buscar(clave, listaClaves);
		pipe.escribir(resultado);
		
		if (resultado == ResultadosIndices::OK) {
			
			pipe.escribir(cantidadBloques);
			
			//Obtengo el índice primario
			DefinitionsManager::ListaNombresClaves* listaNombresClaves = defManager.getListaNombresClavesPrimarias(nombreTipo);
			indice = mapaIndices[*listaNombresClaves];
			
			//Envío la cantidad de registros (Un registro por cada clave primaria)
			cantRegistros = listaClaves->size();
			pipe.escribir(cantRegistros);
			
			//Consulto el registro de datos de cada clave primaria
			for (ListaClaves::iterator it = listaClaves->begin(); it != listaClaves->end(); ++it) {
				
				resultado = indice->buscar(*it, registroDatos, tamRegistro);
				
				//Si el tamaño del registro es 0, la capa de metadata sabe que se produjo un error. 
				pipe.escribir(tamRegistro);
				
				if (resultado == ResultadosIndices::OK)
					pipe.escribir(registroDatos, tamRegistro);
				else break;
			}
			
			delete listaClaves;
		}
	}
	
}


void insertar(const string &nombreTipo, MapaIndices &mapaIndices,
		   	  Indice *indice, Clave *clave,
		   	  DefinitionsManager &defManager, ComuDatos &pipe) {
	
	unsigned short tamRegistro = 0;
	char *registroDatos = NULL;
	
	int resultado = indice->buscar(clave);
	pipe.escribir(resultado);
	
	if (resultado == ResultadosIndices::CLAVE_NO_ENCONTRADA) {
		// Recibe el tamaño del registro a insertar.
		pipe.leer(&tamRegistro);
		
		if (tamRegistro == 0) return;
		
		registroDatos = new char[tamRegistro];
		
		// Recibe el registro de datos.
		pipe.leer(tamRegistro, registroDatos);
		
		resultado = indice->insertar(clave, registroDatos, tamRegistro);

		pipe.escribir(resultado);

		if (resultado == ResultadosIndices::OK) {
			//Actualizo los indices secundarios
			//Saco el índice primario para no volver a insertar.
			mapaIndices.erase(*defManager.getListaNombresClavesPrimarias(nombreTipo));
			delete indice;
			
			Clave* claveSecundaria = NULL;
			
			for (MapaIndices::iterator iter = mapaIndices.begin();
				iter != mapaIndices.end(); ++iter) {
				
				Registro registro(defManager.getTipoOrgRegistro(nombreTipo),
								  registroDatos, defManager.getListaTipos(nombreTipo),
								  defManager.getListaNombresAtributos(nombreTipo));
				
				claveSecundaria = registro.getClave(iter->first);
				indice = iter->second;
				
				indice->insertar(claveSecundaria, clave);
				
				delete claveSecundaria;
				
			}
		}
		
		delete[] registroDatos;
		
	}
	
}


int eliminarClavePrimaria(const string &nombreTipo, MapaIndices &mapaIndices,
 	  	  				   Indice *indicePrimario, Clave *clave,
 	  	  				   DefinitionsManager &defManager) {
	
	int resultado = 0;
	char* registroDatos = NULL;
	unsigned short tamRegistro;
	
	//Antes de eliminar la clave primaria obtengo su registro de datos
	//para luego armar las claves secundarias.
	resultado = indicePrimario->buscar(clave, registroDatos, tamRegistro);
	
	if (resultado == ResultadosIndices::OK) {
		
		//Elimino la clave primaria
		resultado = indicePrimario->eliminar(clave);
		
		if (resultado == ResultadosIndices::OK) {
			
			//Actualizo los indices secundarios
			Indice* indiceSecundario = NULL;
			Clave* claveSecundaria = NULL;
			
			//Se instancia un objeto Registro, el cual es el encargado de
			//administrar el registro de los datos y de proporcionar las claves
			//secundarias creadas con los valores correspondientes.
			Registro registro(defManager.getTipoOrgRegistro(nombreTipo),
							  registroDatos, defManager.getListaTipos(nombreTipo),
							  defManager.getListaNombresAtributos(nombreTipo));
			
			for (MapaIndices::iterator iter = mapaIndices.begin();
				iter != mapaIndices.end(); ++iter) {
				
				//Obtengo la clave secundaria
				claveSecundaria = registro.getClave(iter->first);
				//Obtengo el índice secundario
				indiceSecundario = iter->second;
				
				indiceSecundario->eliminar(claveSecundaria, clave);
				
				delete claveSecundaria;
				
			}
			
		}
		
		delete[] registroDatos;
		
	}
	
	return resultado;
	
}


void eliminar(const string &nombreTipo, MapaIndices &mapaIndices,
	   	  	  Indice *indice, Clave *clave,
	   	  	  DefinitionsManager &defManager, ComuDatos &pipe) {
	
	int resultado = ResultadosIndices::OK;
	
	if (indice->getTipo() == TipoIndices::GRIEGO) {
		
		//Saco el índice primario para no volver a eliminar.
		mapaIndices.erase(*defManager.getListaNombresClavesPrimarias(nombreTipo));
		//Elimino la clave primaria y actualizo los índices secundarios.
		resultado = eliminarClavePrimaria(nombreTipo, mapaIndices, indice, clave, defManager);
		//Elimino el índice primario que saqué del mapa
		delete indice;
		
	} else { //Indice Secundario
		
		ListaClaves* listaClavesPrimarias = NULL;
		int resultado = indice->buscar(clave, listaClavesPrimarias);
		
		if (resultado == ResultadosIndices::OK) {
			
			ListaClaves::iterator iterClaves = listaClavesPrimarias->begin();
			
			//Obtengo el índice primario
			MapaIndices::iterator it = mapaIndices.find(*defManager.getListaNombresClavesPrimarias(nombreTipo));
			Indice* indicePrimario = it->second;
			//Saco el índice primario del mapa para que no sea encontrado cuando se busquen los
			//índices secundarios en 'eliminarClavePrimaria'
			mapaIndices.erase(it);
			
			//Elimino todas las claves primarias vinculadas a esta clave secundaria
			for (unsigned i = 0; i < listaClavesPrimarias->size(); ++i, ++iterClaves)
				resultado = eliminarClavePrimaria(nombreTipo, mapaIndices, indicePrimario, *iterClaves, defManager);
			
			delete indicePrimario;
			
		}
		
		
	}
	
	//Envío el resultado a la capa de metadata
	pipe.escribir(resultado);
	
}

void modificar(const string &nombreTipo, MapaIndices &mapaIndices,
 	  	  	   Indice *indice, Clave *clave,
 	  	  	   DefinitionsManager &defManager, ComuDatos &pipe) {
	
	int resultado = ResultadosIndices::OK;
	unsigned short tamRegistro = 0;
	char *registroDatos = NULL;
	
	if (indice->getTipo() == TipoIndices::GRIEGO) {
		// Busca el registro viejo.
		resultado = indice->buscar(clave, registroDatos, tamRegistro);
		pipe.escribir(resultado);
		if (resultado == ResultadosIndices::OK) {
			// Se envía el tamaño del registro a modificar.
			pipe.escribir(tamRegistro);
			//	Se envía el bloque antiguo
			pipe.escribir(registroDatos, tamRegistro);
			
			// Se recibe el tamaño del registro modificado.
			pipe.leer(&tamRegistro);
			//Se recibe el bloque con los atributos modificados
			pipe.leer(tamRegistro, registroDatos);
	
			//TODO Pedirle la clave nueva a la Clase Registro de Nico.
			Clave* claveNueva;
			resultado = indice->modificar(clave, claveNueva, registroDatos, tamRegistro);
			pipe.escribir(resultado);
		}
	} else {
		
		//TODO Obtener la lista de claves primarias y modificar todos los registros
		//correspondientes a cada clave de la lista.
		
	}
	
	if (resultado == ResultadosIndices::OK) {
		//TODO Actualizar indices secundarios
	}
	
}


int procesarOperacion(unsigned char codOp, const string &nombreTipo, ComuDatos &pipe) {
	int resultado = ResultadosIndices::OK;
	string buffer(""), auxStr("");
	unsigned short tamanioBuffer = 0;
	DefinitionsManager& defManager = DefinitionsManager::getInstance();
	DefinitionsManager::ListaNombresClaves listaNombresClaves;
	DefinitionsManager::ListaValoresClaves listaValoresClaves;
	string::size_type posAnterior = 0, posActual = 0, posSeparador = 0;
	
	MapaIndices mapaIndices;
	Clave *clave = NULL;
	Indice* indice = NULL;
	
	//Si la operación es una inserción, recibo la clave o la lista de claves del índice primario
	
	//Leo el tamanio del buffer a recibir
	pipe.leer(&tamanioBuffer);
	
	//Leo el buffer con los nombres y los valores de cada campo de la variable
	pipe.leer(tamanioBuffer, buffer);
	
	//Se crean los indices correspondientes al tipo 'nombreTipo'
	crearIndices(nombreTipo, mapaIndices, defManager);
	
	if (buffer.size() > 3) {
		
		if (buffer[0] == COD_CONSULTAR_TODO)
			indice = mapaIndices[*defManager.getListaNombresClavesPrimarias(nombreTipo)];
		else {
			//Se parsea el buffer obteniendo los nombres de la claves y sus valores correspondientes (NOMBRE_CLAVE=VALOR_CLAVE)
			posActual = buffer.find(CodigosPipe::COD_FIN_CLAVE, posAnterior);
			while ( (posActual != string::npos) ) {
				auxStr = buffer.substr(posAnterior, posActual - posAnterior); //En auxStr tengo NOMBRE_CLAVE=VALOR_CLAVE
				posSeparador = auxStr.find(SEPARADOR);
				listaNombresClaves.push_back(auxStr.substr(0, posSeparador));
				listaValoresClaves.push_back(auxStr.substr(posSeparador+1));
				posAnterior = posActual + 1;
				posActual = buffer.find(CodigosPipe::COD_FIN_CLAVE, posAnterior);
			}
			
			indice = mapaIndices[listaNombresClaves];
			clave = ClaveFactory::getInstance().getClave(listaValoresClaves, *(defManager.getListaTiposClaves(nombreTipo, listaNombresClaves)));

		}
		
	} else {
		
		resultado = ResultadosIndices::ERROR_VALORES_CLAVES;
		pipe.escribir(resultado);
		return resultado;
		
	}
	
	switch(codOp) {
		case OperacionesCapas::INDICES_CONSULTAR:
			// Si la consulta se hace a un índice primario, se envía el registro
			// pedido, sino se envían todos los registros correspondiente a las
			// claves primarias de la lista del indice secundario.			
			consultar(nombreTipo, mapaIndices, indice, clave, defManager, pipe);
			break;
		case OperacionesCapas::INDICES_INSERTAR:
			// Siempre se inserta un registro a partir de su clave primaria,
			// y luego se actualizan los índices secundarios.
			insertar(nombreTipo, mapaIndices, indice, clave, defManager, pipe);
			break;
		case OperacionesCapas::INDICES_ELIMINAR:
			// Si la eliminación se hace a partir de una clave primaria, se elimina
			// dicha clave y su correspondiente registro de datos, y se actualizan
			// todos los indices secundarios.
			// Si la eliminación es a partir de una clave secundaria, se obtienen
			// todas las claves primarias y se lleva a cabo la eliminación para cada
			// una de ellas.
			eliminar(nombreTipo, mapaIndices, indice, clave, defManager, pipe);
			break;
		case OperacionesCapas::INDICES_MODIFICAR:
			// Si la modificación se hace a partir de una clave primaria, se modifica
			// dicha clave y su correspondiente registro de datos, y se actualizan
			// todos los indices secundarios.
			// Si la modificación es a partir de una clave secundaria, se obtienen
			// todas las claves primarias y se lleva a cabo la modificación para cada
			// una de ellas.
			modificar(nombreTipo, mapaIndices, indice, clave, defManager, pipe);
			break;
		default:
			//No se reconoce la operación
			resultado = ResultadosIndices::OPERACION_INVALIDA;
			pipe.escribir(resultado);
	}
	
	destruirIndices(mapaIndices);
	
	return resultado;
	
}


int main(int argc, char* argv[]) {
	
	ComuDatos pipe(argv);
	unsigned char codOp = 0;
	string nombreTipo;
	
	pipe.parametro(0, codOp);
	pipe.parametro(1, nombreTipo);
	
	procesarOperacion(codOp, nombreTipo, pipe);

	// MÉTODOS DE PRUEBA PARA UN ÁRBOL B+
	/*
		DefinitionsManager::ListaTiposAtributos* lista = DefinitionsManager::getInstance().getListaTiposAtributos("PERSONA");
		IndiceArbol indice(TipoIndices::GRIEGO, TipoDatos::TIPO_ENTERO, lista,
						   TipoIndices::ARBOL_BS, 48, 48, "locura",
						   TipoDatos::TIPO_VARIABLE);
		
		char* null = NULL;*/

	/*	
		indice.insertar(new ClaveVariable("hola"), null);
		indice.insertar(new ClaveVariable("chau"), null);
		indice.insertar(new ClaveVariable("mundo"), null);
		indice.insertar(new ClaveVariable("aleee"), null);
		indice.insertar(new ClaveVariable("nico"), null);
		indice.insertar(new ClaveVariable("manu"), null);
		indice.insertar(new ClaveVariable("moe"), null);
		indice.insertar(new ClaveVariable("lau"), null);
		indice.insertar(new ClaveVariable("tifi"), null);
		indice.insertar(new ClaveVariable("yo"), null);
		indice.insertar(new ClaveVariable("hola mundo"), null);
	*/
	/*	
		stringstream texto;	
		for (unsigned i = 0; i < 300; ++i) {
			texto << "hola" << i;
			cout << "inserto: " << texto.str() << endl;
			indice.insertar(new ClaveVariable(texto.str()), null);
			texto.str("");
		}
	*/
	/*	
		ClaveVariable clave("lau");
		indice.eliminar(&clave);
		
		indice.insertar(new ClaveVariable("gollum"), null);
		
		ClaveVariable clave2("chau");
		indice.eliminar(&clave2);
	*/

		
		//Inserto en la raiz:
	/*	indice.insertar(new ClaveEntera(20), null);
		indice.insertar(new ClaveEntera(90), null);
		indice.insertar(new ClaveEntera(49), null);
		indice.insertar(new ClaveEntera(33), null);
		indice.insertar(new ClaveEntera(57), null);
		indice.insertar(new ClaveEntera(30), null);
		indice.insertar(new ClaveEntera(17), null);
		indice.insertar(new ClaveEntera(8), null);
		
		//Splittea la raiz:
		indice.insertar(new ClaveEntera(10), null);

		//Alta común:
		indice.insertar(new ClaveEntera(1), null);
		
		//Alta con redistribución
		indice.insertar(new ClaveEntera(7), null);
		
		//Produce split:
		indice.insertar(new ClaveEntera(86), null);

		//Altas comunes:
		indice.insertar(new ClaveEntera(6), null);
		indice.insertar(new ClaveEntera(19), null);
		indice.insertar(new ClaveEntera(60), null);
		indice.insertar(new ClaveEntera(88), null);
		indice.insertar(new ClaveEntera(97), null);
		
		//Produce split:
		indice.insertar(new ClaveEntera(14), null);
		
		//Lleno la raiz:
		indice.insertar(new ClaveEntera(2), null);
		indice.insertar(new ClaveEntera(3), null);
		indice.insertar(new ClaveEntera(4), null);
		indice.insertar(new ClaveEntera(5), null);
		indice.insertar(new ClaveEntera(9), null);
		indice.insertar(new ClaveEntera(11), null);
		indice.insertar(new ClaveEntera(12), null);
		indice.insertar(new ClaveEntera(13), null);
		indice.insertar(new ClaveEntera(15), null);
		indice.insertar(new ClaveEntera(16), null);
		indice.insertar(new ClaveEntera(18), null);
		indice.insertar(new ClaveEntera(21), null);
		indice.insertar(new ClaveEntera(22), null);
		indice.insertar(new ClaveEntera(23), null);
		indice.insertar(new ClaveEntera(100), null);
		indice.insertar(new ClaveEntera(121), null);
		indice.insertar(new ClaveEntera(93), null);
		
		//Splitteo la raiz:
		indice.insertar(new ClaveEntera(28), null);
		
		//Baja común:
		indice.eliminar(new ClaveEntera(11));
		
		//Baja con redistribución:
		indice.eliminar(new ClaveEntera(7));
		
		//Baja en un nodo interno:
		indice.eliminar(new ClaveEntera(88));

		//Baja en la raiz que ademas produce merge en un nodo interno y en la raiz:
		indice.eliminar(new ClaveEntera(23));

		//Alta común
		indice.insertar(new ClaveEntera(25), null);

		//Bajas comunes
		indice.eliminar(new ClaveEntera(57));
		indice.eliminar(new ClaveEntera(90));
		
		//Baja con redistribucion del hermano del hermano
		indice.eliminar(new ClaveEntera(97));
		
		//Baja en la raiz
		indice.eliminar(new ClaveEntera(18));
		
		//Baja en hoja
		indice.eliminar(new ClaveEntera(16));
		
		//Baja con mergeSplitUnderflow
		indice.eliminar(new ClaveEntera(22));

		//Vacio el arbol
		indice.eliminar(new ClaveEntera(6));
		indice.eliminar(new ClaveEntera(14));
		indice.eliminar(new ClaveEntera(15));
		indice.eliminar(new ClaveEntera(13));
		indice.eliminar(new ClaveEntera(19));
		indice.eliminar(new ClaveEntera(25));
		indice.eliminar(new ClaveEntera(93));
		indice.eliminar(new ClaveEntera(100));
		indice.eliminar(new ClaveEntera(121));
		indice.eliminar(new ClaveEntera(60));
		indice.eliminar(new ClaveEntera(33));
		indice.eliminar(new ClaveEntera(49));
		indice.eliminar(new ClaveEntera(30));
		indice.eliminar(new ClaveEntera(28));
		indice.eliminar(new ClaveEntera(20));
		indice.eliminar(new ClaveEntera(1));
		indice.eliminar(new ClaveEntera(2));
		indice.eliminar(new ClaveEntera(5));
		indice.eliminar(new ClaveEntera(10));
		indice.eliminar(new ClaveEntera(3));
		indice.eliminar(new ClaveEntera(8));
		indice.eliminar(new ClaveEntera(9));
		indice.eliminar(new ClaveEntera(12));
		indice.eliminar(new ClaveEntera(4));
		indice.eliminar(new ClaveEntera(86));
		indice.eliminar(new ClaveEntera(17)); 
		indice.eliminar(new ClaveEntera(21));

		//Inserto en la raiz
		indice.insertar(new ClaveEntera(125), null); */
	/*
		
		Clave* clave = NULL;
		int j = 324;
		stringstream s;
		
		for (int i = 0; i < 3000; i++){
			if ( (i > 100) && (i < 500) )
				j += 230;
			else if ( (i >500) && (i < 1000) ) 
				j-= 33;
			else if ( (i >1000) && (i < 1500) )
				j += 88;
			else if ( (i >1500) && (i < 2000) )
				j -= 9;
			else if ( (i >2000) && (i < 3000) )
				j += 123;
			else j-= 992;
			
			s << (300 - i + j);
			cout << "insertando clave " << s.str();
			cout << endl;
			clave = new ClaveVariable(s.str());//ClaveEntera(300 - i + j);
			indice.insertar(clave, null);
			s.str("");
		}

		for (int i = 0; i < 300; i++){
			cout << "eliminando clave " << (300 - i) << endl;
			clave = new ClaveEntera(300 - i);
			indice.eliminar(clave); 
			delete clave;
		} 
		
		//Come get some!
		indice.eliminar(NULL);
		
		clave = new ClaveEntera(-999);
		indice.eliminar(clave);
		delete clave;
		
		clave = new ClaveEntera(400);
		indice.insertar(clave, null);
		*/

	/*	
		DefinitionsManager::ListaTiposAtributos* lista = DefinitionsManager::getInstance().getListaTiposAtributos("PERSONA");
		IndiceArbol indice(TipoIndices::GRIEGO, TipoDatos::TIPO_STRING, lista,
						   TipoIndices::ARBOL_BS, 96, 96, "locura",
						   TipoDatos::TIPO_VARIABLE);
		
		char* null = NULL;
		*/
		//Inserto en la raiz
	/*	indice.insertar(new ClaveVariable("Manu"), null);
		indice.insertar(new ClaveVariable("Coloquio"), null);
		indice.insertar(new ClaveVariable("Rasputin"), null);
		indice.insertar(new ClaveVariable("San"), null);
		indice.insertar(new ClaveVariable("Lorenzo"), null);
		
		//Splittea la raiz
		indice.insertar(new ClaveVariable("Holas"), null);
		
		//Alta común
		indice.insertar(new ClaveVariable("Je"), null);
		
		//Alta con redistribución
		indice.insertar(new ClaveVariable("Cantamontes"), null);
		
		//Produce split
		indice.insertar(new ClaveVariable("Certant"), null);

		//Produce redistribución
		indice.insertar(new ClaveVariable("Otorinolaringologo"), null);

		//Lleno la raiz
		indice.insertar(new ClaveVariable("Aries"), null);
		indice.insertar(new ClaveVariable("Tauro"), null);
		indice.insertar(new ClaveVariable("Geminis"), null);
		indice.insertar(new ClaveVariable("Cancer"), null);
		indice.insertar(new ClaveVariable("Leo"), null);
		indice.insertar(new ClaveVariable("Virgo"), null);
		indice.insertar(new ClaveVariable("Libra"), null);
		indice.insertar(new ClaveVariable("Escorpio"), null);
		
		//Split en hoja y raiz
		indice.insertar(new ClaveVariable("Sagitario"), null);
		
		//Elimino todo
		
		//Baja con redistribución
		indice.eliminar(new ClaveVariable("Cantamontes"));
		
		indice.eliminar(new ClaveVariable("Escorpio"));
		indice.eliminar(new ClaveVariable("Geminis"));
		indice.eliminar(new ClaveVariable("Otorinolaringologo"));
		indice.eliminar(new ClaveVariable("Aries"));
		indice.eliminar(new ClaveVariable("Certant"));
		indice.eliminar(new ClaveVariable("Je"));
		indice.eliminar(new ClaveVariable("Tauro"));
		indice.eliminar(new ClaveVariable("Coloquio"));
		indice.eliminar(new ClaveVariable("Lorenzo"));
		indice.eliminar(new ClaveVariable("Libra"));
		indice.eliminar(new ClaveVariable("Holas"));
		indice.eliminar(new ClaveVariable("Manu"));
		indice.eliminar(new ClaveVariable("San"));
		indice.eliminar(new ClaveVariable("Virgo"));
		indice.eliminar(new ClaveVariable("Rasputin"));
		indice.eliminar(new ClaveVariable("Cancer"));
		indice.eliminar(new ClaveVariable("Leo"));
		indice.eliminar(new ClaveVariable("Sagitario"));
		*/
/*		stringstream s;
		for (int i=0; i < 2000; i++){
			s << (10000-i);
			cout << "Insertando " << s.str() << "..." << endl;
			indice.insertar(new ClaveVariable(s.str()), null);
			s.str("");
		}
		
	
		Clave* clave;
		for (int i=0; i < 2000; i++){
			s << (10000-i);
			clave = new ClaveVariable(s.str());
			cout << "Eliminando " << s.str() << "..." << endl;
			indice.eliminar(clave);
			s.str("");
			delete clave;
		}
		*/
		
	cout << "Fin Capa Indices" << endl;
}

