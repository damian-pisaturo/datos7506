///////////////////////////////////////////////////////////////////////////
//	Archivo   : Manager.h
//  Namespace : CapaIndices
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clase abstracta Manager.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef MANAGER_H_
#define MANAGER_H_


#include "../../ComuDatos/ComuDatos.h"
#include "../../Common/NombresCapas.h"
#include "../../Common/ResultadosFisica.h"
#include "../../Common/OperacionesCapas.h"
#include <string>


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Manager
//			(Abstracta. Define el comportamiento y la interfaz de todas
//			las clases Manager).
///////////////////////////////////////////////////////////////////////////

class Manager
{
	private:
	///////////////////////////////////////////////////////////////////////
	// Atributos
	///////////////////////////////////////////////////////////////////////
		string nombreArchivo; //Nombre del archivo a acceder.
		unsigned int tamanioBloque; //Tamaño de un bloque dentro del archivo.
	
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		
		/*Recibe el tamaño del bloque/registro de datos en disco y el nombre 
		 * del archivo a ser accedido.
		 */
		Manager(unsigned int tamanioBloque, string nombreArchivo);		
		virtual ~Manager();	

	///////////////////////////////////////////////////////////////////////
	// Getters/Setters
	///////////////////////////////////////////////////////////////////////
			unsigned int getTamanioBloque()
			{
				return this->tamanioBloque;
			}
			
			void setTamanioBloque(unsigned int tamanio)
			{
				this->tamanioBloque = tamanio;
			}
			
			string getNombreArchivo()
			{
				return this->nombreArchivo;
			}
			
			void setNombreArchivo(string nombre)
			{
				this->nombreArchivo = nombre;
			}
			
			
	protected:
	///////////////////////////////////////////////////////////////////////
	// Metodo protegido
	/////////////////////////////////////////////////////////////////////////
		
		/* Devuelve una instancia de un pipe de comunicacion entre
		 * la clase actual y la capa fisica.
		 */
		ComuDatos* instanciarPipe();

};

#endif /*MANAGER_H_*/
