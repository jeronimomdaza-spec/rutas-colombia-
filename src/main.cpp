/**
 * ============================================================================
 * PROYECTO INTEGRADOR: ESTRUCTURA DE DATOS
 * Proyecto de Aula: "Rutas y Conexiones de Colombia"
 * 
 * Aplicación de estructuras de datos para modelar, explorar y optimizar redes viales.
 * Representación: Grafo ponderado mediante Lista de Adyacencia en C++.
 * Algoritmos implementados:
 *   - BFS (Breadth-First Search / Búsqueda en Anchura): Exploración por niveles y camino con menos saltos.
 *   - DFS (Depth-First Search / Búsqueda en Profundidad): Exploración profunda y verificación de alcanzabilidad.
 *   - Dijkstra: Ruta de costo mínimo con pesos no negativos (usando Cola de Prioridad / Min-Heap).
 * 
 * Metodología ABP: Observar -> Juzgar -> Actuar
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <stack>
#include <set>
#include <limits>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#endif

// ============================================================================
// DEFINICIÓN DE ESTRUCTURAS DE DATOS
// ============================================================================

/**
 * Estructura para representar una conexión (Arista ponderada dirigida/no dirigida)
 * - destino: Ciudad vecina
 * - costo: Valor numérico no negativo (peaje, distancia estimada, tiempo o costo en combustible)
 */
struct Arista {
    std::string destino;
    double costo;

    Arista(std::string dest, double c) : destino(dest), costo(c) {}
};

/**
 * Clase que encapsula el Grafo de Rutas de Colombia.
 * Utiliza una Lista de Adyacencia:
 *   std::map<std::string, std::vector<Arista>>
 * Complejidad espacial: O(V + E), donde V = número de ciudades y E = número de conexiones.
 */
class GrafoRutas {
private:
    std::map<std::string, std::vector<Arista>> adyacencia;

    // Normaliza una cadena (elimina espacios al inicio y final)
    static std::string recortar(const std::string& str) {
        size_t primero = str.find_first_not_of(" \t\r\n");
        if (primero == std::string::npos) return "";
        size_t ultimo = str.find_last_not_of(" \t\r\n");
        return str.substr(primero, (ultimo - primero + 1));
    }

public:
    // Constructor
    GrafoRutas() = default;

    /**
     * Agrega una ciudad (vértice) al grafo.
     * Complejidad: O(log V) por la inserción en el mapa ordenado.
     */
    void agregarCiudad(const std::string& ciudad) {
        std::string c = recortar(ciudad);
        if (!c.empty() && adyacencia.find(c) == adyacencia.end()) {
            adyacencia[c] = std::vector<Arista>();
        }
    }

    /**
     * Verifica si una ciudad existe en el grafo.
     */
    bool existeCiudad(const std::string& ciudad) const {
        return adyacencia.find(recortar(ciudad)) != adyacencia.end();
    }

    /**
     * Agrega una conexión (arista) entre dos ciudades.
     * Permite especificar si es bidireccional (típico en carreteras de doble vía).
     */
    void agregarConexion(const std::string& origen, const std::string& destino, double costo, bool bidireccional = true) {
        std::string orig = recortar(origen);
        std::string dest = recortar(destino);

        if (orig.empty() || dest.empty() || orig == dest || costo < 0) {
            return;
        }

        agregarCiudad(orig);
        agregarCiudad(dest);

        // Evitar duplicar arista directa si ya existe
        for (const auto& arista : adyacencia[orig]) {
            if (arista.destino == dest) return;
        }

        adyacencia[orig].push_back(Arista(dest, costo));

        if (bidireccional) {
            adyacencia[dest].push_back(Arista(orig, costo));
        }
    }

    /**
     * Retorna la lista de todas las ciudades registradas.
     */
    std::vector<std::string> obtenerCiudades() const {
        std::vector<std::string> ciudades;
        for (const auto& par : adyacencia) {
            ciudades.push_back(par.first);
        }
        return ciudades;
    }

    /**
     * Muestra la estructura completa del grafo (Lista de Adyacencia).
     */
    void mostrarGrafo() const {
        std::cout << "\n=======================================================\n";
        std::cout << "         REPRESENTACIÓN DE LA RED (LISTA DE ADYACENCIA) \n";
        std::cout << "=======================================================\n";
        if (adyacencia.empty()) {
            std::cout << " El grafo no tiene ciudades registradas actualmente.\n";
            return;
        }

        int totalAristas = 0;
        for (const auto& par : adyacencia) {
            std::cout << " [" << par.first << "]\n";
            if (par.second.empty()) {
                std::cout << "    └── (Sin conexiones salientes / Ciudad aislada)\n";
            } else {
                for (size_t i = 0; i < par.second.size(); ++i) {
                    bool esUltima = (i == par.second.size() - 1);
                    std::cout << (esUltima ? "    └── " : "    ├── ")
                              << "-> " << std::left << std::setw(15) << par.second[i].destino
                              << " | Costo: " << std::fixed << std::setprecision(1) << par.second[i].costo << "\n";
                    totalAristas++;
                }
            }
        }
        std::cout << "-------------------------------------------------------\n";
        std::cout << " Total de ciudades (Vértices): " << adyacencia.size() << "\n";
        std::cout << " Total de aristas dirigidas:   " << totalAristas << "\n";
        std::cout << "=======================================================\n";
    }

    // ========================================================================
    // ALGORITMO 1: BFS (Breadth-First Search / Búsqueda en Anchura)
    // ========================================================================
    /**
     * Realiza un recorrido completo en anchura desde una ciudad inicial.
     * Estructura utilizada: std::queue (Cola FIFO) y std::set para nodos visitados.
     * Complejidad temporal: O(V + E)
     * Complejidad espacial: O(V)
     * 
     * ¿Qué responde? ¿Cómo explorar la red nivel a nivel? ¿Cuáles nodos están a k saltos?
     */
    std::vector<std::string> recorridoBFS(const std::string& inicio) const {
        std::string origen = recortar(inicio);
        std::vector<std::string> ordenVisita;
        if (!existeCiudad(origen)) return ordenVisita;

        std::set<std::string> visitados;
        std::queue<std::string> cola;

        cola.push(origen);
        visitados.insert(origen);

        while (!cola.empty()) {
            std::string actual = cola.front();
            cola.pop();
            ordenVisita.push_back(actual);

            auto it = adyacencia.find(actual);
            if (it != adyacencia.end()) {
                for (const auto& arista : it->second) {
                    if (visitados.find(arista.destino) == visitados.end()) {
                        visitados.insert(arista.destino);
                        cola.push(arista.destino);
                    }
                }
            }
        }

        return ordenVisita;
    }

    /**
     * Encuentra la ruta con el menor número de saltos (aristas) usando BFS.
     * Retorna: {camino de ciudades, costo total acumulado en ese camino}
     */
    std::pair<std::vector<std::string>, double> rutaMenosSaltosBFS(const std::string& origen, const std::string& destino) const {
        std::string orig = recortar(origen);
        std::string dest = recortar(destino);
        std::vector<std::string> camino;

        if (!existeCiudad(orig) || !existeCiudad(dest)) {
            return {camino, -1.0};
        }

        if (orig == dest) {
            camino.push_back(orig);
            return {camino, 0.0};
        }

        std::set<std::string> visitados;
        std::queue<std::string> cola;
        std::map<std::string, std::string> predecesor;

        cola.push(orig);
        visitados.insert(orig);

        bool alcanzado = false;
        while (!cola.empty()) {
            std::string actual = cola.front();
            cola.pop();

            if (actual == dest) {
                alcanzado = true;
                break;
            }

            auto it = adyacencia.find(actual);
            if (it != adyacencia.end()) {
                for (const auto& arista : it->second) {
                    if (visitados.find(arista.destino) == visitados.end()) {
                        visitados.insert(arista.destino);
                        predecesor[arista.destino] = actual;
                        cola.push(arista.destino);
                    }
                }
            }
        }

        if (!alcanzado) {
            return {camino, -1.0}; // No hay ruta
        }

        // Reconstrucción del camino
        std::string paso = dest;
        while (paso != orig) {
            camino.push_back(paso);
            paso = predecesor[paso];
        }
        camino.push_back(orig);
        std::reverse(camino.begin(), camino.end());

        // Calcular costo del camino obtenido por BFS
        double costoTotal = 0.0;
        for (size_t i = 0; i + 1 < camino.size(); ++i) {
            std::string u = camino[i];
            std::string v = camino[i + 1];
            for (const auto& arista : adyacencia.at(u)) {
                if (arista.destino == v) {
                    costoTotal += arista.costo;
                    break;
                }
            }
        }

        return {camino, costoTotal};
    }

    // ========================================================================
    // ALGORITMO 2: DFS (Depth-First Search / Búsqueda en Profundidad)
    // ========================================================================
    /**
     * Realiza un recorrido en profundidad desde una ciudad inicial.
     * Estructura utilizada: std::stack (Pila LIFO) / Emulación iterativa.
     * Complejidad temporal: O(V + E)
     * Complejidad espacial: O(V)
     * 
     * ¿Qué responde? ¿Hasta dónde puedo avanzar explorando una rama hasta el fondo?
     * No garantiza camino de costo mínimo ni de menor cantidad de saltos.
     */
    std::vector<std::string> recorridoDFS(const std::string& inicio) const {
        std::string origen = recortar(inicio);
        std::vector<std::string> ordenVisita;
        if (!existeCiudad(origen)) return ordenVisita;

        std::set<std::string> visitados;
        std::stack<std::string> pila;

        pila.push(origen);

        while (!pila.empty()) {
            std::string actual = pila.top();
            pila.pop();

            if (visitados.find(actual) == visitados.end()) {
                visitados.insert(actual);
                ordenVisita.push_back(actual);

                auto it = adyacencia.find(actual);
                if (it != adyacencia.end()) {
                    // Para recorrer en orden predecible se inserta en reversa
                    for (auto rit = it->second.rbegin(); rit != it->second.rend(); ++rit) {
                        if (visitados.find(rit->destino) == visitados.end()) {
                            pila.push(rit->destino);
                        }
                    }
                }
            }
        }

        return ordenVisita;
    }

    // ========================================================================
    // ALGORITMO 3: DIJKSTRA (Ruta de Costo Mínimo con Pesos No Negativos)
    // ========================================================================
    /**
     * Encuentra el camino de menor costo ponderado entre origen y destino.
     * Estructura: std::priority_queue (Min-Heap de pares {distancia, ciudad}).
     * Complejidad temporal: O((V + E) log V)
     * Complejidad espacial: O(V)
     * 
     * ¿Qué responde? ¿Cuál es la ruta más económica o rápida considerando los pesos?
     */
    std::pair<std::vector<std::string>, double> dijkstra(const std::string& origen, const std::string& destino) const {
        std::string orig = recortar(origen);
        std::string dest = recortar(destino);
        std::vector<std::string> camino;

        if (!existeCiudad(orig) || !existeCiudad(dest)) {
            return {camino, -1.0};
        }

        if (orig == dest) {
            camino.push_back(orig);
            return {camino, 0.0};
        }

        const double INFINITO = std::numeric_limits<double>::infinity();
        std::map<std::string, double> distancias;
        std::map<std::string, std::string> predecesor;

        for (const auto& par : adyacencia) {
            distancias[par.first] = INFINITO;
        }
        distancias[orig] = 0.0;

        // Min-Heap de pares <distancia, ciudad>
        typedef std::pair<double, std::string> ElementoCola;
        std::priority_queue<ElementoCola, std::vector<ElementoCola>, std::greater<ElementoCola>> minHeap;

        minHeap.push({0.0, orig});

        while (!minHeap.empty()) {
            double distActual = minHeap.top().first;
            std::string u = minHeap.top().second;
            minHeap.pop();

            // Si encontramos el destino, podemos terminar de inmediato la búsqueda
            if (u == dest) break;

            // Si la distancia extraída es mayor a la ya registrada, se descarta (nodo desactualizado)
            if (distActual > distancias[u]) continue;

            auto it = adyacencia.find(u);
            if (it != adyacencia.end()) {
                for (const auto& arista : it->second) {
                    std::string v = arista.destino;
                    double peso = arista.costo;

                    // Relajación de la arista (u, v)
                    if (distancias[u] + peso < distancias[v]) {
                        distancias[v] = distancias[u] + peso;
                        predecesor[v] = u;
                        minHeap.push({distancias[v], v});
                    }
                }
            }
        }

        // Si la distancia al destino sigue siendo infinita, no existe conexión
        if (distancias[dest] == INFINITO) {
            return {camino, -1.0};
        }

        // Reconstrucción del camino óptimo
        std::string paso = dest;
        while (paso != orig) {
            camino.push_back(paso);
            paso = predecesor[paso];
        }
        camino.push_back(orig);
        std::reverse(camino.begin(), camino.end());

        return {camino, distancias[dest]};
    }

    // ========================================================================
    // CONSULTA DE CONECTIVIDAD: ¿Existe una ruta?
    // ========================================================================
    /**
     * Determina si existe algún camino entre dos ciudades usando BFS.
     * Complejidad: O(V + E)
     */
    bool existeRuta(const std::string& origen, const std::string& destino) const {
        std::string orig = recortar(origen);
        std::string dest = recortar(destino);
        if (!existeCiudad(orig) || !existeCiudad(dest)) return false;
        if (orig == dest) return true;

        std::vector<std::string> visitados = recorridoBFS(orig);
        for (const auto& c : visitados) {
            if (c == dest) return true;
        }
        return false;
    }

    // ========================================================================
    // COMPARATIVA EDUCATIVA: Menos Saltos (BFS) vs Menor Costo (Dijkstra)
    // Ejemplo de la Diapositiva 05: Bogotá -> Villavicencio (90) vs Bogotá -> Ibagué -> Villavicencio (45)
    // ========================================================================
    void compararBFSvsDijkstra(const std::string& origen, const std::string& destino) const {
        std::cout << "\n=======================================================\n";
        std::cout << "  COMPARACIÓN CONCEPTUAL: MENOS SALTOS vs MENOR COSTO   \n";
        std::cout << "  Origen: " << origen << "  --->  Destino: " << destino << "\n";
        std::cout << "=======================================================\n";

        if (!existeCiudad(origen) || !existeCiudad(destino)) {
            std::cout << " [Error] Una o ambas ciudades no existen en la red.\n";
            return;
        }

        auto resBFS = rutaMenosSaltosBFS(origen, destino);
        auto resDijkstra = dijkstra(origen, destino);

        if (resBFS.second < 0 || resDijkstra.second < 0) {
            std::cout << " [Aviso] No existe ninguna ruta entre " << origen << " y " << destino << ".\n";
            return;
        }

        std::cout << "\n [1] RESULTADO POR BFS (Menor cantidad de saltos / aristas):\n";
        std::cout << "     Ruta: ";
        for (size_t i = 0; i < resBFS.first.size(); ++i) {
            std::cout << resBFS.first[i] << (i + 1 < resBFS.first.size() ? " -> " : "");
        }
        std::cout << "\n     Saltos (aristas): " << (resBFS.first.size() - 1) << "\n";
        std::cout << "     Costo total:      " << std::fixed << std::setprecision(1) << resBFS.second << "\n";

        std::cout << "\n [2] RESULTADO POR DIJKSTRA (Menor suma ponderada de pesos):\n";
        std::cout << "     Ruta óptima: ";
        for (size_t i = 0; i < resDijkstra.first.size(); ++i) {
            std::cout << resDijkstra.first[i] << (i + 1 < resDijkstra.first.size() ? " -> " : "");
        }
        std::cout << "\n     Saltos (aristas): " << (resDijkstra.first.size() - 1) << "\n";
        std::cout << "     Costo total:      " << std::fixed << std::setprecision(1) << resDijkstra.second << "\n";

        std::cout << "\n [3] CONCLUSIÓN TEÓRICA:\n";
        if (resBFS.first == resDijkstra.first) {
            std::cout << "     En este par de ciudades, la ruta con menos saltos COINCIDE con la de menor costo.\n";
        } else {
            std::cout << "     ¡Se comprueba la premisa: 'Menos saltos != Menor costo'!\n";
            std::cout << "     BFS eligió un trayecto con " << (resBFS.first.size() - 1) << " salto(s) pero costo " << resBFS.second << ".\n";
            std::cout << "     Dijkstra encontró una ruta más económica con costo " << resDijkstra.second
                      << " utilizando " << (resDijkstra.first.size() - 1) << " saltos.\n";
        }
        std::cout << "=======================================================\n";
    }

    // ========================================================================
    // CARGA DE DATOS DESDE ARCHIVO CSV
    // ========================================================================
    bool cargarDesdeCSV(const std::string& rutaArchivo) {
        std::ifstream archivo(rutaArchivo);
        if (!archivo.is_open()) {
            return false;
        }

        std::string linea;
        bool primeraLinea = true;
        int conexionesLeidas = 0;

        while (std::getline(archivo, linea)) {
            linea = recortar(linea);
            if (linea.empty() || linea[0] == '#') continue;

            // Saltar cabecera si existe ("origen,destino,costo")
            if (primeraLinea) {
                primeraLinea = false;
                std::string encabezado = linea;
                std::transform(encabezado.begin(), encabezado.end(), encabezado.begin(), ::tolower);
                if (encabezado.find("origen") != std::string::npos) {
                    continue;
                }
            }

            std::stringstream ss(linea);
            std::string origen, destino, costoStr;

            if (std::getline(ss, origen, ',') &&
                std::getline(ss, destino, ',') &&
                std::getline(ss, costoStr, ',')) {

                origen = recortar(origen);
                destino = recortar(destino);
                try {
                    double costo = std::stod(recortar(costoStr));
                    agregarConexion(origen, destino, costo, true);
                    conexionesLeidas++;
                } catch (...) {
                    // Ignorar línea con costo inválido
                }
            }
        }

        archivo.close();
        return (conexionesLeidas > 0);
    }

    // ========================================================================
    // INICIALIZACIÓN CON DATOS DE EJEMPLO DE COLOMBIA
    // Cumple con el alcance mínimo: >= 6 ciudades y >= 8 conexiones
    // ========================================================================
    void cargarRedEjemploColombia() {
        adyacencia.clear();

        // Conexiones de ejemplo (incluye el caso de diapositiva 05)
        // Bogotá - Villavicencio directo = 90
        // Bogotá - Ibagué = 20
        // Ibagué - Villavicencio = 25  (Suma = 45 vs 90)
        agregarConexion("Bogota", "Villavicencio", 90.0);
        agregarConexion("Bogota", "Ibague", 20.0);
        agregarConexion("Ibague", "Villavicencio", 25.0);

        // Troncal Occidental y Eje Cafetero
        agregarConexion("Bogota", "Medellin", 50.0);
        agregarConexion("Medellin", "Pereira", 30.0);
        agregarConexion("Pereira", "Manizales", 15.0);
        agregarConexion("Manizales", "Ibague", 25.0);
        agregarConexion("Bogota", "Cali", 60.0);
        agregarConexion("Cali", "Ibague", 40.0);

        // Troncal Nororiente y Caribe
        agregarConexion("Bogota", "Bucaramanga", 70.0);
        agregarConexion("Bucaramanga", "Medellin", 65.0);
        agregarConexion("Bucaramanga", "Barranquilla", 80.0);
        agregarConexion("Medellin", "Barranquilla", 110.0);

        // Componente desconectada (para probar casos límite donde no existe ruta)
        agregarConexion("Cartagena", "Santa Marta", 30.0);
    }
};

// ============================================================================
// FUNCIONES AUXILIARES DE ENTRADA Y FORMATEO
// ============================================================================

void configurarConsola() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

std::string leerLinea(const std::string& mensaje) {
    std::cout << mensaje;
    std::string entrada;
    std::getline(std::cin, entrada);
    // Eliminar espacios al inicio y final
    size_t primero = entrada.find_first_not_of(" \t\r\n");
    if (primero == std::string::npos) return "";
    size_t ultimo = entrada.find_last_not_of(" \t\r\n");
    return entrada.substr(primero, (ultimo - primero + 1));
}

void pausarConsola() {
    std::cout << "\nPresione ENTER para continuar...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

// ============================================================================
// MENÚ PRINCIPAL Y CONTROLADOR DE LA APLICACIÓN
// ============================================================================

int main() {
    configurarConsola();
    GrafoRutas red;

    // Carga de red inicial por defecto
    // Intentar leer desde archivo CSV o usar la precargada
    if (!red.cargarDesdeCSV("rutas.csv")) {
        red.cargarRedEjemploColombia();
    }

    int opcion = -1;

    do {
        std::cout << "\n";
        std::cout << "=======================================================\n";
        std::cout << "       PROYECTO INTEGRADOR - ESTRUCTURA DE DATOS        \n";
        std::cout << "            RUTAS Y CONEXIONES DE COLOMBIA             \n";
        std::cout << "=======================================================\n";
        std::cout << " 1. Mostrar ciudades y conexiones (Lista de Adyacencia)\n";
        std::cout << " 2. Recorrido BFS (Búsqueda en Anchura / Exploración)\n";
        std::cout << " 3. Recorrido DFS (Búsqueda en Profundidad)\n";
        std::cout << " 4. Ruta de costo mínimo (Dijkstra)\n";
        std::cout << " 5. Consultar si existe una ruta (Alcanzabilidad)\n";
        std::cout << " 6. Comparar BFS vs Dijkstra (Menos saltos != Menor costo)\n";
        std::cout << " 7. Agregar nueva conexión / ciudad a la red\n";
        std::cout << " 0. Salir\n";
        std::cout << "=======================================================\n";
        std::cout << "Seleccione una opción: ";

        std::string entradaOpcion;
        std::getline(std::cin, entradaOpcion);

        try {
            opcion = std::stoi(entradaOpcion);
        } catch (...) {
            opcion = -1;
        }

        switch (opcion) {
            case 1: {
                red.mostrarGrafo();
                pausarConsola();
                break;
            }

            case 2: {
                std::cout << "\n--- RECORRIDO BFS (Búsqueda en Anchura) ---\n";
                std::cout << "Explora la red por niveles desde la ciudad de origen.\n";
                std::string origen = leerLinea("Ingrese la ciudad de origen: ");

                if (!red.existeCiudad(origen)) {
                    std::cout << " [Error] La ciudad '" << origen << "' no está registrada en el grafo.\n";
                } else {
                    std::vector<std::string> recorrido = red.recorridoBFS(origen);
                    std::cout << "\nOrden de exploración BFS (" << recorrido.size() << " ciudades alcanzables):\n";
                    for (size_t i = 0; i < recorrido.size(); ++i) {
                        std::cout << "  " << (i + 1) << ". " << recorrido[i] << "\n";
                    }
                }
                pausarConsola();
                break;
            }

            case 3: {
                std::cout << "\n--- RECORRIDO DFS (Búsqueda en Profundidad) ---\n";
                std::cout << "Explora la red avanzando lo más profundo posible por cada rama.\n";
                std::string origen = leerLinea("Ingrese la ciudad de origen: ");

                if (!red.existeCiudad(origen)) {
                    std::cout << " [Error] La ciudad '" << origen << "' no está registrada en el grafo.\n";
                } else {
                    std::vector<std::string> recorrido = red.recorridoDFS(origen);
                    std::cout << "\nOrden de exploración DFS (" << recorrido.size() << " ciudades alcanzables):\n";
                    for (size_t i = 0; i < recorrido.size(); ++i) {
                        std::cout << "  " << (i + 1) << ". " << recorrido[i] << "\n";
                    }
                }
                pausarConsola();
                break;
            }

            case 4: {
                std::cout << "\n--- RUTA DE COSTO MÍNIMO (ALGORITMO DE DIJKSTRA) ---\n";
                std::cout << "Calcula la ruta óptima minimizando la suma ponderada de costos.\n";
                std::string origen = leerLinea("Ingrese la ciudad de ORIGEN: ");
                std::string destino = leerLinea("Ingrese la ciudad de DESTINO: ");

                if (!red.existeCiudad(origen) || !red.existeCiudad(destino)) {
                    std::cout << " [Error] Verifique los nombres: una o ambas ciudades no existen.\n";
                } else {
                    auto resultado = red.dijkstra(origen, destino);
                    if (resultado.second < 0) {
                        std::cout << "\n [Resultado] NO EXISTE NINGUNA RUTA viable entre " 
                                  << origen << " y " << destino << ".\n";
                        std::cout << " (Las ciudades pertenecen a componentes desconectadas del grafo).\n";
                    } else {
                        std::cout << "\n=======================================================\n";
                        std::cout << "  RUTA ÓPTIMA ENCONTRADA:\n";
                        std::cout << "  ";
                        for (size_t i = 0; i < resultado.first.size(); ++i) {
                            std::cout << resultado.first[i] << (i + 1 < resultado.first.size() ? "  ──>  " : "");
                        }
                        std::cout << "\n-------------------------------------------------------\n";
                        std::cout << "  Cantidad de tramos (aristas): " << (resultado.first.size() - 1) << "\n";
                        std::cout << "  Costo total acumulado:        " << std::fixed << std::setprecision(2) 
                                  << resultado.second << " unidades\n";
                        std::cout << "=======================================================\n";
                    }
                }
                pausarConsola();
                break;
            }

            case 5: {
                std::cout << "\n--- CONSULTA DE CONECTIVIDAD (¿SE PUEDE LLEGAR?) ---\n";
                std::string origen = leerLinea("Ingrese la ciudad de ORIGEN: ");
                std::string destino = leerLinea("Ingrese la ciudad de DESTINO: ");

                if (!red.existeCiudad(origen) || !red.existeCiudad(destino)) {
                    std::cout << " [Error] Una o ambas ciudades no están en el sistema.\n";
                } else {
                    bool existe = red.existeRuta(origen, destino);
                    if (existe) {
                        std::cout << "\n [SÍ SE PUEDE LLEGAR]: Existe conexión entre " 
                                  << origen << " y " << destino << ".\n";
                    } else {
                        std::cout << "\n [NO SE PUEDE LLEGAR]: No existe camino entre " 
                                  << origen << " y " << destino << ".\n";
                    }
                }
                pausarConsola();
                break;
            }

            case 6: {
                std::cout << "\n--- DEMOSTRACIÓN CONCEPTUAL (DIAPOSITIVA 05) ---\n";
                std::cout << "Compara la ruta con menos saltos (BFS) vs la ruta con menor costo (Dijkstra).\n";
                std::cout << "Tip: Pruebe con Origen = 'Bogota' y Destino = 'Villavicencio'.\n";
                std::string origen = leerLinea("Ingrese ciudad ORIGEN [Enter para 'Bogota']: ");
                if (origen.empty()) origen = "Bogota";

                std::string destino = leerLinea("Ingrese ciudad DESTINO [Enter para 'Villavicencio']: ");
                if (destino.empty()) destino = "Villavicencio";

                red.compararBFSvsDijkstra(origen, destino);
                pausarConsola();
                break;
            }

            case 7: {
                std::cout << "\n--- AGREGAR NUEVA CONEXIÓN A LA RED ---\n";
                std::string orig = leerLinea("Ciudad Origen: ");
                std::string dest = leerLinea("Ciudad Destino: ");
                std::string costoStr = leerLinea("Costo/Distancia (número positivo): ");

                try {
                    double costo = std::stod(costoStr);
                    if (costo < 0) {
                        std::cout << " [Error] El costo debe ser un valor no negativo.\n";
                    } else {
                        red.agregarConexion(orig, dest, costo, true);
                        std::cout << " [Éxito] Conexión bidireccional agregada entre " 
                                  << orig << " y " << dest << " con costo " << costo << ".\n";
                    }
                } catch (...) {
                    std::cout << " [Error] Formato de costo inválido.\n";
                }
                pausarConsola();
                break;
            }

            case 0: {
                std::cout << "\nGracias por utilizar el Sistema de Rutas y Conexiones de Colombia.\n";
                std::cout << "Proyecto de Aula - Estructura de Datos (2026).\n\n";
                break;
            }

            default: {
                std::cout << "\n [Opción inválida] Por favor ingrese un número del menú.\n";
                pausarConsola();
                break;
            }
        }

    } while (opcion != 0);

    return 0;
}
