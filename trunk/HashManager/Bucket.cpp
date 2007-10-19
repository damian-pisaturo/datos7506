#include "Bucket.h"

Bucket::Bucket(int tamanioBucket)
{
	tamanio = tamanioBucket;
	datos = new char[tamanio];
	registro = NULL;
}

Bucket::~Bucket()
{
	delete[] datos;
}

void Bucket::leer(int offset)
{
	// TODO: pedir a la capa fisica que levante tamanio desde offset
}

void Bucket::escribir(int offset)
{
	// TODO: pedir a la capa fisica q escriba datos a partir de offset en el archivo.
}

/****************************************************************************************/
/* Este método busca un registro en el bloque, si lo encuentra devuelve true y guarda   */
/* el contenido del mismo en el atributo registro; de lo contrario, devuelve false.		*/
/****************************************************************************************/
bool Bucket::buscarRegistro(char *clave,int tamanioClave)
{ // OJO este metodo esta a medio hacer!
	char * claveAux;
	if (tamanioClave == -1)
	{
		bool notFound;
		char* aux = new char[2]; // lo uso para obtener la longitud del registro.
		int longReg;
		int offset = 1;
		RegistroV* reg;
		
		for (int i = 1; (i <= cantRegs)& notFound; i++)
		{
			/*TODO: definir bien la estructura del bloque. Supongo que el primer char tiene 
			 * el nroBloque y a continuacion estan los registros. */
			memcpy(aux,&datos[offset],1);
			//TODO: ver si lo del \0 es necesario.
			aux[1]= '\0'; //Agrego el '\0' para poder transformar a int.
			longReg = atoi(aux);
			reg = new RegistroV(&datos[offset+1],longReg);
			claveAux = reg->getClave();
			
		}
	}
	else
	{
		
	}
	return true;
}

