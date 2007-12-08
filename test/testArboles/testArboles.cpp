#include "../../CapaIndices/Manager/IndiceManager/IndiceManager.h"
#include "../../CapaIndices/BStar/BStarTree.h"
#include "../../CapaIndices/BPlus/BPlusTree.h"
#include "../../CapaIndices/Common/IndiceManagerFactory.h"

using namespace std;

int main(int argc, char* argv[]) {
	/*
	ComuDatos pipe(argv);
	unsigned char codOp = 0;
	string nombreTipo;
	
	pipe.parametro(0, codOp);
	pipe.parametro(1, nombreTipo);
	
	procesarOperacion(codOp, nombreTipo, pipe);
	
	*/
	// MÉTODOS DE PRUEBA PARA UN ÁRBOL B+
	/*
		DefinitionsManager::ListaTiposAtributos* lista = DefinitionsManager::getInstance().getListaTiposAtributos("PERSONA");
		IndiceArbol indice(TipoIndices::GRIEGO, TipoDatos::TIPO_ENTERO, lista,
						   TipoIndices::ARBOL_BS, 48, 48, "locura",
						   TipoDatos::TIPO_VARIABLE);
		
		char* null = NULL;*/

	/*	
		indice.insertar(new ClaveVariable("hola"), null);
		indice.insertar(new ClaveVariable("chau"), null);
		indice.insertar(new ClaveVariable("mundo"), null);
		indice.insertar(new ClaveVariable("aleee"), null);
		indice.insertar(new ClaveVariable("nico"), null);
		indice.insertar(new ClaveVariable("manu"), null);
		indice.insertar(new ClaveVariable("moe"), null);
		indice.insertar(new ClaveVariable("lau"), null);
		indice.insertar(new ClaveVariable("tifi"), null);
		indice.insertar(new ClaveVariable("yo"), null);
		indice.insertar(new ClaveVariable("hola mundo"), null);
	*/
	/*	
		stringstream texto;	
		for (unsigned i = 0; i < 300; ++i) {
			texto << "hola" << i;
			cout << "inserto: " << texto.str() << endl;
			indice.insertar(new ClaveVariable(texto.str()), null);
			texto.str("");
		}
	*/
	/*	
		ClaveVariable clave("lau");
		indice.eliminar(&clave);
		
		indice.insertar(new ClaveVariable("gollum"), null);
		
		ClaveVariable clave2("chau");
		indice.eliminar(&clave2);
	*/

		
		//Inserto en la raiz:
	/*	indice.insertar(new ClaveEntera(20), null);
		indice.insertar(new ClaveEntera(90), null);
		indice.insertar(new ClaveEntera(49), null);
		indice.insertar(new ClaveEntera(33), null);
		indice.insertar(new ClaveEntera(57), null);
		indice.insertar(new ClaveEntera(30), null);
		indice.insertar(new ClaveEntera(17), null);
		indice.insertar(new ClaveEntera(8), null);
		
		//Splittea la raiz:
		indice.insertar(new ClaveEntera(10), null);

		//Alta común:
		indice.insertar(new ClaveEntera(1), null);
		
		//Alta con redistribución
		indice.insertar(new ClaveEntera(7), null);
		
		//Produce split:
		indice.insertar(new ClaveEntera(86), null);

		//Altas comunes:
		indice.insertar(new ClaveEntera(6), null);
		indice.insertar(new ClaveEntera(19), null);
		indice.insertar(new ClaveEntera(60), null);
		indice.insertar(new ClaveEntera(88), null);
		indice.insertar(new ClaveEntera(97), null);
		
		//Produce split:
		indice.insertar(new ClaveEntera(14), null);
		
		//Lleno la raiz:
		indice.insertar(new ClaveEntera(2), null);
		indice.insertar(new ClaveEntera(3), null);
		indice.insertar(new ClaveEntera(4), null);
		indice.insertar(new ClaveEntera(5), null);
		indice.insertar(new ClaveEntera(9), null);
		indice.insertar(new ClaveEntera(11), null);
		indice.insertar(new ClaveEntera(12), null);
		indice.insertar(new ClaveEntera(13), null);
		indice.insertar(new ClaveEntera(15), null);
		indice.insertar(new ClaveEntera(16), null);
		indice.insertar(new ClaveEntera(18), null);
		indice.insertar(new ClaveEntera(21), null);
		indice.insertar(new ClaveEntera(22), null);
		indice.insertar(new ClaveEntera(23), null);
		indice.insertar(new ClaveEntera(100), null);
		indice.insertar(new ClaveEntera(121), null);
		indice.insertar(new ClaveEntera(93), null);
		
		//Splitteo la raiz:
		indice.insertar(new ClaveEntera(28), null);
		
		//Baja común:
		indice.eliminar(new ClaveEntera(11));
		
		//Baja con redistribución:
		indice.eliminar(new ClaveEntera(7));
		
		//Baja en un nodo interno:
		indice.eliminar(new ClaveEntera(88));

		//Baja en la raiz que ademas produce merge en un nodo interno y en la raiz:
		indice.eliminar(new ClaveEntera(23));

		//Alta común
		indice.insertar(new ClaveEntera(25), null);

		//Bajas comunes
		indice.eliminar(new ClaveEntera(57));
		indice.eliminar(new ClaveEntera(90));
		
		//Baja con redistribucion del hermano del hermano
		indice.eliminar(new ClaveEntera(97));
		
		//Baja en la raiz
		indice.eliminar(new ClaveEntera(18));
		
		//Baja en hoja
		indice.eliminar(new ClaveEntera(16));
		
		//Baja con mergeSplitUnderflow
		indice.eliminar(new ClaveEntera(22));

		//Vacio el arbol
		indice.eliminar(new ClaveEntera(6));
		indice.eliminar(new ClaveEntera(14));
		indice.eliminar(new ClaveEntera(15));
		indice.eliminar(new ClaveEntera(13));
		indice.eliminar(new ClaveEntera(19));
		indice.eliminar(new ClaveEntera(25));
		indice.eliminar(new ClaveEntera(93));
		indice.eliminar(new ClaveEntera(100));
		indice.eliminar(new ClaveEntera(121));
		indice.eliminar(new ClaveEntera(60));
		indice.eliminar(new ClaveEntera(33));
		indice.eliminar(new ClaveEntera(49));
		indice.eliminar(new ClaveEntera(30));
		indice.eliminar(new ClaveEntera(28));
		indice.eliminar(new ClaveEntera(20));
		indice.eliminar(new ClaveEntera(1));
		indice.eliminar(new ClaveEntera(2));
		indice.eliminar(new ClaveEntera(5));
		indice.eliminar(new ClaveEntera(10));
		indice.eliminar(new ClaveEntera(3));
		indice.eliminar(new ClaveEntera(8));
		indice.eliminar(new ClaveEntera(9));
		indice.eliminar(new ClaveEntera(12));
		indice.eliminar(new ClaveEntera(4));
		indice.eliminar(new ClaveEntera(86));
		indice.eliminar(new ClaveEntera(17)); 
		indice.eliminar(new ClaveEntera(21));

		//Inserto en la raiz
		indice.insertar(new ClaveEntera(125), null); */
	/*
		
		Clave* clave = NULL;
		int j = 324;
		stringstream s;
		
		for (int i = 0; i < 3000; i++){
			if ( (i > 100) && (i < 500) )
				j += 230;
			else if ( (i >500) && (i < 1000) ) 
				j-= 33;
			else if ( (i >1000) && (i < 1500) )
				j += 88;
			else if ( (i >1500) && (i < 2000) )
				j -= 9;
			else if ( (i >2000) && (i < 3000) )
				j += 123;
			else j-= 992;
			
			s << (300 - i + j);
			cout << "insertando clave " << s.str();
			cout << endl;
			clave = new ClaveVariable(s.str());//ClaveEntera(300 - i + j);
			indice.insertar(clave, null);
			s.str("");
		}

		for (int i = 0; i < 300; i++){
			cout << "eliminando clave " << (300 - i) << endl;
			clave = new ClaveEntera(300 - i);
			indice.eliminar(clave); 
			delete clave;
		} 
		
		//Come get some!
		indice.eliminar(NULL);
		
		clave = new ClaveEntera(-999);
		indice.eliminar(clave);
		delete clave;
		
		clave = new ClaveEntera(400);
		indice.insertar(clave, null);
		*/
/*
		DefinitionsManager::ListaTiposAtributos* lista = DefinitionsManager::getInstance().getListaTiposAtributos("PERSONA");
		IndiceArbol indice(TipoIndices::GRIEGO, TipoDatos::TIPO_STRING, lista,
						   TipoIndices::ARBOL_BS, 96, 96, "locura",
						   TipoDatos::TIPO_VARIABLE);
		
		
		char* null = NULL;
		*/
		//Inserto en la raiz
	/*	indice.insertar(new ClaveVariable("Manu"), null);
		indice.insertar(new ClaveVariable("Coloquio"), null);
		indice.insertar(new ClaveVariable("Rasputin"), null);
		indice.insertar(new ClaveVariable("San"), null);
		indice.insertar(new ClaveVariable("Lorenzo"), null);
		
		//Splittea la raiz
		indice.insertar(new ClaveVariable("Holas"), null);
		
		//Alta común
		indice.insertar(new ClaveVariable("Je"), null);
		
		//Alta con redistribución
		indice.insertar(new ClaveVariable("Cantamontes"), null);
		
		//Produce split
		indice.insertar(new ClaveVariable("Certant"), null);

		//Produce redistribución
		indice.insertar(new ClaveVariable("Otorinolaringologo"), null);

		//Lleno la raiz
		indice.insertar(new ClaveVariable("Aries"), null);
		indice.insertar(new ClaveVariable("Tauro"), null);
		indice.insertar(new ClaveVariable("Geminis"), null);
		indice.insertar(new ClaveVariable("Cancer"), null);
		indice.insertar(new ClaveVariable("Leo"), null);
		indice.insertar(new ClaveVariable("Virgo"), null);
		indice.insertar(new ClaveVariable("Libra"), null);
		indice.insertar(new ClaveVariable("Escorpio"), null);
		
		//Split en hoja y raiz
		indice.insertar(new ClaveVariable("Sagitario"), null);
		
		//Elimino todos
		
		//Baja con redistribución
		indice.eliminar(new ClaveVariable("Cantamontes"));
		
		indice.eliminar(new ClaveVariable("Escorpio"));
		indice.eliminar(new ClaveVariable("Geminis"));
		indice.eliminar(new ClaveVariable("Otorinolaringologo"));
		indice.eliminar(new ClaveVariable("Aries"));
		indice.eliminar(new ClaveVariable("Certant"));
		indice.eliminar(new ClaveVariable("Je"));
		indice.eliminar(new ClaveVariable("Tauro"));
		indice.eliminar(new ClaveVariable("Coloquio"));
		indice.eliminar(new ClaveVariable("Lorenzo"));
		indice.eliminar(new ClaveVariable("Libra"));
		indice.eliminar(new ClaveVariable("Holas"));
		indice.eliminar(new ClaveVariable("Manu"));
		indice.eliminar(new ClaveVariable("San"));
		indice.eliminar(new ClaveVariable("Virgo"));
		indice.eliminar(new ClaveVariable("Rasputin"));
		indice.eliminar(new ClaveVariable("Cancer"));
		indice.eliminar(new ClaveVariable("Leo"));
		indice.eliminar(new ClaveVariable("Sagitario"));
		*/
/*		stringstream s;
		for (int i=0; i < 2000; i++){
			s << (10000-i);
			cout << "Insertando " << s.str() << "..." << endl;
			indice.insertar(new ClaveVariable(s.str()), null);
			s.str("");
		}
		
	
		Clave* clave;
		for (int i=0; i < 2000; i++){
			s << (10000-i);
			clave = new ClaveVariable(s.str());
			cout << "Eliminando " << s.str() << "..." << endl;
			indice.eliminar(clave);
			s.str("");
			delete clave;
		}
		*/
		
	unsigned short tamNodo = 96;
	IndiceManager* indiceManager = IndiceManagerFactory::getInstance().getIndiceManager(TipoIndices::GRIEGO,
																						TipoDatos::TIPO_ENTERO,
																					   	NULL,TipoIndices::ARBOL_BS,
																					   	tamNodo, 0,
																					   	"persona");
	
	BStarTree indice(*indiceManager, tamNodo);
	
	int entero;
	for (int i = 0; i < 1000; i++){
		entero = 1000-i;//rand() % 1000;
		cout << "insertando " << entero << "..." << endl;
		indice.insertar(new ClaveEntera(entero));
	}

	Clave* clave = NULL;
	
	for (int i = 100; i < 901; i++){
		clave = new ClaveEntera(i);
		cout << "Borrando " << *((int*)clave->getValor()) << "..." << endl;
		indice.eliminar(clave);
		delete clave;
	}

	cout << "Buscando el primero..." << endl;
	indice.primero();
	
	
	for (int i = 0; i < 5222; i++){
		clave = indice.siguiente();
		if (clave){
		cout << "La clave siguiente es " << *((int*)clave->getValor()) << endl;
		delete clave;
		}
	}
	
	
	cout << "Fin Test" << endl;
}

