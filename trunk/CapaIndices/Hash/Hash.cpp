///////////////////////////////////////////////////////////////////////////
//	Archivo   : Hash.cpp
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clase Hash.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#include "Hash.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Hash (Implementa indices de dispersion extensible)
//////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		Hash::Hash(IndiceHashManager* arch, ListaNodos* lista, unsigned int tamBucket)
		{
			this->archivo    = arch;
			this->listaParam = lista;
			
			this->tabla = new Tabla(this->archivo,tamBucket);
		}
	
		Hash::~Hash()
		{
			if (this->tabla)
				delete this->tabla;
		}

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////

		/*
		 * Este método se utiliza para hacer una operación de alta en el archivo.
		 * Si el registro se puede insertar devuelve OK; si ya existe un registro
		 * con la misma clave, devuelve CLAVE_DUPLICADA.
		 * En caso de que el registro no entre en el bucket correspondiente, se 
		 * toman las medidas necesarias para hacer extensible la función de hash.
		 * Si el registro es variable "registro" contendrá su longitud en los 
		 * primeros bytes.
		 **/
		int Hash::insertarRegistro(char* registro, Clave &clave)
		{
			// Se aplica la función de hash para saber en que bucket se debe insertar.	
			unsigned int hash = this->aplicarHash(clave);
			
			unsigned int posicion = hash % this->tabla->getTamanio();
			
			// Se obtiene el bucket donde hay que insertar el registro.
			unsigned int numBucket = this->tabla->getNroBucket(posicion);
			
			// Levanta un bucket ya existente del disco y carga sus datos.
			Bucket * bucket = new Bucket(this->archivo, numBucket);
			
			// Se busca si el registro ya existe.
			unsigned short aux = 0;
			
			if (bucket->buscarRegistro(this->listaParam, clave, &aux))
			{
				if (bucket) delete bucket;
			
				// ya existe un registro con esa clave.
				return ResultadosIndices::CLAVE_DUPLICADA; 
			}			
			
			// Si el registro no existe, hay que insertarlo.
			// Se obtiene la longitud del registro independientemente de si es variable o fija.
			unsigned short longReg = bucket->getTamanioRegistros(this->listaParam, registro);
			
			// si el registro es variable se agrega a su longitud los bytes que contienen la misma.
			if (esRegistroVariable())
				longReg += Tamanios::TAMANIO_LONGITUD;
			
			if (bucket->verificarEspacioDisponible(longReg, bucket->getEspacioLibre()))
			{
				// Da de alta el registro en el bloque y persiste en disco.
				bucket->altaRegistro(this->listaParam,registro);
				bucket->incrementarCantRegistros();
				bucket->actualizarEspLibre();
				
				this->archivo->escribirBloque(numBucket, bucket);
				
				if (bucket) delete bucket;
			
				return ResultadosIndices::OK;
			}
			else
			// Hay OVERFLOW
			// Si el registro no entra en el bucket hay que crear un nuevo bucket.
			{				
				// Se duplica el tamaño de dispersión del bucket que se divide.
				
				bucket->setTamDispersion(bucket->getTamDispersion()*2);
				
				if (bucket->getTamDispersion() == 0)
					exit(0);
				// Se crea un nuevo bucket vacío. Se devuelve en numBucketNuevo el número del mismo. 
				unsigned int numBucketNuevo = 0;
				
				// Se crea un bucket vacío en memoria y se lo graba en disco.
				Bucket * nuevoBucket = new Bucket(numBucketNuevo,bucket->getTamDispersion(), this->archivo->getTamanioBloque());
				
				numBucketNuevo = this->archivo->escribirBloque(nuevoBucket);
				
				// Duplica la tabla o actualiza sus referencias dependiendo del tamaño de dispersión 
				// (ya duplicado) del bucket donde se produjo overflow.
				
				this->tabla->reorganizarTabla(bucket->getTamDispersion(), posicion, nuevoBucket->getNroBloque()); 
				
				this->redistribuirElementos(bucket, nuevoBucket);
				
				// Guarda en disco la redistribución de los elementos.
				this->archivo->escribirBloque(bucket->getNroBloque(), bucket);
				this->archivo->escribirBloque(nuevoBucket->getNroBloque(), nuevoBucket);
				
				if (bucket) delete bucket;				
				if (nuevoBucket) delete nuevoBucket;
				
				// Intenta nuevamente la inserción del registro.				
				return this->insertarRegistro(registro, clave);
			}
		}

		/* Este método se utiliza para hacer una operación de baja en el archivo.
		 * Si el registro se puede eliminar devuelve OK, si no existe el registro
		 * a eliminar, se devuelve NO_ENCONTRADO.
		 * En caso de que el bucket quede vacío, se considera la posibilidad de 
		 * disminuir el tamaño de la tabla de hash.
		 **/
		int Hash::eliminarRegistro(Clave &clave)
		{
			int resultado = ResultadosIndices::OK;
			
			// Se aplica la función de hash para ver en que bucket se debe buscar el
			// registro a eliminar.
			int posicion = aplicarHash(clave) % this->tabla->getTamanio();
			unsigned int nroBucket = this->tabla->getNroBucket(posicion);
			
			// Se levanta dicho bucket.
			Bucket * bucket = new Bucket(this->archivo, nroBucket);				
			unsigned short cantRegs = bucket->getCantRegs();
			
			//Se da de baja el registro cuya clave coincide con la pasada
			//por parametro.
			resultado = bucket->bajaRegistro(this->listaParam, clave);		
			
			if (resultado == ResultadosIndices::OK){

				// Se actualiza la baja en disco.
				this->archivo->escribirBloque(nroBucket, bucket);				
				 
				// Si hay un solo registro se debe verificar si se puede borrar el bucket del archivo.
				// Si el tamaño de dispersión del bucket coincide con el tamaño de la tabla, se elimina el bucket.
				if ( (cantRegs == 1) && (bucket->getTamDispersion() == this->tabla->getTamanio())) {
					
					// Se llama a marcar al bucket como vacío en el archivo.
					this->archivo->eliminarBloque(nroBucket);
					// Se renueva la referencia de la tabla que antes apuntaba al bucket que se eliminó.
					unsigned int nroBucketIgualDisp = this->tabla->buscarBucketIgualDispersion(posicion, bucket->getTamDispersion());
					
					this->tabla->setNroBucket(posicion, nroBucketIgualDisp);

					// Se actualiza el tamaño de dispersión del bloque y se lo guarda en archivo.
					this->dividirDispersion(nroBucketIgualDisp);

					// En caso de que deba realizarse una reducción a la mitad del tamaño de la tabla se lo hace.
					this->tabla->considerarReduccion();		
				} 
			}		
			

			if (bucket)
				delete bucket;
			
			return resultado;
		}

		int Hash::modificarRegistro(Clave &claveVieja, Clave &claveNueva, char* registroNuevo)
		{
			// Se elimina el registro de clave claveVieja y se lo reemplaza con el contenido
			// de registroNuevo
			if (this->eliminarRegistro(claveVieja) == ResultadosIndices::OK)
				return this->insertarRegistro(registroNuevo, claveNueva);
			else return ResultadosIndices::ERROR_MODIFICACION;
		}

		/*
		 * Recupera un registro a partir de una clave. 
		 * Si lo encuentra devuelve true. De lo contrario, devuelve false.
		 **/	
		bool Hash::recuperarRegistro(Clave &clave, char* &registro)
		{
			// Se aplica la función de hash para ver en que bucket se debe buscar el
			// registro a recuperar.
			int posicion           = aplicarHash(clave) % this->tabla->getTamanio();
			unsigned int nroBucket = this->tabla->getNroBucket(posicion);
				
			// Se levanta dicho bucket.
			Bucket * bucket = new Bucket(this->archivo, nroBucket);	
			unsigned short offsetToReg = 0;
			
			// Si no encuentro el registro buscado devuelvo false. Pero si lo encuentro,
			// tengo el offset al mismo en offsetToReg.
			if (bucket->buscarRegistro(this->listaParam, clave, &offsetToReg)){
			
				unsigned short longReg = 0;	
				char *datos = bucket->getDatos();
				
				// Obtiene la longitud.
				longReg = bucket->getTamanioRegistros(this->listaParam, datos + offsetToReg);	
				
				ListaNodos::const_iterator it;
				it = this->listaParam->begin();
				
				if (registro) 
					delete[] registro;
				
				if (it->tipo == TipoDatos::TIPO_VARIABLE){
					registro = new char[Tamanios::TAMANIO_LONGITUD + longReg];
					
					memcpy(registro, datos + offsetToReg,Tamanios::TAMANIO_LONGITUD + longReg);
				}else{
					registro = new char[longReg];
					
					memcpy(registro, datos + offsetToReg, longReg);
				}
				return true;
			}
			
			if (bucket)
				delete bucket;
			return false;
		}

	///////////////////////////////////////////////////////////////////////
	// Metodos privados
	///////////////////////////////////////////////////////////////////////

		/*
		 * Este método aplica una función de dispersión a la clave.
		 **/ 
		unsigned int Hash::aplicarHash(Clave &clave)
		{
			unsigned int aux = 0;
			void ** claveHash      = clave.getValorParaHash();
			char* claveSerializada = serializarClave(claveHash);
			
			int valorHash = hashInterno(claveSerializada);
			
			int ultimoBit = 0;
			
			// Doy vuelta los bits, y tomo los 16 menos significativos.
			for (int i = 0; i < 16; i++)
			{
				aux = aux << 1;
				ultimoBit = valorHash & 1;
				aux = aux | ultimoBit;
				valorHash = valorHash >> 1;
			}
			
			delete[] claveSerializada;
			delete[] claveHash;
			
			return aux;
		}
	
		/*
		 * Método utilizado internamente cuando se aplica la función de dispersión a una clave.
		 **/
		int Hash::hashInterno(char* clave)
		{
			int acumulador = 0;
			int largo      = strlen(clave);
			
			// Necesito que largo sea par. Para eso, si es impar le sumo 1, con lo cual
			// se usa el "\0" como parte de la clave.
			if (largo % 2 == 1)
				largo++;
			
			for(int j=0; j < largo; j+=2)
				acumulador = (acumulador + 100*clave[j] + clave[j+1]) % 19937 ;
				// Se usa el número 19937 por ser primo, lo cual genera una mejor distribución
				// de las claves. Además evita un overflow del acumulador.
			
			return acumulador;
		}

		/*
		 * Este método se encarga de redistribuir los registros contenidos en bucket 
		 * entre este mismo y bucketNuevo.
		 **/
		void Hash::redistribuirElementos(Bucket* &bucket, Bucket* nuevoBucket)
		{
			unsigned short longReg = 0;
			unsigned short offsetReg = bucket->getOffsetADatos();
			char * datos = bucket->getDatos();
			
			Clave *clave = NULL;
			int posicion = 0;
			unsigned int nroBucket = 0;
			
			// Se crea un bucket auxiliar para poner los registros que deberían quedar en bucket.
			Bucket * bucketAux = new Bucket(bucket->getNroBloque(),bucket->getTamDispersion(), bucket->getTamanioBloque());
			
			ListaNodos::const_iterator it = this->listaParam->begin();
			nodoLista regAtribute = *it;
			
			// Se obtiene el tipo de atributo del registro.
			int tipo = regAtribute.tipo;
		
			// Se recorren los registros reubicándolos.
			for(int i = 0; i < bucket->getCantRegs(); i++)
			{		
				// Obtengo el tamaño del registro.	
				longReg = bucket->getTamanioRegistros(this->listaParam, &datos[offsetReg]);
				
				// Obtengo la clave primaria.
				clave = bucket->getClavePrimaria(this->listaParam, &datos[offsetReg]);
				
				
				posicion = aplicarHash(*clave) % this->tabla->getTamanio(); 
				
				nroBucket = this->tabla->getNroBucket(posicion);
				
				delete clave;
				
				// Se decide en cual de los 2 buckets se inserta el registro.
				if (nroBucket == nuevoBucket->getNroBloque())
				{
					nuevoBucket->altaRegistro(this->listaParam, &datos[offsetReg]);
					nuevoBucket->incrementarCantRegistros();
				}
				else
				{
					bucketAux->altaRegistro(this->listaParam, &datos[offsetReg]);
					bucketAux->incrementarCantRegistros();
				}
				
				offsetReg += longReg;
				
				if(tipo == TipoDatos::TIPO_VARIABLE)
					offsetReg += Tamanios::TAMANIO_LONGITUD;
			}
			
			bucketAux->actualizarEspLibre();
			nuevoBucket->actualizarEspLibre();
			
			if (bucket){
				delete bucket;			
				bucket = bucketAux;
			}
		}

		/*
		 * Este método se engarga de dividir el tamaño de dispersión del bucket nroBucket
		 * y actualizarlo en el archivo.
		 **/
		void Hash::dividirDispersion(unsigned int nroBucket)
		{
			// Se levanta el bucket de disco.
			Bucket* bucket = new Bucket(this->archivo, nroBucket);
			
			// Se divide el tamaño de dispersión.
			bucket->setTamDispersion(bucket->getTamDispersion()/2);
			
			this->archivo->escribirBloque(nroBucket, bucket);
			
			if (bucket)
				delete bucket;
		}

		/* Si el indice de hash está trabajando con registros de tipo variable retorna true;
		 * de lo contrario retorna false.
		 **/ 
		bool Hash::esRegistroVariable()
		{
			ListaNodos::const_iterator it;
			it = this->listaParam->begin();
			
			return (it->tipo == TipoDatos::TIPO_VARIABLE);
		}

		char* Hash::serializarClave(void** claveVoid)
		{
			ListaNodos::const_iterator it = this->listaParam->begin();
			
			int tipo = 0;			
			unsigned char cantClaves = it->cantClaves; //Cantidad de claves primarias
			
			// En este vector se pondrá la longitud de cada clave.
			int* vectorTamanios = new int[cantClaves];
			int tamanio = 0;
			int i = 0;  // recorre las posiciones del vectorTamanios.
			
			for( ++it ;it != listaParam->end() ; ++it){
				tipo = it->tipo;
				if (it->pk == "true"){
					
					switch(tipo){
					
					case TipoDatos::TIPO_BOOL:
						vectorTamanios[i] = sizeof(bool);
						tamanio += sizeof(bool);
						break;
					case TipoDatos::TIPO_CHAR:
						vectorTamanios[i] = sizeof(char);
						tamanio += sizeof(char);
						break;
					case TipoDatos::TIPO_SHORT:
						vectorTamanios[i] = sizeof(short);
						tamanio += sizeof(short);
						break;
					case TipoDatos::TIPO_ENTERO:
						vectorTamanios[i] = sizeof(int);
						tamanio += sizeof(int);
						break;
					case TipoDatos::TIPO_FLOAT:
						vectorTamanios[i] = sizeof(float);
						tamanio += sizeof(float);
						break;
					case TipoDatos::TIPO_FECHA:
						vectorTamanios[i] = Tamanios::TAMANIO_FECHA;
						tamanio += Tamanios::TAMANIO_FECHA;
						break;
					case TipoDatos::TIPO_STRING:
						vectorTamanios[i] = strlen((char*)claveVoid[i]);
						tamanio += vectorTamanios[i];
						break;
					}
					i++;
				}
			}
				
			char* clave = new char[tamanio + 1];
			unsigned int posicion = 0;
			
			for(unsigned char i = 0; i < cantClaves; i++){
				memcpy(clave + posicion, claveVoid[i],vectorTamanios[i]);
				posicion += vectorTamanios[i];
			}
			
			*(clave + tamanio) = 0;
			
			if (vectorTamanios)
				delete[] vectorTamanios;
			
			return clave;
				
		}

