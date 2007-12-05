///////////////////////////////////////////////////////////////////////////
//	Archivo   : ArchivoMaestro.h
//  Namespace : Common
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clase ArchivoMaestro.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefanía;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#ifndef ARCHIVOMAESTRO_H_
#define ARCHIVOMAESTRO_H_

#define NOM_ARCHIVO_MAESTRO "7506_FP2C07.adm"
#define MASTER_MAGIC_NUMBER "TAAC"

#include <ctime>
#include <fstream>
#include <sstream>
#include "../ParserDefiniciones/ParserDefiniciones.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoMaestro
//			(Permite el manejo del archivo de control maestro de la 
//			aplicacion. El archivo maestro almacena los datos de todos 
//			los tipos definidos, sus definiciones y estructuras).
///////////////////////////////////////////////////////////////////////////

class ArchivoMaestro
{	
	private:
	///////////////////////////////////////////////////////////////////////
	// Atributos
	///////////////////////////////////////////////////////////////////////
		
		Archivo archivo; // File descriptor del archivo maestro.
		
		// Listas contenedoras de la informacion de cada tipo definido
		// requerido por el DefinitionsManager.
		ListaInfoRegistro* infoRegistros;
		ListaTiposIndices* tiposIndices;
		ListaNombresAtributos* nombresAtributos;
		ListaNombresClaves* nombresClaves;
		
		unsigned char magicNumber; // Hace magia (mucha magia)
		
	public:		
		const static char TAMANIO_BLOQUE_INVALIDO = -22;
		const static char ATRIBUTO_NO_DEFINIDO    = -33; // Menos Cristo
		const static char INDICE_GRIEGO_AMBIGUO   = -44;
		const static char ESTRUCTURA_RF_INVALIDA  = -99;
		const static char ARCHIVO_INVALIDO        = -66;
		const static char TIPO_EXISTENTE          = -77;
		const static char TIPO_INEXISTENTE        = -55;
		const static char OK                      = -88;
		
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoMaestro();
		virtual ~ArchivoMaestro();
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		/* Añade un nuevo tipo definido al archivo maestro.
		 * Si el tipo a insertar ya se encuentra definido,
		 * devuelve TIPO_EXISTENTE; de lo contrario, retorna
		 * OK.
		 */
		char agregarTipo(const DefinicionTipo& tipo);
		
		/* Añade todos los tipos definidos en la lista
		 * al archivo maestro.
		 * El proceso de insercion se detiene si algun 
		 * tipo a insertar ya se encuentra definido;
		 * en ese caso devuelve TIPO_EXISTENTE; de lo contrario,
		 * retorna OK.
		 */
		char agregarTipo(const ListaDefinicionesTipos& tipos);
		
		/* Parsea el archivo de definiciones cuyo nombre es pasado
		 * por parametro y carga los tipos alli definidos en el 
		 * archivo maestro. El proceso de insercion se detiene si
		 * algun tipo a insertar ya se encuentra definido; en ese
		 * caso devuelve TIPO_EXISTENTE; de lo contrario retorna OK.
		 */
		char agregarTipo(const string& nomArchivoDefiniciones);
		
		/* Parsea el archivo maestro y llena las listas con la 
		 * informacion correspondiente al tipo cuyo nombre
		 * coincide con el pasado por parametro. Luego, pueden
		 * obtenerse las listas mediante llamadas a los getters
		 * correspondientes.
		 * Si el tipo buscado no existe devuelve TIPO_INEXISTENTE;
		 * en caso contrario, devuelve OK.
		 */
		char recuperarTipo(const string& nombreTipo);
		
		/* Busca el tipo definido en el archivo maestro cuyo
		 * nombre es pasado por parametro. Si el tipo buscado
		 * existe devuelve TIPO_EXISTENTE, de lo contrario
		 * devuelve OK.
		 */
		char buscarTipo(const string& nombreTipo);
		
		ListaInfoRegistro* getListaInfoRegistro() const
		{
			ListaInfoRegistro* lif = NULL;
			
			if (this->infoRegistros)
				lif = new ListaInfoRegistro(*this->infoRegistros);
			
			return lif;
		}
		
		ListaTiposIndices* getListaTiposIndices() const
		{
			ListaTiposIndices* lti = NULL;
			
			if (this->tiposIndices)
				lti = new ListaTiposIndices(*this->tiposIndices);
			
			return lti;
		}
		
		ListaNombresAtributos* getListaNombresAtributos() const
		{
			ListaNombresAtributos* lna = NULL;
			
			if (this->nombresAtributos)
				lna = new ListaNombresAtributos(*this->nombresAtributos);
			
			return lna;
		}
		
		ListaNombresClaves* getListaNombresClaves() const
		{
			ListaNombresClaves* lnc = NULL;
			
			if (this->nombresClaves)
				lnc = new ListaNombresClaves(*this->nombresClaves);
			
			return lnc;
		}		
		
	private:
	///////////////////////////////////////////////////////////////////////
	// Metodos privados
	///////////////////////////////////////////////////////////////////////
		/* Aplica una funcion de hash a un nombre
		 * de tipo y retorna su valor para ser utilizado como
		 * numero dentro del indice de tipos en el archivo
		 * maestro.
		 */
		unsigned long hashNombreTipo(const string& nombreTipo) const;
		
		/* Metodos de validacion del tipo definido
		 * en el archivo de definiciones.
		 */
		char validarEstructuraRF(const VectorTiposAtributos& tiposAtributos) const;
		char validarIndiceGriego(const DatosIndices& datosGriego, const ListaNombresAtributos& nombresAtributos) const;
		char validarIndicesRomanos(const DatosIndices& datosRomanos, const ListaNombresAtributos& nombresAtributos) const;
		char validarTipo(const DefinicionTipo& tipo) const;
		char validarDefinicionAtributos(const ListaNombresClaves& nombresClaves, const ListaNombresAtributos& nombresAtributos) const;
		char validarTamBloque(const unsigned short tamBloque) const;
		
		/* Buscar el tipo de dato en el archivo maestro cuyo ID coincide
		 * con el pasado por parametro.
		 */
		char buscarTipo(int idTipo);
		
		/* Genera un nombre de archivo a partir del nombre del tipo
		 * de dato a persistir.
		 */
		string generarNombreArchivo(string nombreTipo);
		
		/* Crea una ListaInfoRegistro para ser almacenada en el archivo maestro
		 */
		void armarListaInfoRegistro(const DefinicionTipo& tipo, ListaInfoRegistro& listaInfoRegistro) const;
		
		/* Recorre una ListaInfoRegistro y la almacena en disco. Devuelve la cantidad de
		 * bytes escritos en el archivo.
		 */
		streampos almacenarListaInfoRegistro(const ListaInfoRegistro& listaInfoRegistro);
		
		/* Crea una ListaTiposIndices para ser almacenada en el archivo maestro
		 */
		void armarListaTiposIndices(const DefinicionTipo& tipo, ListaTiposIndices& listaTiposIndices);
		
		/* Recorre una ListaTiposIndices y la almacena en disco. Devuelve la cantidad de
		 * bytes escritos en el archivo.
		 */
		streampos almacenarListaTiposIndices(const ListaTiposIndices& listaTiposIndices);
		
		/* Recorre una ListaNombresAtributos y la almacena en disco. Devuelve la cantidad de
		 * bytes escritos en el archivo.
		 */
		streampos almacenarListaNombresAtributos(const ListaNombresAtributos& listaNombresAtributos);
		
		/* Recorre una ListaNombresClaves y la almacena en disco. Devuelve la cantidad de
		 * bytes escritos en el archivo.
		 */
		streampos almacenarListaNombresClaves(const ListaNombresClaves& listaNombresClaves);
		
		/* Recorre una ListaTipos y la almacena en disco. Devuelve la cantidad de
		 * bytes escritos en el archivo.
		 */
		streampos almacenarListaTipos(const ListaTipos& listaTipos);
		
		/* Metodos de obtencion de las estructuras listas persistidas en disco 
		 * para el tipo actual. Se espera una llamada previa a buscarTipo() para
		 * determinar que listas deben obtenerse.
		 */
		void recuperarListaInfoRegistro();
		void recuperarListaTiposIndices();
		void recuperarListaNombresAtributos();
		void recuperarListaTipos(ListaTipos& listaTipos);
		void recuperarListaNombresClaves(ListaNombresClaves& nombresClaves);

		/* Calcula el tamaño del registro de organizacion RF cuyos tipos de atributos
		 * son pasados por parametro.
		 */
		unsigned short calcularTamRegistro(const VectorTiposAtributos& tiposAtributos) const;
};

#endif /*ARCHIVOMAESTRO_H_*/
