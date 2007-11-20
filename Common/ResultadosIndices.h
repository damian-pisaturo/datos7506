#ifndef RESINDICES_H_
#define RESINDICES_H_

class ResultadosIndices {

	public:
		static const char OK						= -100;
		static const char ERROR_INSERCION			= -101;
		static const char ERROR_ELIMINACION			= -102;
		static const char ERROR_MODIFICACION		= -103;
		static const char CLAVE_NO_ENCONTRADA		= -104;
		static const char CLAVE_ENCONTRADA			= -105;
		static const char CLAVE_DUPLICADA			= -106;
		static const char SOBREFLUJO       			= -107;
		static const char REGISTRO_NO_ENCONTRADO	= -108;
		static const char REQUIERE_REGISTRO			= -109;
		static const char REQUIERE_BLOQUE			= -110;
		static const char ERROR_CONSULTA			= -111;
		static const char SIN_ESPACIO_LIBRE			= -112;
};

#endif /*RESINDICES_H_*/
