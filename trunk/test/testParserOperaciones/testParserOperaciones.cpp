#include "../../Common/ResultadosMetadata.h"
#include "../../Common/Resultado.h"
#include "../../Common/ImpresorResultado.h"
#include "../../CapaConsultas/ParserOperaciones/ParserOperaciones.h"
#include "../../Common/DefManager/DefinitionsManager.h"


using namespace std;

int main(int argc, char* argv[]) {
	
	Resultado resultado;
	int resultadoProxOp;
	unsigned char tipoOperacion;
	ParserOperaciones parser("");
	string nombreTipo;
	MapaValoresAtributos* map;
	EstructuraCampos estructura;
	EstructuraConsulta estructuraConsulta;
	
	while ((resultadoProxOp = parser.proximaOperacion()) != ResultadosParserOperaciones::FIN_ARCHIVO) {
		if (resultadoProxOp != ResultadosParserOperaciones::SINTAXIS_CORRECTA){
			resultado << resultadoProxOp;
			cout << resultado << endl;
		}
		else{
			tipoOperacion = parser.getTipoOperacion();
			cout << "Tipo Operacion: " << (int) tipoOperacion << endl;
			
			switch (tipoOperacion){
				case OperacionesCapas::CONSULTAS_ALTA: {
					nombreTipo = parser.getNombreTipo();
					map = parser.getMapaValoresAtributos();
					cout << "TIPO: " << nombreTipo << endl;
					cout << "NOMBRE: " << (*map)["NOMBRE"] << endl;
					cout << "DNI: " << (*map)["DNI"] << endl;
					cout << "FECHANACIMIENTO: " << (*map)["FECHANACIMIENTO"] << endl;
				} break;
				case OperacionesCapas::CONSULTAS_BAJA:
					nombreTipo = parser.getNombreTipo();
					cout << "TIPO: " << nombreTipo << endl;
					estructura = parser.getEstructuraCampos();
					cout << "Operacion: " << (int) estructura.operacion << endl;
					for (ListaCampos::iterator iter = estructura.listaCampos.begin(); iter != estructura.listaCampos.end(); ++iter){
						cout << "Nombre: " << (*iter).nombreCampo << endl;
						cout << "Valor: " << (*iter).valorCampo << endl;
						cout << "Op: " << (int)(*iter).operacion << endl;
					}
					break;
				case OperacionesCapas::CONSULTAS_MODIFICACION: {
					nombreTipo = parser.getNombreTipo();
					map = parser.getMapaValoresAtributos();
					cout << "TIPO: " << nombreTipo << endl;
					cout << "NOMBRE: " << (*map)["NOMBRE"] << endl;
					cout << "DNI: " << (*map)["DNI"] << endl;
					cout << "FECHANACIMIENTO: " << (*map)["FECHANACIMIENTO"] << endl;
					
					estructura = parser.getEstructuraCampos();
					cout << "Operacion: " << (int) estructura.operacion << endl;
					for (ListaCampos::iterator iter = estructura.listaCampos.begin(); iter != estructura.listaCampos.end(); ++iter){
						cout << "Nombre: " << (*iter).nombreCampo << endl;
						cout << "Valor: " << (*iter).valorCampo << endl;
						cout << "Op: " << (int)(*iter).operacion << endl;
					}
				} break;
				
				case OperacionesCapas::CONSULTAS_CONSULTA: {
					estructuraConsulta = parser.getEstructuraConsulta();
					for (ListaEstructuraNombres::iterator iter = estructuraConsulta.listaCamposSeleccionados.begin(); iter != estructuraConsulta.listaCamposSeleccionados.end(); ++iter){
						cout << "Tipo: " << (*iter).nombreTipo << endl;
						cout << "Nombre: " << (*iter).nombreCampo << endl;
					}
					for(ListaStrings::iterator iter = estructuraConsulta.listaNombresTipos.begin(); iter != estructuraConsulta.listaNombresTipos.end(); ++iter){
						cout << *iter << endl;
					}
					cout << "Op del Join: " << (int) estructuraConsulta.estructuraJoins.operacion << endl;
					for(ListaOperaciones::iterator iter = estructuraConsulta.estructuraJoins.listaOperaciones.begin(); iter != estructuraConsulta.estructuraJoins.listaOperaciones.end(); ++iter){
						cout << "TipoIzq: " << (*iter).estructuraNombresIzq.nombreTipo << endl;
						cout << "CampoIzq: " << (*iter).estructuraNombresIzq.nombreCampo << endl;
						cout << "Op: " << (int) (*iter).operacion << endl;
						cout << "TipoDer: " << (*iter).estructuraNombresDer.nombreTipo << endl;
						cout << "CampoDer: " << (*iter).estructuraNombresDer.nombreCampo << endl;
					}
					cout << "Op del Where: " << (int) estructuraConsulta.estructuraWhere.operacion << endl;
					for(ListaOperaciones::iterator iter = estructuraConsulta.estructuraWhere.listaOperaciones.begin(); iter != estructuraConsulta.estructuraWhere.listaOperaciones.end(); ++iter){
						cout << "TipoIzq: " << (*iter).estructuraNombresIzq.nombreTipo << endl;
						cout << "CampoIzq: " << (*iter).estructuraNombresIzq.nombreCampo << endl;
						cout << "Op: " << (int) (*iter).operacion << endl;
						cout << "TipoDer: " << (*iter).estructuraNombresDer.nombreTipo << endl;
						cout << "CampoDer: " << (*iter).estructuraNombresDer.nombreCampo << endl;
					}
					for (ListaEstructuraNombres::iterator iter = estructuraConsulta.listaOrderBy.begin(); iter != estructuraConsulta.listaOrderBy.end(); ++iter){
						cout << "Tipo del order by: " << (*iter).nombreTipo << endl;
						cout << "Campo del order by: " << (*iter).nombreCampo << endl;
					}
					
				} break;
			}
		}
	
	}
	
	cout << "Fin Test" << endl;

}
