#ifndef SRPINPUT
#define SRPINPUT

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include "MiniBitmap.hpp"
#include "SRP_Utils.hpp"

using namespace std;

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
		vector<vector<pair<float,int>>> ubica_dist_mins;

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
		time_t getInicioVentanaVisita(int posVisita);
		time_t getFinVetanaVisita(string idVisita);

		bool personaPuedeVisitar(int posPersona, int posVisita);
		bool bloqueContiguoConSiguiente(int posBl);


	private:
		
};




#endif