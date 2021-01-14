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
	
	viaje_hora = vector<time_t>(t);
	viaje_idvehi = vector<string>(t);
	viaje_idubica_orig = vector<string>(t);
	viaje_idubica_dest = vector<string>(t);
	viaje_pers = vector<MiniBitmap*>(t);
	viaje_idvisita = vector<string>(t);

	for(int i=0; i<t; i++){
		getline(salida, linea);
		iss = istringstream(linea);
		iss >> traslado_ts >> viaje_idvehi[i] >> viaje_idubica_orig[i] >> viaje_idubica_dest[i];
		viaje_hora[i] = obtenerTime(traslado_ts);
		cout << stringTime(viaje_hora[i]) << " " << viaje_idvehi[i] << " " << viaje_idubica_orig[i] << " " << viaje_idubica_dest[i] << " ";
		int aux;
		// Procesando personas que pueden realizar la visita
		viaje_pers[i] = new MiniBitmap(n);
		for(int j=0; j<n; j++){
			iss >> aux;
			if(aux == 1){
				viaje_pers[i]->setBit(j);
			}
		}
		viaje_pers[i]->printBitmap();
		cout << " ";
		iss >> viaje_idvisita[i];
		cout << viaje_idvisita[i] << endl;
	}
	

}


InstanceOutput::~InstanceOutput(){
	cout << "Eliminando InstanceOutput..." << endl;
	for(int i=0; i < t; i++){
		delete viaje_pers[i];
	}
}