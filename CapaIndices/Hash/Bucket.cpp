///////////////////////////////////////////////////////////////////////////
//	Archivo   : Bucket.cpp
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clase Bucket.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#include "Bucket.h"

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////

	Bucket::Bucket(ArchivoIndice* indiceHash, unsigned int referencia)
	{
		/*
		tamanio = tamanioBucket;
		datos = new char[tamanio];
		registro = NULL;
		*/
		registro = NULL;
		indiceHash->leer(referencia, this)
	}

	Bucket::~Bucket()
	{
		delete[] datos;
		if (registro)
			delete[]registro;
	}
	

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
	
	/*
	* Este método busca un registro en el bloque, si lo encuentra devuelve true y guarda   
	* el contenido del mismo en el atributo registro; de lo contrario, devuelve false.		
	*/
	bool Bucket::buscarRegistro(Clave* clave)
	{ 
		if (tamanioClave == -1) // si la clave es de longitud variable.
			return buscarRegClaveV(clave,tamanioClave);		
		else
			return buscarRegClaveF(clave,tamanioClave);
	}

	/*
void Bucket::leer(int offset)
{
	// TODO: pedir a la capa fisica que levante tamanio desde offset y lo asigne a datos.
	
	// Obtengo el espacio libre.
	char * auxEspLibre = new char[5];
	memcpy(auxEspLibre,datos,4);
	auxEspLibre[4] = '\0';
	espacioLibre = atoi(auxEspLibre);
	delete[] auxEspLibre;

	//Obtengo el tamaño de dispersión.
	char* auxTD = new char[2];
	memcpy(auxTD,&datos[4],1);
	auxTD[1]='\0';
	tamDispersion = atoi(auxTD); 
	delete[] auxTD;
	
	//Obtengo la cantidad de registros.
	char* auxCRegs = new char[2];
	memcpy(auxCRegs,&datos[5],1);
	auxCRegs[1] = '\0';
	cantRegs = atoi(auxCRegs);
	delete[] auxCRegs;
	
	
}

void Bucket::escribir(int offset)
{
	// TODO: pedir a la capa fisica q escriba datos a partir de offset en el archivo.
}
*/


	//TODO Como el Hash desconoce el tipo de dato que esta siendo almacenado
	//en el y trata todo como char, es imposible usar la clase Clave para manejar
	//las claves que estan los registros, a menos que exista alguna manera de conocer
	//dicho tipo. Tratar toda clave como si fuese un char* es saludable ?
	bool Bucket::buscarRegClaveV(Clave* clave)
	{
		char * claveAux;
		bool notFound = true;
		int offset = 6;
		char* aux = new char[3]; // lo uso para obtener la longitud del registro.
		short int longReg;
		RegistroV* reg;
				
		for (int i = 1; (i <= cantRegs)& notFound; i++)
		{
			/*
			 * TODO: definir bien la estructura del bloque. Supongo que los primeros 4 bytes 
			 * tienen el espacio libre, luego 1 para el tamaño de dispersion y 1 para la cantidad de 
			 * registros. A continuacion estan los registros. 
			 */
			memcpy(aux,&datos[offset],2);  // Obtengo la longitud del registro
			offset += 2;
			aux[2]= '\0'; //Agrego el '\0' para poder transformar a int.
			longReg = atoi(aux);
	
			reg = new RegistroV(&datos[offset],longReg);
			claveAux = reg->getClave();
	
			if (strcmp(clave,claveAux) == 0)
			{
				notFound = false;
				registro = new char[longReg+1];
				memcpy(registro, &datos[offset],longReg);
				registro[longReg] = '\0';
			}
			offset += longReg;
			delete reg;
			delete[] claveAux;
		}
		delete[]aux;
		return (!notFound);
	}
	
	bool Bucket::buscarRegClaveF(Clave* clave)
	{
		Clave* claveAux = NULL;
		string valorClave;
		bool notFound = true;
		int offset = 6;
		char* aux = new char[3]; 
		short int longReg;
		
		for (int i = 1; (i <= cantRegs)&& notFound; i++)
		{
			//Obtengo la longitud del registro.
			memcpy(aux,&datos[offset],2);
			offset += 2;
			aux[2]= '\0'; //Agrego el '\0' para poder transformar a int.
			longReg = atoi(aux);
			
			//Obtengo la clave del registro.
			claveAux = new ClaveVariable(valorClave);
			memcpy(claveAux,&datos[offset],tamanioClave);
			claveAux[tamanioClave]='\0';
			
			if (strcmp(clave,claveAux) == 0)
			{
				notFound = false;
				registro = new char[longReg+1];
				memcpy(registro, &datos[offset],longReg);
				registro[longReg] = '\0';
			}
			offset += longReg;
		}
		return (!notFound);
	}