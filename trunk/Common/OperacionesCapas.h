#ifndef OPERACIONESCAPAS_H_
#define OPERACIONESCAPAS_H_

class OperacionesCapas
{
	public:
		const static unsigned char FISICA_ESCRIBIR_NODO        = 0;
		const static unsigned char FISICA_LEER_NODO            = 1;
		const static unsigned char FISICA_MODIFICAR_NODO       = 2;
		const static unsigned char FISICA_ELIMINAR_NODO        = 3;
		
		const static unsigned char FISICA_LEER_LISTA           = 4;
		const static unsigned char FISICA_ESCRIBIR_LISTA       = 5;
		const static unsigned char FISICA_MODIFICAR_LISTA      = 6;
		const static unsigned char FISICA_ELIMINAR_LISTA       = 7;
		
		const static unsigned char FISICA_ESCRIBIR_BUCKET      = 8;
		const static unsigned char FISICA_LEER_BUCKET          = 9;
		const static unsigned char FISICA_MODIFICAR_BUCKET     = 10;
		const static unsigned char FISICA_ELIMINAR_BUCKET      = 11;
		const static unsigned char FISICA_LEER_TABLA_HASH      = 12;
		const static unsigned char FISICA_ESCRIBIR_TABLA_HASH  = 13;
		
		const static unsigned char FISICA_ESCRIBIR_DATO        = 14;
		const static unsigned char FISICA_LEER_DATO            = 15;
		const static unsigned char FISICA_MODIFICAR_DATO       = 16;
		const static unsigned char FISICA_ELIMINAR_DATO        = 17;
};

#endif /*OPERACIONESCAPAS_H_*/
