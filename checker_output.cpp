#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include "MiniBitmap.hpp"
#include <vector>
#include <map>
#include <algorithm>

using namespace std;
	
typedef struct traslado{
	time_t t_inicio;
	time_t t_fin;
	string id_vehiculo;
	string id_origen;
	string id_destino;
	MiniBitmap* bitmap_personas;
	string id_visita;
}TRASLADO;

time_t loadtime(string ts);

string stringTime(time_t t);

string stringTraslado(TRASLADO tt);

time_t sumarMinutos(time_t t, int m);

bool sortPorVehiculo(TRASLADO i1, TRASLADO i2);

int main(int argc, char const *argv[]){
	if(argc < 3){
		cout << "Error! Falta argunmento con nombre del archivo de entrada." << endl;
		cout << "Ejemplo: " << endl;
		cout << argv[0] << " entrada.txt salida.txt" << endl;
		return -1;
	}

	// ------------------------------ LECTURA DEL ARCHIVO DE ENTRADA ------------------------------
	
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
	map<string,int> mapa_personas;
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
		mapa_personas[persona_id[i]] = i;
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
	map<string,int> mapa_vehiculos;
	int vehiculo_capacidad[m];
	int vehiculo_costo[m];
	for(int i=0; i < m; i++){
		// Cada línea cotiene:
		// id_vehichulo capacidad costo
		getline(entrada, linea);
		iss = istringstream(linea);
		iss >> vehiculo_id[i] >> vehiculo_capacidad[i] >> vehiculo_costo[i];
		mapa_vehiculos[vehiculo_id[i]] = i;
		cout << vehiculo_id[i] << " " << vehiculo_capacidad[i] << " " << vehiculo_costo[i] << endl;
	}

	// Lectura de k líneas con las distancias entre ubicaciones
	// Nota: primera ubicación siempre es SERVIU
	string ubicaciones_id[k];
	map<string,int> mapa_ubicaciones;
	pair<double,int> ubicaciones_dist_mins[k][k];
	for(int i=0; i < k; i++){
		// Cada línea cotiene:
		// id_ubicacion k pares distancia,tiempo
		getline(entrada, linea);
		iss = istringstream(linea);
		iss >> ubicaciones_id[i];
		cout << ubicaciones_id[i] << " ";
		mapa_ubicaciones[ubicaciones_id[i]] = i;
		// Procesando pares distancia,tiempo
		double auxdist;
		int auxtime;
		for(int j=0; j<k; j++){
			iss >> auxdist >> auxtime;
			ubicaciones_dist_mins[i][j] = make_pair(auxdist,auxtime);
			cout << "(" << ubicaciones_dist_mins[i][j].first << "-" << ubicaciones_dist_mins[i][j].second << ") ";
		}
		cout << endl;
	}

	// Lectura de h líneas con al información de bloques horarios
	string bloque_id[h];
	time_t bloque_timestamp[h];
	string auxstring;
	for(int i=0; i < h; i++){
		// Cada línea cotiene:
		// id_bloque timestamp_bloque
		getline(entrada, linea);
		iss = istringstream(linea);
		iss >> bloque_id[i] >> auxstring;
		bloque_timestamp[i] = loadtime(auxstring);
		cout << bloque_id[i] << " " << ctime(&bloque_timestamp[i]);
	}

	// Lectura de l líneas con al información de las visitas
	string visita_id[l];
	map<string,int> mapa_visitas;
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
		mapa_visitas[visita_id[i]] = i;
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

	// ------------------------------ LECTURA DEL ARCHIVO DE SALIDA ------------------------------

	ifstream salida(argv[2]);
	getline(salida, linea);
	iss = istringstream(linea);
	int tras;
	iss >> tras;

	string traslado_ts;
	time_t traslado_tt[tras];
	string traslado_idv[tras];
	string traslado_idubo[tras];
	string traslado_idubd[tras];
	MiniBitmap* traslado_personas[tras];
	string traslado_idvi[tras];

	for(int i=0; i<tras; i++){
		getline(salida, linea);
		iss = istringstream(linea);
		iss >> traslado_ts >> traslado_idv[i] >> traslado_idubo[i] >> traslado_idubd[i];
		traslado_tt[i] = loadtime(traslado_ts);
		cout << stringTime(traslado_tt[i]) << " " << traslado_idv[i] << " " << traslado_idubo[i] << " " << traslado_idubd[i] << " ";
		int aux;
		// Procesando personas que pueden realizar la visita
		traslado_personas[i] = new MiniBitmap(n);
		for(int j=0; j<n; j++){
			iss >> aux;
			if(aux == 1){
				traslado_personas[i]->setBit(j);
			}
		}
		traslado_personas[i]->printBitmap();
		cout << " ";
		iss >> traslado_idvi[i];
		cout << traslado_idvi[i] << endl;
	}
	
	// Estructura con toda la información de los traslados
	vector<struct traslado> traslados_(tras);
	for(int i=0; i<tras; i++){
		// posiciòn de las Ids de ubicaciones de origen y destino
		int pid_origen = mapa_ubicaciones[traslado_idubo[i]];
		int pid_destino = mapa_ubicaciones[traslado_idubd[i]];
		// Obtener tiempo de traslado
		int minutos_od = ubicaciones_dist_mins[pid_origen][pid_destino].second;
		// Calcular hora de llegada segun hora de inicio y timempo del traslado
		time_t auxtt = sumarMinutos(traslado_tt[i], minutos_od);
		int pid_vehiculo = mapa_vehiculos[traslado_idv[i]];
		traslados_[i].t_inicio = traslado_tt[i];
		traslados_[i].t_fin = auxtt;
		traslados_[i].id_vehiculo = traslado_idv[i];
		traslados_[i].id_origen = traslado_idubo[i];
		traslados_[i].id_destino = traslado_idubd[i];
		traslados_[i].bitmap_personas = traslado_personas[i];
		traslados_[i].id_visita = traslado_idvi[i];
	}
	
	sort(traslados_.begin(), traslados_.end(), sortPorVehiculo);

	// Validación de traslados por vehículos
	cout << "Traslados por idVehículo:" << endl;
	int solapes_trayectos_vehiculos = 0;
	for(int i=0; i<tras; i++){
		cout << stringTraslado(traslados_[i]);
		if(i>1 && traslados_[i].id_vehiculo.compare(traslados_[i-1].id_vehiculo) == 0 && traslados_[i].t_inicio < traslados_[i-1].t_fin){
			solapes_trayectos_vehiculos++;
			cout << " <- !!!!!";
		}
		cout << endl;
	}
	cout << "Solapes en trayectos de vehículos: " << solapes_trayectos_vehiculos << endl;

	return 0;
}


time_t loadtime(string ts){
	time_t t;
	char * cts = new char [ts.length()+1];
	strcpy (cts, ts.c_str());
	struct tm tm;
	strptime(cts, "%Y-%m-%dT%H:%M:%S%Z", &tm);
	t = mktime(&tm);  // t is now your desired time_t
	return t;
}

time_t sumarMinutos(time_t t, int m){
	struct tm tm = *localtime(&t);
	tm.tm_min += m;
	time_t nt = mktime(&tm);
	return nt;
}

bool sortPorVehiculo(TRASLADO i1, TRASLADO i2){
	if(i1.id_vehiculo.compare(i2.id_vehiculo) == 0){
		return i1.t_inicio < i2.t_inicio;
	}
	return i1.id_vehiculo.compare(i2.id_vehiculo) < 0;
}

string stringTraslado(TRASLADO tt){
	string salida = "";
	salida += stringTime(tt.t_inicio);
	salida += " " + stringTime(tt.t_fin);
	salida += " " + tt.id_vehiculo;
	salida += " " + tt.id_origen;
	salida += " " + tt.id_destino;
	salida += " " + tt.id_visita;
	return salida;
}