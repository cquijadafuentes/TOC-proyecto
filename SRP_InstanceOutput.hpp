#ifndef SRP_OUTPUT
#define SRP_OUTPUT

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "MiniBitmap.hpp"
#include "SRP_Utils.hpp"

using namespace std;

class Traslado{
	public:
		time_t horainicio;
		time_t horafin;
		string idvehi;
		string iduborigen;
		string idubdestino;
		MiniBitmap* bm_personas;
		string idvisita;
		Traslado(time_t tt, string idve, string idor, string iddes, MiniBitmap* mb, string idvis);
		~Traslado();
		string toString();
	private:

};


class InstanceOutput{
	public:
		int n;			// Cantidad de Personas
		int m;			// Cantidad de Vehículos
		int k;			// Cantidad de Ubicaciones
		int l;			// Cantidad de Visitas
		int t;			// Cantidad de Viajes
		vector<Traslado*> tr;
		
		InstanceOutput(string filename, int nn, int mm, int kk, int ll);
		InstanceOutput();
		~InstanceOutput();
		void generarFichero(string filename);

		void sortByIdVehiculoFecha();
		int validaViajesPorVehiculos();
	private:
		static bool comparadorIdVehiculoFecha(Traslado* i1, Traslado* i2);
};





#endif