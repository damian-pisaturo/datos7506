#ifndef TAMANIOS_H
#define TAMANIOS_H

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Tamanios
//			(Clase utilitaria con constantes que definen tamaños 
//			 de atributos invariantes en la implementacion).
///////////////////////////////////////////////////////////////////////////
class Tamanios 
{	
	public:
		static const unsigned char TAMANIO_REFERENCIA		  = sizeof(unsigned int);
		static const unsigned char TAMANIO_NIVEL			  = sizeof(unsigned char);
		static const unsigned char TAMANIO_ESPACIO_LIBRE	  = sizeof(unsigned short);
		static const unsigned char TAMANIO_LONGITUD_CADENA	  = sizeof(unsigned char);
		static const unsigned char TAMANIO_LONGITUD			  = sizeof(unsigned short);
		static const unsigned char TAMANIO_CANTIDAD_REGISTROS = sizeof(unsigned short);
		static const unsigned char TAMANIO_DISPERSION		  = sizeof(unsigned short);
		static const unsigned char TAMANIO_FECHA			  = sizeof(unsigned short) + 2*sizeof(unsigned char);
		static const unsigned short TAMANIO_BLOQUE_DATO		  = 4096;
		static const unsigned short TAMANIO_IDENTIFICADOR	  = sizeof(unsigned long);
};

#endif /*TAMANIOS_H*/
