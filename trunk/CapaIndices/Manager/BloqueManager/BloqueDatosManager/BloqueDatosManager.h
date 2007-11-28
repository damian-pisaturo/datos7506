///////////////////////////////////////////////////////////////////////////
//	Archivo   : BloqueDatosManager.h
//  Namespace : CapaIndices
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de la clase BloqueDatosManager.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#ifndef BLOQUEDATOSMANAGER_H_
#define BLOQUEDATOSMANAGER_H_

#include "../BloqueManager.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: BloqueDatosManager
//			(Permite la escritura, lectura, modificacion y eliminacion de
//			bloques de datos indexados, accediendo remotamente a la 
// 			Capa Fisica).
///////////////////////////////////////////////////////////////////////////

class BloqueDatosManager : public BloqueManager
{
	private:
		unsigned char tipoOrg;
		unsigned int bloqueSiguiente;
		unsigned short espacioBloqueLibre;
	
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		
		/*Recibe el tamaño del bloque/registro de datos en disco, el nombre del archivo 
		 * y su tipo de organizacion: REG_FIJOS  o REG_VARIABLES.
		 */
		BloqueDatosManager(unsigned int tamanioBloque, string nombreArchivo, unsigned char tipoOrg);		
		virtual ~BloqueDatosManager();
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		/*Todos los metodos de lectura/escritura a disco devuelven
		 * ResultadosFisica::ARCHIVO_INVALIDO si el archivo siendo utilizado
		 * no corresponde al archivo propio del proyecto esperado.
		 */
		
		/*Recibe un bloque de datos creado/inicializado de tamaño 'tamanioBloque' 
		 * y lo llena con la informacion leida del archivo de datos en 
		 * la posicion 'numBloque'.
		 * Utiliza ComuDatos para comunicarse con la Capa Fisica.
		 */
		int leerBloqueDatos(unsigned int numBloque, void* bloqueLeido);		

		/* Utiliza ComuDatos para comunicarse con la Capa Fisica y escribir
		 * el bloqueNuevo en el archivo de datos especificado por nombreArchivo.
		 * Devuelve la posicion donde fue escrito finalmente. Se espera que el bloque
		 * mantenga el formato fisico definido para él (ver Documentación).
		 */
		int escribirBloqueDatos(const void* bloqueNuevo);
		
		/* Utiliza ComuDatos para comunicarse con la Capa Fisica y
		 * sobre-escribir el bloque de datos modificado en el disco
		 * el bloqueNuevo en el archivo especificado por nombreArchivo.
		 * Se espera que el bloque mantenga el formato fisico definido 
		 * para él (ver Documentación).
		 */
		int escribirBloqueDatos(unsigned short numBloque, const void* bloqueModif);
		
		/*Actualiza el archivo de espacio libre, indicando que el bloque cuya
		 * posicion en el archivo es 'numBloque' esta disponible para escritura.
		 */
		int eliminarBloqueDatos(unsigned int numBloque);
		
		/*Emplea ComuDatos para acceder al archivo de control de espacio
		 * libre y buscar el primer bloque de datos que contenga, al menos,
		 * una cantidad de bytes libres igual a 'espLibre'. Si lo encuentra,
		 * lo devuelve en 'bloque'; en caso contrario 
		 * devuelve ResultadosFisica::BLOQUES_OCUPADOS.
		 */
		int buscarEspacioLibre(void* bloque, unsigned int espLibre);
		
		/*Devuelve en 'bloque' el siguiente bloque de datos presente en el archivo. 
		 * Si pudo obtener un bloque retorna ResultadosFisica::OK, de lo contrario 
		 * ResultadosFisica::FIN_BLOQUES.
		 */
		int siguienteBloque(void* bloque);

	///////////////////////////////////////////////////////////////////////
	// Getters/Setters
	///////////////////////////////////////////////////////////////////////		
		unsigned char getTipoOrganizacion()
		{
			return this->tipoOrg;
		}
		
		void setTipoOrganizacion(unsigned char tipoOrg)
		{
			this->tipoOrg = tipoOrg;
		}
		
}; /*Fin clase BloqueDatosManager*/

#endif /*BLOQUEDATOSMANAGER_H_*/
