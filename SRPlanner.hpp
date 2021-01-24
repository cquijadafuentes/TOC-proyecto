#ifndef SRPLANNER
#define SRPLANNER

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "MiniBitmap.hpp"
#include "SRP_Utils.hpp"
#include "SRP_InstanceOutput.hpp"
#include "SRP_InstanceInput.hpp"

#include "ortools/graph/min_cost_flow.h"
#include "ortools/base/logging.h"

using namespace std;

time_t obtenerTime(string ts);

string stringTime(time_t t);

time_t sumaMinutos(time_t t, int m);

struct Cuarteta{
	string idbh;	// id de Bloque Horario
	string idp;		// id de Persona
	string idve;	// id del Vehículo (Sólo en caso que sea un traslado)
	string idvi;	// id de la Visita (Sólo cuando la persona hace una visita)
};


class InstanceSolution{
	public:
		vector<Cuarteta> instance;	// vector para persona, horario, vehículo o visita.

		InstanceSolution(InstanceInput* ii);
		~InstanceSolution();

		bool validarInstancia();
		double evaluarInstancia();

		InstanceSolution* generarVecinos();
		void SortByPersona();
		void SortByBloque();

		vector<pair<string,string>> MinCostFlow(InstanceInput* ii);
		static bool comparadorParPersVis(pair<string,string> a, pair<string,string> b);
		vector<Cuarteta> solucionarJornada(vector<pair<int,int>> pervis);
};


class Solver{
	public:
		InstanceInput* ii;
		InstanceSolution* is_actual;
		Solver(string filename);
		~Solver();

		void SolucionPorBusquedaLocal(string outputFileName);
	private:
};

#endif