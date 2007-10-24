///////////////////////////////////////////////////////////////////////////
//	Archivo   : Bloque.h
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clase abstracta BloqueIndice.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef BLOQUEINDICE_H_
#define BLOQUEINDICE_H_

#include "Tamanios.h"
#include "Clave.h"
//#include "ArchivoIndice.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: BloqueIndice (Abstracta)
//////////////////////////////////////////////////////////////////////////
class BloqueIndice
{
	private:
		///////////////////////////////////////////////////////////////////////////
		// Atributo
		///////////////////////////////////////////////////////////////////////////
		unsigned int espacioLibre;
		
	public:
		BloqueIndice() {}
		
		~BloqueIndice() {}
		
		///////////////////////////////////////////////////////////////////////////
		// Getter/Setter
		///////////////////////////////////////////////////////////////////////////		
		unsigned int getEspacioLibre() const
		{
			return espacioLibre;
		}
		
		void setEspacioLibre(unsigned int espacio)
		{
			this->espacioLibre = espacio;
		}
};

#endif /*BLOQUEINDICE_H_*/
