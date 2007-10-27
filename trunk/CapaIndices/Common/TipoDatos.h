#ifndef TIPODATOS_H_
#define TIPODATOS_H_
#include <iostream>

using namespace std;

class TipoDatos
{
	public:
		const static string TIPO_ENTERO;
		const static string TIPO_SHORT;
		const static string TIPO_CHAR;
		const static string TIPO_BOOL;
		const static string TIPO_REAL;
		const static string TIPO_VARIABLE;
		const static string TIPO_FECHA;
};

const string TipoDatos::TIPO_ENTERO   = "tipoEntero";
const string TipoDatos::TIPO_SHORT    = "tipoShort";
const string TipoDatos::TIPO_CHAR     = "tipoChar";
const string TipoDatos::TIPO_REAL     = "tipoReal";
const string TipoDatos::TIPO_VARIABLE = "tipoVariable";
const string TipoDatos::TIPO_FECHA    = "tipoFecha";

#endif /*TIPODATOS_H_*/
