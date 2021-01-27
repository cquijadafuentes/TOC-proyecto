#include "SRP_InstanceInput.hpp"

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
	pers_horasdisp = vector<MiniBitmap*>(n,NULL);
	pers_afin = vector<MiniBitmap*>(n,NULL);
	pers_prefvisita = vector<MiniBitmap*>(n,NULL);

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
	ubica_dist_mins = vector<vector<pair<float,int>>>(k,vector<pair<float,int>>(k));
	for(int i=0; i < k; i++){
		// Cada línea cotiene:
		// id_ubicacion k pares distancia,tiempo
		getline(entrada, linea);
		iss = istringstream(linea);
		iss >> ubica_id[i];
		cout << ubica_id[i] << " ";
		// Procesando pares distancia,tiempo
		float auxdist;
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
	visita_personas = vector<MiniBitmap*>(l,NULL);
	visita_vehiculos = vector<MiniBitmap*>(l,NULL);
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


InstanceInput::InstanceInput(){

}


InstanceInput::~InstanceInput(){
	cout << "Eliminando InstanceInput..." << endl;
	for(int i=0; i < n; i++){
		if(pers_horasdisp[i] != NULL){
			delete pers_horasdisp[i];
		}

		if(pers_afin[i] != NULL){
			delete pers_afin[i];
		}

		if(pers_prefvisita[i] != NULL){
			delete pers_prefvisita[i];
		}
	}
	for(int i=0; i < l; i++){
		if(visita_personas[i] != NULL){
			delete visita_personas[i];
		}

		if(visita_vehiculos[i] != NULL){
			delete visita_vehiculos[i];
		}
	}
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


time_t InstanceInput::getInicioVentanaVisita(int posVisita){
	int posBloque = mapa_bloquesh[visita_bloque_inicio[posVisita]];
	return bloque_timestamp[posBloque];
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


bool InstanceInput::bloqueContiguoConSiguiente(int posBl){
	if(posBl < 0 || posBl >= bloque_timestamp.size()-1){
		return false;
	}
	return sumaMinutos(bloque_timestamp[posBl],z) == bloque_timestamp[posBl+1];
}


InstanceInput* InstanceInput::copiaInstanceInput(){
	InstanceInput* copia = new InstanceInput();
	copia->n = n;
	copia->m = m;
	copia->k = k;
	copia->h = h;
	copia->z = z;
	copia->l = l;
	copia->v = v;

	copia->pers_id = vector<string>(pers_id);
	copia->pers_maxvisitas = vector<int>(pers_maxvisitas);
	copia->pers_horasdisp = vector<MiniBitmap*>(n,NULL);
	copia->pers_afin = vector<MiniBitmap*>(n,NULL);
	copia->pers_prefvisita = vector<MiniBitmap*>(n,NULL);
	for(int i=0; i < n; i++){
		copia->pers_horasdisp[i] = pers_horasdisp[i]->copia();
		copia->pers_afin[i] = pers_afin[i]->copia();
		copia->pers_prefvisita[i] = pers_prefvisita[i]->copia();
	}

	copia->vehi_id = vector<string>(vehi_id);
	copia->vehi_cap = vector<int>(vehi_cap);
	copia->vehi_cost = vector<int>(vehi_cost);

	copia->ubica_id = vector<string>(ubica_id);
	copia->ubica_dist_mins = vector<vector<pair<float,int>>>(ubica_dist_mins);

	copia->bloque_id = vector<string>(bloque_id);
	copia->bloque_timestamp = vector<time_t>(bloque_timestamp);

	copia->visita_id = vector<string>(visita_id);
	copia->visita_ubicacion = vector<string>(visita_ubicacion);
	copia->visita_bloque_inicio = vector<string>(visita_bloque_inicio);
	copia->visita_bloque_fin = vector<string>(visita_bloque_fin);
	copia->visita_personas = vector<MiniBitmap*>(l,NULL);
	copia->visita_vehiculos = vector<MiniBitmap*>(l,NULL);
	copia->visita_testimado_bloques = vector<int>(visita_testimado_bloques);
	copia->visita_prioridad = vector<int>(visita_prioridad);
	copia->visita_cant_personas = vector<int>(visita_cant_personas);
	for(int i=0; i < l; i++){
		copia->visita_personas[i] = visita_personas[i]->copia();
		copia->visita_vehiculos[i] = visita_vehiculos[i]->copia();
	}

	copia->mapa_personas = unordered_map<string,int>(mapa_personas);
	copia->mapa_vehiculos = unordered_map<string,int>(mapa_vehiculos);
	copia->mapa_ubicaciones = unordered_map<string,int>(mapa_ubicaciones);
	copia->mapa_bloquesh = unordered_map<string,int>(mapa_bloquesh);
	copia->mapa_visitas = unordered_map<string,int>(mapa_visitas);

	return copia;

}

