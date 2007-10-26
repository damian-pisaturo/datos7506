#ifndef NOMBRESCAPAS_H_
#define NOMBRESCAPAS_H_
#include <iostream>

using namespace std;

//TODO Quiero que ande esto, CANEJO !
//		Quiero constantes string en una clase utilitaria para los
//		nombres de los ejecutables de las capas.
//		Quiero que Servetto me invite a su fiesta de 15.

class NombreCapas
{
	public:
		const static string CapaFisica    = "capaFisica";
		const static string CapaIndices   = "capaIndices";
		const static string CapaConsultas = "capaConsultas";
		const static string CapaMetadata  = "capaMetadata";
};

#endif /*NOMBRESCAPAS_H_*/
