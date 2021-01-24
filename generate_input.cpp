#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include "MiniBitmap.hpp"

using namespace std;

double distancia(pair<int,int>* dist, int x, int y);

int main(int argc, char const *argv[]){
	cout << " :) " << endl;

	if(argc < 9){
		cout << "Error! Faltan argunmentos." << endl;
		cout << "Ejemplo: " << endl;
		cout << argv[0] << " <output_file> <n> <m> <k> <h> <z> <l> <v>" << endl;
		cout << "n es la cantidad de personas" << endl;
		cout << "m es la cantidad de vehículos" << endl;
		cout << "k es la cantidad de ubicaciones" << endl;
		cout << "h es la cantidad de bloques horarios" << endl;
		cout << "z es la cantidad de minutos por bloque" << endl;
		cout << "l es la cantidad de visitas a realizar" << endl;
		cout << "v es el valor del viático" << endl;
		return -1;
	}
	srand (time(NULL));

	ofstream os;
	os.open(argv[1]);
	// La primera linea contiene:
	int n = atoi(argv[2]);			// Cantidad de Personas
	int m = atoi(argv[3]);			// Cantidad de Vehículos
	int k = atoi(argv[4]);			// Cantidad de Ubicaciones
	int h = atoi(argv[5]);			// Cantidad de Bloques horarios
	int z = atoi(argv[6]); 			// Minutos por bloque
	int l = atoi(argv[7]);			// Cantidad de Visitas
	int v = atoi(argv[8]);			// Valor del Viático

	os << n << " "	<< m << " "	<< k << " "	<< h << " "	<< z << " "	<< l << " "	<< v << " "	<< endl;

	string ids_ubicacion[k];
	string ids_bloques[h];

	// Líneas con información de las personas
	// id visitas_maximas horas_disp (bitmap tamaño h) afinidad (bitmap tamaño n) preferencia_visitas (bitmap tamaño l)
	for(int i=0; i<n; i++){
		// id
		os << "pid_"<< setfill ('0') << setw (3) << i;

		// visitas_maximas
		int* aux = new int[h];
		int intaux;
		for(int j=0; j<h; j++){
			intaux = (rand() % (l-2)) + 2;
		}
		os << " " << intaux;
		// Bloques que serán de visitas para la persona
		int cantSi = (rand() % (h/2)) + (h/2); // min 50%
		MiniBitmap mb(h);
		int cantbloques2horas = 120 / z;
		while(mb.cant < cantSi){
			intaux = rand() % h;
			for(int j=0;j<cantbloques2horas; j++){
				mb.setBit((intaux+j));
			}
		}
		for(int j=0; j<h; j++){
			os << " " << mb.access(j);
		}
		delete aux;

		// Afinidad con los comañeros
		aux = new int[n];
		for(int j=0; j<n; j++){
			aux[j] = 1;
		}
		// Personas que no serán afines
		int cantNo = rand() % (n/2); // max 50%
		for(int j=0; j<cantNo; j++){
			aux[(rand() % n)] = 0;
		}
		aux[i] = 1; // Asegurar que sea compatible consigo mismo
		for(int j=0; j<n; j++){
			os << " " << aux[j];
		}
		delete aux;

		// Preferencias a visitas de obras
		aux = new int[l];
		for(int j=0; j<l; j++){
			aux[j] = 1;
		}
		// Obras que no son preferencia
		cantNo = rand() % (l/2); // max 50%
		for(int j=0; j<cantNo; j++){
			aux[(rand() % l)] = 0;
		}
		for(int j=0; j<l; j++){
			os << " " << aux[j];
		}
		delete aux;

		os << endl;
	}


	// Líneas con información de los vehículos
	// id capacidad costo
	for(int i=0; i<m; i++){
		// id
		os << "vid_"<< setfill ('0') << setw (3) << i;

		int cap = (rand() % 8) + 3;	// De 3 a 10 personas
		os << " " << cap;

		int rend = (rand() % 251) + 50; // De 50 a 300
		os << " " << rend;

		os << endl;
	}


	// Líneas con información de las ubicaciones
	// id distancias-tiempos
	pair<int,int>* ubicaciones = new pair<int,int>[k];
	for(int i=0; i<k; i++){
		ubicaciones[i] = make_pair(rand()%100,rand()%100);
	}

	for(int i=0; i<k; i++){
		// id
		stringstream ss;
		ss << "uid_" << setfill ('0') << setw (3) << i;
		ids_ubicacion[i] = ss.str();
		os << ids_ubicacion[i];

		for(int j=0; j<k; j++){
			double di = distancia(ubicaciones, i, j);
			int ti =(int) (di * ((rand() % 150 + 0.0) / 100.0));			
			os << " " << di << " " << ti;
		}

		os << endl;
	}
	delete(ubicaciones);


	// Líneas con información de los bloques horarios
	// id timestamp_inicio
	int dia = 1;
	int hora = 9;
	int min = 0;
	string pre = "2020-01-";
	for(int i=0; i<h; i++){
		// id
		stringstream ss;
		ss << "bhid_"<< setfill ('0') << setw (3) << i;
		ids_bloques[i] = ss.str();
		os << ids_bloques[i];

		// timestamp
		// Validaciones minutos
		if(min >= 60){
			hora++;
			min = min % 60;
		}
		// Hora de colación
		if((hora == 12 && min > 30) || hora == 13){
			hora++;
			min += 30;
			if(min >= 60){
				hora++;
				min = min % 60;
			}
		}
		// Hora de salida
		if(hora >= 19){
			dia++;
			hora = 9;
			min = 0;
		}
		os << " " << pre << setfill ('0') << setw (2) << dia;
		os << "T" << setfill ('0') << setw (2) << hora;
		os << ":" << setfill ('0') << setw (2) << min;
		os << ":00Z" << endl;

		// Sumar minutos para el siguiente bloque
		min += z;
	}


	// Líneas con información de las visitas
	// id id_ubicacion idbh_inicio idbh_fin personas (bitmap tamaño n) vehiculos (bitmap tamaño m) cant_bloques_visita prioridad
	// Desordenar las ids de las ubicaciones
	random_shuffle(ids_ubicacion, ids_ubicacion + k);
	int maxBloquesvisita = 3 * 60 / z;	// Visitas de máximo 3 horas.
	for(int i=0; i<l; i++){
		int* aux;
		int cantSi;
		// id
		os << "vid_"<< setfill ('0') << setw (3) << i;
		// id_ubicacion
		os << " " << ids_ubicacion[i%k];
		// id_bloqueinicio
		int aux_bhinicio = (rand() % (h/2));		// En la primera mitad de los bloques
		// id_bloquefin
		int aux_bhfin = (h/2) + (rand() % (h/2));	// En la segunda mitad de los bloques
		// Si llega a ser menos tiempo del necesario para los bloques
		if(aux_bhfin - aux_bhinicio < maxBloquesvisita){
			aux_bhfin = (aux_bhfin + maxBloquesvisita) % h;
			if(aux_bhfin - aux_bhinicio < maxBloquesvisita){
				aux_bhinicio = 0;
				aux_bhfin = h-1;
			}
		}
		os << " " << ids_bloques[aux_bhinicio];
		os << " " << ids_bloques[aux_bhfin];

		// bitmap de personas que pueden hacer la visita
		aux = new int[n];
		for(int j=0; j<n; j++){
			aux[j] = 0;
		}
		// Personas que pueden hacer la visita
		cantSi = rand() % (n/2) + 3; // max 50% mínimo 3
		for(int j=0; j<cantSi; j++){
			aux[(rand() % n)] = 1;
		}
		for(int j=0; j<n; j++){
			os << " " << aux[j];
		}
		delete aux;

		// bitmap de vehículos para la visita
		aux = new int[m];
		for(int j=0; j<m; j++){
			aux[j] = 0;
		}
		// Personas que pueden hacer la visita
		cantSi = rand() % (m/2 - 3) + 3; // max 50% min 3
		int conteoPersonas = 0;
		int posAux;
		for(int j=0; j<cantSi; j++){
			posAux = (rand() % m);
			if(aux[posAux] == 0){
				conteoPersonas++;
			}
			aux[posAux] = 1;
		}
		for(int j=0; j<m; j++){
			os << " " << aux[j];
		}
		delete aux;

		// Duración estiamada en bloques
		int duracionestimada = rand() % maxBloquesvisita + 1;
		os << " " << duracionestimada;

		// Prioridad (de 1 a 5)
		int prioridad = rand() % 5 + 1;
		os << " " << prioridad;

		// Cantidad de personas para visita
		int cantPer = (rand() % 3) + 1;		// 1 a 3
		if(cantPer < conteoPersonas){
			cantPer = conteoPersonas;
		}
		os << " " << cantPer;
		os << endl;
	}


	return 0;
}


double distancia(pair<int,int>* dist, int a, int b){
	int deltax = dist[a].first - dist[b].first;
	int deltay = dist[a].second - dist[b].second;
	long sum = deltax*deltax + deltay*deltay;
	return sqrt(sum);
}
