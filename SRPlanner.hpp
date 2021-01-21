#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "MiniBitmap.hpp"

#include "ortools/graph/min_cost_flow.h"
#include "ortools/base/logging.h"

using namespace std;

time_t obtenerTime(string ts);

string stringTime(time_t t);

time_t sumaMinutos(time_t t, int m);

class InstanceInput{
	public:
		InstanceInput(string filename);
		~InstanceInput();

		int n;			// Cantidad de Personas
		int m;			// Cantidad de Vehículos
		int k;			// Cantidad de Ubicaciones
		int h;			// Cantidad de Bloques horarios
		int z; 			// Minutos por bloque
		int l;			// Cantidad de Visitas
		int v;			// Valor del Viático

		vector<string> pers_id;
		vector<int> pers_maxvisitas;
		vector<MiniBitmap*> pers_horasdisp;
		vector<MiniBitmap*> pers_afin;
		vector<MiniBitmap*> pers_prefvisita;

		vector<string> vehi_id;
		vector<int> vehi_cap;
		vector<int> vehi_cost;

		vector<string> ubica_id;
		vector<vector<pair<double,int>>> ubica_dist_mins;

		vector<string> bloque_id;
		vector<time_t> bloque_timestamp;

		vector<string> visita_id;
		vector<string> visita_ubicacion;
		vector<string> visita_bloque_inicio;
		vector<string> visita_bloque_fin;
		vector<MiniBitmap*> visita_personas;
		vector<MiniBitmap*> visita_vehiculos;
		vector<int> visita_testimado_bloques;
		vector<int> visita_prioridad;
		vector<int>	visita_cant_personas;

		unordered_map<string,int> mapa_personas;
		unordered_map<string,int> mapa_vehiculos;
		unordered_map<string,int> mapa_ubicaciones;
		unordered_map<string,int> mapa_bloquesh;
		unordered_map<string,int> mapa_visitas;

		vector<pair<string,string>> MinCostFlow();

		int getMaxVisitasDePersona(string idPersona);
		MiniBitmap* getBloquesDispDePersona(string idPersona);
		MiniBitmap* getPersonasAfinesDePersona(string idPersona);
		MiniBitmap* getPrefVisitaDePersona(string idPersona);

		int getTiempoTraslado(string idUbOr, string idUbDes);
		int getDistanciaTraslado(string idUbOr, string idUbDes);

		int getCapacidadVehiculo(string idVehiculo);
		int getCostoVehiculo(string idVehiculo);

		MiniBitmap* getPersonasParaVisita(string idVisita);
		MiniBitmap* getVehiculosParaVisita(string idVisita);
		int getPrioridadVisita(string idVisita);
		int getTiempoEstimadoVisita(string idVisita);
		time_t getInicioVentanaVisita(string idVisita);
		time_t getFinVetanaVisita(string idVisita);

		bool personaPuedeVisitar(int posPersona, int posVisita);


	private:
};

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
		~InstanceOutput();

		void sortByIdVehiculoFecha();
		int validaViajesPorVehiculos();
	private:
		static bool comparadorIdVehiculoFecha(Traslado* i1, Traslado* i2);
};

class InstanceSolution{
	public:
		vector<pair<pair<string,string>,string>> instance;	// vector para < < idbloque - idpersona > - idvisita/idvehiculo >

		InstanceSolution();
		~InstanceSolution();

		bool validarInstancia();
		double evaluarInstancia();
};
