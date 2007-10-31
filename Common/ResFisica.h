#ifndef RESFISICA_H_
#define RESFISICA_H_

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ResFisica
//			(Clase utilitaria con constantes de error y resultados que 
//			arroja la Capa Fisica hacia el resto de las capas).
///////////////////////////////////////////////////////////////////////////
class ResFisica
{	
	public:
		static const char OK               = -1;
		static const char NO_ABIERTO       = -2;
		static const char ERROR_ESCRITURA  = -3;
		static const char ERROR_LECTURA    = -4;
		static const char ERROR_POSICION   = -5;
		static const char BLOQUES_OCUPADOS = -6;
		
}; /*Fin clase CodArchivo*/
#endif /*RESFISICA_H_*/
