///////////////////////////////////////////////////////////////////////////
//	Archivo   : Bloque.cpp
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clase Bloque.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#include "Bloque.h"


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Bloque (Implementa manejo y control de bloques en memoria)
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructores/Destructor
	///////////////////////////////////////////////////////////////////////

		/* 
		 * Crea un Bloque vacio.
		 * Sirve para levantar bloques de disco sin pisar su información.
		 **/
		Bloque::Bloque() 
		{
			this->datos   = NULL;
			this->tamanio = 0;
			this->numero  = 0;
			this->offsetADatos = Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS;
		}


		/**
		 * Este constructor recibe el número de bloque dentro del archivo, y el tamaño del 
		 * bloque medido en bytes.
		 */
		Bloque::Bloque(unsigned int numeroBloque, unsigned int tamanioBloque) 
		{
			this->numero       = numeroBloque;
			this->tamanio      = tamanioBloque;
			this->datos        = new char[tamanioBloque];
			
			memset(datos, 0, this->getTamanioBloque());
			
			this->offsetADatos = Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS;
			
			// Inicializa el offset a espacio libre dentro del bloque.
			unsigned short espLibre = this->offsetADatos;
			memcpy(this->datos, &espLibre, Tamanios::TAMANIO_ESPACIO_LIBRE);
			
			// Inicializa la cantidad de registros dentro del bloque.
			unsigned short cantRegs = 0;
			memcpy(this->datos + Tamanios::TAMANIO_ESPACIO_LIBRE, &cantRegs,
				   Tamanios::TAMANIO_CANTIDAD_REGISTROS);
		}
		
		Bloque::~Bloque() 
		{
			if (this->datos)
				delete[] this->datos;
		}

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		/*
		 * Este metodo busca un registro en el bloque segun su clavePrimaria. Si lo encuentra
		 * devuelve true y su offset dentro del bloque en "offsetReg"; y si no lo encuentra
		 * devuelve false.
		 **/
		bool Bloque::buscarRegistro(const ListaNodos *listaParam, Clave &clave,
									unsigned short *offsetReg) 
		{
			void** clavePrimaria = clave.getValorParaHash();
			
			// Obtengo el offset a los registros
			int offsetToReg       = this->getOffsetADatos();
			int offsetToProxCampo = 0;
		
			
			// Se obtiene la cantidad de registros dentro del bloque.
			unsigned short cantRegistros = 0;
	
			memcpy(&cantRegistros, &(this->datos[Tamanios::TAMANIO_ESPACIO_LIBRE]), Tamanios::TAMANIO_CANTIDAD_REGISTROS);
		
			unsigned char cantClaves       = 0;
			unsigned char clavesChequeadas = 0;
			unsigned char clavesIguales    = 0;
			bool encontrado = false;
			bool checkPk    = false; // Indica si ya reviso la clave primaria del registro.
			char *registro  = NULL;
			ListaNodos::const_iterator it;
			nodoLista regAttribute;
			int tipo           = 0; //Indica si el tipo de campo del registro es variable o fijo
			int tipoDeRegistro = 0; // Indica si el registro es variable o fijo
			string pk;
			unsigned short longRegistro = 0;
			unsigned char longCampo    = 0;
			unsigned short bytesLong    = 0;
			
			//Valores de los distintos tipos
			//de campos existentes.
			char *campo          = NULL;
			float campoNumerico  = 0;
			int campoNumericoInt = 0;
			short campoShort     = 0;
		
			int i = 1;
			it = listaParam->begin();
			regAttribute = *it;
		
			// Se obtiene el tipo de atributo del registro.
			tipoDeRegistro = regAttribute.tipo;
			// Se obtiene la cantidad de claves primarias.
			cantClaves = regAttribute.cantClaves;
		
			// Mientras haya mas registros y no lo haya encontrado.
			while ( (i < cantRegistros + 1) && (!encontrado)) {
		
				// Resetea los atributos que sirven para el control de la busqueda dentro de un registro.
				offsetToProxCampo = 0;
				checkPk           = false;
				clavesChequeadas  = 0;
				clavesIguales     = 0;
		
				if (tipoDeRegistro == TipoDatos::TIPO_VARIABLE) {
					bytesLong = Tamanios::TAMANIO_LONGITUD;
					//Se obtiene la longitud del registro.
					memcpy(&longRegistro, &(this->datos[offsetToReg]),
							Tamanios::TAMANIO_LONGITUD);
					
				} else
					// Se obtiene la longitud del registro.
					// En este caso la longitud del registro fijo viene dada en el primer nodo de la lista
					// en pk.
					longRegistro = ((unsigned short)atoi(regAttribute.pk.c_str()));
		
				// Se obtiene el registro (sin incluir la longitud del mismo)
				registro = this->getRegistro(longRegistro, offsetToReg + bytesLong);
		
				// Se itera la lista de atributos del registro.
				// Se arranco del segundo nodo ya que el primero se utiliza para guardar
				// si el registro es de longitud fija o variable.
				for (it = (++listaParam->begin()); ((it != listaParam->end())
						&& (!checkPk)); ++it) {
					regAttribute = *it;
		
					// Se obtiene el tipo de atributo del registro.
					tipo = regAttribute.tipo;
		
					// Se obtiene el indicador de clave primaria.
					pk = regAttribute.pk;
		
					if (tipo == TipoDatos::TIPO_STRING) {
		
						// Se obtiene la longitud del campo variable.
						memcpy(&longCampo, &registro[offsetToProxCampo],
								Tamanios::TAMANIO_LONGITUD_CADENA);
		
						offsetToProxCampo += Tamanios::TAMANIO_LONGITUD_CADENA;
		
						// Si es pk, se obtiene el campo en sí.
						if (pk == "true") {
							campo = new char[longCampo +1];
							memcpy(campo, &registro[offsetToProxCampo], longCampo);
							*(campo + longCampo) = 0;
							// Se compara si las claves son iguales.
							if (strcmp((char*)clavePrimaria[clavesChequeadas], campo)
									== 0) {
								*offsetReg = offsetToReg;
								clavesIguales++;
							}
							clavesChequeadas++;
							
							delete[]campo;
						}
		
						// Se setea el nuevo offset del próximo campo para la siguiente iteración.
						offsetToProxCampo += longCampo;
					} else if (tipo == TipoDatos::TIPO_ENTERO) {
						
						if (pk == "true") {
							memcpy(&campoNumericoInt, &registro[offsetToProxCampo],
									sizeof(int));
						
							if (campoNumericoInt
									== *((int*)clavePrimaria[clavesChequeadas])) {
								*offsetReg = offsetToReg;
								clavesIguales++;
							}
							clavesChequeadas++;
						}
						offsetToProxCampo += sizeof(int);
					} else if (tipo == TipoDatos::TIPO_SHORT) {
						if (pk == "true") {
							memcpy(&campoShort, &registro[offsetToProxCampo], sizeof(short int));
		
							if (campoShort == *((short int*)clavePrimaria[clavesChequeadas])) {
								
								*offsetReg = offsetToReg;
								clavesIguales++;
							}
							clavesChequeadas++;
						}
						offsetToProxCampo += sizeof(short int);
					} else if (tipo == TipoDatos::TIPO_FLOAT) {
						if (pk == "true") {
							memcpy(&campoNumerico, &registro[offsetToProxCampo],
									sizeof(float));
		
							if (campoNumerico == *((float*)clavePrimaria[clavesChequeadas])) {
								*offsetReg = offsetToReg;
								clavesIguales++;
							}
							clavesChequeadas++;
						}
						offsetToProxCampo += sizeof(float);
					} else if (tipo == TipoDatos::TIPO_CHAR) {
						if (pk == "true") {
							campo = new char[2];
							campo[0] = registro[offsetToProxCampo];
							*(campo +1) = 0;
							if (campo[0] == ((char*)clavePrimaria[clavesChequeadas])[0]) {
								*offsetReg = offsetToReg;
								clavesIguales++;
							}
							clavesChequeadas++;
							delete[]campo;
						}
						offsetToProxCampo += sizeof(char);
					} else if (tipo == TipoDatos::TIPO_FECHA) {
						if (pk == "true") {
							ClaveFecha::TFECHA fecha;
							memcpy(&fecha, &registro[offsetToProxCampo],Tamanios::TAMANIO_FECHA);
							ClaveFecha::TFECHA clave;
							memcpy(&clave, clavePrimaria[clavesChequeadas], Tamanios::TAMANIO_FECHA);
		
							if ((fecha.anio == clave.anio)&&(fecha.mes == clave.mes)
									&&(fecha.dia == clave.dia)) {
								*offsetReg = offsetToReg;
								clavesIguales++;
							}
							clavesChequeadas++;
						}
						offsetToProxCampo += Tamanios::TAMANIO_FECHA;
					}
					if (clavesChequeadas == cantClaves) {
						checkPk = true;
						if (clavesChequeadas == clavesIguales)
							encontrado = true;
					}
				}
				i++;
				//La próxima iteración se levantará la longitud del registro siguiente
				offsetToReg += bytesLong + longRegistro;
				
				if (registro) delete []registro;
			}
			
			delete[] clavePrimaria;
			return encontrado;
		}

		/*
		 * Este metodo reorganiza el bloque luego de una baja.
		 * longReg incluye los bytes utilizados para la longitud del registro en caso
		 * de que el mismo sea variable.
		 **/
		void Bloque::organizarBloque(int offsetToReg, int longReg)
		{		
			char* datosAux = new char[this->tamanio];
			unsigned short espLibre = 0;
			unsigned short cantRegs = 0;
		
			// Levanta el espacio libre en el bloque.
			memcpy(&espLibre, this->datos, Tamanios::TAMANIO_ESPACIO_LIBRE);
		
			// Levanta la cantidad de registros del bloque.
			memcpy(&cantRegs, &this->datos[Tamanios::TAMANIO_ESPACIO_LIBRE],
					Tamanios::TAMANIO_CANTIDAD_REGISTROS);
		
			// Se hace una copia del bloque omitiendo el bloque borrado.
			memcpy(datosAux, this->datos, offsetToReg);
			memcpy(&datosAux[offsetToReg], &this->datos[offsetToReg+longReg], this->tamanio
					-offsetToReg-longReg);
		
			// Se actualizan los valores de espacio libre y cantidad de registros.
			// Se resta la longitud del registro porque espLibre contiene el offset al espacio libre
			// y no el espacio libre en sí.
			espLibre -= longReg;
			cantRegs -= 1;
			memcpy(datosAux, &espLibre, Tamanios::TAMANIO_ESPACIO_LIBRE);
			memcpy(&datosAux[Tamanios::TAMANIO_ESPACIO_LIBRE], &cantRegs,
					Tamanios::TAMANIO_CANTIDAD_REGISTROS);
		
			// Se actualizan los datos.
			this->setDatos(datosAux);
		}

		/*
		 * Inserta un nuevo registro dentro del bloque.
		 * Retorna true si la inserción fue exitosa, o false en caso contrario
		 * No comtempla el caso de claves repetidas.
		 **/
		int Bloque::altaRegistro(const ListaNodos * listaParam, char *registro) 
		{
			int resultado = ResultadosIndices::OK;
			unsigned short offsetEspLibre = 0;
			unsigned short longReg        = 0;
		
			// Se obtiene la longitud del registro, no incluye los bytes de longitud del mismo.
			longReg = getTamanioRegistros(listaParam, registro);
			
			// Se obtiene el offset al espacio libre.
			memcpy(&offsetEspLibre, this->datos, Tamanios::TAMANIO_ESPACIO_LIBRE);
		
			// Si el registro tiene espacio dentro del bloque se realiza la inserción.
			if (verificarEspacioDisponible(longReg, offsetEspLibre)) {
				ListaNodos::const_iterator it;
				it = listaParam->begin();
				
				// Si el registro es de longitud variable le sumo la cantidad de bytes de la longitud 
				// del mismo.
				if (it->tipo == TipoDatos::TIPO_VARIABLE)
					longReg += Tamanios::TAMANIO_LONGITUD;
		
				// Registro incluye su longitud en los primeros 2 bytes.
				insertarRegistro(registro, offsetEspLibre, longReg);
				
			}else resultado = ResultadosIndices::SOBREFLUJO;
			
			return resultado;
		}

		/*
		 * Da de baja dentro del bloque al registro cuya clave es clavePrimaria.
		 **/
		int Bloque::bajaRegistro(const ListaNodos *listaParam, Clave &clave) 
		{			
			// TODO Que alguien, por favor, me explique por que este metodo repite exactamente
			// lo mismo que el buscarRegistro(). Si no entiendo mal, la baja debiera ser buscar
			// el registro con la clave usando el buscarRegistro(), levantar su longitud, y armarse
			// un nuevo bloque que no contenga los bytes de ese registro, alineando "a izquierda". 
			
			void** clavePrimaria = clave.getValorParaHash();
			unsigned short offsetToReg = this->offsetADatos;
			unsigned short offsetToProxCampo = 0;
		
			unsigned short longReg       = 0;
			unsigned short cantRegistros = 0;
			bool registroBorrado         = false;
			char *registro               = NULL;
			ListaNodos::const_iterator it = listaParam->begin();
			nodoLista regAtribute;
			int tipoRegistro = 0; // Indica si el registro es variable o fijo
			int tipo         = 0; // Indica el tipo de dato del campo de un registro
			string pk;
			bool checkPk = false;
			unsigned char longCampo  = 0;
			int campoNumericoInt      = 0;
			float campoNumerico       = 0;
			unsigned short campoShort = 0;
			unsigned short bytesLongitud = 0;
			unsigned char cantClaves     = 0;
			unsigned char clavesChequeadas = 0;
			unsigned char clavesIguales    = 0;
		
			// Se obtiene la cantidad de registros dentro del bloque.
			memcpy(&cantRegistros, &datos[Tamanios::TAMANIO_ESPACIO_LIBRE],
					Tamanios::TAMANIO_CANTIDAD_REGISTROS);
		
			// Se obtiene la longitud del registro. (no incluye lo 2 bytes de longitud si es variable.)
			longReg = getTamanioRegistros(listaParam, &datos[offsetToReg]);
		
			regAtribute = *it;
			// Se obtiene el tipo de atributo del registro.
			tipoRegistro = regAtribute.tipo;
		
			// Se obtiene la cantidad de claves primarias.
			cantClaves = regAtribute.cantClaves;
		
			int i = 1;
			// Mientras no se borre al registro y haya más registros..
			while ( (i<cantRegistros + 1) && (!registroBorrado)) {
				// Reseteo el indicador de corte del for
				checkPk = false;
				clavesChequeadas = 0;
				clavesIguales = 0;
		
				// Reseteo el iterador en las sucesivas iteraciones a la primera
				if (i>1)
					it = listaParam->begin();
		
				// Se obtiene la longitud del registro.
				longReg = getTamanioRegistros(listaParam, &datos[offsetToReg]);
		
				registro = new char[longReg];
		
				if (tipoRegistro == TipoDatos::TIPO_VARIABLE) {
					// Se omite la longitud del registro.
					offsetToReg += Tamanios::TAMANIO_LONGITUD;
					bytesLongitud = Tamanios::TAMANIO_LONGITUD;
				}
		
				// Se obtiene el registro.
				registro = getRegistro(longReg, offsetToReg);
		
				//Itero la lista de atributos del registro
				for (++it; ((it != listaParam->end()) && (!checkPk)); ++it) {
					regAtribute = *it;
		
					//Obtengo el tipo de atributo del registro
					tipo = regAtribute.tipo;
					//Obtengo el indicador de clave primaria
					pk = regAtribute.pk;
					if (tipo == TipoDatos::TIPO_STRING) {
		
						//obtengo la longitud del campo variable
						memcpy(&longCampo, &registro[offsetToProxCampo],
								Tamanios::TAMANIO_LONGITUD_CADENA);
						offsetToProxCampo += Tamanios::TAMANIO_LONGITUD_CADENA;
						//Si es pk, me preocupo por obtener el campo en si
		
						if (pk == "true") {
							char* campo = new char[longCampo +1];
							memcpy(campo, &registro[offsetToProxCampo], longCampo);
							*(campo + longCampo) = 0;
		
							if (strcmp((char*)clavePrimaria[clavesChequeadas], campo)
									== 0)
								clavesIguales++;
		
							clavesChequeadas++;
							delete[]campo;
						}
						//Seteo el nuevo offset del próximo campo para la siguiente iteración
						offsetToProxCampo += longCampo;
					} else if (tipo == TipoDatos::TIPO_ENTERO) {
		
						if (pk == "true") {
							memcpy(&campoNumericoInt, &registro[offsetToProxCampo], sizeof(int));
		
							if (campoNumericoInt == *((int*)clavePrimaria[clavesChequeadas]))
								clavesIguales++;
		
							clavesChequeadas++;
						}
						offsetToProxCampo += sizeof(int);
					} else if (tipo == TipoDatos::TIPO_SHORT) {
						if (pk == "true") {
							memcpy(&campoShort, &registro[offsetToProxCampo],
									sizeof(short int));
							if (campoShort == *((short int*)clavePrimaria[clavesChequeadas]))
								clavesIguales++;
		
							clavesChequeadas++;
						}
						offsetToProxCampo += sizeof(short int);
					} else if (tipo == TipoDatos::TIPO_FLOAT) {
						if (pk == "true") {
							memcpy(&campoNumerico, &registro[offsetToProxCampo],
									sizeof(float));
							if (campoNumerico
									== *((float*)clavePrimaria[clavesChequeadas]))
								clavesIguales++;
		
							clavesChequeadas++;
						}
						offsetToProxCampo += sizeof(float);
					} else if (tipo == TipoDatos::TIPO_CHAR) {
						if (pk == "true") {
							char* campo = new char[2];
							campo[0] = registro[offsetToProxCampo];
							// Agrega el '\0'
							*(campo + 1) = 0;
		
							if (campo[0] == ((char*)clavePrimaria[clavesChequeadas])[0])
								clavesIguales++;
		
							clavesChequeadas++;
							delete[]campo;
						}
						offsetToProxCampo += sizeof(char);
					} else if (tipo == TipoDatos::TIPO_FECHA) {
						if (pk == "true") {
							ClaveFecha::TFECHA fecha;
							memcpy(&fecha, &registro[offsetToProxCampo], Tamanios::TAMANIO_FECHA);
							ClaveFecha::TFECHA clave;
							memcpy(&clave, clavePrimaria[clavesChequeadas],Tamanios::TAMANIO_FECHA);
		
							if ((fecha.anio == clave.anio)&&(fecha.mes == clave.mes)
									&&(fecha.dia == clave.dia))
								clavesIguales++;
							clavesChequeadas++;
						}
						offsetToProxCampo += Tamanios::TAMANIO_FECHA;
					}
					if (clavesChequeadas == cantClaves) {
						checkPk = true;
						if (clavesChequeadas == clavesIguales) {
							organizarBloque(offsetToReg-bytesLongitud, longReg
									+ bytesLongitud);
							registroBorrado = true;
						}
					}
				}
				// La próxima iteración se levantará la longitud del registro siguiente
				offsetToReg += longReg;
				// Incremento el contador de registros consultados
				i++;
				// Reseteo el offset al prox campo dentro del registro
				offsetToProxCampo = 0;
				delete []registro;
			}
			
			delete[] clavePrimaria;
			
			if (!registroBorrado)
				return ResultadosIndices::REGISTRO_NO_ENCONTRADO;
			
			return ResultadosIndices::OK;
		}

		int Bloque::modificarRegistro(const ListaNodos *listaParam,
				unsigned short longReg, Clave &clavePrimaria, char* registro) 
		{	
			int resultado = ResultadosIndices::OK;
			unsigned short offsetReg = 0;
		
			if (buscarRegistro(listaParam, clavePrimaria, &offsetReg)){				
		
				// Levanta la longitud del registro original.
				unsigned short longRegOrig = getTamanioRegistros(listaParam, &this->datos[offsetReg]);
				memcpy(&longRegOrig, &this->datos[offsetReg], Tamanios::TAMANIO_LONGITUD);
			
				unsigned short offsetEspLibre = 0;
				memcpy(&offsetEspLibre, this->datos, Tamanios::TAMANIO_ESPACIO_LIBRE);
			
				if ((getTamanioBloque()- offsetEspLibre + longRegOrig) < longReg)
					resultado = ResultadosIndices::SOBREFLUJO;
				else{				
					bajaRegistro(listaParam, clavePrimaria);
					altaRegistro(listaParam, registro);
				}
			
			}else resultado = ResultadosIndices::REGISTRO_NO_ENCONTRADO;
			
			return resultado;
		}

		
		/*
		 * Devuelve true si se puede insertar un registro de longitud "longReg".
		 * De lo contrario devuelve false.
		 **/
		bool Bloque::verificarEspacioDisponible(unsigned short longReg, unsigned short offsetEspLibre) 
		{
			return ( (getTamanioBloque() - offsetEspLibre) > longReg);
		}

		void Bloque::insertarRegistro(char *registro, unsigned short offsetEspLibre, unsigned short longReg) 
		{
			memcpy(&this->datos[offsetEspLibre], registro, longReg);
			
			// Actualiza el offset al espacio libre.
			offsetEspLibre += longReg;
			memcpy(this->datos, &offsetEspLibre, Tamanios::TAMANIO_ESPACIO_LIBRE);
		
			// Actualiza la cantidad de registros.
			unsigned short cantRegs = 0;
			memcpy(&cantRegs, &this->datos[Tamanios::TAMANIO_ESPACIO_LIBRE],
					Tamanios::TAMANIO_CANTIDAD_REGISTROS);
			
			cantRegs++;
			
			memcpy(&datos[Tamanios::TAMANIO_ESPACIO_LIBRE], &cantRegs,
					Tamanios::TAMANIO_CANTIDAD_REGISTROS);
		}

		/*
		 * Este método devuelve el registro que empieza en datos[offsetToRegs] y tiene longitud longReg.
		 **/
		char* Bloque::getRegistro(int longReg, int offsetToReg) 
		{
			char* registro = new char [longReg];
			
			memcpy(registro, &this->datos[offsetToReg], longReg);
			
			return registro;
		}

		/*
		 * Este método recibe un registro, y retorna su clave primaria.
		 **/
		Clave* Bloque::getClavePrimaria(const ListaNodos *listaParam, char* registro) 
		{
			ListaClaves listaClaves;
			int offsetToProxCampo = 0;
			ListaNodos::const_iterator it;
			nodoLista regAtribute;
			int tipo = 0; //Indica si el tipo de campo del registro es variable o fijo
			int tipoDeRegistro = 0; // Indica si el registro es variable o fijo
			string pk, str;
			unsigned char longCampo = 0;
			char *campo = NULL;
			char campoChar       = 0;
			float campoNumerico  = 0;
			int campoNumericoInt = 0;
			short int campoShort = 0;
			ClaveFecha::TFECHA campoFecha;
			unsigned char cantClaves        = 0;
			unsigned char clavesEncontradas = 0;			
		
			it = listaParam->begin();
			regAtribute = *it;
		
			cantClaves = regAtribute.cantClaves;
		
			tipoDeRegistro = regAtribute.tipo;
			if (tipoDeRegistro == TipoDatos::TIPO_VARIABLE)
				// Se omite la longitud del registro.
				offsetToProxCampo += 2;
		
			// Se itera la lista de atributos del registro.
			// Se arranca del segundo nodo ya que el primero se utiliza para guardar
			// si el registro es de longitud fija o variable.
			for (it = (++listaParam->begin()); it != listaParam->end(); ++it) {
				regAtribute = *it;
		
				// Se obtiene el tipo de atributo del registro.
				tipo = regAtribute.tipo;
		
				// Se obtiene el indicador de clave primaria.
				pk = regAtribute.pk;
		
				if (tipo == TipoDatos::TIPO_STRING) {
					// Se obtiene la longitud del campo variable.
					memcpy(&longCampo, &registro[offsetToProxCampo],
							Tamanios::TAMANIO_LONGITUD_CADENA);
		
					offsetToProxCampo += Tamanios::TAMANIO_LONGITUD_CADENA;
		
					// Si es pk, se obtiene el campo en sí.
					if (pk == "true") {
						campo = new char[longCampo +1];
						memcpy(campo, &registro[offsetToProxCampo], longCampo);
						*(campo + longCampo) = 0;
						str = campo;
						listaClaves.push_back(new ClaveVariable(str));
						
						clavesEncontradas++;
						
						if (campo) delete []campo;
					}
					// Se setea el nuevo offset del próximo campo para la siguiente iteración .
					offsetToProxCampo += longCampo;
				} else if (tipo == TipoDatos::TIPO_ENTERO) {
					if (pk == "true") {
						memcpy(&campoNumericoInt, &registro[offsetToProxCampo], sizeof(int));
						listaClaves.push_back(new ClaveEntera(campoNumericoInt));
						clavesEncontradas++;
					}
		
					offsetToProxCampo += sizeof(int);
				} else if (tipo == TipoDatos::TIPO_SHORT) {
					if (pk == "true") {
						memcpy(&campoShort, &registro[offsetToProxCampo],
								sizeof(short int));
						listaClaves.push_back(new ClaveShort(campoShort));
						clavesEncontradas++;
					}
					offsetToProxCampo += sizeof(short int);
				} else if (tipo == TipoDatos::TIPO_FLOAT) {
					if (pk == "true") {
						memcpy(&campoNumerico, &registro[offsetToProxCampo],
								sizeof(float));
						
						listaClaves.push_back(new ClaveReal(campoNumerico));
						
						clavesEncontradas++;
					}
					offsetToProxCampo += sizeof(float);
				} else if (tipo == TipoDatos::TIPO_CHAR) {
					if (pk == "true") {
						memcpy(&campoChar, &registro[offsetToProxCampo], sizeof(char));
						listaClaves.push_back(new ClaveChar(campoChar));
						
						clavesEncontradas++;
					}
					offsetToProxCampo += sizeof(char);
				} else if (tipo == TipoDatos::TIPO_FECHA) {
					if (pk == "true") {
						
						memcpy(&campoFecha.anio,&registro[offsetToProxCampo],sizeof(unsigned short));
						offsetToProxCampo += sizeof(unsigned short);
						memcpy(&campoFecha.mes, &registro[offsetToProxCampo], sizeof(unsigned char));
						offsetToProxCampo += sizeof(unsigned char);
						memcpy(&campoFecha.dia,&registro[offsetToProxCampo],sizeof(unsigned char));
						offsetToProxCampo += sizeof(unsigned char);
		
						listaClaves.push_back(new ClaveFecha(campoFecha));
						
						clavesEncontradas++;
					}
					else  
						offsetToProxCampo += Tamanios::TAMANIO_FECHA;
				}
			}
			
			if (cantClaves == 1){
				ListaClaves::const_iterator iter = listaClaves.begin();
				return *iter;
			}
			
			return new ClaveCompuesta(listaClaves);
		}

		/*
		 * Devuelve la longitud del registro, sin incluir los 2 bytes de la longitud.
		 **/
		unsigned short Bloque::getTamanioRegistros(const ListaNodos * listaParam, char *registro) 
		{
			ListaNodos::const_iterator it = listaParam->begin();
			nodoLista regAtribute = *it;
		
			// Se obtiene el tipo de atributo del registro.
			int tipo = regAtribute.tipo;
			unsigned short longReg = 0;
		
			if (tipo == TipoDatos::TIPO_VARIABLE)
				// Se obtiene la longitud del registro variable.
				memcpy(&longReg, registro, Tamanios::TAMANIO_LONGITUD);
			else
				// Se obtiene la longitud del registro fijo.
				longReg = ((unsigned short)atoi(regAtribute.pk.c_str()));
		
			return longReg;
		}

		unsigned int Bloque::getTamanioBloque() 
		{
			return this->tamanio;
		}

		void Bloque::setTamanioBloque(unsigned int tam) 
		{
			this->tamanio = tam;
		}

		char * Bloque::getDatos() 
		{
			return this->datos;
		}
	
		void Bloque::setDatos(char* data) 
		{
			if (this->datos)
				delete[] this->datos;
			
			this->datos = data;
		}
	
		unsigned int Bloque::getNroBloque() 
		{
			return this->numero;
		}
	
		void Bloque::setNroBloque(unsigned int num) 
		{
			this->numero = num;
		}
	
		unsigned short Bloque::getOffsetADatos() const 
		{
			return this->offsetADatos;
		}
	
		void Bloque::setOffsetADatos(unsigned short offset)
		{
			this->offsetADatos = offset;
		}
