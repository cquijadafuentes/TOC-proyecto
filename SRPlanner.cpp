#include "SRPlanner.hpp"


namespace operations_research {
	struct Arc {
		std::pair<NodeIndex, NodeIndex> nodes;
		FlowQuantity capacity;
		FlowQuantity unit_cost;
	};
}


//	**********************************************************
//	***************** CLASE INSTANCE SOLUTION ****************
//	**********************************************************
Solver::Solver(string filename){
	// Cargar la información
	ii = new InstanceInput(filename);
	// Generar solución inicial
	is_actual = new InstanceSolution(ii);

}


Solver::~Solver(){
	if(ii != NULL){
		delete ii;
	}
	if(is_actual != NULL){
		delete is_actual;
	}
}


void Solver::SolucionPorBusquedaLocal(string outputFileName){
	// Ejecutar búsqueda


	// Generar archivo con la salida de los datos.
	InstanceOutput salida;
	salida.generarFichero(outputFileName);
}









//	*************************************************************
//	****************** CLASE INSTANCE SOLUTION ******************
//	*************************************************************


InstanceSolution::InstanceSolution(InstanceInput* ii){
	cout << "Creando InstanceSolution" << endl;
	// Generar asignación inicial personas-visitas
	vector <pair<string,string>> insIn = MinCostFlow(ii);
	cout << "Visitas a ralizar idPersona -> idVisita: " << endl;
	for(int i=0; i<insIn.size(); i++){
		cout << insIn[i].first << " -> " << insIn[i].second << endl;
	}

	// Generar asignación con bloques horarios y vehículos
	// Identificar jornadas y seleccionar los pares a la jornada

	vector<bool> visitasAsignadas = vector<bool>(false);
	// 1° Ordenar el vector por el tiempo de la visita
	sort(insIn.begin(), insIn.end(), comparadorParPersVis);
	// 2° Identificar las jornadas
	//		2°a) por cada jornada, crear est con las visitas correspondientes.
	//		2°b) Invocar la estructura y las asignaciones no usadas devolveras a insIn

}


InstanceSolution::~InstanceSolution(){
	cout << "Borrando InstanceSolution" << endl;
	
}


bool InstanceSolution::validarInstancia(){
	cout << "validarInstancia" << endl;

	return false;
}


double InstanceSolution::evaluarInstancia(){
	cout << "evaluarInstancia" << endl;

	return 0.0;
}


InstanceSolution* InstanceSolution::generarVecinos(){
	// ToDo: Generar vecinos

	return NULL;
}


void InstanceSolution::SortByPersona(){

}


void InstanceSolution::SortByBloque(){
	
}

vector<pair<string,string>> InstanceSolution::MinCostFlow(InstanceInput* ii){

	cout << "MinCostFlow datos:" << endl;
	int cvertices = ii->n + ii->l + 2; 	// Personas + visitas + source + sink
	cout << "Cant de nodos: " << cvertices << endl;
	// Cálculo de la cantidad de aristas en el grafo
	int caristas = 0;
	caristas += ii->n;		// Desde source a cada persona
	caristas += ii->l;		// Descde cada visita a sink
	for(int i=0; i<ii->l; i++){
		caristas += ii->visita_personas[i]->cant;	// Personas que pueden hacer la visita
	}
	cout << "Cant. de aristas: " << caristas << endl;

// Iniciar StarGraph y MinCostFlow
	operations_research::StarGraph graph(cvertices, caristas);
	operations_research::MinCostFlow min_cost_flow(&graph);

	// Se imprime por fila, lo que se debería entregar a .AddArcWithCapacityAndUnitCost(·,·,·,·)
	int origen, destino, capacidad, costo;
	// 1- Desde source a cada persona.
	origen = 0;
	for(int i=0; i<ii->n; i++){
		destino = i+1;
		capacidad = ii->pers_maxvisitas[i];
		costo = 1; 		// ToDo: Esto se debería calcular más adelante
		cout << origen << " " << destino << " " << capacidad << " " << costo << endl;
		
// Incorporando arcos
		operations_research::ArcIndex arc = graph.AddArc(origen, destino);
		min_cost_flow.SetArcCapacity(arc, capacidad);
		min_cost_flow.SetArcUnitCost(arc, costo);
	}
	// 2- Personas a visitas
	for(int i=0; i<ii->l; i++){
		for(int j=0; j<ii->n; j++){
			if(ii->visita_personas[i]->access(j) == 1 && ii->personaPuedeVisitar(j,i)){
				origen = j+1;
				destino = ii->n+1+i;
				capacidad = 1; 		// ToDo: Calcular capacidad (?) de visita de la persona j a la obra i
				costo = 1;			// ToDo: Calcular costo de visita de la persona j a la obra i
				cout << origen << " " << destino << " " << capacidad << " " << costo << endl;

// Incorporando arcos
				operations_research::ArcIndex arc = graph.AddArc(origen, destino);
				min_cost_flow.SetArcCapacity(arc, capacidad);
				min_cost_flow.SetArcUnitCost(arc, costo);
			}
		}
	}

	int supply = 0;
	// 3- Aristas desde Visitas a sink
	destino = 1+ii->n+ii->l;
	for(int i=0; i<ii->l; i++){
		origen = ii->n+1+i;
		capacidad = ii->visita_cant_personas[i];
		costo = 1;
		cout << origen << " " << destino << " " << capacidad << " " << costo << endl;

// Incorporando arcos
		operations_research::ArcIndex arc = graph.AddArc(origen, destino);
		min_cost_flow.SetArcCapacity(arc, capacidad);
		min_cost_flow.SetArcUnitCost(arc, costo);
		supply += capacidad;
	}

	// Lo que debe ir a .SetNodeSupply(·,·)
	// 1- Source:
	cout << "0 " << supply << endl;
	min_cost_flow.SetNodeSupply(0, supply);
	// 2- Vértices de personas y visitas en 0
	for(int i=1; i <= (ii->n+ii->l); i++){
		cout << i << " 0" << endl;
		min_cost_flow.SetNodeSupply(i, 0);
	}
	// 3- Sink
	int aux = ii->n+ii->l+1;
	cout << aux << " -" << supply << endl;
	min_cost_flow.SetNodeSupply(aux, -supply);

	// Copy-Paste del ejemplo:

	// Find the maximum flow between node 0 and node 4.
	min_cost_flow.Solve();
	if (operations_research::MinCostFlow::OPTIMAL != min_cost_flow.status()) {
		LOG(FATAL) << "Solving the max flow is not optimal!";
	}
	operations_research::FlowQuantity total_flow_cost = min_cost_flow.GetOptimalCost();
	
	vector<pair<string,string>> relPV;	// Relaciones Persona-Visita
	for (int i = 0; i < caristas; ++i) {
		if(graph.Tail(i) > 0 && graph.Tail(i) <= ii->n && min_cost_flow.Flow(i) > 0){	// Sólo personas a visitas:
			int posPer = graph.Tail(i) - 1;
			int posVis = graph.Head(i) - (ii->n+1);
			relPV.push_back(make_pair(ii->pers_id[posPer],ii->visita_id[posVis]));
		}
	}
	return relPV;
}


vector<Cuarteta> InstanceSolution::solucionarJornada(vector<pair<int,int>> pervis){
	// Evaluar el conjunto de pares que corresponde a una jornada
	// Se asume que todos los pares enviados corresponden a una jornada

	return vector<Cuarteta>();
}


bool InstanceSolution::comparadorParPersVis(pair<string,string> a, pair<string,string> b){
	// ToDo
	return false;
}