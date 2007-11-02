#ifndef OPERACIONESCAPAS_H_
#define OPERACIONESCAPAS_H_

class OperacionesCapas
{
	public:
		const static unsigned char FISICA_ESCRIBIR_NODO        = 0;
		const static unsigned char FISICA_LEER_NODO            = 1;
		const static unsigned char FISICA_MODIFICAR_NODO       = 2;
		const static unsigned char FISICA_ELIMINAR_NODO        = 3;
		
		const static unsigned char FISICA_ESCRIBIR_BUCKET      = 4;
		const static unsigned char FISICA_LEER_BUCKET          = 5;
		const static unsigned char FISICA_MODIFICAR_BUCKET     = 6;
		const static unsigned char FISICA_ELIMINAR_BUCKET      = 7;
		const static unsigned char FISICA_LEER_TABLA_HASH      = 8;
		const static unsigned char FISICA_ESCRIBIR_TABLA_HASH  = 9;
		
		const static unsigned char FISICA_ESCRIBIR_DATO       = 10;
		const static unsigned char FISICA_LEER_DATO           = 11;
		const static unsigned char FISICA_MODIFICAR_DATO      = 12;
		const static unsigned char FISICA_ELIMINAR_DATO       = 13;
};

#endif /*OPERACIONESCAPAS_H_*/
