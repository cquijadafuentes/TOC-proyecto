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
	int posbh;	// pos de Bloque Horario
	int posp;	// pos de Persona
	int posve;	// pos del Vehículo (Sólo en caso que sea un traslado)
	int posvi;	// pos de la Visita (Sólo cuando la persona hace una visita)
};

struct Tripleta{
	int posPersona;
	int posVisita;
	time_t tInicioVisita;
};

void stringCuarteta(Cuarteta x);

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