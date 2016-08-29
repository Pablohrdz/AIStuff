#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <queue>
#include <cmath>

using namespace std;

//Pablo Hernández Cepeda

struct Nodo 
{
	vector<Nodo *> hijos;
	vector<int> estado;
	int gcost = 0;
    int hcost = 0;
    int fcost = 0;
	string accion = "";

	//Función de comparación para que el max-heap (priority queue) pueda ordenar a los nodos
	friend bool operator<(const Nodo &a, const Nodo &b)
	{
		return a.fcost < b.fcost;
	}
};

struct Posicion
{
	int x = 0;
	int y = 0;

	Posicion(int xPos, int yPos) : x(xPos), y(yPos) {}
	Posicion(){}
};

enum Heuristica
{
	MANHATTAN, FICHAS_FUERA_DE_LUGAR
};


//Búsquedas
string AStar(Nodo *root, int heuristica);
string dfs(Nodo *root);
string bfs(Nodo *root);

//Heurísticas
int computeManhattanDistance(Nodo *nodo);
int fichasFueraDeLugar(Nodo *nodo);

//Métodos para crear el árbol
void expandir(Nodo *nodo);
string accionesPosibles(Nodo *nodo);

//Métodos auxiliares
void swap(vector<int> &arr, int i, int j);
string estadoToString(Nodo *nodo);
void dibujarEstado(Nodo *node);
void dibujarCamino(string s);
bool esEstadoFinal(Nodo *node);


//variables globales
Nodo estadoFinal;
Nodo inicial;
map<string, bool> visitados;


int main()
{

	cout << "|" << "1 |" << " 2 " << "| 3 |"<< endl;
	cout << "|" << "4 |" << " 5 " << "| 6 |"<< endl;
	cout << "|" << "7 |" << " 8 " << "| 0 |"<< endl;

	cout << "El 0 denota el espacio vació y los demás números, los números correspondientes" << endl;
	cout << "Como ejemplo, el estado anterior correspondería a 123456780" << endl;

	cout << "Escribe el estado inicial" << endl;
	
	string inputI, inputF;

	cin >> inputI;

	cout << "Escribe el estado final" << endl;

	cin >> inputF;


 	//Debug
 	/*
 	for(int i = 0; i < 9; i++)
 	{
 		cout << ei[i] << endl;
 	}
	*/
	int opcion = 0;

	while(opcion != 4)
	{
		//Llenar los estados 
		for(int i = 0; i < 9; i++)
		{
			(inicial.estado).push_back(inputI[i] - '0');
			(estadoFinal.estado).push_back(inputF[i] - '0');
 		}

		cout << "Escoge el tipo de búsqueda" << endl;
		cout << "(Toma en cuenta que la solución puede tardar mucho en algunos casos, \n sobre todo para la primera opción)" << endl;
		cout << "1) Busqueda en profundidad (depth-first search)" << endl;
		cout << "2) Busqueda en anchura (breadth-first search)" << endl;
		cout << "3) Busqueda A*" << endl;
		cout << "4) Salir " << endl;

		cin >> opcion;
	
		switch(opcion)
		{
			case 1:
				dibujarCamino(dfs(&inicial));
				break;

			case 2:
				dibujarCamino(bfs(&inicial));
				break;

			case 3:
				cout << "Selecciona el tipo de heurística que deseas usar." << endl;
				cout << "1) Manhattan" << endl;
				cout << "2) # de fichas fuera de lugar" << endl;
				
				int hrstca = 0;

				cin >> hrstca;

				switch(hrstca)
				{
					case 1:
					dibujarCamino(AStar(&inicial, MANHATTAN));
					break;

					case 2:
					dibujarCamino(AStar(&inicial, FICHAS_FUERA_DE_LUGAR));
					break;

					default:
					cout << "Escoja una heurística válida" << endl;
					break;
				}
				break;
		}

		visitados.clear();
		
	}
}

/************************************* Búsquedas **************************************/

string AStar(Nodo *root, int heuristica)
{

  int iteraciones = 1;
  priority_queue<Nodo *> fringe;
  
  root -> gcost = 0;
  heuristica == MANHATTAN ? root -> hcost = computeManhattanDistance(root) : root -> hcost = fichasFueraDeLugar(root);
  root -> fcost = (root -> gcost) + (root -> fcost);
  
  fringe.push(root);

  while(!fringe.empty())
  {
    Nodo *node = fringe.top();
    fringe.pop();
    
    //Expandimos el nodo
    expandir(node);

    //Lo metemos a la lista de visitados
    string s = estadoToString(node);
    visitados[s] = true;
   
   	//Si es el estado final, devolvemos la acción.
    if(esEstadoFinal(node))		
    {
    	cout << "/*******************************************************************/" << endl;
		cout << "Se encontró el estado final" << endl;
		cout << "Estado final -> " << s << endl;
		cout << "Secuencia desde el estado inicial: " << endl;
		cout << node -> accion << endl;
		cout << "/*******************************************************************/" << endl;
		return node -> accion;
    }
   
    for(int i = 0; i < (node -> hijos).size(); i++)
    {
      //Si no se ha expandido ese nodo
      if(visitados.find( estadoToString( (node -> hijos)[i] ) ) == visitados.end())
      {
			//Modificación para A*. Calcular los fcosts de cada hijo.
      		//Todos los gcost son 1 + gcost del padre.
      		if(heuristica == MANHATTAN)					
      		{
      			node -> hijos[i] -> hcost = computeManhattanDistance(node -> hijos[i]);
      		}	
			else 
			{
				node -> hijos[i] -> hcost = fichasFueraDeLugar(node -> hijos[i]);
			}
			node -> hijos[i] -> gcost = 1 + node -> gcost;
			node -> hijos[i] -> fcost = (node -> hijos[i] -> gcost) + (node -> hijos[i] -> hcost);

			//Meter el hijo al fringe
			fringe.push((node -> hijos)[i]);
      }
    }

    //Mostrar iteraciones en múltiplos de 5000
    if(iteraciones % 5000 == 0)
		{
			cout << "#Iteraciones: " << iteraciones << endl;
		}
		
		iteraciones++;

  }

  cout << "No existe solución" << endl;
  return "";

}


//Búsqueda en anchura
//Regresa un string para poder pasárselo al método que lo dibuja
string bfs(Nodo *root)
{
	queue<Nodo *> queue;

	queue.push(root);

	int iteraciones = 1;

	while(!queue.empty())
	{
		//if(iteraciones == 10)	return;
		
		Nodo *node = queue.front();

		queue.pop();

		//Marcar como visitado al nodo actual
		string s = estadoToString(node);

		//cout << "estado: " << s << endl;

		visitados[s] = true;

		//cout << "Acción: " << node -> accion << endl;

		//Comparar con estado final
		if(esEstadoFinal(node))
		{
			cout << "/*******************************************************************/" << endl;
			cout << "Se encontró el estado final" << endl;
			cout << "Estado final -> " << s << endl;
			cout << "Secuencia desde el estado inicial: " << endl;
			cout << node -> accion << endl;
			cout << "/*******************************************************************/" << endl;
			return node -> accion;
		}

		//Expandir el nodo actual
		expandir(node);


		for(int i = 0; i < (node -> hijos).size(); i++)
		{
			//cout << "hijo #" << i << ": " << estadoToString((node -> hijos)[i]) << endl;

			//Si no hemos expandido ese nodo
			if( visitados.find( estadoToString( (node -> hijos)[i] ) ) == visitados.end())
			{
				queue.push((node -> hijos)[i]);
			}
		}


		if(iteraciones % 5000 == 0)
		{
			cout << "#Iteraciones: " << iteraciones << endl;
		}
		
		iteraciones++;
	}

	cout << "No existe solución." << endl;

	return "";
}


//Búsqueda en profundidad
//Regresa un string para poder pasárselo al método que lo dibuja
string dfs(Nodo *root)
{
	stack<Nodo *> stack;

	stack.push(root);

	int iteraciones = 1;

	while(!stack.empty())
	{
		//if(iteraciones == 10)	return;
		
		Nodo *node = stack.top();

		stack.pop();

		//Marcar como visitado al nodo actual
		string s = estadoToString(node);

		//cout << "estado: " << s << endl;

		visitados[s] = true;

		//cout << "Acción: " << node -> accion << endl;

		//Comparar con estado final
		if(esEstadoFinal(node))
		{
			cout << "/*******************************************************************/" << endl;
			cout << "Se encontró el estado final" << endl;
			cout << "Estado final -> " << s << endl;
			cout << "Secuencia desde el estado inicial: " << endl;
			cout << node -> accion << endl;
			cout << "/*******************************************************************/" << endl;
			return node -> accion;
		}

		//Expandir el nodo actual
		expandir(node);


		for(int i = 0; i < (node -> hijos).size(); i++)
		{
			//cout << "hijo #" << i << ": " << estadoToString((node -> hijos)[i]) << endl;

			//Si no hemos pasado por ahí
			if( visitados.find( estadoToString( (node -> hijos)[i] ) ) == visitados.end())
			{
				//dfs((root -> hijos)[i]);
				stack.push((node -> hijos)[i]);
			}
		}

		if(iteraciones % 5000 == 0)
		{
			cout << "#Iteraciones: " << iteraciones << endl;
		}
		
		iteraciones++;

	}

	cout << "No existe solución." << endl;

	return "";
}


/************************************* Heurísticas **************************************/


//Cuenta el número de fichas fuera de lugar con respecto al estado final
int fichasFueraDeLugar(Nodo *nodo)
{
	int sum = 0;

	for(int i = 0; i < estadoFinal.estado.size(); i++)
	{
		if(nodo -> estado[i] != estadoFinal.estado[i])	sum++;
	}

	return sum;
}


//Regresa la suma de las distancias Manhattan de todos los números del puzzle
int computeManhattanDistance(Nodo *nodo)
{
	int sum = 0;

	//Estructuras de datos auxiliares para comparar las posiciones de las casillas
	map<int, Posicion> eFinal;
	map<int, Posicion> eActual;

	for(int i = 0; i < estadoFinal.estado.size(); i++)
	{
		Posicion p;
		p.x = ((i + 1) % 3);
		p.y = ((i / 3) + 1);

		//Pequeño if para compensar por los casos de (k)3 % 3, en donde k es un #entero positivo y múltiplo de 3
		if(p.x == 0)		p.x = 3;

		eFinal[estadoFinal.estado[i]] = p;
		eActual[nodo -> estado[i]] = p;
	}

	//Realizar la suma de las distancias Manhattan
	for(int i = 0; i < estadoFinal.estado.size(); i++)
	{	
		Posicion posFinal = eFinal.find(i) -> second;
		Posicion posActual = eActual.find(i) -> second;

		sum += abs( ((posFinal.x) - (posActual.x)) + ((posFinal.y) - (posActual.y)) );
	}

	return sum;
}


/************************************* Métodos Auxiliares **************************************/
//Dibuja el camino del nodo inicial al final
void dibujarCamino(string s)
{	
	//Nodo nodo = inicial;
	Nodo nodo;

	for(int i = 0; i < 9; i++)
	{
		nodo.estado.push_back( (inicial.estado)[i] );
	}


	int k = 0;

	cout << "#" << k << " - inicial" << endl;

	dibujarEstado(&nodo);

	

	int pos; //La posición del espacio en blanco
	
	for(pos = 0; pos < 9; pos++)
	{
		if( (nodo.estado)[pos] == 0)	break;
	}

	for(int i = 0; i < s.length(); i++)
	{

		if(s[i] == 'U')			
		{
			swap(nodo.estado, pos, pos - 3);
			pos -= 3;
		}
		else if(s[i] == 'D')	
		{	
			swap(nodo.estado, pos, pos + 3);
			pos += 3;
		}
		else if(s[i] == 'R')	
		{
			swap(nodo.estado, pos, pos + 1);
			pos++;
		}
		else if(s[i] == 'L')		
		{
			swap(nodo.estado, pos, pos - 1);
			pos--;
		}

		k++;

		if(k == s.length())			cout << "#" << k << " - " << s[i] << " - final" << endl;
		else						cout << "#" << k << " - " << s[i] << endl;

		dibujarEstado(&nodo);
	}

}


//Dibuja un solo estado
void dibujarEstado(Nodo *node)
{
	string s = estadoToString(node);

	cout << endl;
	cout << "| " << s[0] << " | " << s[1] << " | " << s[2] << " |" << endl;
	cout << "| " << s[3] << " | " << s[4] << " | " << s[5] << " |" << endl;
	cout << "| " << s[6] << " | " << s[7] << " | " << s[8] << " |" << endl;
	cout << endl;
}

bool esEstadoFinal(Nodo *node)
{
	//Comparar con estado final
	bool flag = true;
	
	for(int i = 0; i < 9; i++)
	{
		if((node -> estado)[i] != (estadoFinal.estado)[i])	flag = false;
	}

	return flag;
}

//Método que pasa un estado (desde su nodo) a un string
string estadoToString(Nodo *nodo)
{
	string s = "";

	for(int i = 0; i < 9; i++)
	{
		s += to_string((nodo -> estado)[i]);
	}

	return s;
}

//Swap en un vector (para mover los números de un estado)
void swap(vector<int> &arr, int i, int j)
{
	int temp = arr[i];
	arr[i] = arr[j];
	arr[j] = temp;
}

/************************************* Métodos para la Creación del Árbol **************************************/

//Expandir nodo y guardar la expansión de estados como nodos hijo
void expandir(Nodo *nodo)
{
	string acciones = accionesPosibles(nodo);

	int pos; //La posición del espacio en blanco
	
	for(pos = 0; pos < 9; pos++)
	{
		if( (nodo -> estado)[pos] == 0)	break;
	}


	//Para cada acción posible, crear un nuevo estado
	//y añadirlo a los hijos del estado actual
	for(int i = 0; i < acciones.length(); i++)
	{
		Nodo *nuevoNodo = new Nodo;

		//Copiar estado
		for(int j = 0; j < 9; j++)
		{
			(nuevoNodo -> estado).push_back( (nodo -> estado)[j] );
		}
		//nuevoNodo -> estado = nodo -> estado;

		if(acciones[i] == 'U')			swap(nuevoNodo -> estado, pos, pos - 3);
		else if(acciones[i] == 'D')		swap(nuevoNodo -> estado, pos, pos + 3);
		else if(acciones[i] == 'R')		swap(nuevoNodo -> estado, pos, pos + 1);
		else if(acciones[i] == 'L')		swap(nuevoNodo -> estado, pos, pos - 1);
		
		string acc = (nodo -> accion);

		acc += acciones[i];

		(nuevoNodo -> accion) = acc;

		(nodo -> hijos).push_back(nuevoNodo);
	}

	//cout << "expansión terminada" << endl;

}


//Regresa las acciones posibles desde el estado actual
string accionesPosibles(Nodo *nodo)
{
	string acciones = "";
	int i;

	//Averiguar la posición del contador
	for(i = 0; i < 9; i++)
	{
		if( (nodo -> estado)[i] == 0)	break;
	}

	if(i >= 0 && i <= 2)					acciones += "D";
	else if(i >= 3 && i <= 5)				acciones += "UD";
	else if(i >= 6 && i <= 8)				acciones += "U";

	if(i == 1 || i == 4 || i == 7)			acciones += "RL";
	else if(i == 0 || i == 3 || i == 6)		acciones += "R";
	else if(i == 2 || i == 5 || i == 8)		acciones += "L";

	//cout << "Acciones posibles: " << acciones << endl;

	return acciones;
}
