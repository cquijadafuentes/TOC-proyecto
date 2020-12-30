#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "MiniBitmap.hpp"

using namespace std;

int main(int argc, char const *argv[]){
	if(argc < 2){
		cout << "Error! Falta argunmento con nombre del archivo de entrada." << endl;
		cout << "Ejemplo: " << endl;
		cout << argv[0] << " datos.txt" << endl;
		return -1;
	}	
	
	ifstream entrada(argv[1]);
	string linea;
	getline(entrada, linea);
	istringstream iss;

	iss = istringstream(linea);
	// La primera linea contiene los siguientes valores:
	int n;			// Cantidad de Personas
	int m;			// Cantidad de Vehículos
	int k;			// Cantidad de Ubicaciones
	int h;			// Cantidad de Bloques horarios
	int z; 			// Minutos por bloque
	int l;			// Cantidad de Visitas
	int v;			// Valor del Viático
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
	string persona_id[n];
	int persona_maxvisitas[n];
	MiniBitmap* persona_horasdisp[n];
	MiniBitmap* persona_afinidad[n];
	MiniBitmap* persona_prefvisita[n];
	for(int i=0; i < n; i++){
		// Cada línea cotiene:
		// id_persona visitas_max horas_disponibles(h bits) afinidad (n bits) preferencias_visitas(l bits)
		getline(entrada, linea);
		iss = istringstream(linea);
		iss >> persona_id[i] >> persona_maxvisitas[i];
		cout << persona_id[i] << " " << persona_maxvisitas[i] << " ";
		int aux;
		// Procesando horas disponibles
		persona_horasdisp[i] = new MiniBitmap(h);
		for(int j=0; j<h; j++){
			iss >> aux;
			if(aux == 1){
				persona_horasdisp[i]->setBit(j);
			}
		}
		persona_horasdisp[i]->printBitmap();
		cout << " ";
		// Procesando afinidad
		persona_afinidad[i] = new MiniBitmap(n);
		for(int j=0; j<n; j++){
			iss >> aux;
			if(aux == 1){
				persona_afinidad[i]->setBit(j);
			}
		}
		persona_afinidad[i]->printBitmap();
		cout << " ";
		// Procesando preferencias visitas
		persona_prefvisita[i] = new MiniBitmap(l);
		for(int j=0; j<l; j++){
			iss >> aux;
			if(aux == 1){
				persona_prefvisita[i]->setBit(j);
			}
		}
		persona_prefvisita[i]->printBitmap();
		cout << " ";

		cout << endl;
	}

	// Lectura de m líneas con la información de los vehículos
	string vehiculo_id[m];
	int vehiculo_capacidad[m];
	int vehiculo_costo[m];
	for(int i=0; i < m; i++){
		// Cada línea cotiene:
		// id_vehichulo capacidad costo
		getline(entrada, linea);
		iss = istringstream(linea);
		iss >> vehiculo_id[i] >> vehiculo_capacidad[i] >> vehiculo_costo[i];
		cout << vehiculo_id[i] << " " << vehiculo_capacidad[i] << " " << vehiculo_costo[i] << endl;
	}

	// Lectura de k líneas con las distancias entre ubicaciones
	// Nota: primera ubicación siempre es SERVIU
	string ubicaciones_id[k];
	pair<double,int> ubicaciones_dist_mins[k];
	for(int i=0; i < k; i++){
		// Cada línea cotiene:
		// id_ubicacion k pares distancia,tiempo
		getline(entrada, linea);
		iss = istringstream(linea);
		iss >> ubicaciones_id[i];
		cout << ubicaciones_id[i] << " ";
		// Procesando pares distancia,tiempo
		double auxdist;
		int auxtime;
		for(int j=0; j<k; j++){
			iss >> auxdist >> auxtime;
			ubicaciones_dist_mins[j] = make_pair(auxdist,auxtime);
			cout << "(" << ubicaciones_dist_mins[j].first << "-" << ubicaciones_dist_mins[j].second << ") ";
		}
		cout << endl;
	}

	// Lectura de h líneas con al información de bloques horarios
	string bloque_id[h];
	string bloque_timestamp[h];
	for(int i=0; i < h; i++){
		// Cada línea cotiene:
		// id_bloque timestamp_bloque
		getline(entrada, linea);
		iss = istringstream(linea);
		iss >> bloque_id[i] >> bloque_timestamp[i];
		cout << bloque_id[i] << " " << bloque_timestamp[i] << endl;
	}

	// Lectura de l líneas con al información de las visitas
	string visita_id[l];
	string visita_ubicacion[l];
	string visita_bloque_inicio[l];
	string visita_bloque_fin[l];
	MiniBitmap* visita_personas[n];
	MiniBitmap* visita_vehiculos[m];
	int visita_duracion_bloques[l];
	int visita_prioridad[l];
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
		iss >> visita_duracion_bloques[i] >> visita_prioridad[i];
		cout << visita_duracion_bloques[i] << " " << visita_prioridad[i] << endl;
	}
	cout << "Sobrante:" << endl;
	while(getline(entrada, linea)){
		cout << linea << endl;
	}

	return 0;
}