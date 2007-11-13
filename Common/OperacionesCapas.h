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
		
		const static unsigned char FISICA_ESCRIBIR_NODO_DOBLE   = 8;
		const static unsigned char FISICA_LEER_NODO_DOBLE       = 9;
		const static unsigned char FISICA_MODIFICAR_NODO_DOBLE  = 10;
		const static unsigned char FISICA_ELIMINAR_NODO_DOBLE   = 11;
		
		const static unsigned char FISICA_ESCRIBIR_BUCKET      = 12;
		const static unsigned char FISICA_LEER_BUCKET          = 13;
		const static unsigned char FISICA_MODIFICAR_BUCKET     = 14;
		const static unsigned char FISICA_ELIMINAR_BUCKET      = 15;
		const static unsigned char FISICA_LEER_TABLA_HASH      = 16;
		const static unsigned char FISICA_ESCRIBIR_TABLA_HASH  = 17;
		
		const static unsigned char FISICA_ESCRIBIR_DATO        = 18;
		const static unsigned char FISICA_LEER_DATO            = 19;
		const static unsigned char FISICA_MODIFICAR_DATO       = 20;
		const static unsigned char FISICA_ELIMINAR_DATO        = 21;
		const static unsigned char FISICA_BUSCAR_ESPACIO_LIBRE = 22;		
		
		const static unsigned char INDICES_CONSULTAR			= 'C';
		const static unsigned char INDICES_CONSULTAR_EXISTENCIA	= 'E';
		const static unsigned char INDICES_BUSCAR_ESPACIO_LIBRE	= 'L';
		const static unsigned char INDICES_INSERTAR				= 'A';
		const static unsigned char INDICES_ELIMINAR				= 'B';
		const static unsigned char INDICES_MODIFICAR			= 'M';
};

#endif /*OPERACIONESCAPAS_H_*/
