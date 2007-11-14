#include "../CapaIndices/Indices/IndiceHash.h"
 
int main(int argc, char** argv)
{
	RegisterInfo* regInfo = new RegisterInfo();
	
	Indice *manuColoquio = new IndiceHash(regInfo->getParameterList(), 512, "capaFisica");
	
	int entero1 = 25;
	char* cadena = new char[24];
	strcpy(cadena, "HOLA");
	unsigned short longCadena = 4;
	int entero2 = 55;

	unsigned short tamReg = 14; // 4 + 2 + 4 + 4
	char *registro = new char [tamReg + 2];
	
//	//Crea el registro numero 0
//	memcpy(registro,&tamReg, Tamanios::TAMANIO_LONGITUD);
//	memcpy(registro + 2,&entero1, sizeof(int));
//	memcpy(registro + 6,&longCadena, Tamanios::TAMANIO_LONGITUD);
//	memcpy(registro + 8,cadena, longCadena);
//	memcpy(registro + 12,&entero2, sizeof(int));	
//	
//	cout << endl;
//	cout << "Insertando registro 0 (Clave " << entero2 << ")...";
//	
	Clave* clave = NULL;
//	manuColoquio->insertar(clave, registro);
//	cout << "...OK!" << endl;
//	
	delete[] registro;
//	
//	//Insercion registro numero 1
//	registro = new char[tamReg + 2];
//	entero1 = 10;
//	strcpy(cadena, "FOCA");
//	entero2 = 122;
//	
//	memcpy(registro, &tamReg, Tamanios::TAMANIO_LONGITUD);
//	memcpy(registro + 2,  &entero1, sizeof(int));
//	memcpy(registro + 6,  &longCadena, Tamanios::TAMANIO_LONGITUD);
//	memcpy(registro + 8,  cadena, longCadena);
//	memcpy(registro + 12, &entero2, sizeof(int));	
//	
//	cout << "Insertando registro 1 (Clave " << entero2 << ")..." ;
//	clave = new ClaveEntera(122);
//	manuColoquio->insertar(clave, registro);
//	cout << "OK!" << endl;
//	
//	delete clave;	
//	delete[] registro;
//	
//	//Insercion registro numero 2
//	tamReg   = 17; 
//	registro = new char[tamReg + 2];
//	
//	entero1 = 1010;
//	strcpy(cadena, "INVITRO");
//	longCadena = 7;
//	entero2 = 33;
//		
//	memcpy(registro,&tamReg, Tamanios::TAMANIO_LONGITUD);
//	memcpy(registro + 2,  &entero1, sizeof(int));
//	memcpy(registro + 6,  &longCadena, Tamanios::TAMANIO_LONGITUD);
//	memcpy(registro + 8,  cadena, longCadena);
//	memcpy(registro + 15, &entero2, sizeof(int));
//	
//	cout << "Insertando registro 2 (Clave " << entero2 << ")...";
//	clave = new ClaveEntera(33);
//	manuColoquio->insertar(clave,registro);
//	cout << "OK!" << endl;
//	
//	delete clave;	
//	delete[] registro;
	
	//Insercion registro numero 3
	tamReg   = 27; 
	registro = new char[tamReg + 2];
		
	entero1 = 222;
	strcpy(cadena, "CORNELIO SAAVEDRA");
	longCadena = 17;
	entero2 = 555;
	
	memcpy(registro,&tamReg,Tamanios::TAMANIO_LONGITUD);
	memcpy(registro + 2,  &entero1,sizeof(int));
	memcpy(registro + 6,  &longCadena,Tamanios::TAMANIO_LONGITUD);
	memcpy(registro + 8,  cadena,longCadena);
	memcpy(registro + 25, &entero2,sizeof(int));
	
//	cout << "Insertando registro 3 (Clave " << entero2 << ")...";
//	clave = new ClaveEntera(555);
//	manuColoquio->insertar(clave, registro);
//	cout << "OK!" << endl;
//	
//	delete clave;	
	
	for (int i = 0 ; i < 225 ; i++) {
		entero2 = i;
		memcpy(registro + 25, &entero2,sizeof(int));
		
		cout << "Insertando registro (Clave " << entero2 << ")";			
		
		clave = new ClaveEntera(i);
		manuColoquio->insertar(clave, registro);
		cout << "...OK!"<< endl;
		delete clave;
	}
	
//	for (int i = 1; i < 100; i++) {	
//		cout << "Eliminando registro " << i + 3 << " (Clave " << entero2 << ")";			
//		
//		clave = new ClaveEntera(55 + i);
//		manuColoquio->eliminar(clave);
//		cout << "...OK!"<< endl;
//		delete clave;
//	}
	

	// Eliminacion
//	cout << endl;
//	cout << "Eliminando registro de clave 60...";
//	clave = new ClaveEntera(60);
//	manuColoquio->eliminar(clave);
//	cout << "OK!"<< endl;
//	delete clave;
//	
//	cout << "Eliminando registro de clave 61...";
//	clave = new ClaveEntera(61);
//	manuColoquio->eliminar(clave);
//	cout << "OK!" << endl;
//	delete clave;	
//
//	cout << "Eliminando registro de clave 62...";
//	clave = new ClaveEntera(62);
//	manuColoquio->eliminar(clave);
//	cout << "OK!" << endl;
//	delete clave;
//	
//	cout << "Eliminando registro de clave 63...";		
//	clave = new ClaveEntera(63);
//	manuColoquio->eliminar(clave);
//	cout << "OK!" << endl;
//	delete clave;

//	cout << "Eliminando registro de clave 63...";		
//	clave = new ClaveEntera(63);
//	manuColoquio->eliminar(clave);
//	cout << "OK!" << endl;
//	delete clave;
//
//	cout << "Eliminando registro de clave 68...";
//	clave = new ClaveEntera(68);
//	manuColoquio->eliminar(clave);
//	cout << "OK!" << endl;
//	delete clave;
//
//	cout << "Eliminando registro de clave 69...";
//	clave = new ClaveEntera(69);
//	manuColoquio->eliminar(clave);
//	cout << "OK!" << endl;
//	delete clave;
//
//	cout << "Eliminando registro de clave 70...";
//	clave = new ClaveEntera(70);
//	manuColoquio->eliminar(clave);
//	cout << "OK!" << endl;
//	delete clave;
	
	delete[] registro;
	delete manuColoquio;
	delete[] cadena;	
	delete regInfo; 
	
	cout << "==========FIN APLICACION==========" << endl;
	
	return 0;
}
