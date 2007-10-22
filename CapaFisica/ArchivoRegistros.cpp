#include "ArchivoRegistros.h"

using namespace std;

/******************************************************************************/
/* Implementación de Metodos */
/*---------------------------*/

ArchivoRegistros::ArchivoRegistros(string nombre,int tamanioRegistro){

  /* abre el archivo en modo lectura - escritura binario*/  
  this->archivo.open(nombre.c_str(),
                                ios::in |ios::out |ios::binary);
                                         
  /* determina si tuvo éxito la apertura del archivo */
  if (!this->archivo.is_open()) {

    /* limpia los flags de control de estado del archivo */
    this->archivo.clear();
  
    /* crea el archivo */    
    this->archivo.open(nombre.c_str(),
                                  ios::out | ios::binary);      
    this->archivo.close();
    
    /* reabre el archivo para lectura - escritura binario */
    this->archivo.open(nombre.c_str(),ios::in|ios::out|ios::binary);

    /* verifica que haya podido crear el archivo */                                  
    if (! this->archivo.is_open())
    
      /* arroja una excepción */
      throw string("El archivo no pudo ser abierto");   
  }

  /* almacena el tamaño de los registros */
  this->tamanioRegistro = tamanioRegistro;         
}

/*----------------------------------------------------------------------------*/
ArchivoRegistros::~ArchivoRegistros(){

  /* cierra el archivo */
  this->archivo.close();
  this->tamanioRegistro = 0;
}

/*----------------------------------------------------------------------------*/
void ArchivoRegistros::escribir(const void* registro) {

  /* verifica que el archivo esté abierto */
  if (this->archivo.is_open()) {

    /* escribe el registro en el archivo */
    this->archivo.write(static_cast<const char*>(registro),
                                   this->tamanioRegistro);

    /* chequea si se ha producido un error */
    if (this->archivo.fail())
      /* arroja una excepción ante la imposibilidad de escribir el reg */
      throw string("No se pudo escribir correctamente el registro");
  }
  else {
    /* arroja una excepción porque el archivo no está abierto */
    throw string("El archivo no está abierto");
  }
}

/*----------------------------------------------------------------------------*/
void ArchivoRegistros::leer(void* registro) {

  /* verifica que el archivo esté abierto */
  if (this->archivo.is_open()) {
  	
    /* lee del archivo un registro */
    this->archivo.read(static_cast<char*>(registro),
                                  this->tamanioRegistro);

    /* chequea si se ha producido un error */
    if (this->archivo.fail()){
      /* arroja una excepción ante la imposibilidad de leer un reg */
      this->archivo.clear();
      throw string("No se pudo leer correctamente el registro");
       
    } 
  }
  else {
    /* arroja una excepción porque el archivo no está abierto */
    throw string("El archivo no está abierto");
  }
}

/*----------------------------------------------------------------------------*/
bool ArchivoRegistros::fin(){
  /* para comprobar el fin lee un char del buffer, sin retirarlo y lo
     compara con el fin de archivo */
  bool esEof = (this->archivo.peek() == char_traits<char>::eof());

  if (esEof)
    /* si llegó al fin del archivo limpia los flags */
    this->archivo.clear();

  return esEof;
}

/*----------------------------------------------------------------------------*/
long int ArchivoRegistros::posicion(){

  long int pos = 0;

  /* verifica que el archivo esté abierto */
  if (this->archivo.is_open())

    /* calcula el número de registro según la posición del byte actual */
    pos = this->archivo.tellg() / this->tamanioRegistro;

  else
    /* arroja una excepción porque el archivo no está abierto */
    throw string("El archivo no está abierto");

  return pos;
}

/*----------------------------------------------------------------------------*/
void ArchivoRegistros::posicionarse(long int posicion) {

  /* verifica que el archivo esté abierto */
  if (this->archivo.is_open()) {

    /* mueve la posición actual según sea el tamano del registro */
    this->archivo.seekg(posicion * this->tamanioRegistro,
                                   ios_base::beg);

    /* chequea si se ha producido un error */
    if (this->archivo.fail())
      /* arroja una excepción ante la imposibilidad de leer un reg */
      throw string("No se pudo posicionar correctamente el registro");
  }
  else
    /* arroja una excepción porque el archivo no está abierto */
    throw string("El archivo no está abierto");
}
/*----------------------------------------------------------------------------*/
void ArchivoRegistros::posicionarseFin(){
	this->archivo.seekg(0,ios_base::end);
}
