#include <atomic> // Para utilizar variables atómicas
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <iomanip>

using namespace std;
using namespace chrono;
atomic<int> thread_count(0); // Contador para contar la cantidad de hilos creados
// Función para intercambiar dos elementos en un vector
void swap(vector<string>& arr, int i, int j) {
    string temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

// Función para dividir el arreglo y encontrar el pivote
int partition(vector<string>& arr, int low, int high) {
    string pivot = arr[high]; // Seleccionamos el último elemento como pivote
    int i = low - 1; // Índice del elemento más pequeño

    for (int j = low; j <= high - 1; j++) {
        // Si el elemento actual es menor o igual al pivote
        if (arr[j] <= pivot) {
            i++; // Incrementamos el índice del elemento más pequeño
            swap(arr, i, j);
        }
    }
    swap(arr, i + 1, high);
    return i + 1;
}

// Función principal de Quick Sort
void quickSort(vector<string>& arr, int low, int high) {
    if (low < high) {
        // Encuentra el índice del pivote
        int pi = partition(arr, low, high);

        // Ordena los elementos antes y después del pivote
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// Función de envoltorio para llamar a Quick Sort
void sort(vector<string>& arr) {
    quickSort(arr, 0, arr.size() - 1);
}

// Función para imprimir un vector de cadenas
void printVector(const vector<string>& arr) {
    for (const string& s : arr) {
        cout << s << " ";
    }
    cout << endl;
}

// Quick Sort paralelo con profundidad máxima
void parallelQuickSort(vector<string>& arr, int low, int high, int depth) {
    if (low < high) {
        if (depth <= 0) {
            quickSort(arr, low, high); // Cambiar a QuickSort secuencial si se alcanza la profundidad máxima
            return;
        }
        int pi = partition(arr, low, high);
        thread_count += 2;
        thread left([&]() {
            parallelQuickSort(arr, low, pi - 1, depth - 1);
        });
        thread right([&]() {
            parallelQuickSort(arr, pi + 1, high, depth - 1);
        });
        left.join();
        right.join();
    }
}

// Función para llamar a Quick Sort paralelo con profundidad máxima
void parallelSort(vector<string>& arr, int depth) {
    parallelQuickSort(arr, 0, arr.size() - 1, depth);
}

int main() {
    int word_count;
    cout << "Ingrese la cantidad de palabras (frutas) a generar: ";
    cin >> word_count;

    vector<string> arr(word_count);
    vector<string> frutas = {"manzana", "pera", "uva", "platano", "naranja", "sandia", "melón", "cereza", "piña", "fresa"};

    // Generamos el arreglo de palabras (frutas) aleatorias
    for (int i = 0; i < word_count; ++i) {
        int index = rand() % frutas.size();
        arr[i] = frutas[index];
    }

    //cout << "Array antes de ordenar: ";
    //printVector(arr);

    // Ordenación secuencial
    auto start_seq = high_resolution_clock::now();
    sort(arr);
    auto stop_seq = high_resolution_clock::now();
    auto duration_seq = duration_cast<microseconds>(stop_seq - start_seq);
    //cout << "\nTiempo de ejecución secuencial: " << duration_seq.count()/1000 << " milisegundos" << endl;
    double duration_parS=duration_seq.count()/1000000.0;
    cout << "\nTiempo de ejecución secuencial: " << duration_parS << " segundos" << endl;
    //cout << "\nArray después de ordenar secuencialmente: ";
    //printVector(arr);

    // Ordenación paralela con profundidad máxima de 2
    arr = vector<string>(word_count); // Reiniciamos el vector
    for (int i = 0; i < word_count; ++i) {
        int index = rand() % frutas.size();
        arr[i] = frutas[index];
    }
    int prof=10;//EL OPTIMO ES 10
    auto start_par = high_resolution_clock::now();
    parallelSort(arr, prof);
    auto stop_par = high_resolution_clock::now();
    auto duration_par = duration_cast<microseconds>(stop_par - start_par);
    double duration_parP=duration_par.count()/1000000.0;
    cout << "\nTiempo de ejecución paralelo con profundidad máxima " <<prof<<": "<< duration_parP << " segundos" << endl;
    //cout << "\nArray después de ordenar paralelamente con profundidad máxima " <<prof;
    //printVector(arr);
    float speed=duration_parS/duration_parP;
    float efi=speed/8.0;
    cout << "SPEEDUP: " << speed << endl;
    cout<<"\nEFICIENCIA: "<<efi*100<<"%";
    //cout << "\nCantidad de hilos creados: " << thread_count << endl;
    return 0;
}