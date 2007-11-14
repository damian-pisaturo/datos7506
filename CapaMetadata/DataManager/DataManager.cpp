#include "DataManager.h"
// listaAtributos { numeroNodoListaLoca:valor para modif}
DataManager::DataManager()
{
	this->registro = NULL;
}

DataManager::~DataManager()
{
	if(this->registro){
		delete[] registro;
		this->registro = NULL;
	}
}

void DataManager::crearRegistroAlta(const DefinitionsManager::ListaValoresAtributos &listaVA,
									const DefinitionsManager::ListaTiposAtributos &listaTipos) {
	
	DefinitionsManager::ListaTiposAtributos::const_iterator itTipos = listaTipos.begin();
	DefinitionsManager::ListaValoresAtributos::const_iterator itValoresAtributos = listaVA.begin();
	int tipo; //Indica si el tipo de campo del registro es variable o fijo
	nodoLista regAttribute;
	unsigned short offsetToCampo = 0;
	unsigned short longRegistro = this->getTamanioRegistro(listaTipos,listaVA);
	char *registro = new char[longRegistro];
	string campoRegistro;
	
	regAttribute = *itTipos;
	// Si el registro es de tipo variable le guardo la longitud al registro
	if(regAttribute.tipo == TipoDatos::TIPO_VARIABLE){
		// Guardo la longitud del registro en los primeros dos bytes del mismo
		memcpy(registro,&longRegistro,Tamanios::TAMANIO_LONGITUD);
		offsetToCampo = sizeof(unsigned short);
	}
	// Se itera la lista de atributos del registro.
	// Y se van guardando los datos del registro
	for(++itTipos ; itTipos != listaTipos.end() ; ++itTipos,++itValoresAtributos){
		regAttribute = *itTipos;
		tipo = regAttribute.tipo;
		// Obtengo el campo del registro
		campoRegistro = *itValoresAtributos;
		if(tipo == TipoDatos::TIPO_ENTERO){
			int entero = atoi(campoRegistro.c_str());
			memcpy(&registro[offsetToCampo],&entero,sizeof(int));
			offsetToCampo += sizeof(int);
		}
		else if(tipo == TipoDatos::TIPO_FECHA){
			
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
			float *campoFloat = (float*)campoRegistro.c_str();
			memcpy(&registro[offsetToCampo],campoFloat,sizeof(float));  
			offsetToCampo += sizeof(float);
		}
		else if(tipo == TipoDatos::TIPO_SHORT){
			short *campoShort = (short*)campoRegistro.c_str();
			memcpy(&registro[offsetToCampo],campoShort,sizeof(short));  
			offsetToCampo += sizeof(short);
		}
		else if(tipo == TipoDatos::TIPO_STRING){
			// Obtengo la longitud del campo variable
			unsigned short longCampoVariable =  campoRegistro.size();
			// Guardo la longitud del campo variable en el registro
			memcpy(&registro[offsetToCampo],&longCampoVariable,sizeof(unsigned short));
			// Al offset le sumo los dos bytes de la longitud
			offsetToCampo += sizeof(unsigned short);
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
	if(this->registro){
		delete[] this->registro;
		this->registro = NULL;
	}
	this->registro = registro;
}	
unsigned short DataManager::getTamanioRegistro(const DefinitionsManager::ListaTiposAtributos &listaTiposAtributos,
											   const DefinitionsManager::ListaValoresAtributos &listaVA){
	// Obtengo los iteradores de ambas listas
	DefinitionsManager::ListaTiposAtributos::const_iterator itTipos = listaTiposAtributos.begin();
	DefinitionsManager::ListaValoresAtributos::const_iterator itValoresAtributos = listaVA.begin();
	
	unsigned short tamanioRegistro = 0;
	nodoLista regAttribute = *itTipos;
	string campoRegistro;
	int tipo;
	
	// Si el tipo de registro es fijo obtengo directamente la longitud 
		if(regAttribute.tipo == TipoDatos::TIPO_FIJO)
			tamanioRegistro = ((unsigned short)atoi(regAttribute.pk.c_str()));
			
		else{
			// Le sumo los bytes que contendran la longitud del registro
			tamanioRegistro = sizeof(unsigned short);
			
			// Itero la listas para ir obteniendo la longitud del registro
			// La lista de tipos de atributos se incrementa en un nodo mas ya que le primero contiene informacion 
			// acerca del registro
			for(++itTipos ;itTipos != listaTiposAtributos.end(); ++itTipos,++itValoresAtributos){
				regAttribute = *itTipos;
				tipo = regAttribute.tipo;
				
				if(tipo == TipoDatos::TIPO_ENTERO)
					tamanioRegistro += sizeof(int);
				
				else if(tipo == TipoDatos::TIPO_FECHA)
					tamanioRegistro += sizeof(unsigned short) + sizeof(unsigned char) + sizeof(unsigned char);
				
				else if(tipo == TipoDatos::TIPO_FLOAT)
				  	tamanioRegistro += sizeof(float);
				  	
				else if(tipo == TipoDatos::TIPO_SHORT)
					tamanioRegistro += sizeof(short);
				
				else if(tipo == TipoDatos::TIPO_CHAR)
					tamanioRegistro += sizeof(char);
					
				else if(tipo == TipoDatos::TIPO_STRING){
					// Obtengo la longitud del campo variable
					unsigned short longCampoVariable;
					longCampoVariable = itValoresAtributos->size();
					
					// le sumo al tamanio la longitud del string mas los dos bytes que indicaran la longitud del mismo
					tamanioRegistro += sizeof(unsigned short) + longCampoVariable;

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
void DataManager::crearRegistroModificacion(const DefinitionsManager::ListaTiposAtributos &listaTiposAtributos,
											const DefinitionsManager::ListaValoresAtributos &listaVA,
											char *registroEnDisco){
	// Obtengo los iteradores de ambas listas
	DefinitionsManager::ListaTiposAtributos::const_iterator itTipos = listaTiposAtributos.begin();
	DefinitionsManager::ListaValoresAtributos::const_iterator itValoresAtributos = listaVA.begin();

	nodoLista regAttribute = *itTipos;
	unsigned short offsetRegDisco = 0;
	unsigned short longRegNuevo;
	int tipo;
	string valorModificado;
	unsigned short longCampoVariable; 
	
	// Si el tipo de registro es fijo obtengo la longitud de la lista de tipos 
	if(regAttribute.tipo == TipoDatos::TIPO_FIJO)
		longRegNuevo = ((unsigned short)atoi(regAttribute.pk.c_str()));
	// Obtengo el tamanio del nuevo registro, reservo espacio para el msimo y lo genero con las nuevas modificaciones
	else{
			// Incremento el offset en dos bytes, que son los que ocupan la longitud del registro
			offsetRegDisco += sizeof(unsigned short);
			longRegNuevo += sizeof(unsigned short);
			
			
			// Itero las listas para obtener el tamanio del nuevo registro y reservar memoria para el mismo
			for(++itTipos; itTipos != listaTiposAtributos.end(); ++itTipos, ++itValoresAtributos){
				regAttribute = *itTipos;
				tipo = regAttribute.tipo;
				
				if(tipo == TipoDatos::TIPO_ENTERO){
					longRegNuevo += sizeof(int);
					offsetRegDisco += sizeof(int);
				}
				else if(tipo == TipoDatos::TIPO_FECHA){
					longRegNuevo += sizeof(unsigned short) + 2*sizeof(unsigned char);
					offsetRegDisco += sizeof(unsigned short) + 2*sizeof(unsigned char);
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
					// Sumo los dos bytes de la longitud del registro
					longRegNuevo += sizeof(unsigned short);
					// Obtengo la longitud del campo variable en el registro original para incrementar el offset del mismo
					memcpy(&longCampoVariable,&registroEnDisco[offsetRegDisco],sizeof(unsigned short));
					// Le sumo al offset los dos bytes que indican la longitud del campo variable + la longitud del campo variable
					offsetRegDisco += sizeof(unsigned short) + longCampoVariable;
					
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
	char *registroNuevo = new char [longRegNuevo];
	
	this->generarRegistroModificado(registroNuevo,registroEnDisco,longRegNuevo,listaTiposAtributos,listaVA);
	if(this->registro){
		delete[] this->registro;
		this->registro = NULL;
	}
	this->registro = registroNuevo;
}

/*
 * Genera el nuevo registro con las modificaciones correspondientes
 * */
void DataManager::generarRegistroModificado(char *registroNuevo, char *registroViejo, unsigned short longNuevoReg, 
							   const DefinitionsManager::ListaTiposAtributos &listaTiposAtributos,
		   					   const DefinitionsManager::ListaValoresAtributos &listaVA
		   					  ){
	// Obtengo los iteradores de ambas listas
	DefinitionsManager::ListaTiposAtributos::const_iterator itTipos = listaTiposAtributos.begin();
	DefinitionsManager::ListaValoresAtributos::const_iterator itValoresAtributos = listaVA.begin();
	unsigned short offsetRegViejo = 0;
	unsigned short offsetRegNuevo = 0;
	nodoLista registerAttribute;
	string valorModificado;
	nodoLista regAttribute = *itTipos;
	int tipo = regAttribute.tipo;
	
	// Si el registro es variable guardo la longitud del mismo en los primeros dos bytes
	if(tipo == TipoDatos::TIPO_VARIABLE){
		memcpy(registroNuevo,&longNuevoReg,sizeof(unsigned short));
		offsetRegNuevo += sizeof(unsigned short);
		offsetRegViejo += sizeof(unsigned short);
	}	
	 
	for(++itTipos ;itTipos != listaTiposAtributos.end(); ++itTipos,++itValoresAtributos){
		regAttribute = *itTipos;
		tipo = regAttribute.tipo;
		valorModificado = *itValoresAtributos;
		bool estaModificado  = (valorModificado != CAMPO_NO_MODIFICADO);
		
		switch (tipo){
		
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
			if (estaModificado){
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
				memcpy(&registroNuevo[offsetRegNuevo], &registroViejo[offsetRegViejo], sizeof(unsigned short) + 2*sizeof(unsigned char));
				offsetRegNuevo += sizeof(unsigned short) + 2*sizeof(unsigned char);
			}
			
			offsetRegViejo += sizeof(unsigned short) + 2*sizeof(unsigned char);
		
			break;
		case TipoDatos::TIPO_STRING:
			unsigned short longCampoVariable;
			if (estaModificado){
				longCampoVariable = valorModificado.size(); 								
				// Guardo la longitud del campo variable en los dos bytes anteriores al dato
				memcpy(&registroNuevo[offsetRegNuevo],&longCampoVariable,sizeof(unsigned short));
				//Incremento el offset en dos bytes
				offsetRegNuevo += sizeof(unsigned short);
				// Guardo el campo variable
				memcpy(&registroNuevo[offsetRegNuevo],valorModificado.c_str(),longCampoVariable);
				// Incremento el offset del registro nuevo
				offsetRegNuevo += longCampoVariable;
				
				// Movimientos en el registro viejo
				memcpy(&longCampoVariable,&registroViejo[offsetRegViejo],sizeof(unsigned short));
				// Incremento el offset para apuntar al proximo campo por si lo tengo que guardar en el registro nuevo
				offsetRegViejo += sizeof(unsigned short) + longCampoVariable;
				
			}else{
				memcpy(&longCampoVariable,&registroViejo[offsetRegViejo],sizeof(unsigned short));
				// Incremento el offset en dos bytes que corresponden a la longitud del campo variable
				offsetRegViejo += sizeof(unsigned short);
				// Guardo la longitud del campo variable en el registro nuevo
				memcpy(&registroNuevo[offsetRegNuevo],&longCampoVariable,sizeof(unsigned short));
				offsetRegNuevo += sizeof(unsigned short);
				// Guardo el campo variable
				memcpy(&registroNuevo[offsetRegNuevo],&registroViejo[offsetRegViejo],longCampoVariable);
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
		}
		
	}
	
}

int DataManager::insertar(const DefinitionsManager::ListaValoresAtributos* listaVA,
				 		  const DefinitionsManager::ListaTiposAtributos* listaTipos, Bloque* bloque) {
	
	this->crearRegistroAlta(*listaVA, *listaTipos);
	return bloque->altaRegistro(listaTipos, this->registro); 
	
}

int DataManager::eliminar(DefinitionsManager::ListaClaves* listaClaves,
						  const DefinitionsManager::ListaTiposAtributos* listaTipos, Bloque* bloque) {
	
	return bloque->bajaRegistro(listaTipos, *ClaveFactory::getInstance().getClave(*listaClaves, *listaTipos));
	
}

int DataManager::modificar(const DefinitionsManager::ListaValoresAtributos* listaVA,
			  			   const DefinitionsManager::ListaTiposAtributos* listaTipos,
			  			   DefinitionsManager::ListaClaves* listaClaves, Bloque* bloque,
			  			   char* registroEnDisco) {
	
	this->crearRegistroModificacion(*listaTipos, *listaVA, registroEnDisco);
	unsigned short longReg;
	nodoLista nodo = *(listaTipos->begin());
	// Si el registro es fijo, obtengo la longitud del primer nodo de la lista de tipos.
	if (nodo.tipo == TipoDatos::TIPO_FIJO)
		longReg = atoi(nodo.pk.c_str());
	else
		memcpy(&longReg, this->registro, sizeof(unsigned short));
	
	return bloque->modificarRegistro(listaTipos, longReg, *ClaveFactory::getInstance().getClave(*listaClaves, *listaTipos), this->registro ); 

}

