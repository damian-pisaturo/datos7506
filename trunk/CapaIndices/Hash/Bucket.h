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
#include "../../CapaFisica/ArchivoIndice.h"
#include "../Bloque/Bloque.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Bucket (Permite el manejo de buckets en memoria)
//////////////////////////////////////////////////////////////////////////

class Bucket : public BloqueIndice , public Bloque 
{
private:
//////////////////////////////////////////////////////////////////////
// Atributos
//////////////////////////////////////////////////////////////////////
	// TODO Los datos estan en bloque.
	//TODO El tamanio del bucket vendra en el ArchivoIndiceHash, ademas lo tiene la clase bloque.
	// El num de bucket esta en el bloque.
	unsigned short tamDispersion;
	unsigned short cantRegs;
	
///////////////////////////////////////////////////////////////////////
// Metodos privados
///////////////////////////////////////////////////////////////////////
	
public:
///////////////////////////////////////////////////////////////////////
// Constructor/Destructor
///////////////////////////////////////////////////////////////////////

	/*Crea un Bucket vacio*/
	Bucket(unsigned short numBucket,unsigned short tamDispersion, ArchivoIndice *indiceHash);
	
	/*Crea un Bucket con los datos del bloque cuya 
	 * referencia en el archivo es la pasada por parametro.
	 */
	Bucket(ArchivoIndice* indiceHash, unsigned int referencia);
	virtual ~Bucket();

	
	
///////////////////////////////////////////////////////////////////////
// Metodos publicos
///////////////////////////////////////////////////////////////////////
	int insertar(char* clave, char* registro);
	int borrar(char * clave);
	Bucket* splitBucket();
	int redistribuirClaves(Bucket &bucket);	
	unsigned short getOffsetToRegs();
	void setTamDispersion(unsigned short tDisp);
	unsigned short getTamDispersion();
	void setCantRegs(unsigned short cRegs);
	
};

#endif /*BUCKET_H_*/
