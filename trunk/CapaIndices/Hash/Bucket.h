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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../Manager/IndiceManager/IndiceManager.h"
#include "../../Common/Bloque/Bloque.h"


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Bucket (Permite el manejo de buckets en memoria)
///////////////////////////////////////////////////////////////////////////

class Bucket : public Bloque , public BloqueIndice
{
	
	private:
	//////////////////////////////////////////////////////////////////////
	// Atributos
	//////////////////////////////////////////////////////////////////////

		unsigned short tamDispersion;
		unsigned short cantRegs;
	
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		
		/*
		 * Crea un Bucket con los datos del bloque cuyo numBucket es la posición del mismo 
		 * en el archivo.
		 **/
		Bucket(IndiceHashManager* indiceHash, unsigned int numBucket);
		
		/*
		 * Constructor utilizado para crear un bucket en memoria y no acceder a disco
		 **/
		Bucket(unsigned int nroBucket,unsigned short tamDispersion,unsigned int tamanioBloque);
		
		/*
		 * Constructor utilizado para crear un bucket en memoria con el tipo de  organizacion del mismo
		 * */
		Bucket(IndiceHashManager* indiceHash, unsigned int numBucket, int tipoOrganizacion);
		
		virtual ~Bucket();

	
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		void incrementarCantRegistros();
		void decrementarCantRegistros();
		void actualizarEspLibre();

	///////////////////////////////////////////////////////////////////////
	// Getters/Setters
	///////////////////////////////////////////////////////////////////////
		void setTamDispersion(unsigned short tDisp);
		unsigned short getTamDispersion();
		void setCantRegs(unsigned short cRegs);
		unsigned short getCantRegs();
	
};

#endif /*BUCKET_H_*/
