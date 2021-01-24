#include "SRP_InstanceOutput.hpp"

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

InstanceOutput::InstanceOutput(){
	// ToDo: generar formato InstanceSolution (vector de traslados) desde la solución.

}


InstanceOutput::~InstanceOutput(){
	cout << "Eliminando InstanceOutput..." << endl;
	for(int i=0; i < tr.size(); i++){
		delete tr[i];
	}
}


void InstanceOutput::generarFichero(string filename){
	// ToDo: sacar a fichero el formato de salida
	cout << "Imprimiendo fichero de salida" << endl;
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