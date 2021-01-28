#include "SRPlanner.hpp"


namespace operations_research {
	struct Arc {
		std::pair<NodeIndex, NodeIndex> nodes;
		FlowQuantity capacity;
		FlowQuantity unit_cost;
	};
}


void printCuarteta(Cuarteta x){
	cout << "bh_" << x.posbh << "\tper_" << x.posp << "\tposvi" << x.posvi << "\tposve" << x.posve;
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
		cout << "Desde solver: ";
		delete ii;
	}
	
	if(is_actual != NULL){
		cout << "Desde solver: ";
		delete is_actual;
	}
}


void Solver::SolucionPorBusquedaLocal(string outputFileName){






	// Generar archivo con la salida de los datos.
	is_actual->printToFile(outputFileName);
}



//	*************************************************************
//	****************** CLASE INSTANCE SOLUTION ******************
//	*************************************************************


InstanceSolution::InstanceSolution(InstanceInput* ii){
	punteroII = ii->copiaInstanceInput();
	costo = 0.0;
	cout << "Creando InstanceSolution" << endl;
	// Generar asignación inicial personas-visitas
	vector <Tripleta> insIn = MinCostFlow(ii);
	cout << "Visitas a ralizar idPersona -> idVisita: " << endl;
	for(int i=0; i<insIn.size(); i++){
		cout << insIn[i].posPersona << " -> " << insIn[i].posVisita << " - Inicia en: " << stringTime(insIn[i].tInicioVisita) << endl;
	}
	// 1° Ordenar el vector por el bloque de inicio de la visita
	sort(insIn.begin(), insIn.end(), sortTripletaPorTiempoInicio);
	// Generar estructura para marcar visitas asignadas
	vector<bool> visitasAsignadas = vector<bool>(ii->l, false);
	// Iniciar estructura para marcar el uso de los vehículos
	horasUsoVeh = vector<MiniBitmap*>(ii->m, NULL);
	for(int i=0; i<ii->m; i++){
		horasUsoVeh[i] = new MiniBitmap(ii->h);
	}
	// Copiar estructura con disponibilidad de las personas
	horasDispPer = vector<MiniBitmap*>(ii->n, NULL);
	for(int i=0; i<ii->n; i++){
		horasDispPer[i] = ii->pers_horasdisp[i]->copia();
		
		cout << " ---- " << endl;
		ii->pers_horasdisp[i]->printBitmap();
		cout << endl;
		horasDispPer[i]->printBitmap();
		cout << endl;
	}
	// Identificar jornadas y seleccionar los pares a la jornada
	int jor_pos_fin, jor_pos_inicio;
	time_t t_jorn_fin, t_jorn_inicio;
	cout << "Identificando jornadas..." << endl;
	jor_pos_fin = 0;
	while(jor_pos_fin < ii->h){
		t_jorn_inicio = ii->bloque_timestamp[jor_pos_fin];
		jor_pos_inicio = jor_pos_fin;
		t_jorn_fin = sumaMinutos(ii->bloque_timestamp[jor_pos_fin],ii->z);
		while(jor_pos_fin < ii->h-1 && ii->bloqueContiguoConSiguiente(jor_pos_fin)){
			t_jorn_fin = sumaMinutos(t_jorn_fin,ii->z);
			jor_pos_fin++;
		}
		jor_pos_fin++;
		// <--- Hasta aquí se ha identificado un bloque [t_jorn_inicio,t_jorn_fin]
		// Cualquier visita no realizada que tenga tiempo_inicio < t_jorn_fin entra en la ventana
		cout << "Jornada desde: " << stringTime(t_jorn_inicio) << " hasta " << stringTime(t_jorn_fin) << endl;
		vector<Cuarteta> visJornada;
		int i=0;
		while(i < insIn.size() && insIn[i].tInicioVisita < t_jorn_fin){
			if(!visitasAsignadas[insIn[i].posVisita]){
				Cuarteta xx;
				xx.posp = insIn[i].posPersona;
				xx.posvi = insIn[i].posVisita;
				xx.posbh = -1;
				xx.posve = -1;
				visJornada.push_back(xx);
			}
			i++;
		}
		cout << "Visitas en evaluación antes de " << stringTime(t_jorn_fin) << endl;
		for(int i=0; i<visJornada.size(); i++){
			int pvisitalocal = visJornada[i].posvi;
			printCuarteta(visJornada[i]);
			cout << endl;
			// Identificar si hay bloques suficientes en la jornada para la visita desde serviu
			int posUbicaOrigen = 0;	// Siempre se comienza desde el serviu
			int posUbicaDestino = ii->mapa_ubicaciones[ii->visita_ubicacion[pvisitalocal]];
			int minutosTraslado = ii->ubica_dist_mins[posUbicaOrigen][posUbicaDestino].second;
			float distanciaTraslado = ii->ubica_dist_mins[posUbicaOrigen][posUbicaDestino].first;
			int bloquesTraslado = minutosTraslado / ii->z;
			if(bloquesTraslado * ii->z < minutosTraslado){
				bloquesTraslado++;
			}
			int bloquesVisita = ii->visita_testimado_bloques[pvisitalocal];
			int bloquesTotal = bloquesVisita + (2 * bloquesTraslado);
//			cout << "Visita " << ii->visita_id[pvisitalocal] << " toma " << bloquesTraslado << " bloques en traslado";
//			cout << " y " << bloquesVisita << " bloques en la visita." << endl;
//			cout << "\tTotal: " << bloquesTotal << " bloques en traslado" <<  endl;
			// Calcular tiempo de inicio de la visita
			int deltaBloques = 0;
			time_t t_iniciaVisita = t_jorn_inicio;
			while(!visitasAsignadas[pvisitalocal] && sumaMinutos(t_iniciaVisita, (bloquesTotal+deltaBloques)*ii->z) <= t_jorn_fin){
				// La visita no ha sido asignada y 
				// El tiempo de traslado + visita es mejor que la jornada
				// Revisar si las personas de la visita pueden en el bloque
				int cantPerDisponibles = 0;
				int countBloqueInicio = jor_pos_inicio + deltaBloques;
				int countBloqueFin = countBloqueInicio + bloquesTotal - 1;
				for(int ij = 0; ij < ii->visita_cant_personas[pvisitalocal]; ij++){
					if(horasDispPer[visJornada[i+ij].posp]->count(countBloqueInicio, countBloqueFin) == bloquesTotal){
						cantPerDisponibles++;
					}
				}
				if(cantPerDisponibles == ii->visita_cant_personas[pvisitalocal]){
					// Todas las personas tienen horario disponible
					// Buscar un vehículo para el traslado
					int idVe = 0;
					while(idVe < ii->m && !visitasAsignadas[pvisitalocal] && ii->visita_vehiculos[pvisitalocal]->access(idVe) == 1){
						cout << "Evaluando Vehículo " << idVe << " para visita " << pvisitalocal << endl;
						if(horasUsoVeh[idVe]->count(countBloqueInicio, countBloqueFin) == 0){
							// El vehículo está disponible y se puede asignar el viaje
							costo += (distanciaTraslado * ii->vehi_cost[idVe]) * 2;
							for(int ij = 0; ij < bloquesTotal; ij++){
								horasUsoVeh[idVe]->setBit(ij+countBloqueInicio);
								for(int kk = 0; kk < cantPerDisponibles; kk++){
									Cuarteta xx;
									xx.posp = insIn[i].posPersona;
									xx.posbh = jor_pos_inicio + ij;
									xx.posvi = -1;
									xx.posve = -1;
									// Asignar según el bloque:
									if(ij < bloquesTraslado){
										// Si el traslado de ida
										xx.posve = idVe;
									}else if(ij < bloquesVisita+bloquesTraslado){
										// Si es la visita
										xx.posvi = pvisitalocal;
									}else{
										// Si es el traslado de vuelta
										xx.posve = idVe;
									}
									instance.push_back(xx);
									// Marcar que la persona ya no tiene tiempo
									horasDispPer[xx.posp]->unsetBit(xx.posbh);
								}
							}
							// Marcar Visita como asignada
							visitasAsignadas[pvisitalocal] = true;
							// Actualizar i en función de las personas que deben hacer la visita
							i += (cantPerDisponibles - 1);
							cout << "Visita asignada exitosamente" << endl;
						}
						idVe++;
					}
				}

				// Revisar si hay vehículo dispnible en el bloque

				// Si es posible hacer la visita, registrar resultado en instance.
				deltaBloques++;
			}
		}
	}
	int cva = 0;
	for(int i=0; i<visitasAsignadas.size(); i++){
		if(visitasAsignadas[i]){
			cva++;
		}
	}
	printInstanceSolution();
	cout << "Conteo final de visitas asignadas: " << cva << "/" << visitasAsignadas.size() << endl;
	cout << "Costo de la asignación: " << costo << endl;
	// Checkear la solución inicial (si es válida)
	isValid = checker();

	cout << "La solución es valida? ";
	if (isValid) {
		cout << "Si";
	} else {
		cout << "No";
	}
	cout << endl;

	// Evaluar la solución inicial
	evaluacion = evaluator();
}


InstanceSolution::InstanceSolution(InstanceInput* ii, InstanceOutput* io){
	// Generar solución desde un InstanceInput y un InstanceOutput
	punteroII = ii->copiaInstanceInput();
	// Para verificar una solución en base a los ficheros de entrada y salida

	
	// Checkear la solución inicial (si es válida)
	isValid = checker();

	// Evaluar la solución inicial
	evaluacion = evaluator();
}


InstanceSolution::InstanceSolution(){

}


InstanceSolution::~InstanceSolution(){
	cout << "Borrando InstanceSolution" << endl;
	
	// No se borra punteroII porque es de solver y él lo borra.
	if(punteroII != NULL){
		delete punteroII;
	}
	for(int i=0; i<horasUsoVeh.size(); i++){
		if(horasUsoVeh[i] != NULL){
			delete horasUsoVeh[i];
		}
	}

	for(int i=0; i<horasDispPer.size(); i++){
		if(horasDispPer[i] != NULL){
			delete horasDispPer[i];
		}
	}
}


bool InstanceSolution::validarInstancia(){
	cout << "validarInstancia" << endl;

	return false;
}


float InstanceSolution::evaluarInstancia(){
	cout << "evaluarInstancia" << endl;
	// Calcular costo de la solución
	//
	costo = 0.0;
	for(int i=0; i<instance.size(); i++){
		// 

	}

	// Calcular calidad de la solución

	return 0.0;
}




vector<InstanceSolution> InstanceSolution::generarVecinos(){
	// ToDo: Generar vecinos
	vector<InstanceSolution> vecinos;
/*
	bool condGenerarVecinos = true;
	int maxIteraciones = 10;
	float factor = 0.8;	// Factor para aceptar la solución generada entre los vecinos


	return vecinos;		// Se cae el código aún.


	while(condGenerarVecinos && maxIteraciones-- > 0){
		InstanceSolution xx = copiaInstanceSolution();
		// Revisar si es posible movimiento local
		// NOTA: si logramos mantener siempre ordenado por este criterio, no sería necesario hacer el sort.
		sort(instance.begin(), instance.end(), sortCuartetaPorPersonaBloque);

		// Identificar una ruta que se realiza en una jornada
		int bri = 0;		// posición del Bloque Ruta Inicial
		int brf = 1;		// posición del Bloque Ruta Final
		int cant_visitas = 0;
		vector<int> pubv; 	// Posiciones de las Ubicaciones de las visitas entre bri y brf.
//		vector<int> cbhev;	// Cantidad de bloques horarios entre visitas
//		cbhev.push_back(1);
//		int pcbhev = 0;
		while(brf-1 < instance.size() && punteroII->bloqueContiguoConSiguiente(brf) && instance[brf+1].posp == instance[bri].posp ){
			brf++;
			if(instance[brf].posvi > instance[brf-1].posvi){
				// Dado que la instancia es correca y no habrá dos bloques de visitas en 
				// contiguos que tengan distinta ubicación se deduce que -1 < cualquier visita
				cant_visitas++;
				pubv.push_back(instance[brf].posvi);
//				pcbhev++;
//				cbhev.push_back(0);
//			}else{
//				cbhev[pcbhev]++;
			}
		}

		cout << "Ruta identificada: " << endl;
		for(int i=0; i < pubv.size(); i++){
			cout << pubv[i] << " - " << endl;
		}
		cout << "En " << (brf - bri + 1) << " bloques." << endl;
		return vecinos;

		// Si hay más visitas planificadas para la misma persona
		if((brf + 1) < instance.size() && instance[brf+1].posp == instance[brf].posp){
			// Identificar una segunda visita de la misma persona que podría integrarse a su última ruta
			int bri2 = brf + 1;
			int brf2 = bri2 + 1;
			while(brf2-1 < instance.size() && punteroII->bloqueContiguoConSiguiente(brf2) && instance[brf2+1].posp == instance[bri2].posp){
				
			}


			// Verificar si se pueden unir las visitas


			// Hacer movimiento local (En lo posible actualizar costo)



			// checkear solución
			xx.isValid = xx.checker();

			if(xx.isValid){
				// Si no se actualizó costo, hacerlo ahora
				
				// Evaluar la solución
				xx.evaluacion = xx.evaluator();

				if(xx.evaluacion > (evaluacion * factor)){
					vecinos.push_back(xx);
				}
			}
		}
		condGenerarVecinos = false;
	}

	*/
	return vecinos;

}




InstanceSolution InstanceSolution::copiaInstanceSolution(){
	InstanceSolution ret;
	ret.instance = vector<Cuarteta>(instance);
	ret.horasUsoVeh = vector<MiniBitmap*>(horasUsoVeh.size(), NULL);
	for (int i = 0; i < horasUsoVeh.size(); ++i){
		ret.horasUsoVeh[i] = horasUsoVeh[i]->copia();
	}
	ret.horasDispPer = vector<MiniBitmap*>(horasDispPer.size(), NULL);
	for (int i = 0; i < horasDispPer.size(); ++i){
		ret.horasDispPer[i] = horasDispPer[i]->copia();
	}
	ret.costo = costo;
	ret.isValid = isValid;
	ret.evaluacion = evaluacion;

	return ret;
}


vector<Tripleta> InstanceSolution::MinCostFlow(InstanceInput* ii){

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
		LOG(FATAL) << "El SERVIU no puede realizar todas las visitas por el límite de visitas en los funcionarios.";
	}
	operations_research::FlowQuantity total_flow_cost = min_cost_flow.GetOptimalCost();
	
	vector<Tripleta> relPV;	// Relaciones Persona-Visita
	for (int i = 0; i < caristas; ++i) {
		if(graph.Tail(i) > 0 && graph.Tail(i) <= ii->n && min_cost_flow.Flow(i) > 0){	// Sólo personas a visitas:
			Tripleta aux;
			int posPer = graph.Tail(i) - 1;
			int posVis = graph.Head(i) - (ii->n+1);
			aux.posPersona = posPer;
			aux.posVisita = posVis;
			aux.tInicioVisita = ii->getInicioVentanaVisita(ii->visita_id[posVis]);
			relPV.push_back(aux);
		}
	}
	return relPV;
}


bool InstanceSolution::sortTripletaPorTiempoInicio(Tripleta a, Tripleta b){
	if(a.tInicioVisita != b.tInicioVisita){
		return a.tInicioVisita < b.tInicioVisita;
	}
	if(a.posVisita != b.posVisita){
		return a.posVisita < b.posVisita;
	}
	return a.posPersona < b.posPersona;
}

bool InstanceSolution::sortCuartetaPorBloquePersona(Cuarteta a, Cuarteta b){
	// Ordena el instance por Bloque horario y luego por id de la Persona
	if(a.posbh != b.posbh){
		return a.posbh < b.posbh;
	}
	return a.posp < b.posp;
}

bool InstanceSolution::sortCuartetaPorPersonaBloque(Cuarteta a, Cuarteta b){
	// Ordena el instance por persona y luego por bloque horario.
	// Por definición una persona no puede tener dos actividades en el mismo horario
	// por lo que no podría haber dos cuartetas con igual bloque horario && igual persona

	if(a.posp != b.posp){
		return a.posp < b.posp;
	}
	return a.posbh < b.posbh;
}


bool InstanceSolution::checker(){
	// Verificar restriccioes duras de la instancia

	// Se ordena por persona y luego por bloque, puede resultar más fácil verificar que una persona no esté en dos sitios al mismo tiempo
	// Si los movimientos de una persona son lógicos (los vehículos lo llevan desde el origen hasta una ubicación)
	// Y luego un vehículo lo lleva desde su ubicación hasta el destino, se asume que el vehículo también va bien.
	sort(instance.begin(), instance.end(), sortCuartetaPorPersonaBloque);

	// - Una persona no está en dos sitios al mismo tiempo
	for(auto a = instance.begin(); a != instance.end()-1; a++) {
		auto b = a+1;

		// no es necesario ir al vector
		if (a->posp == b->posp) {
			if (a->posbh == b->posbh) {			// siquiera está bien que esté dos veces el mismo bloque horario para la misma persona??
//				cout << "checker: personas" << endl;
//				cout << a->posp << " " << b->posp << " " << a->posbh << " " << b->posbh << " " << endl;
				return false;
			}
		}
	}
/*
	for (int i = 0; i < instance.size(); ++i) {
		cout << instance[i].posbh << " " << instance[i].posp << " " << instance[i].posve << " " << instance[i].posvi << endl;
	}
*/
	// - Un vehículo no está en dos sitios al mismo tiempo
	for (auto a = instance.begin(); a != instance.end()-1; a++) {
		auto b = a+1;

		if (a->posbh == b->posbh) {
			if (a->posve == b->posve) {
//				cout << "checker: vehiculo" << endl;
				return false;
			}
		}
	}

	// - Una persona llega a una visita posterior a un desplazamiento
	for (auto a = instance.begin(); a != instance.end()-1; a++) {
		auto b = a+1;

		if (a->posp == b->posp) {
			if (b->posve == -1) {	// b es visita
				if (a->posvi != -1 && a->posvi != b->posvi && (1+a->posbh)==(b->posbh)) {	// a es visita pero en otro lugar
//					cout << "checker: -> visita" << endl;
					return false;
				}
				// falta checkear si el desplazamiento correspondo a la ubicación
			}
		}
		// caso contrario (true): a es desplazamiento o a es visita en el mismo lugar
	}

	// - Una persona se desplaza luego de una visita
	for (auto a = instance.begin(); a != instance.end()-1; a++) {
		auto b = a+1;

		if (a->posp == b->posp) {
			if (b->posvi == -1) { // b es desplazamiento
				if (a->posvi == -1 && a->posve != b->posve && (1+a->posbh)==(b->posbh)) {	// a es desplazamiento y no corresponde al desplazamiento de b
//					cout << "checker: -> desplaza" << endl;
//					cout << a->posbh << " " << a->posp << " " << a->posve << " " << a->posvi << endl;
//					cout << b->posbh << " " << b->posp << " " << b->posve << " " << b->posvi << endl;					
					return false;		// no estoy seguro si esto verifica bien que los desplazamientos correspondan.
				}
			}
		}
		// caso contrario: a es visita o a es desplz. = b
	}


	// - Todo vehículo sale desde el serviu y vuelve al serviu en una misma jornada (?)
	

	// - Toda persona sale desde el serviu y vuelve al serviu en una misma jornada (?)


	// Si no falló antes
	return true;
}


double InstanceSolution::evaluator(){
	// Considerar para el factor de evlauación: 
	// - La función objetivo (el costo)
	// - Los tiempos de espera
	double ev = 0.0;
	
	return ev;
}


bool InstanceSolution::cuartetasContiguasIguales(Cuarteta a, Cuarteta b){
	if(a.posp != b.posp){
		return false;
	}
	if(a.posvi != b.posvi){
		return false;
	}
	if(a.posve != b.posve){
		return false;
	}
	return punteroII->bloqueContiguoConSiguiente(a.posbh);
	
}


void InstanceSolution::printInstanceSolution(){
	cout << "(" << instance.size() << " bloques-persona-actividad)" << endl;
	cout << "bh\tp\tve\tvi" << endl;
	for(int i=0; i<instance.size(); i++){
		cout << instance[i].posbh;
		cout << "\t" << instance[i].posp;
		cout << "\t" << instance[i].posve;
		cout << "\t" << instance[i].posvi << endl;
	}
}



void InstanceSolution::printToFile(string fn){
	cout <<  "Mostrando OUTPUT: " << endl;
	vector<Cuarteta> sinrep;
	ofstream os;
	sort(instance.begin(), instance.end(), sortCuartetaPorPersonaBloque);
	string ubOrigen = punteroII->ubica_id[0];
	for(int i=0; i<instance.size(); i++){
		if(i==0 || (i>0 && !cuartetasContiguasIguales(instance[i-1], instance[i]))){
			Cuarteta xx;
			xx.posp = instance[i].posp;
			xx.posbh = instance[i].posbh;
			if(instance[i].posve != -1){
				xx.posve = instance[i].posve;
				xx.posvi = -1;
			}else{
				xx.posvi = instance[i].posvi;
				xx.posve = -1;
			}
			sinrep.push_back(xx);
		}
	}
	os.open(fn);
	os << sinrep.size() << endl;
	for(int i=0; i<sinrep.size(); i++){
		os << punteroII->pers_id[sinrep[i].posp];
		os << "\t" << punteroII->bloque_id[sinrep[i].posbh];
		if(sinrep[i].posve != -1){
			os << "\t" << punteroII->vehi_id[sinrep[i].posve];
			if((i+1) < sinrep.size() && sinrep[i+1].posvi != -1){
				os << "\t" << punteroII->visita_ubicacion[sinrep[i+1].posvi] << endl;
			}else{
				os << "\t" << punteroII->ubica_id[0] << endl;
			}
		}else{
			os << "\t" << punteroII->visita_id[sinrep[i].posvi] << endl;
		}
	}
}


