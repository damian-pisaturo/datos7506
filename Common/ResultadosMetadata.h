#ifndef RESULTADOSMETADATA_H_
#define RESULTADOSMETADATA_H_


class ResultadosMetadata
{	
	public:
		static const char OK                 		= -50;
		static const char OPERACION_INVALIDA 		= -51;
		static const char ENTERO_INVALIDO           = -52;
		static const char SHORT_INVALIDO            = -57;
		static const char CHAR_INVALIDO             = -58;
		static const char FECHA_INVALIDA            = -59;
		static const char BOOL_INVALIDO             = -60;
		static const char STRING_INVALIDO			= -61;
		static const char FIN_REGISTROS         	= -53;
		static const char CAMPOS_IGUALES			= -54;
		static const char CAMPOS_DISTINTOS			= -55;
		static const char ERROR_DISTINTO_TIPO_DATO	= -56;
		
}; /*Fin clase ResultadosMetadata*/

#endif /*RESULTADOSMETADATA_H_*/
