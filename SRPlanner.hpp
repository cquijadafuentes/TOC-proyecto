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
		vector<MiniBitmap*> horasUsoVeh;	// Para marcar los bloques en que se usan vehículos (1 vehículo en uso)
		vector<MiniBitmap*> horasDispPer;	// Para marcar los bloques disponibles de la persona
		bool isValid;				// resultado del checker
		double evaluacion;			// factor de calidad de la instancia
		float costo;				// costo de la solución (traslado de vehículos)

		InstanceInput* punteroII;

		
		InstanceSolution(InstanceInput* ii);
		InstanceSolution(InstanceInput* ii, InstanceOutput* io);
		~InstanceSolution();

		bool validarInstancia();
		float evaluarInstancia();

		InstanceSolution* generarVecinos();

		vector<Tripleta> MinCostFlow(InstanceInput* ii);
		static bool sortTripletaPorTiempoInicio(Tripleta a, Tripleta b);
		static bool sortCuartetaPorBloquePersona(Cuarteta a, Cuarteta b);
		static bool sortCuartetaPorPersonaBloque(Cuarteta a, Cuarteta b);
		
		bool checker();
		double evaluator();

		void printInstanceSolution();
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