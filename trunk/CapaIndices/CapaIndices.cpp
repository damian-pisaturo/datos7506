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
				indice = new IndiceArbol(estructura.tipoIndice, estructura.tamanioBloque, estructura.tipoClave,
										defManager.getListaTiposAtributos(nombreTipo), estructura.tipoEstructura, estructura.tamanioBloque,
										Tamanios::TAMANIO_BLOQUE_DATO, estructura.nombreArchivo, tipoOrg);
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
	
	if (indice->getTipo() == TipoIndices::GRIEGO) {
		
		resultado = indice->buscar(clave, registroDatos, tamRegistro);
		pipe.escribir(resultado);
		
		//Envío la cantidad de registros
		pipe.escribir(cantRegistros);
		
		if (resultado == ResultadosIndices::OK) {
			pipe.escribir(tamRegistro);
			pipe.escribir(registroDatos, tamRegistro);
		}
	}
	else{
						
		resultado = indice->buscar(clave, registroDatos);
		pipe.escribir(resultado);
		
		if (resultado == ResultadosIndices::OK) {
			
			//Obtengo el índice primario
			DefinitionsManager::ListaNombresClaves* listaNombresClaves = defManager.getListaNombresClavesPrimarias(nombreTipo);
			indice = mapaIndices[*listaNombresClaves];
			
			BloqueListaPrimaria listaPrimariaManager(indice->getTamanioBloqueLista());
			
			ListaClaves* listaClaves = listaPrimariaManager.getListaClaves(registroDatos, defManager.getListaTipos(nombreTipo));
			
			//Envío la cantidad de registros
			cantRegistros = listaClaves->size();
			pipe.escribir(cantRegistros);
			
			//Consulto el registro de datos de cada clave primaria
			for (ListaClaves::iterator it = listaClaves->begin(); it != listaClaves->end(); ++it) {
				
				resultado = indice->buscar(*it, registroDatos, tamRegistro);
				pipe.escribir(resultado);
				
				if (resultado == ResultadosIndices::OK) {
					pipe.escribir(tamRegistro);
					pipe.escribir(registroDatos, tamRegistro);
				} else break;
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
		
		registroDatos = new char[tamRegistro];
		
		// Recibe el registro de datos.
		pipe.leer(tamRegistro, registroDatos);

		resultado = indice->insertar(clave,registroDatos,tamRegistro);

		pipe.escribir(resultado);
		
		if (resultado == ResultadosIndices::OK) {
			//Actualizo los indices secundarios
			//Saco el índice primario para no volver a insertar.
			mapaIndices.erase(*defManager.getListaNombresClavesPrimarias(nombreTipo));
			delete indice;
			
			BloqueListaPrimaria listaPrimariaManager(0);
			char* registroLista = NULL;
			Clave* claveSecundaria = NULL;
			
			for (MapaIndices::iterator iter = mapaIndices.begin();
				iter != mapaIndices.end(); ++iter) {
				
				Registro registro(defManager.getTipoOrgRegistro(nombreTipo),
								  registroDatos, defManager.getListaTipos(nombreTipo),
								  defManager.getListaNombresAtributos(nombreTipo));
				
				claveSecundaria = registro.getClave(iter->first);
				indice = iter->second;
				
				resultado = indice->buscar(claveSecundaria, registroLista);
				
				listaPrimariaManager.setTamanioLista(indice->getTamanioBloqueLista());
				
				if (resultado == ResultadosIndices::OK) {
					//La clave secundaria ya estaba insertada. Sólo se actualiza la lista de claves primaria.
					listaPrimariaManager.insertarClave(registroLista, clave, defManager.getListaTipos(nombreTipo));
					//TODO Modificar la lista en disco.
					
				} else {
					//La clave no se encontró en el índice secundario.
					//Creo la lista de claves primarias, inserto la clave primaria,
					//e inserto la clave secundaria y la lista en el indice secundario.
					listaPrimariaManager.crearLista(registroLista, clave, defManager.getListaTipos(nombreTipo));
					indice->insertar(claveSecundaria, registroLista);						
				}
				
				delete claveSecundaria;
				
			}
		}
		
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
	
	char* registroDatos = NULL;
	unsigned short tamRegistro = 0;
	
	//Si la operación es una inserción, recibo la clave o la lista de claves del índice primario
	
	//Leo el tamanio del buffer a recibir
	pipe.leer(&tamanioBuffer);
	
	//Leo el buffer con los nombres y los valores de cada campo de la variable
	pipe.leer(tamanioBuffer, buffer);
	
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
	
	//Se crean los indices correspondientes al tipo 'nombreTipo'
	crearIndices(nombreTipo, mapaIndices, defManager);
	
	indice = mapaIndices[listaNombresClaves];
	clave = ClaveFactory::getInstance().getClave(listaValoresClaves, *(defManager.getListaTiposClaves(nombreTipo, listaNombresClaves)));
	
	switch(codOp) {
		case OperacionesCapas::INDICES_CONSULTAR:
			// Si la consulta se hace a un índice primario, se envía el registro
			// pedido, sino se envían todos los registros correspondiente a las
			// claves primarias de la lista del indice secundario.
			consultar(nombreTipo, mapaIndices, indice, clave, defManager, pipe);
			break;
		case OperacionesCapas::INDICES_INSERTAR:
			insertar(nombreTipo, mapaIndices, indice, clave, defManager, pipe);
			break;
		case OperacionesCapas::INDICES_ELIMINAR:
		{
			resultado = indice->eliminar(clave);
			pipe.escribir(resultado);
			if (resultado == ResultadosIndices::OK) {
				//TODO Actualizar indices secundarios
			}
			break;
		}
		case OperacionesCapas::INDICES_MODIFICAR:
		{
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
				
				//TODO Obtner la lista de claves primarias y modificar todos los registros
				//correspondientes a cada clave de la lista.
				
			}
			
			if (resultado == ResultadosIndices::OK) {
				//TODO Actualizar indices secundarios
			}
			
			break;
		}
		default:
			resultado = ResultadosIndices::OPERACION_INVALIDA;
			pipe.escribir(resultado);
	}
	
	destruirIndices(mapaIndices);
	
	if (registroDatos) delete[] registroDatos;
	
	return 0;
	
}


int main(int argc, char* argv[]) {
/*	
	ComuDatos pipe(argv);
	unsigned char codOp = 0;
	string nombreTipo;
	
	pipe.parametro(0, codOp);
	
	pipe.parametro(1, nombreTipo);
	
	procesarOperacion(codOp, nombreTipo, pipe);
*/	
// MÉTODOS DE PRUEBA PARA UN ÁRBOL B+
/*
	DefinitionsManager::ListaTiposAtributos* lista = DefinitionsManager::getInstance().getListaTiposAtributos("PERSONA");
	IndiceArbol indice(TipoIndices::GRIEGO, 48, TipoDatos::TIPO_ENTERO, lista, TipoIndices::ARBOL_BS, 48, 48, "locura", TipoDatos::TIPO_VARIABLE);
	
	char* null = NULL;
*/
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
/*	indice.insertar(new ClaveEntera(20), null, 48);
	indice.insertar(new ClaveEntera(90), null, 48);
	indice.insertar(new ClaveEntera(49), null, 48);
	indice.insertar(new ClaveEntera(33), null, 48);
	indice.insertar(new ClaveEntera(57), null, 48);
	indice.insertar(new ClaveEntera(30), null, 48);
	indice.insertar(new ClaveEntera(17), null, 48);
	indice.insertar(new ClaveEntera(8), null, 48);
	
	//Splittea la raiz:
	indice.insertar(new ClaveEntera(10), null, 48);

	//Alta común:
	indice.insertar(new ClaveEntera(1), null, 48);
	
	//Alta con redistribución
	indice.insertar(new ClaveEntera(7), null, 48);
	
	//Produce split:
	indice.insertar(new ClaveEntera(86), null, 48);

	//Altas comunes:
	indice.insertar(new ClaveEntera(6), null, 48);
	indice.insertar(new ClaveEntera(19), null, 48);
	indice.insertar(new ClaveEntera(60), null, 48);
	indice.insertar(new ClaveEntera(88), null, 48);
	indice.insertar(new ClaveEntera(97), null, 48);
	
	//Produce split:
	indice.insertar(new ClaveEntera(14), null, 48);
	
	//Lleno la raiz:
	indice.insertar(new ClaveEntera(2), null, 48);
	indice.insertar(new ClaveEntera(3), null, 48);
	indice.insertar(new ClaveEntera(4), null, 48);
	indice.insertar(new ClaveEntera(5), null, 48);
	indice.insertar(new ClaveEntera(9), null, 48);
	indice.insertar(new ClaveEntera(11), null, 48);
	indice.insertar(new ClaveEntera(12), null, 48);
	indice.insertar(new ClaveEntera(13), null, 48);
	indice.insertar(new ClaveEntera(15), null, 48);
	indice.insertar(new ClaveEntera(16), null, 48);
	indice.insertar(new ClaveEntera(18), null, 48);
	indice.insertar(new ClaveEntera(21), null, 48);
	indice.insertar(new ClaveEntera(22), null, 48);
	indice.insertar(new ClaveEntera(23), null, 48);
	indice.insertar(new ClaveEntera(100), null, 48);
	indice.insertar(new ClaveEntera(121), null, 48);
	indice.insertar(new ClaveEntera(93), null, 48);
	
	//Splitteo la raiz:
	indice.insertar(new ClaveEntera(28), null, 48);
	
	//Baja común:
	indice.eliminar(new ClaveEntera(11));
	
	//Baja con redistribución:
	indice.eliminar(new ClaveEntera(7));
	
	//Baja en un nodo interno:
	indice.eliminar(new ClaveEntera(88));

	//Baja en la raiz que ademas produce merge en un nodo interno y en la raiz:
	indice.eliminar(new ClaveEntera(23));

	//Alta común
	indice.insertar(new ClaveEntera(25), null, 48);

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
	indice.insertar(new ClaveEntera(125), null, 48); 
*/
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
	}*/
/*
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

	
	DefinitionsManager::ListaTiposAtributos* lista = DefinitionsManager::getInstance().getListaTiposAtributos("PERSONA");
	IndiceArbol indice(TipoIndices::GRIEGO, 48, TipoDatos::TIPO_STRING, lista, TipoIndices::ARBOL_BS, 48, 48, "locura", TipoDatos::TIPO_VARIABLE);
	
	char* null = NULL;
	
	//Inserto en la raiz
	indice.insertar(new ClaveVariable("Manu"), null);
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
	
	cout << "Fin Main" << endl;
}

