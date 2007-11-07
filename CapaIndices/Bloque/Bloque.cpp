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

///////////////////////////////////////////////////////////////////////
// Constructor/Destructor
///////////////////////////////////////////////////////////////////////

/**
 * Este constructor recibe el número de bloque dentro del archivo, y el tamaño del 
 * bloque medido en bytes.
 */
Bloque::Bloque(unsigned int numeroBloque, unsigned int tamanioBloque) {
	numero = numeroBloque;
	tamanio = tamanioBloque;
	datos = new char[tamanio];
	unsigned short espLibre = Tamanios::TAMANIO_ESPACIO_LIBRE
			+ Tamanios::TAMANIO_CANTIDAD_REGISTROS;
	unsigned short cantRegs = 0;
	memcpy(datos, &espLibre, Tamanios::TAMANIO_ESPACIO_LIBRE);
	memcpy(&datos[Tamanios::TAMANIO_ESPACIO_LIBRE], &cantRegs,
			Tamanios::TAMANIO_CANTIDAD_REGISTROS);
}

Bloque::~Bloque() {
	if (datos)
		delete [] datos;
}

/*
 * Este metodo busca un registro en el bloque segun su clavePrimaria. Si lo encuentra
 * devuelve true y su offset dentro del bloque en "offsetReg"; y si no lo encuentra
 * devuelve false.
 **/
bool Bloque::buscarRegistro(const ListaNodos *listaParam, Clave &clave,
		unsigned short *offsetReg) {

	void** clavePrimaria = clave.getValorParaHash();

	// Obtengo el offset a los registros
	int offsetToReg = getOffsetToRegs();
	int offsetToProxCampo;

	// Se obtiene la cantidad de registros dentro del bloque.
	unsigned short cantRegistros;
	memcpy(&cantRegistros, &datos[2], Tamanios::TAMANIO_LONGITUD);

	unsigned char cantClaves;
	unsigned char clavesChequeadas;
	unsigned char clavesIguales;
	bool encontrado = false;
	bool checkPk; // Indica si ya reviso la clave primaria del registro.
	char *registro;
	list<nodoLista>::const_iterator it;
	nodoLista regAtribute;
	int tipo; //Indica si el tipo de campo del registro es variable o fijo
	int tipoDeRegistro; // Indica si el registro es variable o fijo
	string pk;
	unsigned short longRegistro;
	unsigned short longCampo;
	unsigned short bytesLong = 0;
	char *campo;
	float campoNumerico;
	int campoNumericoInt;
	short int campoShort;

	int i = 1;
	it = listaParam->begin();
	regAtribute = *it;

	// Se obtiene el tipo de atributo del registro.
	tipoDeRegistro = regAtribute.tipo;
	// Se obtiene la cantidad de claves primarias.
	cantClaves = regAtribute.cantClaves;

	// Mientras haya mas registros y no lo haya encontrado.
	while ( (i<cantRegistros + 1) && (!encontrado)) {
		// Resetea los atributos que sirven para el control de la busqueda dentro de un registro.
		offsetToProxCampo = 0;
		checkPk = false;
		clavesChequeadas = 0;
		clavesIguales = 0;

		if (tipoDeRegistro == TipoDatos::TIPO_VARIABLE) {
			bytesLong = Tamanios::TAMANIO_LONGITUD;
			//Se obtiene la longitud del registro.
			memcpy(&longRegistro, &datos[offsetToReg],
					Tamanios::TAMANIO_LONGITUD);
		} else
			// Se obtiene la longitud del registro.
			// En este caso la longitud del registro fijo viene dada en el primer nodo de la lista
			// en pk.
			longRegistro = ((unsigned short)atoi(regAtribute.pk.c_str()));

		// Se obtiene el registro (sin incluir la longitud del mismo)
		registro = getRegistro(longRegistro, offsetToReg + bytesLong);

		// Se itera la lista de atributos del registro.
		// Se arranco del segundo nodo ya que el primero se utiliza para guardar
		// si el registro es de longitud fija o variable.
		for (it = (++listaParam->begin()); ((it != listaParam->end())
				&& (!checkPk)); ++it) {
			regAtribute = *it;

			// Se obtiene el tipo de atributo del registro.
			tipo = regAtribute.tipo;

			// Se obtiene el indicador de clave primaria.
			pk = regAtribute.pk;

			if (tipo == TipoDatos::TIPO_STRING) {
				// Se obtiene la longitud del campo variable.
				memcpy(&longCampo, &registro[offsetToProxCampo],
						Tamanios::TAMANIO_LONGITUD);

				offsetToProxCampo += 2;

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
					memcpy(&campoShort, &registro[offsetToProxCampo],
							Tamanios::TAMANIO_LONGITUD);

					if (campoShort
							== *((short int*)clavePrimaria[clavesChequeadas])) {
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

					if (campoNumerico
							== *((float*)clavePrimaria[clavesChequeadas])) {
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
					memcpy(&fecha, &registro[offsetToProxCampo],
							sizeof(ClaveFecha::TFECHA));
					ClaveFecha::TFECHA clave;
					memcpy(&clave, clavePrimaria[clavesChequeadas],
							sizeof(ClaveFecha::TFECHA));

					if ((fecha.anio == clave.anio)&&(fecha.mes == clave.mes)
							&&(fecha.dia == clave.dia)) {
						*offsetReg = offsetToReg;
						clavesIguales++;
					}
					clavesChequeadas++;
				}
				offsetToProxCampo += sizeof(ClaveFecha::TFECHA);
			}
			if (clavesChequeadas == cantClaves) {
				checkPk = true;
				if (clavesChequeadas == clavesIguales)
					encontrado = true;
			}
		}

		//La próxima iteración se levantará la longitud del registro siguiente
		offsetToReg += bytesLong + longRegistro;
		delete []registro;
	}

	if (!encontrado)
		return false;
	return true;
}

/*
 * Este metodo reorganiza el bloque luego de una baja.
 * longReg incluye los bytes utilizados para la longitud del registro en caso
 * de que el mismo sea variable.
 **/
void Bloque::organizarBloque(int offsetToReg, int longReg) {

	char* datosAux = new char[tamanio];
	unsigned short espLibre;
	unsigned short cantRegs;

	// Levanta el espacio libre en el bloque.
	memcpy(&espLibre, datos, Tamanios::TAMANIO_LONGITUD);

	// Levanta la cantidad de registros del bloque.
	memcpy(&cantRegs, &datos[Tamanios::TAMANIO_LONGITUD],
			Tamanios::TAMANIO_LONGITUD);

	// Se hace una copia del bloque omitiendo el bloque borrado.
	memcpy(datosAux, datos, offsetToReg);
	memcpy(&datosAux[offsetToReg], &datos[offsetToReg+longReg], tamanio
			-offsetToReg-longReg);

	// Se actualizan los valores de espacio libre y cantidad de registros.
	// Se resta la longitud del registro porque espLibre contiene el offset al espacio libre
	// y no el espacio libre en sí.
	espLibre -= longReg;
	cantRegs -= 1;
	memcpy(datosAux, &espLibre, Tamanios::TAMANIO_LONGITUD);
	memcpy(&datosAux[Tamanios::TAMANIO_LONGITUD], &cantRegs,
			Tamanios::TAMANIO_LONGITUD);

	// Se actualizan los datos.
	delete[] datos;
	datos = datosAux;
}

/*
 * Inserta un nuevo registro dentro del bloque.
 * Retorna true si la inserción fue exitosa, o false en caso contrario
 * No comtempla el caso de claves repetidas.
 **/
int Bloque::altaRegistro(const ListaNodos * listaParam, char *registro) {
	unsigned short offsetEspLibre;
	unsigned short longReg;

	// Se obtiene la longitud del registro, no incluye los bytes de longitud del mismo.
	longReg = getTamanioRegistros(listaParam, registro);

	// Se obtiene el offset al espacio libre.
	memcpy(&offsetEspLibre, datos, Tamanios::TAMANIO_LONGITUD);

	//Si el registro tiene espacio dentro del bloque se realiza la inserción.
	if (verificarEspacioDisponible(longReg, offsetEspLibre)) {
		list<nodoLista>::const_iterator it;
		it = listaParam->begin();
		// Si el registro es de longitud variable le sumo la cantidad de bytes de la longitud del mismo.
		if (it->tipo == TipoDatos::TIPO_VARIABLE)
			longReg += Tamanios::TAMANIO_LONGITUD;

		// registro incluye su longitud en los primeros 2 bytes.
		insertarRegistro(registro, offsetEspLibre, longReg);
		return OK;
	}
	return SOBREFLUJO;
}

/*
 * Da de baja dentro del bloque al registro cuya clave es clavePrimaria.
 **/
int Bloque::bajaRegistro(const ListaNodos *listaParam, Clave &clave) {

	void** clavePrimaria = clave.getValorParaHash();
	unsigned short offsetToReg = getOffsetToRegs();
	unsigned short offsetToProxCampo = 0;

	unsigned short longReg;
	unsigned short cantRegistros;
	bool registroBorrado = false;
	char *registro;
	list<nodoLista>::const_iterator it = listaParam->begin();
	nodoLista regAtribute;
	int tipoRegistro; // Indica si el registro es variable o fijo
	int tipo; // Indica el tipo de dato del campo de un registro
	string pk;
	bool checkPk;
	unsigned short longCampo;
	int campoNumericoInt;
	float campoNumerico;
	unsigned short campoShort;
	unsigned short bytesLongitud = 0;
	unsigned char cantClaves;
	unsigned char clavesChequeadas;
	unsigned char clavesIguales;

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
						Tamanios::TAMANIO_LONGITUD);
				offsetToProxCampo += Tamanios::TAMANIO_LONGITUD;
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
					memcpy(&campoNumericoInt, &registro[offsetToProxCampo],
							sizeof(int));

					if (campoNumericoInt
							== *((int*)clavePrimaria[clavesChequeadas]))
						clavesIguales++;

					clavesChequeadas++;
				}
				offsetToProxCampo += sizeof(int);
			} else if (tipo == TipoDatos::TIPO_SHORT) {
				if (pk == "true") {
					memcpy(&campoShort, &registro[offsetToProxCampo],
							sizeof(short int));
					if (campoShort
							== *((short int*)clavePrimaria[clavesChequeadas]))
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
					memcpy(&fecha, &registro[offsetToProxCampo],
							sizeof(ClaveFecha::TFECHA));
					ClaveFecha::TFECHA clave;
					memcpy(&clave, clavePrimaria[clavesChequeadas],
							sizeof(ClaveFecha::TFECHA));

					if ((fecha.anio == clave.anio)&&(fecha.mes == clave.mes)
							&&(fecha.dia == clave.dia))
						clavesIguales++;
					clavesChequeadas++;
				}
				offsetToProxCampo += sizeof(ClaveFecha::TFECHA);
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
	if (!registroBorrado)
		return NO_ENCONTRADO;
	return OK;
}

int Bloque::modificarRegistro(const ListaNodos *listaParam,
		unsigned short longReg, Clave &clavePrimaria, char* registro) {

	unsigned short offsetReg;
	bool encontrado = buscarRegistro(listaParam, clavePrimaria, &offsetReg);

	if (!encontrado)
		return NO_ENCONTRADO;

	// Levanta la longitud del registro original.
	unsigned short longRegOrig;
	longRegOrig = getTamanioRegistros(listaParam, &datos[offsetReg]);
	memcpy(&longRegOrig, &datos[offsetReg], Tamanios::TAMANIO_LONGITUD);

	unsigned short offsetEspLibre;
	memcpy(&offsetEspLibre, datos, Tamanios::TAMANIO_ESPACIO_LIBRE);

	if ((getTamanioBloque()-offsetEspLibre + longRegOrig)<longReg)
		return SOBREFLUJO;
	bajaRegistro(listaParam, clavePrimaria);
	altaRegistro(listaParam, registro);
	return OK;
}

bool Bloque::verificarEspacioDisponible(int longReg, int offsetEspLibre) {
	int espacioDisponible;

	espacioDisponible = getTamanioBloque() - offsetEspLibre;
	if (espacioDisponible < longReg)
		return false;
	else
		return true;
}

void Bloque::insertarRegistro(char *registro, unsigned short offsetEspLibre,
		unsigned short longReg) {

	memcpy(&datos[offsetEspLibre], registro, longReg);
	// Actualiza el offset al espacio libre.
	offsetEspLibre += longReg;
	memcpy(datos, &offsetEspLibre, Tamanios::TAMANIO_ESPACIO_LIBRE);

	// Actualiza la cantidad de registros.
	unsigned short cantRegs;
	memcpy(&cantRegs, &datos[Tamanios::TAMANIO_ESPACIO_LIBRE],
			Tamanios::TAMANIO_CANTIDAD_REGISTROS);
	cantRegs++;
	memcpy(&datos[Tamanios::TAMANIO_ESPACIO_LIBRE], &cantRegs,
			Tamanios::TAMANIO_CANTIDAD_REGISTROS);
}

/*
 * Este método devuelve el registro que empieza en datos[offsetToRegs] y tiene longitud longReg.
 **/
char* Bloque::getRegistro(int longReg, int offsetToReg) {
	char *registro = new char [longReg];
	memcpy(registro, &datos[offsetToReg], longReg);
	return registro;
}

char* Bloque::getRegisterAtribute(string registro, int offsetCampo,
		int longCampo) {
	char *campo = new char[longCampo + 1];

	memcpy(campo, &registro[offsetCampo], longCampo);
	campo[longCampo] = '\0';
	return campo;
}

/*
 * Este método recibe un registro, y retorna su clave primaria en un char*
 **/
Clave* Bloque::getClavePrimaria(const ListaNodos *listaParam, char* registro) {
	ListaClaves listaClaves;
	int offsetToProxCampo = 0;
	list<nodoLista>::const_iterator it;
	nodoLista regAtribute;
	int tipo; //Indica si el tipo de campo del registro es variable o fijo
	int tipoDeRegistro; // Indica si el registro es variable o fijo
	string pk;
	unsigned short longCampo;
	char *campo;
	float campoNumerico;
	int campoNumericoInt;
	short int campoShort;
	unsigned char cantClaves;
	unsigned char clavesEncontradas = 0;

	it = listaParam->begin();
	regAtribute = *it;

	cantClaves = regAtribute.cantClaves;

	tipoDeRegistro = regAtribute.tipo;
	if (tipoDeRegistro == TipoDatos::TIPO_VARIABLE)
		// Se omite la longitud del registro.
		offsetToProxCampo += 2;

	// Se itera la lista de atributos del registro.
	// Se arranco del segundo nodo ya que el primero se utiliza para guardar
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
					Tamanios::TAMANIO_LONGITUD);

			offsetToProxCampo += 2;

			// Si es pk, se obtiene el campo en sí.
			if (pk == "true") {
				campo = new char[longCampo +1];
				memcpy(campo, &registro[offsetToProxCampo], longCampo);
				*(campo + longCampo) = 0;
				string str = campo;
				Clave* clave = new ClaveVariable(str);
				listaClaves.push_back(clave);
				clavesEncontradas++;
				delete []campo;
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
				char c;
				memcpy(&c, &registro[offsetToProxCampo], sizeof(char));
				listaClaves.push_back(new ClaveChar(c));
				clavesEncontradas++;
			}
			offsetToProxCampo += sizeof(char);
		} else if (tipo == TipoDatos::TIPO_FECHA) {
			if (pk == "true") {
				ClaveFecha::fecha f;
				memcpy(&f.anio,&registro[offsetToProxCampo],sizeof(unsigned short));
				offsetToProxCampo += sizeof(unsigned short);
				memcpy(&f.mes, &registro[offsetToProxCampo], sizeof(unsigned char));
				offsetToProxCampo += sizeof(unsigned char);
				memcpy(&f.dia,&registro[offsetToProxCampo],sizeof(unsigned char));
				offsetToProxCampo += sizeof(unsigned char);
				
				listaClaves.push_back(new ClaveFecha(&f));
				clavesEncontradas++;
			}
			else
				offsetToProxCampo += sizeof(ClaveFecha::TFECHA);
		}
	}
	if (cantClaves == 1){
		ListaClaves::const_iterator iter = listaClaves.begin();
		return *iter;
	}
	
	return new ClaveCompuesta(listaClaves);
}

/*
 * Retorna el offset al primer registro del bloque.
 **/
unsigned short Bloque::getOffsetToRegs() {
	return (Tamanios::TAMANIO_ESPACIO_LIBRE
			+ Tamanios::TAMANIO_CANTIDAD_REGISTROS);
}

/*
 * Devuelve la longitud del registro, sin incluir los 2 bytes de la longitud.
 **/
unsigned short Bloque::getTamanioRegistros(const ListaNodos * listaParam,
		char *registro) {
	list<nodoLista>::const_iterator it = listaParam->begin();
	nodoLista regAtribute = *it;

	// Se obtiene el tipo de atributo del registro.
	int tipo = regAtribute.tipo;
	unsigned short longReg;

	if (tipo == TipoDatos::TIPO_VARIABLE)
		// Se obtiene la longitud del registro variable.
		memcpy(&longReg, registro, Tamanios::TAMANIO_LONGITUD);
	else
		// Se obtiene la longitud del registro fijo.
		longReg = ((unsigned short)atoi(regAtribute.pk.c_str()));

	return longReg;
}

unsigned int Bloque::getTamanioBloque() {
	return tamanio;
}

void Bloque::setTamanioBloque(unsigned int tam) {
	tamanio = tam;
}

char * Bloque::getDatos() {
	return datos;
}

void Bloque::setDatos(char* data) {
	datos = data;
}

unsigned int Bloque::getNroBloque() {
	return numero;
}

void Bloque::setNroBloque(unsigned int num) {
	numero = num;
}

