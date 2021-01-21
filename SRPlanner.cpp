#include "SRPlanner.hpp"

namespace operations_research {
struct Arc {
  std::pair<NodeIndex, NodeIndex> nodes;
  FlowQuantity capacity;
  FlowQuantity unit_cost;
};
}

time_t obtenerTime(string ts){
	time_t t;
	//return t;
	//time (&t);
	char * cts = new char [ts.length()+1];
	strcpy (cts, ts.c_str());
	struct tm tm;
	strptime(cts, "%Y-%m-%dT%H:%M:%S%Z", &tm);
	free(cts);
	t = mktime(&tm);  // t is now your desired time_t
	return t;
}


string stringTime(time_t t){
	string aux = ctime(&t);
	return aux.substr(0,aux.length()-1);
}

time_t sumaMinutos(time_t t, int m){
	struct tm tm = *localtime(&t);
	tm.tm_min += m;
	time_t nt = mktime(&tm);
	return nt;
}



//	**********************************************************
//	***************** CLASE INSTANCE SOLUTION ****************
//	**********************************************************
Solver::Solver(string filename){
	// Cargar la información
	ii = new InstanceInput(filename);
	// Generar solución inicial
	is_actual = ii->instanciaInicial();

}


Solver::~Solver(){
	if(ii != NULL){
		delete ii;
	}
}


void Solver::SolucionPorBusquedaLocal(string outputFileName){
	// Ejecutar búsqueda


	// Generar archivo con la salida de los datos.
	InstanceOutput salida = (is_actual);
	salida.generarFichero(outputFileName);
}



//	***********************************************************
//	******************* CLASE INSTANCE INPUT ******************
//	***********************************************************

InstanceInput::InstanceInput(string filename){
	ifstream entrada(filename);
	string linea;
	getline(entrada, linea);
	istringstream iss;

	iss = istringstream(linea);
	// Lectura de la primera linea.
	iss >> n >> m >> k >> h >> z >> l >> v;

	cout << n << " personas" << endl;
	cout << m << " vehículos" << endl;
	cout << k << " ubicaciones" << endl;
	cout << h << " bloques" << endl;
	cout << z << " minutos por bloque" << endl;
	cout << l << " visitas" << endl;
	cout << v << " valor viático" << endl;

	// Lectura de n líneas con la información de las personas
	pers_id = vector<string>(n);
	pers_maxvisitas = vector<int>(n);
	pers_horasdisp = vector<MiniBitmap*>(n);
	pers_afin = vector<MiniBitmap*>(n);
	pers_prefvisita = vector<MiniBitmap*>(n);

	for(int i=0; i < n; i++){
		// Cada línea cotiene:
		// id_persona visitas_max horas_disponibles(h bits) afinidad (n bits) preferencias_visitas(l bits)
		getline(entrada, linea);
		iss = istringstream(linea);
		iss >> pers_id[i] >> pers_maxvisitas[i];
		cout << pers_id[i] << " " << pers_maxvisitas[i] << " ";
		int aux;
		// Procesando horas disponibles
		pers_horasdisp[i] = new MiniBitmap(h);
		for(int j=0; j<h; j++){
			iss >> aux;
			if(aux == 1){
				pers_horasdisp[i]->setBit(j);
			}
		}
		pers_horasdisp[i]->printBitmap();
		cout << " ";
		// Procesando afinidad
		pers_afin[i] = new MiniBitmap(n);
		for(int j=0; j<n; j++){
			iss >> aux;
			if(aux == 1){
				pers_afin[i]->setBit(j);
			}
		}
		pers_afin[i]->printBitmap();
		cout << " ";
		// Procesando preferencias visitas
		pers_prefvisita[i] = new MiniBitmap(l);
		for(int j=0; j<l; j++){
			iss >> aux;
			if(aux == 1){
				pers_prefvisita[i]->setBit(j);
			}
		}
		pers_prefvisita[i]->printBitmap();
		cout << " ";
		cout << endl;

		mapa_personas[pers_id[i]] = i;
	}

	// Lectura de m líneas con la información de los vehículos
	vehi_id = vector<string>(m);
	vehi_cap = vector<int>(m);
	vehi_cost = vector<int>(m);
	for(int i=0; i < m; i++){
		// Cada línea cotiene:
		// id_vehichulo capacidad costo
		getline(entrada, linea);
		iss = istringstream(linea);
		iss >> vehi_id[i] >> vehi_cap[i] >> vehi_cost[i];
		cout << vehi_id[i] << " " << vehi_cap[i] << " " << vehi_cost[i] << endl;

		mapa_vehiculos[vehi_id[i]] = i;
	}

	// Lectura de k líneas con las distancias entre ubicaciones
	// Nota: primera ubicación siempre es SERVIU
	ubica_id = vector<string>(k);
	ubica_dist_mins = vector<vector<pair<double,int>>>(k,vector<pair<double,int>>(k));
	for(int i=0; i < k; i++){
		// Cada línea cotiene:
		// id_ubicacion k pares distancia,tiempo
		getline(entrada, linea);
		iss = istringstream(linea);
		iss >> ubica_id[i];
		cout << ubica_id[i] << " ";
		// Procesando pares distancia,tiempo
		double auxdist;
		int auxtime;
		for(int j=0; j<k; j++){
			iss >> auxdist >> auxtime;
			ubica_dist_mins[i][j] = make_pair(auxdist,auxtime);
			cout << "(" << ubica_dist_mins[i][j].first << "-" << ubica_dist_mins[i][j].second << ") ";
		}
		cout << endl;

		mapa_ubicaciones[ubica_id[i]] = i;
	}

	// Lectura de h líneas con al información de bloques horarios
	bloque_id = vector<string>(h);
	bloque_timestamp = vector<time_t>(h);
	string auxstring;
	for(int i=0; i < h; i++){
		// Cada línea cotiene:
		// id_bloque timestamp_bloque
		getline(entrada, linea);
		iss = istringstream(linea);
		iss >> bloque_id[i] >> auxstring;
		bloque_timestamp[i] = obtenerTime(auxstring);
		cout << bloque_id[i] << " " << ctime(&bloque_timestamp[i]);

		mapa_bloquesh[bloque_id[i]] = i;
	}

	// Lectura de l líneas con al información de las visitas
	visita_id = vector<string>(l);
	visita_ubicacion = vector<string>(l);
	visita_bloque_inicio = vector<string>(l);
	visita_bloque_fin = vector<string>(l);
	visita_personas = vector<MiniBitmap*>(l);
	visita_vehiculos = vector<MiniBitmap*>(l);
	visita_testimado_bloques = vector<int>(l);
	visita_prioridad = vector<int>(l);
	visita_cant_personas = vector<int>(l);
	for(int i=0; i < l; i++){
		// Cada línea cotiene:
		// id_visita id_bloque_inicio id_bloque_fin personas (n bits) vehículos (m bits) bloques_duracion prioridad
		getline(entrada, linea);
		iss = istringstream(linea);
		iss >> visita_id[i] >> visita_ubicacion[i] >> visita_bloque_inicio[i] >> visita_bloque_fin[i];
		cout << visita_id[i] << " " << visita_ubicacion[i] << " " << visita_bloque_inicio[i] << " " << visita_bloque_fin[i] << " ";
		int aux;
		// Procesando personas que pueden realizar la visita
		visita_personas[i] = new MiniBitmap(n);
		for(int j=0; j<n; j++){
			iss >> aux;
			if(aux == 1){
				visita_personas[i]->setBit(j);
			}
		}
		visita_personas[i]->printBitmap();
		cout << " ";
		// Procesando vehículos habilitados para realizar la visita
		visita_vehiculos[i] = new MiniBitmap(m);
		for(int j=0; j<m; j++){
			iss >> aux;
			if(aux == 1){
				visita_vehiculos[i]->setBit(j);
			}
		}
		visita_vehiculos[i]->printBitmap();
		cout << " ";
		iss >> visita_testimado_bloques[i] >> visita_prioridad[i] >> visita_cant_personas[i];
		cout << visita_testimado_bloques[i] << " " << visita_prioridad[i] << " " << visita_cant_personas[i] << endl;

		mapa_visitas[visita_id[i]] = i;
	}
}

InstanceInput::~InstanceInput(){
	cout << "Eliminando InstanceInput..." << endl;
	for(int i=0; i < n; i++){
		delete pers_horasdisp[i];
		delete pers_afin[i];
		delete pers_prefvisita[i];
	}
	for(int i=0; i < l; i++){
		delete visita_personas[i];
		delete visita_vehiculos[i];
	}
}


vector<pair<string,string>> InstanceInput::MinCostFlow(){

	cout << "MinCostFlow datos:" << endl;
	int cvertices = n + l + 2; 	// Personas + visitas + source + sink
	cout << "Cant de nodos: " << cvertices << endl;
	// Cálculo de la cantidad de aristas en el grafo
	int caristas = 0;
	caristas += n;		// Desde source a cada persona
	caristas += l;		// Descde cada visita a sink
	for(int i=0; i<l; i++){
		caristas += visita_personas[i]->cant;	// Personas que pueden hacer la visita
	}
	cout << "Cant. de aristas: " << caristas << endl;

// Iniciar StarGraph y MinCostFlow
	operations_research::StarGraph graph(cvertices, caristas);
	operations_research::MinCostFlow min_cost_flow(&graph);

	// Se imprime por fila, lo que se debería entregar a .AddArcWithCapacityAndUnitCost(·,·,·,·)
	int origen, destino, capacidad, costo;
	// 1- Desde source a cada persona.
	origen = 0;
	for(int i=0; i<n; i++){
		destino = i+1;
		capacidad = pers_maxvisitas[i];
		costo = 1; 		// ToDo: Esto se debería calcular más adelante
		cout << origen << " " << destino << " " << capacidad << " " << costo << endl;
		
// Incorporando arcos
		operations_research::ArcIndex arc = graph.AddArc(origen, destino);
		min_cost_flow.SetArcCapacity(arc, capacidad);
		min_cost_flow.SetArcUnitCost(arc, costo);
	}
	// 2- Personas a visitas
	for(int i=0; i<l; i++){
		for(int j=0; j<n; j++){
			if(visita_personas[i]->access(j) == 1 && personaPuedeVisitar(j,i)){
				origen = j+1;
				destino = n+1+i;
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
	destino = 1+n+l;
	for(int i=0; i<l; i++){
		origen = n+1+i;
		capacidad = visita_cant_personas[i];
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
	for(int i=1; i <= (n+l); i++){
		cout << i << " 0" << endl;
		min_cost_flow.SetNodeSupply(i, 0);
	}
	// 3- Sink
	int aux = n+l+1;
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
		if(graph.Tail(i) > 0 && graph.Tail(i) <= n && min_cost_flow.Flow(i) > 0){	// Sólo personas a visitas:
			int posPer = graph.Tail(i) - 1;
			int posVis = graph.Head(i) - (n+1);
			relPV.push_back(make_pair(pers_id[posPer],visita_id[posVis]));
		}
	}
	return relPV;
}



InstanceSolution InstanceInput::instanciaInicial(){
	// Generar asignación inicial personas-visitas
	vector <pair<string,string>> insIn = MinCostFlow();

	// Generar asignación con bloques horarios


	// Generar asignación con vehículos


	return InstanceSolution();
}


int InstanceInput::getMaxVisitasDePersona(string idPersona){
	unordered_map<string,int>::const_iterator ipos = mapa_personas.find(idPersona);
	if(ipos == mapa_personas.end()){
		return -1;
	}
	return pers_maxvisitas[ipos->second];
}


MiniBitmap* InstanceInput::getBloquesDispDePersona(string idPersona){
	unordered_map<string,int>::const_iterator ipos = mapa_personas.find(idPersona);
	if(ipos == mapa_personas.end()){
		return NULL;
	}
	return pers_horasdisp[ipos->second];
}


MiniBitmap* InstanceInput::getPersonasAfinesDePersona(string idPersona){
	unordered_map<string,int>::const_iterator ipos = mapa_personas.find(idPersona);
	if(ipos == mapa_personas.end()){
		return NULL;
	}
	return pers_afin[ipos->second];
}


MiniBitmap* InstanceInput::getPrefVisitaDePersona(string idPersona){
	unordered_map<string,int>::const_iterator ipos = mapa_personas.find(idPersona);
	if(ipos == mapa_personas.end()){
		return NULL;
	}
	return pers_prefvisita[ipos->second];
}


int InstanceInput::getCapacidadVehiculo(string idVehiculo){
	unordered_map<string,int>::const_iterator ipos = mapa_vehiculos.find(idVehiculo);
	if(ipos == mapa_vehiculos.end()){
		return -1;
	}
	return vehi_cap[ipos->second];
}


int InstanceInput::getCostoVehiculo(string idVehiculo){
	unordered_map<string,int>::const_iterator ipos = mapa_vehiculos.find(idVehiculo);
	if(ipos == mapa_vehiculos.end()){
		return -1;
	}
	return vehi_cost[ipos->second];
}


int InstanceInput::getTiempoTraslado(string idUbOr, string idUbDes){
	// Encontrar pos de la ubicación de origen
	unordered_map<string,int>::const_iterator ipos = mapa_ubicaciones.find(idUbOr);
	if(ipos == mapa_ubicaciones.end()){
		return -1;
	}
	int pOrigen = ipos->second;

	// Encontrar pos de la ubicación de destino
	ipos = mapa_ubicaciones.find(idUbDes);
	if(ipos == mapa_ubicaciones.end()){
		return -1;
	}
	int pDestino = ipos->second;

	return ubica_dist_mins[pOrigen][pDestino].second;
}


int InstanceInput::getDistanciaTraslado(string idUbOr, string idUbDes){
	// Encontrar pos de la ubicación de origen
	unordered_map<string,int>::const_iterator ipos = mapa_ubicaciones.find(idUbOr);
	if(ipos == mapa_ubicaciones.end()){
		return -1;
	}
	int pOrigen = ipos->second;

	// Encontrar pos de la ubicación de destino
	ipos = mapa_ubicaciones.find(idUbDes);
	if(ipos == mapa_ubicaciones.end()){
		return -1;
	}
	int pDestino = ipos->second;

	return ubica_dist_mins[pOrigen][pDestino].first;
}


MiniBitmap* InstanceInput::getPersonasParaVisita(string idVisita){
	unordered_map<string,int>::const_iterator ipos = mapa_visitas.find(idVisita);
	if(ipos == mapa_visitas.end()){
		return NULL;
	}
	return visita_personas[ipos->second];
}


MiniBitmap* InstanceInput::getVehiculosParaVisita(string idVisita){
	unordered_map<string,int>::const_iterator ipos = mapa_visitas.find(idVisita);
	if(ipos == mapa_visitas.end()){
		return NULL;
	}
	return visita_vehiculos[ipos->second];
}


int InstanceInput::getPrioridadVisita(string idVisita){
	unordered_map<string,int>::const_iterator ipos = mapa_visitas.find(idVisita);
	if(ipos == mapa_visitas.end()){
		return -1;
	}
	return visita_prioridad[ipos->second];
}


int InstanceInput::getTiempoEstimadoVisita(string idVisita){
	unordered_map<string,int>::const_iterator ipos = mapa_visitas.find(idVisita);
	if(ipos == mapa_visitas.end()){
		return -1;
	}
	return visita_testimado_bloques[ipos->second];
}


time_t InstanceInput::getInicioVentanaVisita(string idVisita){
	// Encontrar la id del blque en que inicia la ventanan de la visita
	unordered_map<string,int>::const_iterator ipos = mapa_visitas.find(idVisita);
	if(ipos == mapa_visitas.end()){
		return 0;	// ToDo: Revisar si 0 es equivalente a nada.
	}
	string idBInicio = visita_bloque_inicio[ipos->second];

	// Encontrar el time_t donde inicia el último bloque de la visita
	ipos = mapa_bloquesh.find(idBInicio);
	if(ipos == mapa_visitas.end()){
		return 0;	// ToDo: Revisar si 0 es equivalente a nada.
	}
	return bloque_timestamp[ipos->second];
}


time_t InstanceInput::getFinVetanaVisita(string idVisita){
	// Encontrar la id del blque en que termina la ventanan de la visita
	unordered_map<string,int>::const_iterator ipos = mapa_visitas.find(idVisita);
	if(ipos == mapa_visitas.end()){
		return 0; // ToDo: Revisar si 0 es equivalente a nada.
	}
	string idBFin = visita_bloque_fin[ipos->second];

	// Encontrar el time_t donde inicia el último bloque de la visita
	ipos = mapa_bloquesh.find(idBFin);
	if(ipos == mapa_visitas.end()){
		return 0; // ToDo: Revisar si 0 es equivalente a nada.
	}
	time_t inicioBFin = bloque_timestamp[ipos->second];

	// Sumar el tiempo del bloque para tener el tiempo límite
	return sumaMinutos(inicioBFin,z);
}

bool InstanceInput::personaPuedeVisitar(int posPersona, int posVisita){
	// Verificar si hay intersección entre el horario de la persona y la ventana de visita
	// Basado en la cantidad de bloques que tiene la persona en la ventana
	// (Podría ser en la cantidad de bloques contiguos del mismo día, pero por ahora no)

	// Encontrar IDs de los bloques para las ventanas
	string idBloqueInicial = visita_bloque_inicio[posVisita];
	unordered_map<string,int>::const_iterator ipos = mapa_bloquesh.find(idBloqueInicial);
	if(ipos == mapa_visitas.end()){
		return false;
	}
	int numBloqueInicial = ipos->second;

	string idBloqueFinal = visita_bloque_fin[posVisita];
	ipos = mapa_bloquesh.find(idBloqueFinal);
	if(ipos == mapa_visitas.end()){
		return false;
	}
	int numBloqueFinal = ipos->second;

	// Calcular cantidad de bloques disponibles
	int bloquesInter = pers_horasdisp[posPersona]->count(numBloqueInicial,numBloqueFinal);
	if(bloquesInter < visita_testimado_bloques[posVisita]){
		return false;
	}
	return true;
}


//	***********************************************************
//	********************** CLASE TRASLADO *********************
//	***********************************************************

Traslado::Traslado(time_t tt, string idve, string idor, string iddes, MiniBitmap* mb, string idvis){
	horainicio = tt;
	idvehi = idve;
	iduborigen = idor;
	idubdestino = iddes;
	bm_personas = mb;
	idvisita = idvis;
	mb = NULL;
}

Traslado::~Traslado(){
	delete bm_personas;
}



//	***********************************************************
//	****************** CLASE INSTANCE OUTPUT ******************
//	***********************************************************

InstanceOutput::InstanceOutput(string filename, int nn, int mm, int kk, int ll){
	n = nn;
	m = mm;
	k = kk;
	l = ll;

	ifstream salida(filename);
	string linea;
	getline(salida, linea);
	istringstream iss;

	iss = istringstream(linea);
	iss >> t;

	string traslado_ts;
	time_t viaje_hora;
	string viaje_idvehi;
	string viaje_idubica_orig;
	string viaje_idubica_dest;
	MiniBitmap* viaje_pers;
	string viaje_idvisita;

	tr = vector<Traslado*>(t);

	for(int i=0; i<t; i++){
		getline(salida, linea);
		iss = istringstream(linea);
		iss >> traslado_ts >> viaje_idvehi >> viaje_idubica_orig >> viaje_idubica_dest;
		viaje_hora = obtenerTime(traslado_ts);
//		cout << stringTime(viaje_hora) << " " << viaje_idvehi << " " << viaje_idubica_orig << " " << viaje_idubica_dest << " ";
		int aux;
		// Procesando personas que pueden realizar la visita
		viaje_pers = new MiniBitmap(n);
		for(int j=0; j<n; j++){
			iss >> aux;
			if(aux == 1){
				viaje_pers->setBit(j);
			}
		}
//		viaje_pers->printBitmap();
//		cout << " ";
		iss >> viaje_idvisita;
//		cout << viaje_idvisita << endl;

		tr[i] = new Traslado(viaje_hora, viaje_idvehi, viaje_idubica_orig, viaje_idubica_dest, viaje_pers, viaje_idvisita);

		cout << stringTime(tr[i]->horainicio) << " " << tr[i]->idvehi << " " << tr[i]->iduborigen << " " << tr[i]->idubdestino << " ";
		tr[i]->bm_personas->printBitmap();
		cout << " " << tr[i]->idvisita << endl;
	}
}

InstanceOutput::InstanceOutput(InstanceSolution solucion){
	// ToDo: generar formato InstanceSolution (vector de traslados) desde la solución.

}


InstanceOutput::~InstanceOutput(){
	cout << "Eliminando InstanceOutput..." << endl;
	for(int i=0; i < t; i++){
		delete tr[i];
	}
}


void InstanceOutput::generarFichero(string filename){
	// ToDo: sacar a fichero el formato de salida
}


void InstanceOutput::sortByIdVehiculoFecha(){
	sort(tr.begin(), tr.end(), InstanceOutput::comparadorIdVehiculoFecha);
}


bool InstanceOutput::comparadorIdVehiculoFecha(Traslado* i1, Traslado* i2){
	if(i1->idvehi.compare(i2->idvehi) == 0){
		return i1->horainicio < i2->horainicio;
	}
	return i1->idvehi.compare(i2->idvehi) < 0;
}


int InstanceOutput::validaViajesPorVehiculos(){
	cout << "Traslados por idVehículo:" << endl;
	int solapes_trayectos_vehiculos = 0;
	for(int i=0; i<t; i++){
		cout << tr[i]->toString();
		if(i>1 && tr[i]->idvehi.compare(tr[i-1]->idvehi) == 0 && tr[i]->horainicio < tr[i-1]->horainicio){
			solapes_trayectos_vehiculos++;
			cout << " <- !!!!!";
		}
		cout << endl;
	}
	cout << "Solapes en trayectos de vehículos: " << solapes_trayectos_vehiculos << endl;
	return solapes_trayectos_vehiculos;
}

string Traslado::toString(){
	string salida = "";
	salida += stringTime(horainicio);
	salida += " " + stringTime(horafin);
	salida += " " + idvehi;
	salida += " " + iduborigen;
	salida += " " + idubdestino;
	salida += " " + idvisita;
	return salida;
}



//	*************************************************************
//	****************** CLASE INSTANCE SOLUTION ******************
//	*************************************************************


InstanceSolution::InstanceSolution(){
	cout << "Creando InstanceSolution" << endl;
	
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

	return NULL;
}

