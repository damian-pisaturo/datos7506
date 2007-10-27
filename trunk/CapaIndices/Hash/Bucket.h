///////////////////////////////////////////////////////////////////////////
//	Archivo   : Bucket.h
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clase Bucket.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef BUCKET_H_
#define BUCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Common/BloqueIndice.h"
#include "RegistroV.h"
#include "../../CapaFisica/ArchivoIndicePadre.h"
#include "../../CapaFisica/ArchivoIndice.h"
#include "../Bloque/Bloque.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Bucket (Permite el manejo de buckets en memoria)
//////////////////////////////////////////////////////////////////////////

class Bucket : public BloqueIndice , Bloque 
{
private:
	//////////////////////////////////////////////////////////////////////
	// Atributos
	//////////////////////////////////////////////////////////////////////
	char* datos;
	unsigned int tamanio; //TODO El tamanio del bucket vendra en el ArchivoIndiceHash.
	char* registro;
	bool vacio; //Indica si el bloque contiene al menos un registro.
	unsigned short numBucket; //Indica el numero de bucket dentro del archivo.
	unsigned short tamDispersion;
	unsigned short cantRegs;
	unsigned short espLibre;
	ArchivoIndiceHash *archivo;
	
public:
///////////////////////////////////////////////////////////////////////
// Constructor/Destructor
///////////////////////////////////////////////////////////////////////

	/*Crea un Bucket vacio*/
	Bucket(ArchivoIndice *indiceHash, unsigned short tamDispersion);
		
	/*Crea un Bucket con los datos del bloque cuya 
	 * referencia en el archivo es la pasada por parametro.
	 */
	Bucket(ArchivoIndice* indiceHash, unsigned int referencia);
	virtual ~Bucket();
	
///////////////////////////////////////////////////////////////////////
// Metodos publicos
///////////////////////////////////////////////////////////////////////

	
	/*
	 * Busca un registro por su clave dentro del bucket. Si existe lo carga en el 
	 * atributo registro y devuelve true; de lo contrario devuelve false.
	 */ 
	bool buscarRegistro(Clave* clave);
	
	char* getDatos();
	void setEspLibre(unsigned short eLibre);
	void setTamDispersion(unsigned short tDisp);
	void setCantRegs(unsigned short cRegs);
	
private:
///////////////////////////////////////////////////////////////////////
// Metodos privadosgetDatos()
///////////////////////////////////////////////////////////////////////
	
};

#endif /*BUCKET_H_*/
