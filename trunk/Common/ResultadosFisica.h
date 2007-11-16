#ifndef RESFISICA_H_
#define RESFISICA_H_

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: TipoOrganizacion
//			(Clase utilitaria con constantes que definen el tipo de 
// 			organizacion de los archivos).
///////////////////////////////////////////////////////////////////////////

class TipoOrganizacion
{
		static const unsigned char REG_FIJOS     = 0;
		static const unsigned char REG_VARIABLES = 1;
};

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ResultadosFisica
//			(Clase utilitaria con constantes de error y resultados que 
//			arroja la Capa Fisica hacia el resto de las capas).
///////////////////////////////////////////////////////////////////////////

class ResultadosFisica
{	
	public:
		static const char OK                       = -1;
		static const char NO_ABIERTO               = -2;
		static const char ERROR_ESCRITURA          = -3;
		static const char ERROR_LECTURA            = -4;
		static const char ERROR_POSICION           = -5;
		static const char BLOQUES_OCUPADOS         = -6;
		static const char CANT_ARGUMENTOS_INVALIDA = -7;
		static const char OPERACION_INVALIDA 	   = -8;
		
		
}; /*Fin clase ResultadosFisica*/
#endif /*RESFISICA_H_*/
