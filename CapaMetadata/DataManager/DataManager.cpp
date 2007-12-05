#include "DataManager.h"

DataManager::DataManager()
{
	this->registro = NULL;
}

DataManager::~DataManager()
{
	if(this->registro)
		delete[] registro;
}

unsigned short DataManager::crearRegistroAlta(const ListaValoresAtributos &listaVA,
									const ListaTiposAtributos &listaTipos) {
	
	ListaTiposAtributos::const_iterator itTipos = listaTipos.begin();
	ListaValoresAtributos::const_iterator itValoresAtributos = listaVA.begin();
	unsigned char tipo = 0; //Indica si el tipo de campo del registro es variable o fijo
	NodoInfoRegistro regAttribute;
	
	unsigned short offsetToCampo = 0;
	unsigned short longRegistro = this->getTamanioRegistro(listaTipos,listaVA);
	char *registro = new char[longRegistro];
	string campoRegistro;
	
	regAttribute = *itTipos;
	// Si el registro es de tipo variable le guardo la longitud al registro
	if(regAttribute.tipoDato == TipoDatos::TIPO_VARIABLE){
		// Guardo la longitud del registro en los primeros dos bytes del mismo
		unsigned short longVariable = longRegistro - Tamanios::TAMANIO_LONGITUD;
		memcpy(registro,&longVariable,Tamanios::TAMANIO_LONGITUD);
		offsetToCampo = Tamanios::TAMANIO_LONGITUD;
	}
	// Se itera la lista de atributos del registro.
	// Y se van guardando los datos del registro
	for(++itTipos ; itTipos != listaTipos.end() ; ++itTipos,++itValoresAtributos){
		regAttribute = *itTipos;
		tipo = regAttribute.tipoDato;
		// Obtengo el campo del registro
		campoRegistro = *itValoresAtributos;
		
		if (tipo == TipoDatos::TIPO_ENTERO){
			int entero = atoi(campoRegistro.c_str());
			memcpy(&registro[offsetToCampo],&entero,sizeof(int));
			offsetToCampo += sizeof(int);
		}
		else if(tipo == TipoDatos::TIPO_CHAR){
			char c = campoRegistro[0];
			memcpy(&registro[offsetToCampo],&c,sizeof(char));
			offsetToCampo += sizeof(char);
		}
		else if(tipo == TipoDatos::TIPO_FECHA){
			
			//Se verifica que el formato de la fecha sea válido
			if (campoRegistro.size() < 8) {
				delete[] registro;
				return 0;
			}
			
			string subStr = campoRegistro.substr(0, 4);
			unsigned short anio = atoi(subStr.c_str());
			memcpy(&registro[offsetToCampo], &anio, sizeof(unsigned short));
			offsetToCampo += sizeof(unsigned short);
			
			subStr = campoRegistro.substr(4, 2);
			unsigned char mes = atoi(subStr.c_str());
			memcpy(&registro[offsetToCampo], &mes, sizeof(unsigned char));
			offsetToCampo += sizeof(unsigned char);
			
			subStr = campoRegistro.substr(6, 2);
			unsigned char dia = atoi(subStr.c_str());
			memcpy(&registro[offsetToCampo], &dia, sizeof(unsigned char));
			offsetToCampo += sizeof(unsigned char);			
			
		}
		else if(tipo == TipoDatos::TIPO_FLOAT){
			float campoFloat = atof(campoRegistro.c_str());
			memcpy(&registro[offsetToCampo],&campoFloat,sizeof(float));  
			offsetToCampo += sizeof(float);
		}
		else if(tipo == TipoDatos::TIPO_SHORT){
			int campoShort = atoi(campoRegistro.c_str());
			memcpy(&registro[offsetToCampo],&campoShort,sizeof(short));  
			offsetToCampo += sizeof(short);
		}
		else if(tipo == TipoDatos::TIPO_STRING){
			// Obtengo la longitud del campo variable
			unsigned char longCampoVariable =  campoRegistro.size();
			// Guardo la longitud del campo variable en el registro
			memcpy(&registro[offsetToCampo],&longCampoVariable,Tamanios::TAMANIO_LONGITUD_CADENA);
			// Al offset le sumo los bytes de la longitud
			offsetToCampo += Tamanios::TAMANIO_LONGITUD_CADENA;
			const char *campoVariable = campoRegistro.c_str();
			// Guardo el campo variable sin el '/0'
			memcpy(&registro[offsetToCampo],campoVariable,longCampoVariable);
			offsetToCampo += longCampoVariable;
		}
		else if(tipo == TipoDatos::TIPO_BOOL){
			int campoBooleano = atoi(campoRegistro.c_str());
			memcpy(&registro[offsetToCampo],&campoBooleano,sizeof(bool));
			offsetToCampo += sizeof(bool);
		}
	}
	
	this->setRegistro(registro);
	
	return longRegistro;
}


unsigned short DataManager::crearRegistroAltaRandom(string &valoresClaves, const ListaTiposAtributos &listaTipos) {
	
	ListaTiposAtributos::const_iterator itTipos = listaTipos.begin();
	ListaValoresAtributos listaVA;
	unsigned char tipo; //Indica si el tipo de campo del registro es variable o fijo
	stringstream conversor;
	
	// Se itera la lista de atributos del registro.
	// Y se van generando aleatoriamente los datos del registro.
	for(++itTipos; itTipos != listaTipos.end(); ++itTipos) {
		
		tipo = itTipos->tipoDato;
		
		if(tipo == TipoDatos::TIPO_ENTERO){
			
			int entero = rand() % 50000001; //Genera un número entero hasta 50000000
			conversor << entero;
			
			valoresClaves = "DNI=" + conversor.str() + CodigosPipe::COD_FIN_CLAVE;
			
		}
		else if(tipo == TipoDatos::TIPO_CHAR){
			
			char c = 97 + (rand() % 26); //Caracteres aleatorios de la 'a' a la 'z'
			conversor << c;
			
		}
		else if(tipo == TipoDatos::TIPO_FECHA){
			
			unsigned short cero = 0;
			unsigned short anio = rand() % 10000;
			conversor << anio;
			unsigned longAnio = conversor.str().size();
			if (longAnio < 4) {
				conversor.str("");
				for (unsigned i = 0; i < (4 - longAnio); ++i)
					conversor << cero;
				conversor << anio;
			}
			unsigned char mes = 1 + (rand() % 12);
			if (mes < 10) conversor << cero;
			conversor << (short)mes;
			unsigned char dia = 1 + (rand() % 31);
			if (dia < 10) conversor << cero;
			conversor << (short)dia;
			
		}
		else if(tipo == TipoDatos::TIPO_FLOAT){

			float real = rand()*0.127;
			conversor << real;
			
		}
		else if(tipo == TipoDatos::TIPO_SHORT){
			
			short numero = rand() % 32001; //Genera un número entero hasta 32000
			conversor << numero;
			
		}
		else if(tipo == TipoDatos::TIPO_STRING){
			
			char c;
			int longCadena = rand() % 300;
			
			for (int i = 0; i < longCadena; ++i) {
				c = 97 + (rand() % 26); //Caracteres aleatorios de la 'a' a la 'z'
				conversor << c;
			}
			
		}
		else if(tipo == TipoDatos::TIPO_BOOL){
			
			int campoBooleano = rand() % 2;
			conversor << (bool)campoBooleano;
			
		}
		
		listaVA.push_back(conversor.str());
		conversor.clear();
		conversor.str("");
	}
	
	return this->crearRegistroAlta(listaVA, listaTipos);
	
}


unsigned short DataManager::getTamanioRegistro(const ListaTiposAtributos &listaTiposAtributos,
											   const ListaValoresAtributos &listaVA){
	// Obtengo los iteradores de ambas listas
	ListaTiposAtributos::const_iterator itTipos = listaTiposAtributos.begin();
	ListaValoresAtributos::const_iterator itValoresAtributos = listaVA.begin();
	
	unsigned short tamanioRegistro = 0;
	NodoInfoRegistro regAttribute = *itTipos;
	string campoRegistro;
	unsigned char tipo;
	
	// Si el tipo de registro es fijo obtengo directamente la longitud 
		if(regAttribute.tipoDato == TipoDatos::TIPO_FIJO)
			tamanioRegistro = regAttribute.tamRegistro;
			
		else{
			// Le sumo los bytes que contendran la longitud del registro
			tamanioRegistro = Tamanios::TAMANIO_LONGITUD;
			
			// Itero la listas para ir obteniendo la longitud del registro
			// La lista de tipos de atributos se incrementa en un nodo mas ya que le primero contiene informacion 
			// acerca del registro
			for(++itTipos ;itTipos != listaTiposAtributos.end(); ++itTipos,++itValoresAtributos){
				regAttribute = *itTipos;
				tipo = regAttribute.tipoDato;
				
				if(tipo == TipoDatos::TIPO_ENTERO)
					tamanioRegistro += sizeof(int);
				
				else if(tipo == TipoDatos::TIPO_FECHA)
					tamanioRegistro += Tamanios::TAMANIO_FECHA;
				
				else if(tipo == TipoDatos::TIPO_FLOAT)
				  	tamanioRegistro += sizeof(float);
				  	
				else if(tipo == TipoDatos::TIPO_SHORT)
					tamanioRegistro += sizeof(short);
				
				else if(tipo == TipoDatos::TIPO_CHAR)
					tamanioRegistro += sizeof(char);
					
				else if(tipo == TipoDatos::TIPO_STRING){
					// Obtengo la longitud del campo variable
					unsigned char longCampoVariable;
					longCampoVariable = itValoresAtributos->size();
					
					// le sumo al tamanio la longitud del string mas los bytes que indicaran la longitud del mismo
					tamanioRegistro += Tamanios::TAMANIO_LONGITUD_CADENA + longCampoVariable;

				}
				else if(tipo == TipoDatos::TIPO_BOOL)
					tamanioRegistro += sizeof(bool);
			}		
		}
		
	return tamanioRegistro;
}


/*
 *  Genera un nuevo registro con los cambios pedidos en la modificacion
 *  Una vez que ya lo geenra lo setea en el atributo registro de dataManager
 * */
unsigned short DataManager::crearRegistroModificacion(const ListaTiposAtributos &listaTiposAtributos,
											const ListaValoresAtributos &listaVA,
											char *registroEnDisco){
	// Obtengo los iteradores de ambas listas
	ListaTiposAtributos::const_iterator itTipos = listaTiposAtributos.begin();
	ListaValoresAtributos::const_iterator itValoresAtributos = listaVA.begin();

	NodoInfoRegistro regAttribute = *itTipos;
	unsigned short offsetRegDisco = 0;
	unsigned short longRegNuevo = 0;
	unsigned char tipo = 0;
	string valorModificado("");
	unsigned char longCampoVariable = 0; 
	
	// Si el tipo de registro es fijo obtengo la longitud de la lista de tipos 
	if(regAttribute.tipoDato == TipoDatos::TIPO_FIJO)
		longRegNuevo = regAttribute.tamRegistro;
	// Obtengo el tamanio del nuevo registro, reservo espacio para el msimo y lo genero con las nuevas modificaciones
	else{
			// Incremento el offset en dos bytes, que son los que ocupan la longitud del registro
			offsetRegDisco += Tamanios::TAMANIO_LONGITUD;
			longRegNuevo += Tamanios::TAMANIO_LONGITUD;
			
			// Itero las listas para obtener el tamanio del nuevo registro y reservar memoria para el mismo
			for(++itTipos; itTipos != listaTiposAtributos.end(); ++itTipos, ++itValoresAtributos){
				regAttribute = *itTipos;
				tipo = regAttribute.tipoDato;
				
				if(tipo == TipoDatos::TIPO_ENTERO){
					longRegNuevo += sizeof(int);
					offsetRegDisco += sizeof(int);
				}
				else if(tipo == TipoDatos::TIPO_FECHA){
					longRegNuevo += Tamanios::TAMANIO_FECHA;
					offsetRegDisco += Tamanios::TAMANIO_FECHA;
				}
				else if(tipo == TipoDatos::TIPO_FLOAT){
					longRegNuevo += sizeof(float);
					offsetRegDisco += sizeof(float);
				}
				else if(tipo == TipoDatos::TIPO_SHORT){
					longRegNuevo += sizeof(short);
					offsetRegDisco += sizeof(short);
				}
				else if(tipo == TipoDatos::TIPO_CHAR){
					longRegNuevo += sizeof(char);
					offsetRegDisco += sizeof(char);
				}
				else if(tipo == TipoDatos::TIPO_STRING){
					valorModificado = *itValoresAtributos;
					// Sumo el tamaño del campo que guarda la logitud del string
					longRegNuevo += Tamanios::TAMANIO_LONGITUD_CADENA;
					// Obtengo la longitud del campo variable en el registro original para incrementar el offset del mismo
					memcpy(&longCampoVariable, &registroEnDisco[offsetRegDisco], Tamanios::TAMANIO_LONGITUD_CADENA);
					// Le sumo al offset los dos bytes que indican la longitud del campo variable + la longitud del campo variable
					offsetRegDisco += Tamanios::TAMANIO_LONGITUD_CADENA + longCampoVariable;
					
					// Si el valor en la lista de valores modificados es distinto de "" significa 
					// que ese valor fue modificado  
					if(valorModificado != "")
						longRegNuevo += valorModificado.size();
					 else
						longRegNuevo += longCampoVariable;
				}
				else if(tipo == TipoDatos::TIPO_BOOL){
					longRegNuevo += sizeof(bool);
					offsetRegDisco += sizeof(bool);
				}
									
			}
	}
	
	// una vez que ya se la longitud del nuevo registro reservo espacio para el mismo
	char *registroNuevo = new char[longRegNuevo];
	
	this->generarRegistroModificado(registroNuevo,registroEnDisco,longRegNuevo,listaTiposAtributos,listaVA);
	
	this->setRegistro(registroNuevo);
	
	return longRegNuevo;
}


/*
 * Genera el nuevo registro con las modificaciones correspondientes
 * */
void DataManager::generarRegistroModificado(char *registroNuevo, char *registroViejo, unsigned short longNuevoReg, 
							   const ListaTiposAtributos &listaTiposAtributos,
		   					   const ListaValoresAtributos &listaVA) {
	// Obtengo los iteradores de ambas listas
	ListaTiposAtributos::const_iterator itTipos = listaTiposAtributos.begin();
	ListaValoresAtributos::const_iterator itValoresAtributos = listaVA.begin();
	unsigned short offsetRegViejo = 0;
	unsigned short offsetRegNuevo = 0;
	NodoInfoRegistro registerAttribute;
	string valorModificado("");
	NodoInfoRegistro regAttribute = *itTipos;
	unsigned char tipo = regAttribute.tipoDato;
	
	// Si el registro es variable guardo la longitud del mismo en los primeros dos bytes
	if(tipo == TipoDatos::TIPO_VARIABLE){
		unsigned short longRegVariable = longNuevoReg - Tamanios::TAMANIO_LONGITUD;
		memcpy(registroNuevo, &longRegVariable, Tamanios::TAMANIO_LONGITUD);
		offsetRegNuevo += Tamanios::TAMANIO_LONGITUD;
		offsetRegViejo += Tamanios::TAMANIO_LONGITUD;
	}	
	 
	for(++itTipos; itTipos != listaTiposAtributos.end(); ++itTipos, ++itValoresAtributos) {
		regAttribute = *itTipos;
		tipo = regAttribute.tipoDato;
		valorModificado = *itValoresAtributos;
		bool estaModificado  = (valorModificado != CAMPO_NO_MODIFICADO);
		
		switch (tipo) {
		
		case TipoDatos::TIPO_ENTERO:
			if (estaModificado){
				int entero = atoi(valorModificado.c_str());
				memcpy(&registroNuevo[offsetRegNuevo],&entero,sizeof(int));
			}
			else
				memcpy(&registroNuevo[offsetRegNuevo],&registroViejo[offsetRegViejo],sizeof(int));
			// Muevo los punteros del registro	
			offsetRegNuevo += sizeof(int);
			offsetRegViejo += sizeof(int);
			
			break;		
		case TipoDatos::TIPO_BOOL:
			if (estaModificado){
				bool booleano;
				if (valorModificado == "TRUE") booleano = true;
				else booleano = false;
				memcpy(&registroNuevo[offsetRegNuevo], &booleano, sizeof(bool));
			}else
				memcpy(&registroNuevo[offsetRegNuevo], &registroViejo[offsetRegViejo], sizeof(bool));
			
			// Muevo los punteros del registro
			offsetRegNuevo += sizeof(bool);
			offsetRegViejo += sizeof(bool);
			
			break;		
		case TipoDatos::TIPO_CHAR:
			if (estaModificado)
				memcpy(&registroNuevo[offsetRegNuevo],valorModificado.c_str(),sizeof(char));
			else
				memcpy(&registroNuevo[offsetRegNuevo],&registroViejo[offsetRegViejo],sizeof(char));
			
			// Muevo los punteros del registro
			offsetRegNuevo += sizeof(char);
			offsetRegViejo += sizeof(char);
			
			break;		

		case TipoDatos::TIPO_FECHA:
			if ( (estaModificado) && (valorModificado.size() == 8) ) {
				string subStr = valorModificado.substr(0, 4);
				unsigned short anio = atoi(subStr.c_str());
				memcpy(&registroNuevo[offsetRegNuevo], &anio, sizeof(unsigned short));
				offsetRegNuevo += sizeof(unsigned short);
				
				subStr = valorModificado.substr(4, 2);
				unsigned char mes = atoi(subStr.c_str());
				memcpy(&registroNuevo[offsetRegNuevo], &mes, sizeof(unsigned char));
				offsetRegNuevo += sizeof(unsigned char);
				
				subStr = valorModificado.substr(6, 2);
				unsigned char dia = atoi(subStr.c_str());
				memcpy(&registroNuevo[offsetRegNuevo], &dia, sizeof(unsigned char));
				offsetRegNuevo += sizeof(unsigned char);
			}
			else{
				memcpy(&registroNuevo[offsetRegNuevo], &registroViejo[offsetRegViejo], Tamanios::TAMANIO_FECHA);
				offsetRegNuevo += Tamanios::TAMANIO_FECHA;
			}
			
			offsetRegViejo += Tamanios::TAMANIO_FECHA;
		
			break;
		case TipoDatos::TIPO_STRING:
			unsigned char longCampoVariable;
			if (estaModificado){
				longCampoVariable = valorModificado.size(); 								
				// Guardo la longitud del campo variable en los bytes anteriores al dato
				memcpy(&registroNuevo[offsetRegNuevo], &longCampoVariable, Tamanios::TAMANIO_LONGITUD_CADENA);
				//Incremento el offset en el byte de longitud
				offsetRegNuevo += Tamanios::TAMANIO_LONGITUD_CADENA;
				// Guardo el campo variable
				memcpy(&registroNuevo[offsetRegNuevo],valorModificado.c_str(),longCampoVariable);
				// Incremento el offset del registro nuevo
				offsetRegNuevo += longCampoVariable;
				
				// Movimientos en el registro viejo
				memcpy(&longCampoVariable, &registroViejo[offsetRegViejo], Tamanios::TAMANIO_LONGITUD_CADENA);
				// Incremento el offset para apuntar al proximo campo por si lo tengo que guardar en el registro nuevo
				offsetRegViejo += Tamanios::TAMANIO_LONGITUD_CADENA + longCampoVariable;
				
			}else{
				memcpy(&longCampoVariable, &registroViejo[offsetRegViejo], Tamanios::TAMANIO_LONGITUD_CADENA);
				// Incremento el offset en dos bytes que corresponden a la longitud del campo variable
				offsetRegViejo += Tamanios::TAMANIO_LONGITUD_CADENA;
				// Guardo la longitud del campo variable en el registro nuevo
				memcpy(&registroNuevo[offsetRegNuevo], &longCampoVariable, Tamanios::TAMANIO_LONGITUD_CADENA);
				offsetRegNuevo += Tamanios::TAMANIO_LONGITUD_CADENA;
				// Guardo el campo variable
				memcpy(&registroNuevo[offsetRegNuevo], &registroViejo[offsetRegViejo], longCampoVariable);
				offsetRegNuevo += longCampoVariable;
				offsetRegViejo += longCampoVariable;
			}
				
			break;
		case TipoDatos::TIPO_SHORT:
			if (estaModificado){
				short valorShort = atoi(valorModificado.c_str());
				// Guardo el valor modificado en el registro nuevo 
				memcpy(&registroNuevo[offsetRegNuevo],&valorShort,sizeof(short));
			}else
				memcpy(&registroNuevo[offsetRegNuevo],&registroViejo[offsetRegViejo],sizeof(short));
				
			offsetRegNuevo += sizeof(short);
			offsetRegViejo += sizeof(short);
			
			break;
		case TipoDatos::TIPO_FLOAT:
			if (estaModificado){
				float valorReal = atof(valorModificado.c_str());
				// Guardo el valor modificado en el registro nuevo 
				memcpy(&registroNuevo[offsetRegNuevo],&valorReal,sizeof(float));
			}else
				memcpy(&registroNuevo[offsetRegNuevo],&registroViejo[offsetRegViejo],sizeof(float));
				
			offsetRegNuevo += sizeof(float);
			offsetRegViejo += sizeof(float);
			
			break;
		}
		
	}
	
}
