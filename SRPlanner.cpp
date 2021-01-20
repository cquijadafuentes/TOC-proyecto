#include "SRPlanner.hpp"

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
	}

	// Lectura de k líneas con las distancias entre ubicaciones
	// Nota: primera ubicación siempre es SERVIU
	ubica_id = vector<string>(k);
	ubica_dist_mins = vector<pair<double,int>>(k);
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
			ubica_dist_mins[j] = make_pair(auxdist,auxtime);
			cout << "(" << ubica_dist_mins[j].first << "-" << ubica_dist_mins[j].second << ") ";
		}
		cout << endl;
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
		iss >> visita_testimado_bloques[i] >> visita_prioridad[i];
		cout << visita_testimado_bloques[i] << " " << visita_prioridad[i] << endl;
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


void InstanceInput::MinCostFlow(){

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

	// Se imprime por fila, lo que se debería entregar a .AddArcWithCapacityAndUnitCost(·,·,·,·)
	int origen, destino, capacidad, costo;
	// 1- Desde source a cada persona.
	origen = 0;
	for(int i=0; i<n; i++){
		destino = i+1;
		capacidad = pers_maxvisitas[i];
		costo = 1; 		// ToDo: Esto se debería calcular más adelante
		cout << origen << " " << destino << " " << capacidad << " " << costo << endl;
	}
	// 2- Personas a visitas
	for(int i=0; i<l; i++){
		for(int j=0; j<n; j++){
			if(visita_personas[i]->access(j) == 1){
				origen = j+1;
				destino = n+1+i;
				capacidad = 1; 		// ToDo: Calcular capacidad (?) de visita de la persona j a la obra i
				costo = 1;			// ToDo: Calcular costo de visita de la persona j a la obra i
				cout << origen << " " << destino << " " << capacidad << " " << costo << endl;
			}
		}
	}

	// 3- Visitas a sink
	destino = 1+n+l;
	for(int i=0; i<l; i++){
		origen = n+1+i;
		capacidad = 1;
		costo = 1;
		cout << origen << " " << destino << " " << capacidad << " " << costo << endl;
	}

	// Lo que debe ir a .SetNodeSupply(·,·)
	// 1- Source:
	cout << "0 " << l << endl;
	// 2- Vértices de personas y visitas en 0
	for(int i=1; i <= (n+l); i++){
		cout << i << " 0" << endl;
	}
	// 3- Sink
	int aux = n+l+1;
	cout << aux << " -" << l << endl;

}



MiniBitmap* InstanceInput::getPersonasParaVisita(string idVisita){
	return NULL;
}


MiniBitmap* InstanceInput::getVehiculosParaVisita(string idVisita){
	return NULL;
}


int InstanceInput::getPrioridadVisita(string idVisita){
	return 0;
}


int InstanceInput::getTiempoEstimadoVisita(string idVisita){
	return 0;
}


time_t InstanceInput::getInicioVentanaVisita(string idVisita){
	return 0;
}


time_t InstanceInput::getFinVetanaVisita(string idVisita){
	return 0;
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

InstanceOutput::~InstanceOutput(){
	cout << "Eliminando InstanceOutput..." << endl;
	for(int i=0; i < t; i++){
		delete tr[i];
	}
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
	cout << "InstanceSolution" << endl;

	
}

bool InstanceSolution::validarInstancia(){
	cout << "validarInstancia" << endl;

	
}

double InstanceSolution::evaluarInstancia(){
	cout << "evaluarInstancia" << endl;

	
}
