#ifndef TIPODATOS_H_
#define TIPODATOS_H_

class TipoDatos
{
	public:
		// Indican los distintos tipos de datos que soporta el framework.
		const static int TIPO_ENTERO	= 0;
		const static int TIPO_SHORT		= 1;
		const static int TIPO_CHAR		= 2;
		const static int TIPO_BOOL		= 3;
		const static int TIPO_FLOAT		= 4;
		const static int TIPO_FECHA		= 5;
		const static int TIPO_STRING	= 6; 
		const static int TIPO_COMPUESTO	= 7;
		
		// Las constantes a continuación se usan para saber si los
		// registros son de longitud variable o fija.
		const static int TIPO_VARIABLE	= 8;
		const static int TIPO_FIJO		= 9;
};

#endif /*TIPODATOS_H_*/
