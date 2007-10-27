#ifndef TAMANIOS_
#define TAMANIOS_

class Tamanios {
	
	public:
		static const unsigned char TAMANIO_REFERENCIA			= sizeof(unsigned int);
		static const unsigned char TAMANIO_NIVEL				= sizeof(unsigned char);
		static const unsigned char TAMANIO_ESPACIO_LIBRE		= sizeof(unsigned short);
		static const unsigned char TAMANIO_LONGITUD_CADENA		= sizeof(unsigned char);
		static const unsigned char TAMANIO_LONGITUD				= sizeof(unsigned short);
		static const unsigned char TAMANIO_CANTIDAD_REGISTROS	= sizeof(unsigned short);
		static const unsigned char TAMANIO_DISPERSION			= sizeof(unsigned short);
};

#endif /*TAMANIOS_*/
