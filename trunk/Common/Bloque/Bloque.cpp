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
			this->offsetToProxReg = this->offsetADatos;
			this->tipoOrganizacion = 0;
		}
		
		/*
		 * Crea un bloque vacio.
		 * Levanta bloques de disco y setea elt tipo de organizacion del mismo, se utiliza para
		 * listar todos los registros del bloque
		 * */
		Bloque::Bloque(unsigned int numBloque, unsigned int tamanioBloque, int tipoOrga) 
		{
			this->numero       = numBloque;
			this->tamanio      = tamanioBloque;
			
			this->datos        = new char[tamanioBloque];
			memset(this->datos, 0, this->getTamanioBloque());
			
			this->offsetADatos = Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS;
			this->offsetToProxReg = this->offsetADatos;
			this->tipoOrganizacion = tipoOrga;
			
			// Inicializa el offset a espacio libre dentro del bloque.
			unsigned short espLibre = this->offsetADatos;
			memcpy(this->datos, &espLibre, Tamanios::TAMANIO_ESPACIO_LIBRE);
			
			// Inicializa la cantidad de registros dentro del bloque.
			unsigned short cantRegs = 0;
			memcpy(this->datos + Tamanios::TAMANIO_ESPACIO_LIBRE, &cantRegs, Tamanios::TAMANIO_CANTIDAD_REGISTROS);
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
			
			memset(this->datos, 0, this->getTamanioBloque());
			
			this->offsetADatos = Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS;
			this->offsetToProxReg = this->offsetADatos;
			this->tipoOrganizacion = 0;
			
			// Inicializa el offset a espacio libre dentro del bloque.
			unsigned short espLibre = this->offsetADatos;
			memcpy(this->datos, &espLibre, Tamanios::TAMANIO_ESPACIO_LIBRE);
			
			// Inicializa la cantidad de registros dentro del bloque.
			unsigned short cantRegs = 0;
			memcpy(this->datos + Tamanios::TAMANIO_ESPACIO_LIBRE, &cantRegs, Tamanios::TAMANIO_CANTIDAD_REGISTROS);
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
		bool Bloque::buscarRegistro(const ListaInfoRegistro *listaInfoRegistro, Clave &clave,
									unsigned short *offsetReg) 
		{
			void** clavePrimaria = clave.getValorParaHash();
			
			// Obtengo el offset a los registros
			int offsetToReg       = this->getOffsetADatos();
			int offsetToProxCampo = 0;
		
			
			// Se obtiene la cantidad de registros dentro del bloque.
			unsigned short cantRegistros = 0;
	
			memcpy(&cantRegistros, &(this->datos[Tamanios::TAMANIO_ESPACIO_LIBRE]), Tamanios::TAMANIO_CANTIDAD_REGISTROS);
		
			unsigned char cantClaves       = 0; // Indica la cantidad de claves por las que estan indexados los registros dentro
												// del bloque.
			unsigned char clavesChequeadas = 0; // Indica la cantidad de claves que han sido comparadas. Sirve para el caso de las
												// claves compuestas.
			unsigned char clavesIguales    = 0;
			bool encontrado = false; // Indica si se encontro el registro buscado.
			bool checkPk    = false; // Indica si ya reviso la clave primaria del registro.
			char *registro  = NULL;
			ListaInfoRegistro::const_iterator it;
			NodoInfoRegistro regAttribute;
			unsigned char tipo  = 0; //Indica si el tipo de campo del registro es variable o fijo
			unsigned char tipoDeRegistro = 0; // Indica si el registro es variable o fijo
			bool pk;
			unsigned short longRegistro = 0;
			unsigned char longCampo    = 0;
			unsigned short bytesLong    = 0;
			
			//Valores de los distintos tipos
			//de campos existentes.
			char *campo          = NULL;
			float campoNumerico  = 0;
			int campoNumericoInt = 0;
			short campoShort     = 0;
			bool campoBooleano	 = false;
		
			int i = 1;
			it = listaInfoRegistro->begin();
			regAttribute = *it;
		
			// Se obtiene el tipo de atributo del registro.
			tipoDeRegistro = regAttribute.tipoDato;
			
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
					memcpy(&longRegistro, &(this->datos[offsetToReg]), Tamanios::TAMANIO_LONGITUD);
					
				} else
					// Se obtiene la longitud del registro.
					// En este caso la longitud del registro fijo viene dada en el primer nodo de la lista
					// en pk.
					longRegistro = regAttribute.tamRegistro;
		
				// Se obtiene el registro (sin incluir la longitud del mismo)
				registro = this->getRegistro(longRegistro, offsetToReg + bytesLong);
		
				// Se itera la lista de atributos del registro.
				// Se arranco del segundo nodo ya que el primero se utiliza para guardar
				// si el registro es de longitud fija o variable.
				for (it = (++listaInfoRegistro->begin()); (it != listaInfoRegistro->end()) && (!checkPk); ++it) {
					regAttribute = *it;
		
					// Se obtiene el tipo de atributo del registro.
					tipo = regAttribute.tipoDato;
		
					// Se obtiene el indicador de clave primaria.
					pk = regAttribute.esPk;
		
					if (tipo == TipoDatos::TIPO_STRING) {
		
						// Se obtiene la longitud del campo variable.
						memcpy(&longCampo, &registro[offsetToProxCampo],
								Tamanios::TAMANIO_LONGITUD_CADENA);
		
						offsetToProxCampo += Tamanios::TAMANIO_LONGITUD_CADENA;
		
						// Si es pk, se obtiene el campo en sí.
						if (pk) {
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
						
						if (pk) {
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
						if (pk) {
							memcpy(&campoShort, &registro[offsetToProxCampo], sizeof(short int));
		
							if (campoShort == *((short int*)clavePrimaria[clavesChequeadas])) {
								
								*offsetReg = offsetToReg;
								clavesIguales++;
							}
							clavesChequeadas++;
						}
						offsetToProxCampo += sizeof(short int);
					} else if (tipo == TipoDatos::TIPO_FLOAT) {
						if (pk) {
							memcpy(&campoNumerico, &registro[offsetToProxCampo],
									sizeof(float));
		
							if (campoNumerico == *((float*)clavePrimaria[clavesChequeadas])) {
								*offsetReg = offsetToReg;
								clavesIguales++;
							}
							clavesChequeadas++;
						}
						offsetToProxCampo += sizeof(float);
					}else if (tipo == TipoDatos::TIPO_BOOL) {
						if (pk) {
							memcpy(&campoBooleano, &registro[offsetToProxCampo], sizeof(bool));
							
							if (campoBooleano == *((bool*)clavePrimaria[clavesChequeadas])) {
								*offsetReg = offsetToReg;
								clavesIguales ++;
							}
							clavesChequeadas++;
						}
						offsetToProxCampo += sizeof(bool);
					}else if (tipo == TipoDatos::TIPO_CHAR) {
						if (pk) {
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
						if (pk) {
							ClaveFecha::TFECHA fecha;
							memcpy(&fecha.anio, &registro[offsetToProxCampo], sizeof(unsigned short));
							memcpy(&fecha.mes, &registro[offsetToProxCampo + sizeof(unsigned short)], sizeof(unsigned char));
							memcpy(&fecha.dia, &registro[offsetToProxCampo + sizeof(unsigned short) + sizeof(unsigned char)], sizeof(unsigned char));
							
							ClaveFecha::TFECHA clave;
							memcpy(&clave.anio, (char*)clavePrimaria[clavesChequeadas], sizeof(unsigned short));
							memcpy(&clave.mes, (char*)clavePrimaria[clavesChequeadas] + sizeof(unsigned short), sizeof(unsigned char));
							memcpy(&clave.dia, (char*)clavePrimaria[clavesChequeadas] + sizeof(unsigned short) + sizeof(unsigned char), sizeof(unsigned char));
							
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
			
			if (clavePrimaria) 
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
			
			memset(datosAux, 0, this->tamanio);
		
			// Levanta el espacio libre en el bloque.
			memcpy(&espLibre, this->datos, Tamanios::TAMANIO_ESPACIO_LIBRE);
		
			// Levanta la cantidad de registros del bloque.
			memcpy(&cantRegs, &this->datos[Tamanios::TAMANIO_ESPACIO_LIBRE], Tamanios::TAMANIO_CANTIDAD_REGISTROS);
		
			// Se hace una copia del bloque omitiendo el bloque borrado.
			memcpy(datosAux, this->datos, offsetToReg);
			memcpy(&datosAux[offsetToReg], &this->datos[offsetToReg+longReg], this->tamanio -offsetToReg-longReg);
		
			// Se actualizan los valores de espacio libre y cantidad de registros.
			// Se resta la longitud del registro porque espLibre contiene el offset al espacio libre
			// y no el espacio libre en sí.
			espLibre -= longReg;
			cantRegs -= 1;
			memcpy(datosAux, &espLibre, Tamanios::TAMANIO_ESPACIO_LIBRE);
			memcpy(&datosAux[Tamanios::TAMANIO_ESPACIO_LIBRE], &cantRegs, Tamanios::TAMANIO_CANTIDAD_REGISTROS);
		
			// Se actualizan los datos.
			this->setDatos(datosAux);
		}

		/*
		 * Inserta un nuevo registro dentro del bloque.
		 * Retorna true si la inserción fue exitosa, o false en caso contrario
		 * No comtempla el caso de claves repetidas.
		 **/
		int Bloque::altaRegistro(const ListaInfoRegistro * listaInfoRegistro, char *registro) 
		{
			int resultado = ResultadosIndices::OK;
			unsigned short offsetEspLibre = 0;
			unsigned short longReg        = 0;
		
			// Se obtiene la longitud del registro, no incluye los bytes de longitud del mismo.
			longReg = getTamanioRegistros(listaInfoRegistro, registro);
			
			// Se obtiene el offset al espacio libre.
			memcpy(&offsetEspLibre, this->datos, Tamanios::TAMANIO_ESPACIO_LIBRE);
		
			// Si el registro tiene espacio dentro del bloque se realiza la inserción.
			if (verificarEspacioDisponible(longReg, offsetEspLibre)) {
				ListaInfoRegistro::const_iterator it;
				it = listaInfoRegistro->begin();
				
				// Si el registro es de longitud variable le sumo la cantidad de bytes de la longitud 
				// del mismo.
				if (it->tipoDato == TipoDatos::TIPO_VARIABLE)
					longReg += Tamanios::TAMANIO_LONGITUD;
		
				// Registro incluye su longitud en los primeros 2 bytes.
				insertarRegistro(registro, offsetEspLibre, longReg);
				
			}else resultado = ResultadosIndices::SOBREFLUJO;
			
			return resultado;
		}

		/*
		 * Da de baja dentro del bloque al registro cuya clave es clavePrimaria.
		 **/
		int Bloque::bajaRegistro(const ListaInfoRegistro *listaInfoRegistro, Clave &clave) 
		{						
			void** clavePrimaria = clave.getValorParaHash();
			unsigned short offsetToReg = this->offsetADatos;
			unsigned short offsetToProxCampo = 0;
		
			unsigned short longReg       = 0;
			unsigned short cantRegistros = 0;
			bool registroBorrado         = false;
			char *registro               = NULL;
			ListaInfoRegistro::const_iterator it = listaInfoRegistro->begin();
			NodoInfoRegistro regAtribute;
			unsigned char tipoRegistro = 0; // Indica si el registro es variable o fijo
			unsigned char tipo         = 0; // Indica el tipo de dato del campo de un registro
			bool pk = false;
			bool checkPk = false;
			unsigned char longCampo  = 0;
			int campoNumericoInt      = 0;
			float campoNumerico       = 0;
			bool campoBooleano		  = false;
			unsigned short campoShort = 0;
			unsigned short bytesLongitud = 0;
			unsigned char cantClaves     = 0;
			unsigned char clavesChequeadas = 0;
			unsigned char clavesIguales    = 0;
		
			// Se obtiene la cantidad de registros dentro del bloque.
			memcpy(&cantRegistros, &datos[Tamanios::TAMANIO_ESPACIO_LIBRE],
					Tamanios::TAMANIO_CANTIDAD_REGISTROS);
		
			// Se obtiene la longitud del registro. (no incluye lo 2 bytes de longitud si es variable.)
			longReg = getTamanioRegistros(listaInfoRegistro, &datos[offsetToReg]);
		
			regAtribute = *it;
			// Se obtiene el tipo de atributo del registro.
			tipoRegistro = regAtribute.tipoDato;
		
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
					it = listaInfoRegistro->begin();
		
				// Se obtiene la longitud del registro.
				longReg = getTamanioRegistros(listaInfoRegistro, &datos[offsetToReg]);
		
				registro = new char[longReg];
		
				if (tipoRegistro == TipoDatos::TIPO_VARIABLE) {
					// Se omite la longitud del registro.
					offsetToReg += Tamanios::TAMANIO_LONGITUD;
					bytesLongitud = Tamanios::TAMANIO_LONGITUD;
				}
		
				// Se obtiene el registro.
				registro = getRegistro((int)longReg,(int) offsetToReg);

				//Itero la lista de atributos del registro
				for (++it; ((it != listaInfoRegistro->end()) && (!checkPk)); ++it) {
					regAtribute = *it;

					//Obtengo el tipo de atributo del registro
					tipo = regAtribute.tipoDato;
					//Obtengo el indicador de clave primaria
					pk = regAtribute.esPk;
					if (tipo == TipoDatos::TIPO_STRING) {

						//obtengo la longitud del campo variable
						memcpy(&longCampo, &registro[offsetToProxCampo],
								Tamanios::TAMANIO_LONGITUD_CADENA);

						offsetToProxCampo += Tamanios::TAMANIO_LONGITUD_CADENA;
						//Si es pk, me preocupo por obtener el campo en si
						if (pk) {
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

						if (pk) {
							memcpy(&campoNumericoInt, &registro[offsetToProxCampo], sizeof(int));
		
							if (campoNumericoInt == *((int*)clavePrimaria[clavesChequeadas]))
								clavesIguales++;
		
							clavesChequeadas++;
						}
						offsetToProxCampo += sizeof(int);
					} else if (tipo == TipoDatos::TIPO_SHORT) {

						if (pk) {
							memcpy(&campoShort, &registro[offsetToProxCampo],
									sizeof(short int));
							if (campoShort == *((short int*)clavePrimaria[clavesChequeadas]))
								clavesIguales++;
		
							clavesChequeadas++;
						}
						offsetToProxCampo += sizeof(short int);
					} else if (tipo == TipoDatos::TIPO_FLOAT) {

						if (pk) {
							memcpy(&campoNumerico, &registro[offsetToProxCampo],
									sizeof(float));
							if (campoNumerico
									== *((float*)clavePrimaria[clavesChequeadas]))
								clavesIguales++;
		
							clavesChequeadas++;
						}
						offsetToProxCampo += sizeof(float);
					}else if (tipo == TipoDatos::TIPO_BOOL) {
						
						if(pk) {
							memcpy(&campoBooleano, &registro[offsetToProxCampo], sizeof(bool));
							if (campoBooleano == *((bool*)clavePrimaria[clavesChequeadas]))
								clavesIguales++;
							
							clavesChequeadas++;
						}
						offsetToProxCampo += sizeof(bool);
					}else if (tipo == TipoDatos::TIPO_CHAR) {

						if (pk) {
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

						if (pk) {
							ClaveFecha::TFECHA fecha;
							memcpy(&fecha.anio, &registro[offsetToProxCampo], sizeof(unsigned short));
							memcpy(&fecha.mes, &registro[offsetToProxCampo + sizeof(unsigned short)], sizeof(unsigned char));
							memcpy(&fecha.dia, &registro[offsetToProxCampo + sizeof(unsigned short) + sizeof(unsigned char)], sizeof(unsigned char));
							
							ClaveFecha::TFECHA clave;
							memcpy(&clave.anio, (char*)clavePrimaria[clavesChequeadas], sizeof(unsigned short));
							memcpy(&clave.mes, (char*)clavePrimaria[clavesChequeadas] + sizeof(unsigned short), sizeof(unsigned char));
							memcpy(&clave.dia, (char*)clavePrimaria[clavesChequeadas] + sizeof(unsigned short) + sizeof(unsigned char), sizeof(unsigned char));
		
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

		int Bloque::modificarRegistro(const ListaInfoRegistro *listaInfoRegistro,
				unsigned short longReg, Clave &clavePrimaria, char* registro) 
		{	
			int resultado = ResultadosIndices::OK;
			unsigned short offsetReg = 0;
		
			if (buscarRegistro(listaInfoRegistro, clavePrimaria, &offsetReg)){				
		
				// Levanta la longitud del registro original.
				unsigned short longRegOrig = getTamanioRegistros(listaInfoRegistro, &this->datos[offsetReg]);
				memcpy(&longRegOrig, &this->datos[offsetReg], Tamanios::TAMANIO_LONGITUD);
			
				unsigned short offsetEspLibre = 0;
				memcpy(&offsetEspLibre, this->datos, Tamanios::TAMANIO_ESPACIO_LIBRE);
			
				if ((getTamanioBloque()- offsetEspLibre + longRegOrig) < longReg)
					resultado = ResultadosIndices::SOBREFLUJO;
				else{				
					bajaRegistro(listaInfoRegistro, clavePrimaria);
					altaRegistro(listaInfoRegistro, registro);
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
		char* Bloque::getRegistro(int longReg, int offsetToReg) const
		{
			char* registro = new char [longReg];
			
			memcpy(registro, &this->datos[offsetToReg], longReg);
			
			return registro;
		}
		
		/*
		 * Retorna el registro que se encuentra en la posición 'numReg',
		 * comenzando desde 0. En 'tamReg' devuelve el tamaño del mismo.
		 * */
		char* Bloque::getRegistroPorNro(unsigned short numReg, unsigned short &tamReg) const {
			
			unsigned short offsetToReg = this->offsetADatos;
			unsigned short offsetEspLibre = 0, longReg = 0;
			char* registro = NULL;
			tamReg = 0;
			
			if (this->datos) {
			
				memcpy(&offsetEspLibre, this->datos, Tamanios::TAMANIO_ESPACIO_LIBRE);
				
				unsigned short i;
				
				for (i = 0; (i < numReg) && (offsetToReg < offsetEspLibre); ++i) {
					
					memcpy(&longReg, this->datos + offsetToReg, Tamanios::TAMANIO_LONGITUD);
					
					offsetToReg += Tamanios::TAMANIO_LONGITUD + longReg;
					
				}
				
				if (i == numReg) {
					memcpy(&longReg, this->datos + offsetToReg, Tamanios::TAMANIO_LONGITUD);
					tamReg = Tamanios::TAMANIO_LONGITUD + longReg;
					registro = new char[tamReg];
					memcpy(registro, this->datos + offsetToReg, tamReg);
				}
			
			}
			
			return registro;
			
		}

		/*
		 * Este método recibe un registro, y retorna su clave primaria.
		 **/
		Clave* Bloque::getClavePrimaria(const ListaInfoRegistro *listaInfoRegistro, char* registro) 
		{
			ListaClaves listaClaves;
			int offsetToProxCampo = 0;
			ListaInfoRegistro::const_iterator it;
			NodoInfoRegistro regAttribute;
			unsigned char tipo = 0; //Indica si el tipo de campo del registro es variable o fijo
			unsigned char tipoDeRegistro = 0; // Indica si el registro es variable o fijo
			bool pk = false;
			string str;
			unsigned char longCampo = 0;
			char *campo = NULL;
			char campoChar       = 0;
			float campoNumerico  = 0;
			int campoNumericoInt = 0;
			short int campoShort = 0;
			bool campoBooleano	 = false;
			ClaveFecha::TFECHA campoFecha;
			unsigned char cantClaves        = 0;
			unsigned char clavesEncontradas = 0;			
		
			it = listaInfoRegistro->begin();
			regAttribute = *it;
		
			cantClaves = regAttribute.cantClaves;
		
			tipoDeRegistro = regAttribute.tipoDato;
			if (tipoDeRegistro == TipoDatos::TIPO_VARIABLE)
				// Se omite la longitud del registro.
				offsetToProxCampo += 2;
		
			// Se itera la lista de atributos del registro.
			// Se arranca del segundo nodo ya que el primero se utiliza para guardar
			// si el registro es de longitud fija o variable.
			for (it = (++listaInfoRegistro->begin()); it != listaInfoRegistro->end(); ++it) {
				regAttribute = *it;
		
				// Se obtiene el tipo de atributo del registro.
				tipo = regAttribute.tipoDato;
		
				// Se obtiene el indicador de clave primaria.
				pk = regAttribute.esPk;
		
				if (tipo == TipoDatos::TIPO_STRING) {
					// Se obtiene la longitud del campo variable.
					memcpy(&longCampo, &registro[offsetToProxCampo],
							Tamanios::TAMANIO_LONGITUD_CADENA);
		
					offsetToProxCampo += Tamanios::TAMANIO_LONGITUD_CADENA;
		
					// Si es pk, se obtiene el campo en sí.
					if (pk) {
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
					if (pk) {
						memcpy(&campoNumericoInt, &registro[offsetToProxCampo], sizeof(int));
						listaClaves.push_back(new ClaveEntera(campoNumericoInt));
						clavesEncontradas++;
					}
		
					offsetToProxCampo += sizeof(int);
				} else if (tipo == TipoDatos::TIPO_SHORT) {
					if (pk) {
						memcpy(&campoShort, &registro[offsetToProxCampo],
								sizeof(short int));
						listaClaves.push_back(new ClaveShort(campoShort));
						clavesEncontradas++;
					}
					offsetToProxCampo += sizeof(short int);
				} else if (tipo == TipoDatos::TIPO_FLOAT) {
					if (pk) {
						memcpy(&campoNumerico, &registro[offsetToProxCampo],
								sizeof(float));
						
						listaClaves.push_back(new ClaveReal(campoNumerico));
						
						clavesEncontradas++;
					}
					offsetToProxCampo += sizeof(float);
				}else if (tipo == TipoDatos::TIPO_BOOL) {
					if (pk) {
						memcpy(&campoBooleano, &registro[offsetToProxCampo], sizeof(bool));
						
						listaClaves.push_back(new ClaveBoolean(campoBooleano));
						
						clavesEncontradas++;
					}
					offsetToProxCampo += sizeof(bool);
				}else if (tipo == TipoDatos::TIPO_CHAR) {
					if (pk) {
						memcpy(&campoChar, &registro[offsetToProxCampo], sizeof(char));
						listaClaves.push_back(new ClaveChar(campoChar));
						
						clavesEncontradas++;
					}
					offsetToProxCampo += sizeof(char);
				} else if (tipo == TipoDatos::TIPO_FECHA) {
					if (pk) {
						
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
		 * Devuelve la longitud del registro, sin incluir los bytes de longitud.
		 **/
		unsigned short Bloque::getTamanioRegistros(const ListaInfoRegistro * listaInfoRegistro, char *registro) const
		{
			ListaInfoRegistro::const_iterator it = listaInfoRegistro->begin();
			NodoInfoRegistro regAttribute = *it;
		
			// Se obtiene el tipo de atributo del registro.
			unsigned char tipo = regAttribute.tipoDato;
			unsigned short longReg = 0;
		
			if (tipo == TipoDatos::TIPO_VARIABLE)
				// Se obtiene la longitud del registro variable.
				memcpy(&longReg, registro, Tamanios::TAMANIO_LONGITUD);
			else
				// Se obtiene la longitud del registro fijo.
				longReg = regAttribute.tamRegistro;
		
			return longReg;
		}
		
		unsigned short Bloque::getTamanioRegistroConPrefijo(const ListaInfoRegistro* listaInfoRegistro, char *registro) const
		{
			ListaInfoRegistro::const_iterator it = listaInfoRegistro->begin();
			NodoInfoRegistro regAttribute = *it;
		
			// Se obtiene el tipo de atributo del registro.
			unsigned char tipo = regAttribute.tipoDato;
			unsigned short longReg = 0;
		
			if (tipo == TipoDatos::TIPO_VARIABLE){
				// Se obtiene la longitud del registro variable.
				memcpy(&longReg, registro, Tamanios::TAMANIO_LONGITUD);
				longReg += Tamanios::TAMANIO_LONGITUD;
			}
			else
				// Se obtiene la longitud del registro fijo.
				longReg = regAttribute.tamRegistro;
		
			return longReg;
		}
		
		unsigned int Bloque::getTamanioBloque() const
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
	
		unsigned int Bloque::getNroBloque() const
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
		
		void Bloque::setTipoOrganizacion(int tipo)
		{
			this->tipoOrganizacion = tipo;
		}
		
		int Bloque::getTipoOrganizacion() const
		{
			return this->tipoOrganizacion;
		}
		
		void Bloque::setOffsetToReg(unsigned int offset)
		{
			this->offsetToProxReg = offset;
		}
		
		unsigned int Bloque::getOffsetToReg()
		{
			return this->offsetToProxReg;
		}
		
		
		char* Bloque::serializarClave(Clave* clave, const ListaTipos* listaTipos) {
			
			ListaClaves listaClaves;
			unsigned int tamanioClave = clave->getTamanioValorConPrefijo(), offset = 2;
			// Los claves serializadas siempre son registros de longitud variable
			char* buffer = new char[Tamanios::TAMANIO_LONGITUD + tamanioClave];
			
			//Guardo la longitud de la clave completa, para seguir la convención
			//de los registros de tamaño variable.
			memcpy(buffer, &tamanioClave, Tamanios::TAMANIO_LONGITUD);
			
			if (listaTipos->size() > 1) //'clave' es una clave compuesta
				listaClaves = *(((ClaveCompuesta*)clave)->getListaClaves());
			else listaClaves.push_back(clave);

			ListaClaves::const_iterator iterClaves;
			ListaTipos::const_iterator iterTipos;
			Clave* claveAux = NULL;
			char* cadena = NULL;
			unsigned int sizeCadena = 0;
			
			for (iterClaves = listaClaves.begin(), iterTipos = listaTipos->begin();
				(iterClaves != listaClaves.end()) && (iterTipos != listaTipos->end());
				++iterClaves, ++iterTipos) {	
				
				claveAux = *iterClaves;
				
				if (*iterTipos == TipoDatos::TIPO_STRING) {
					
					cadena = (char*)claveAux->getValor();
					sizeCadena = claveAux->getTamanioValor();
				
					memcpy(buffer + offset, &sizeCadena, Tamanios::TAMANIO_LONGITUD_CADENA);
					memcpy(buffer + offset + Tamanios::TAMANIO_LONGITUD_CADENA, cadena, sizeCadena);
					
				} else memcpy(buffer + offset, claveAux->getValor(), claveAux->getTamanioValor());
				
				offset += claveAux->getTamanioValorConPrefijo();
			}
			
			return buffer;
			
		}
		
		/*
		 * Retorna un registro segun la el offset al mismo y prepara este para que apunte al siguiente registro
		 * */
		
		char* Bloque::getNextRegister()
		{
			unsigned short longReg;
			unsigned int offsetToReg = this->offsetToProxReg;
			unsigned short offsetEspLibre;
			
			// Obtengo el espacio libre
			memcpy(&offsetEspLibre, this->datos, Tamanios::TAMANIO_ESPACIO_LIBRE);
			
			// Si el offset al proximo registro esta apuntando al espacio libre reseteo el mismo, y retorno null
			if(offsetToReg == offsetEspLibre){
				this->resetOffsetToReg();
				return NULL;
			}
			
			// Si el tipo de registro es variable el offsetToProxReg apunta a la longitud del mismo
			if(this->getTipoOrganizacion() == TipoDatos::TIPO_VARIABLE)
			{
				memcpy(&longReg, &this->datos[offsetToReg], Tamanios::TAMANIO_LONGITUD);
				
				// Actualizo la variable que apunta al registro
				offsetToReg += Tamanios::TAMANIO_LONGITUD;
				
				char * registro = new char[longReg + Tamanios::TAMANIO_LONGITUD];
				
				memcpy(registro, this->datos + this->offsetToProxReg, longReg + Tamanios::TAMANIO_LONGITUD);
				
				// Actualizo el offset al prox registro
				this->offsetToProxReg += longReg + Tamanios::TAMANIO_LONGITUD;
				
				
				return registro;
				
			}
			else
			{
				// Calculo la longitud del registro fijo
				longReg = getTamanioRegistros();
				this->offsetToProxReg += longReg;
				
				return getRegistro(longReg, offsetToReg);
			}
		}
		
		/*
		 * Si el tipo de organizacion es de registros de longitud fija calcula el tamanio de los mismos.
		 * Si es de longitud variable, obtiene la longitud del registro corriente
		 * */
		
		unsigned short Bloque::getTamanioRegistros() const
		{
			unsigned short offsetEspLibre;
			unsigned short cantRegistros;
			unsigned short longRegistro;
			
			if(this->getTipoOrganizacion() == TipoDatos::TIPO_FIJO)
			{
				// Obtengo el espacio libre dentro del bloque
				memcpy(&offsetEspLibre, this->datos, Tamanios::TAMANIO_ESPACIO_LIBRE);
			
				// Obtengo la cantidad de registros dentro del bloque
				memcpy(&cantRegistros, &this->datos[Tamanios::TAMANIO_ESPACIO_LIBRE], Tamanios::TAMANIO_CANTIDAD_REGISTROS);
						
				longRegistro  = (offsetEspLibre - this->offsetADatos) / cantRegistros;
			}
			else{
				// Obtengo la longitud variable del registro corriente
				memcpy(&longRegistro, this->datos + this->offsetToProxReg, Tamanios::TAMANIO_LONGITUD);
				
				// Le sumo los bytes de la longitud
				longRegistro += Tamanios::TAMANIO_LONGITUD;
			}
			
			return longRegistro;
		}
		
		
		void Bloque::resetOffsetToReg()
		{
			this->offsetToProxReg = this->offsetADatos;
		}
		
		
		unsigned short Bloque::getCantidadRegistros()
		{
			unsigned short cantRegs;
			memcpy(&cantRegs, &this->datos[Tamanios::TAMANIO_ESPACIO_LIBRE], Tamanios::TAMANIO_CANTIDAD_REGISTROS);
			
			return cantRegs;
		}
		
		/*
		 * Este método limpia el bloque poniendo la cantidad de registros en cero y actualizando el
		 * offset al espacio libre.
		 */
		void Bloque::clear() {
			
			this->numero       = 0;
			this->offsetADatos = Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS;
			this->offsetToProxReg = this->offsetADatos;
			
			if (this->datos) {
				memset(this->datos, 0, this->getTamanioBloque());
				
				// Inicializa el offset a espacio libre dentro del bloque.
				unsigned short espLibre = this->offsetADatos;
				memcpy(this->datos, &espLibre, Tamanios::TAMANIO_ESPACIO_LIBRE);
				
				// Inicializa la cantidad de registros dentro del bloque.
				unsigned short cantRegs = 0;
				memcpy(this->datos + Tamanios::TAMANIO_ESPACIO_LIBRE, &cantRegs, Tamanios::TAMANIO_CANTIDAD_REGISTROS);
			}
			
		}
		
		
		unsigned short Bloque::getTamanioEspacioLibre() const {
			
			if (!this->datos) return 0;
			
			return (this->getTamanioBloque() - *((unsigned short*)this->datos));
			
		}
