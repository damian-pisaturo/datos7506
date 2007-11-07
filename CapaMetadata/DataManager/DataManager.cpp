#include "DataManager.h"
// listaAtributos { numeroNodoListaLoca:valor para modif}
DataManager::DataManager()
{
	this->registro = NULL;
}

DataManager::~DataManager()
{
	if(this->registro)
		this->registro = NULL;
}

void DataManager::crearRegistroAlta(DefinitionsManager::ListaValoresAtributos &listaVA,
									DefinitionsManager::ListaTiposAtributos &listaTipos) {
	
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
			int *entero = (int*)campoRegistro.c_str();
			memcpy(&registro[offsetToCampo],entero,sizeof(int));
			offsetToCampo += sizeof(int);
		}
		else if(tipo == TipoDatos::TIPO_FECHA){
			ClaveFecha::TFECHA *fecha = (ClaveFecha::TFECHA*)campoRegistro.c_str();
			memcpy(&registro[offsetToCampo],fecha,sizeof(ClaveFecha::TFECHA));
			offsetToCampo += sizeof(ClaveFecha::TFECHA);
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
			bool *campoBooleano = (bool*)campoRegistro.c_str();
			memcpy(&registro[offsetToCampo],campoBooleano,sizeof(bool));
			offsetToCampo += sizeof(bool);
		}
	}
	if(this->registro){
		delete[] this->registro;
		this->registro = NULL;
	}
	this->registro = registro;
}	
unsigned short DataManager::getTamanioRegistro(DefinitionsManager::ListaTiposAtributos &listaTiposAtributos,
											   DefinitionsManager::ListaValoresAtributos &listaVA){
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
					tamanioRegistro += sizeof(ClaveFecha::TFECHA);
				
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
void DataManager::crearRegistroModificacion(DefinitionsManager::ListaTiposAtributos &listaTiposAtributos,
											DefinitionsManager::ListaValoresAtributos &listaVA,
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
	// Obtengo el tama�o del nuevo registro, reservo espacio para el msimo y lo genero con las nuevas modificaciones
	else{
			// Incremento el offset en dos bytes, que son los que ocupan la longitud del registro
			offsetRegDisco += sizeof(unsigned short);
			longRegNuevo += sizeof(unsigned short);
			
			
			// Itero las listas para obtener el tama�o del nuevo registro y reservar memoria para el mismo
			for(++itTipos ;itTipos != listaTiposAtributos.end(); ++itTipos,++itValoresAtributos){
				regAttribute = *itTipos;
				tipo = regAttribute.tipo;
				
				if(tipo == TipoDatos::TIPO_ENTERO){
					longRegNuevo += sizeof(int);
					offsetRegDisco += sizeof(int);
				}
				else if(tipo == TipoDatos::TIPO_FECHA){
					longRegNuevo += sizeof(ClaveFecha::TFECHA);
					offsetRegDisco += sizeof(ClaveFecha::TFECHA);
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
				else if(tipo == TipoDatos::TIPO_BOOL)
					longRegNuevo += sizeof(bool);				
			}
	}
	
	// una vez que ya se la longitud del nuevo registro reservo espacio para el mismo
	char *registroNuevo = new char [longRegNuevo];
	//TODO Esto no compila ni un poco. Notifiquese al palurdo que commiteo (tambien conocido como Laura "In Vitro" Rodriguez).
	//this->generarRegistroModificado(registroNuevo,registroEnDisco,longRegNuevo,listaTiposAtributos,listaVA);
	if(this->registro){
		delete[] this->registro;
		this->registro = NULL;
	}
	this->registro = registroNuevo;
}

/*
 * Genera el nuevo registro con las modificaciones correspondientes
 * */
void generarRegistroModificado(char *registroNuevo, char *registroViejo, unsigned short longNuevoReg, 
							   DefinitionsManager::ListaTiposAtributos &listaTiposAtributos,
		   					   DefinitionsManager::ListaValoresAtributos &listaVA
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
	void *campo;
	
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
		
		campo = &registroViejo[offsetRegViejo];	
		
		switch (tipo){
		
		case TipoDatos::TIPO_ENTERO:
			if (estaModificado)
				// Si el campo debe ser modificado, obtiene el nuevo valor.
				*((int*)campo) = atoi(valorModificado.c_str());	
			
			memcpy(&registroNuevo[offsetRegNuevo],campo,sizeof(int));
			
			offsetRegNuevo += sizeof(int);
			offsetRegViejo += sizeof(int);
			
			break;		
		case TipoDatos::TIPO_BOOL:
			if (estaModificado){
				if (valorModificado == "TRUE") *((bool*)campo) = true;
				else *((bool*)campo) = false;
			}
			
			memcpy(&registroNuevo[offsetRegNuevo],campo,sizeof(bool));
			
			offsetRegNuevo += sizeof(bool);
			offsetRegViejo += sizeof(bool);
			
			break;		
		case TipoDatos::TIPO_CHAR:
			if (estaModificado)
				// Si el campo debe ser modificado, obtiene el nuevo valor.
				*((char*)campo) = valorModificado[0]; 						
			
			memcpy(&registroNuevo[offsetRegNuevo],campo,sizeof(char));
			
			offsetRegNuevo += sizeof(char);
			offsetRegViejo += sizeof(char);
			
			break;		

		case TipoDatos::TIPO_FECHA:
			if (estaModificado){
				//TODO: IMPLEMENTAR!
				//int valor = 0;
				
			
				// Si el campo debe ser modificado, obtiene el nuevo valor.
				//*((char*)campo) = valorModificado[0];
			}
			
			//memcpy(&registroNuevo[offsetRegNuevo],campo,sizeof(char));
			
			//offsetRegNuevo += sizeof(int);
			//offsetRegViejo += sizeof(int);
			
						
			break;
		case TipoDatos::TIPO_STRING:
			unsigned short longCampoVariable;
			if (estaModificado){
				longCampoVariable = valorModificado.size(); 								
			}else{
				memcpy(&longCampoVariable,&registroViejo[offsetRegViejo],sizeof(unsigned short));
			} 
			memcpy(&registroNuevo[offsetRegNuevo],&longCampoVariable,sizeof(unsigned short));			
			offsetRegNuevo += Tamanios::TAMANIO_LONGITUD;
			offsetRegViejo += Tamanios::TAMANIO_LONGITUD;
			break;
		case TipoDatos::TIPO_SHORT:
			if (estaModificado){
													
			}else{
				
			}
			break;
		default:
			
			break;
		}
		
	}
	
}

int DataManager::insertar(const string& nombreTipo, const DefinitionsManager::ListaValoresAtributos* listaVA,
				 		  const DefinitionsManager::ListaTiposAtributos* listaTipos) {
	
	return 0;
	
}

int DataManager::eliminar(const string& nombreTipo, DefinitionsManager::ListaClaves* listaClaves) {
	
	return 0;
	
}

int DataManager::modificar(const string& nombreTipo, const DefinitionsManager::ListaValoresAtributos* listaVA,
			  			   const DefinitionsManager::ListaTiposAtributos* listaTipos,
			  			   DefinitionsManager::ListaClaves* listaClaves) {
	
	return 0;
}

