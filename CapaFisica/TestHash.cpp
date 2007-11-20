#include "../CapaIndices/Indices/IndiceHash.h"
 
int main(int argc, char** argv)
{
	RegisterInfo regInfo;
	
	Indice *manuColoquio = new IndiceHash(regInfo.getParameterList(), 512, "testHash");
	Clave* clave = NULL;
	int entero1  = 0;
	char* cadena = new char[24];
	unsigned short longCadena = 0;
	ClaveFecha::TFECHA fecha;
	unsigned short tamReg = 0;
	char *registro = NULL;	
	
	//Insercion de registros
	tamReg   = 17; 
	registro = new char[tamReg + 2];		
	entero1 = 400;
	strcpy(cadena, "MICHELLE");
	longCadena = 8;
	
	memcpy(registro, &tamReg, Tamanios::TAMANIO_LONGITUD);
	memcpy(registro + Tamanios::TAMANIO_LONGITUD,  &entero1, sizeof(int));
	memcpy(registro + Tamanios::TAMANIO_LONGITUD + sizeof(int),  &longCadena,Tamanios::TAMANIO_LONGITUD_CADENA);
	memcpy(registro + Tamanios::TAMANIO_LONGITUD + sizeof(int) + Tamanios::TAMANIO_LONGITUD_CADENA,  cadena, longCadena);	

	//Inserta 200 ClaveFecha
	cout << endl;
	cout << "Insertando 200 nuevos registros...";
	for (int i = 1 ; i < 2 ; i++) {
		fecha.anio = i*100;
		fecha.mes = i;
		fecha.dia = i*2;
		
		memcpy(registro + Tamanios::TAMANIO_LONGITUD + sizeof(int) + Tamanios::TAMANIO_LONGITUD_CADENA + longCadena, &fecha.anio,sizeof(short));
		memcpy(registro + Tamanios::TAMANIO_LONGITUD + sizeof(int) + Tamanios::TAMANIO_LONGITUD_CADENA + longCadena + sizeof(short), &fecha.mes,sizeof(char));
		memcpy(registro + Tamanios::TAMANIO_LONGITUD + sizeof(int) + Tamanios::TAMANIO_LONGITUD_CADENA + longCadena + sizeof(short) + sizeof(char), &fecha.dia,sizeof(char));			
		
		clave = new ClaveFecha(fecha);
		manuColoquio->insertar(clave, registro);
		
		delete clave;
	}
	cout << "OK!"<< endl;
	
/*	cout << "Prueba de insercion (fecha existente)";
	//Intenta insertar una que ya existe...
	fecha.anio = 100;
	fecha.mes = 1;
	fecha.dia = 2;

	clave = new ClaveFecha(fecha);
	memcpy(registro + 15, &fecha.anio,sizeof(short));
	memcpy(registro + 17, &fecha.mes,sizeof(char));
	memcpy(registro + 18, &fecha.dia,sizeof(char));
	manuColoquio->insertar(clave, registro);
	delete clave;
	cout << "...OK!"<< endl;
	
	//Elimina 100 ClaveFechas existentes...
	cout << "Eliminando los primeros 100 registros...";	
	for (int i = 1; i < 101; i++) {			
		fecha.anio = i*100;
		fecha.mes = i;
		fecha.dia = i*2;
			
		clave = new ClaveFecha(fecha);
		manuColoquio->eliminar(clave);
		delete clave;
	}
	cout << "OK!"<< endl;
	
	//Prueba a eliminar una clave ya eliminada previamente...
	fecha.anio = 100;
	fecha.mes = 1;
	fecha.dia = 2;
	
	cout << "Prueba de eliminacion (fecha ya eliminada)...";
	clave = new ClaveFecha(fecha);
	manuColoquio->eliminar(clave);
	cout << "OK!"<< endl;
	delete clave;
*/
	
	delete[] registro;
	delete manuColoquio;
	delete[] cadena;	
	
	cout << "==========FIN APLICACION==========" << endl;
	
	return 0;
}
