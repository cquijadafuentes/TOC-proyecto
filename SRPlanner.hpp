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

struct Cuarteta{
	string idbh;	// id de Bloque Horario
	string idp;		// id de Persona
	string idve;	// id del Vehículo (Sólo en caso que sea un traslado)
	string idvi;	// id de la Visita (Sólo cuando la persona hace una visita)
};

struct Tripleta{
	string idPersona;
	string idVisita;
	time_t tInicioVisita;
};


class InstanceSolution{
	public:
		vector<Cuarteta> instance;	// vector para persona, horario, vehículo o visita.
		InstanceInput* punteroII;

		InstanceSolution(InstanceInput* ii);
		~InstanceSolution();

		bool validarInstancia();
		double evaluarInstancia();

		InstanceSolution* generarVecinos();
		void SortByPersona();
		void SortByBloque();

		vector<Tripleta> MinCostFlow(InstanceInput* ii);
		static bool sortTripletaPorTiempoInicio(Tripleta a, Tripleta b);
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