#include "../CapaIndices/Indices/IndiceHash.h"

/* Tef, aca logre armar un pequenio testeo de manejo
 * de ClaveFecha como clave primaria. Inserta, elimina,
 * prueba hacer inserciones de fechas ya existentes, 
 * eliminacion de cosas ya borradas y, por sobre todas
 * las cosas, no rompe los workspaces. 
 * 
 * Fijate si te parece que va bien, chequeando los hexas
 * (yo los anduve viendo y me parecio que estaban fenomeno,
 * pero aca la experta en hashing es ud.).
 * 
 * Entre los cambios que le hice, note que Bloque armaba
 * horriblemente las comparaciones entre claves. Ahora pareceria
 * estar bien.
 *  
 * Te lo separe en un archivo nuevo, por si tenias
 * ganas de conservar el TestHash original (todo un detalle).
 * 
 * Con afecto,
 * 		Don Fantone (Poquipatico a domiclio).-
 */

int main(int argc, char** argv)
{
	RegisterInfo* regInfo = new RegisterInfo();
	
	Indice *manuColoquio = new IndiceHash(regInfo->getParameterList(), 512, "capaFisica");
	Clave* clave = NULL;
	int entero1  = 0;
	char* cadena = new char[24];
	unsigned short longCadena = 4;
	ClaveFecha::TFECHA fecha;
	unsigned short tamReg = 0;
	char *registro = NULL;	
	
	//Insercion de registros
	tamReg   = 17; 
	registro = new char[tamReg + 2];		
	entero1 = 222;
	strcpy(cadena, "MICHELLE");
	longCadena = 8;
	
	memcpy(registro,&tamReg,Tamanios::TAMANIO_LONGITUD);
	memcpy(registro + 2,  &entero1, sizeof(int));
	memcpy(registro + 6,  &longCadena,Tamanios::TAMANIO_LONGITUD_CADENA);
	memcpy(registro + 7,  cadena, longCadena);	

	//Inserta 200 ClaveFecha
	cout << endl;
	cout << "Insertando 200 nuevos registros...";
	for (int i = 1 ; i < 201 ; i++) {
		fecha.anio = i*100;
		fecha.mes = i;
		fecha.dia = i*2;
		
		memcpy(registro + 15, &fecha.anio,sizeof(short));
		memcpy(registro + 17, &fecha.mes,sizeof(char));
		memcpy(registro + 18, &fecha.dia,sizeof(char));			
		
		clave = new ClaveFecha(fecha);
		manuColoquio->insertar(clave, registro);
		
		delete clave;
	}
	cout << "OK!"<< endl;
	
	cout << "Prueba de insercion (fecha existente)";
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

	
	delete[] registro;
	delete manuColoquio;
	delete[] cadena;	
	delete regInfo; 
	
	cout << "==========FIN APLICACION==========" << endl;
	
	return 0;
}
