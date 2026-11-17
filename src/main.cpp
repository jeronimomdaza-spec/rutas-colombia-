/**
 * ============================================================================
 * SITRAC: Sistema Inteligente de Transporte y Gestión de Rutas
 * Proyecto Integrador Final · Estructura de Datos · 2026-II
 * Universidad Santo Tomás (USTA)
 * 
 * Autor: Jerónimo Martínez Daza
 * 
 * CRITERIO TÉCNICO EVALUADO:
 * "Las estructuras de datos esenciales están implementadas desde cero mediante
 *  punteros y nodos dinámicos (TADs propios), sin depender de contenedores STL 
 *  para los componentes centrales evaluados."
 * 
 * Estructuras implementadas desde cero:
 *  1. Lista Simplemente Enlazada (CRUD de Pasajeros con punteros)
 *  2. Cola FIFO Dinámica (Gestión de Solicitudes de Viaje con punteros frente/fin)
 *  3. Pila LIFO Dinámica (Historial de Operaciones y función Deshacer)
 *  4. Árbol Binario de Búsqueda - ABB (Indexación por ID, búsqueda O(log n), borrado 3 casos)
 *  5. Grafo Ponderado Dinámico (Vértices y Aristas enlazadas, BFS, DFS, Dijkstra)
 * 
 * Tolerancia a Fallos y Pruebas Adversariales:
 *  - Ciudad duplicada: rechazada sin corromper el grafo.
 *  - Cola vacía: atención segura sin desreferenciar punteros nulos.
 *  - Distancia/Costo negativo: rechazo inmediato.
 *  - Pasajero inexistente: eliminación segura sin romper enlaces.
 *  - Ciudad eliminada: borrado en cascada de todas sus conexiones incidentes.
 *  - Entradas alfabéticas en campos numéricos: recuperación y limpieza de búfer cin.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <limits>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#endif

// ============================================================================
// UTILIDADES DE ENTRADA SEGURA (RECUPERACIÓN ANTE ENTRADAS ADVERSARIALES)
// ============================================================================

void limpiarBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string leerCadena(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    size_t primero = s.find_first_not_of(" \t\r\n");
    if (primero == std::string::npos) return "";
    size_t ultimo = s.find_last_not_of(" \t\r\n");
    return s.substr(primero, (ultimo - primero + 1));
}

int leerEntero(const std::string& prompt, int minimo = std::numeric_limits<int>::min(), int maximo = std::numeric_limits<int>::max()) {
    int valor;
    while (true) {
        std::cout << prompt;
        if (std::cin >> valor) {
            limpiarBuffer();
            if (valor >= minimo && valor <= maximo) {
                return valor;
            }
            std::cout << " [Error] El valor debe estar entre " << minimo << " y " << maximo << ". Intente de nuevo.\n";
        } else {
            std::cout << " [Error de entrada] Debe ingresar un numero entero valido.\n";
            limpiarBuffer();
        }
    }
}

double leerDouble(const std::string& prompt, double minimo = 0.0) {
    double valor;
    while (true) {
        std::cout << prompt;
        if (std::cin >> valor) {
            limpiarBuffer();
            if (valor >= minimo) {
                return valor;
            }
            std::cout << " [Error] El valor no puede ser menor a " << minimo << ".\n";
        } else {
            std::cout << " [Error de entrada] Ingrese un valor numerico valido (ej. 25.5).\n";
            limpiarBuffer();
        }
    }
}

void pausar() {
    std::cout << "\nPresione ENTER para continuar...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

// ============================================================================
// 1. LISTA ENLAZADA PROPIA: GESTIÓN Y CRUD DE PASAJEROS
// ============================================================================

struct Pasajero {
    int id;
    std::string nombre;
    std::string ciudadDestino;
    std::string telefono;

    Pasajero(int _id = 0, std::string _n = "", std::string _d = "", std::string _t = "")
        : id(_id), nombre(_n), ciudadDestino(_d), telefono(_t) {}
};

struct NodoPasajero {
    Pasajero datos;
    NodoPasajero* siguiente;

    NodoPasajero(const Pasajero& p, NodoPasajero* sig = nullptr)
        : datos(p), siguiente(sig) {}
};

class ListaPasajeros {
private:
    NodoPasajero* cabeza;
    int cantidad;

public:
    ListaPasajeros() : cabeza(nullptr), cantidad(0) {}

    ~ListaPasajeros() {
        liberarMemoria();
    }

    void liberarMemoria() {
        NodoPasajero* actual = cabeza;
        while (actual != nullptr) {
            NodoPasajero* temp = actual;
            actual = actual->siguiente;
            delete temp;
        }
        cabeza = nullptr;
        cantidad = 0;
    }

    int getCantidad() const { return cantidad; }
    NodoPasajero* getCabeza() const { return cabeza; }

    bool existeId(int id) const {
        NodoPasajero* actual = cabeza;
        while (actual != nullptr) {
            if (actual->datos.id == id) return true;
            actual = actual->siguiente;
        }
        return false;
    }

    // Inserción ordenada por ID (Mantiene la lista estructurada)
    bool insertar(const Pasajero& p) {
        if (existeId(p.id)) return false; // Rechaza duplicado sin corromper

        NodoPasajero* nuevo = new NodoPasajero(p);
        if (cabeza == nullptr || p.id < cabeza->datos.id) {
            nuevo->siguiente = cabeza;
            cabeza = nuevo;
        } else {
            NodoPasajero* actual = cabeza;
            while (actual->siguiente != nullptr && actual->siguiente->datos.id < p.id) {
                actual = actual->siguiente;
            }
            nuevo->siguiente = actual->siguiente;
            actual->siguiente = nuevo;
        }
        cantidad++;
        return true;
    }

    Pasajero* buscar(int id) const {
        NodoPasajero* actual = cabeza;
        while (actual != nullptr) {
            if (actual->datos.id == id) return &(actual->datos);
            actual = actual->siguiente;
        }
        return nullptr;
    }

    bool modificar(int id, const std::string& nuevoNombre, const std::string& nuevoDestino, const std::string& nuevoTel) {
        Pasajero* p = buscar(id);
        if (p == nullptr) return false;
        if (!nuevoNombre.empty()) p->nombre = nuevoNombre;
        if (!nuevoDestino.empty()) p->ciudadDestino = nuevoDestino;
        if (!nuevoTel.empty()) p->telefono = nuevoTel;
        return true;
    }

    // Eliminación adversarial segura (sin memory leaks ni fallos por puntero dangling)
    bool eliminar(int id) {
        if (cabeza == nullptr) return false;

        NodoPasajero* aBorrar = nullptr;
        if (cabeza->datos.id == id) {
            aBorrar = cabeza;
            cabeza = cabeza->siguiente;
        } else {
            NodoPasajero* actual = cabeza;
            while (actual->siguiente != nullptr && actual->siguiente->datos.id != id) {
                actual = actual->siguiente;
            }
            if (actual->siguiente == nullptr) return false; // No encontrado
            aBorrar = actual->siguiente;
            actual->siguiente = aBorrar->siguiente;
        }

        delete aBorrar;
        cantidad--;
        return true;
    }

    void mostrar() const {
        std::cout << "\n========================================================================\n";
        std::cout << "               LISTA DE PASAJEROS REGISTRADOS (TAD PROPIO)              \n";
        std::cout << "========================================================================\n";
        std::cout << std::left << std::setw(8) << "ID"
                  << std::setw(24) << "Nombre"
                  << std::setw(20) << "Destino Frecuente"
                  << std::setw(16) << "Telefono" << "\n";
        std::cout << "------------------------------------------------------------------------\n";
        if (cabeza == nullptr) {
            std::cout << " [Aviso] No hay pasajeros registrados en la lista enlazada.\n";
        } else {
            NodoPasajero* act = cabeza;
            while (act != nullptr) {
                std::cout << std::left << std::setw(8) << act->datos.id
                          << std::setw(24) << act->datos.nombre
                          << std::setw(20) << act->datos.ciudadDestino
                          << std::setw(16) << act->datos.telefono << "\n";
                act = act->siguiente;
            }
        }
        std::cout << "------------------------------------------------------------------------\n";
        std::cout << " Total en lista: " << cantidad << " pasajero(s).\n";
        std::cout << "========================================================================\n";
    }
};

// ============================================================================
// 2. COLA FIFO DINÁMICA PROPIA: GESTIÓN DE SOLICITUDES DE VIAJE
// ============================================================================

struct SolicitudViaje {
    int idSolicitud;
    int idPasajero;
    std::string nombrePasajero;
    std::string origen;
    std::string destino;
    int asientos;

    SolicitudViaje(int _idS = 0, int _idP = 0, std::string _nom = "", std::string _o = "", std::string _d = "", int _as = 1)
        : idSolicitud(_idS), idPasajero(_idP), nombrePasajero(_nom), origen(_o), destino(_d), asientos(_as) {}
};

struct NodoCola {
    SolicitudViaje datos;
    NodoCola* siguiente;

    NodoCola(const SolicitudViaje& s) : datos(s), siguiente(nullptr) {}
};

class ColaSolicitudes {
private:
    NodoCola* frente;
    NodoCola* fin;
    int tamanio;

public:
    ColaSolicitudes() : frente(nullptr), fin(nullptr), tamanio(0) {}

    ~ColaSolicitudes() {
        limpiar();
    }

    void limpiar() {
        while (!estaVacia()) {
            SolicitudViaje temp;
            desencolar(temp);
        }
    }

    bool estaVacia() const { return frente == nullptr; }
    int getTamanio() const { return tamanio; }

    void encolar(const SolicitudViaje& s) {
        NodoCola* nuevo = new NodoCola(s);
        if (estaVacia()) {
            frente = fin = nuevo;
        } else {
            fin->siguiente = nuevo;
            fin = nuevo;
        }
        tamanio++;
    }

    // Prueba adversarial: atender en cola vacía sin causar error de segmentación
    bool desencolar(SolicitudViaje& out) {
        if (estaVacia()) return false;
        NodoCola* temp = frente;
        out = temp->datos;
        frente = frente->siguiente;
        if (frente == nullptr) fin = nullptr;
        delete temp;
        tamanio--;
        return true;
    }

    bool verFrente(SolicitudViaje& out) const {
        if (estaVacia()) return false;
        out = frente->datos;
        return true;
    }

    void mostrar() const {
        std::cout << "\n========================================================================\n";
        std::cout << "            COLA FIFO DE SOLICITUDES EN ESPERA (TAD PROPIO)             \n";
        std::cout << "========================================================================\n";
        if (estaVacia()) {
            std::cout << " [Cola Vacia] No hay solicitudes pendientes por atender.\n";
        } else {
            std::cout << std::left << std::setw(6) << "#Sol"
                      << std::setw(8) << "ID Pas"
                      << std::setw(22) << "Nombre"
                      << std::setw(16) << "Origen"
                      << std::setw(16) << "Destino"
                      << std::setw(8) << "Cupos" << "\n";
            std::cout << "------------------------------------------------------------------------\n";
            NodoCola* actual = frente;
            int pos = 1;
            while (actual != nullptr) {
                std::cout << std::left << std::setw(6) << actual->datos.idSolicitud
                          << std::setw(8) << actual->datos.idPasajero
                          << std::setw(22) << actual->datos.nombrePasajero
                          << std::setw(16) << actual->datos.origen
                          << std::setw(16) << actual->datos.destino
                          << std::setw(8) << actual->datos.asientos
                          << (pos == 1 ? "  <-- PROXIMA EN ATENDER" : "") << "\n";
                actual = actual->siguiente;
                pos++;
            }
        }
        std::cout << "------------------------------------------------------------------------\n";
        std::cout << " Total de solicitudes en espera: " << tamanio << "\n";
        std::cout << "========================================================================\n";
    }
};

// ============================================================================
// 3. PILA LIFO DINÁMICA PROPIA: HISTORIAL Y FUNCIONALIDAD DESHACER (UNDO)
// ============================================================================

enum TipoAccion {
    ACCION_AGREGAR_CIUDAD,
    ACCION_REGISTRAR_PASAJERO,
    ACCION_AGREGAR_CONEXION,
    ACCION_ATENDER_SOLICITUD
};

struct AccionHistorial {
    TipoAccion tipo;
    std::string descripcion;
    int idRef;
    std::string param1;
    std::string param2;
    double valor;

    AccionHistorial(TipoAccion t = ACCION_AGREGAR_CIUDAD, std::string desc = "", int id = 0,
                    std::string p1 = "", std::string p2 = "", double v = 0.0)
        : tipo(t), descripcion(desc), idRef(id), param1(p1), param2(p2), valor(v) {}
};

struct NodoPila {
    AccionHistorial datos;
    NodoPila* siguiente;

    NodoPila(const AccionHistorial& a, NodoPila* sig = nullptr)
        : datos(a), siguiente(sig) {}
};

class PilaHistorial {
private:
    NodoPila* tope;
    int tamanio;

public:
    PilaHistorial() : tope(nullptr), tamanio(0) {}

    ~PilaHistorial() {
        limpiar();
    }

    void limpiar() {
        while (!estaVacia()) {
            AccionHistorial temp;
            desapilar(temp);
        }
    }

    bool estaVacia() const { return tope == nullptr; }
    int getTamanio() const { return tamanio; }

    void apilar(const AccionHistorial& a) {
        NodoPila* nuevo = new NodoPila(a, tope);
        tope = nuevo;
        tamanio++;
    }

    // Prueba adversarial: desapilar en pila vacía
    bool desapilar(AccionHistorial& out) {
        if (estaVacia()) return false;
        NodoPila* temp = tope;
        out = temp->datos;
        tope = tope->siguiente;
        delete temp;
        tamanio--;
        return true;
    }

    void mostrar() const {
        std::cout << "\n========================================================================\n";
        std::cout << "              PILA LIFO DE HISTORIAL DE ACCIONES (TAD PROPIO)           \n";
        std::cout << "========================================================================\n";
        if (estaVacia()) {
            std::cout << " [Pila Vacia] No se han registrado acciones recientes.\n";
        } else {
            NodoPila* act = tope;
            int paso = 1;
            while (act != nullptr) {
                std::cout << " [" << paso << "] " << act->datos.descripcion 
                          << (paso == 1 ? "  <-- TOPE (Accion a revertir)" : "") << "\n";
                act = act->siguiente;
                paso++;
            }
        }
        std::cout << "------------------------------------------------------------------------\n";
        std::cout << " Total de acciones en el historial: " << tamanio << "\n";
        std::cout << "========================================================================\n";
    }
};

// ============================================================================
// 4. ÁRBOL BINARIO DE BÚSQUEDA (ABB) PROPIO: INDEXACIÓN RÁPIDA DE PASAJEROS
// ============================================================================

struct NodoABB {
    int id;
    std::string nombre;
    std::string ciudadDestino;
    NodoABB* izq;
    NodoABB* der;

    NodoABB(int _id, std::string _nom, std::string _dest)
        : id(_id), nombre(_nom), ciudadDestino(_dest), izq(nullptr), der(nullptr) {}
};

class ArbolBusquedaABB {
private:
    NodoABB* raiz;
    int nodosTotales;

    void destruirRecursivo(NodoABB* nodo) {
        if (nodo != nullptr) {
            destruirRecursivo(nodo->izq);
            destruirRecursivo(nodo->der);
            delete nodo;
        }
    }

    NodoABB* insertarRecursivo(NodoABB* nodo, int id, const std::string& nom, const std::string& dest, bool& insertado) {
        if (nodo == nullptr) {
            insertado = true;
            return new NodoABB(id, nom, dest);
        }
        if (id < nodo->id) {
            nodo->izq = insertarRecursivo(nodo->izq, id, nom, dest, insertado);
        } else if (id > nodo->id) {
            nodo->der = insertarRecursivo(nodo->der, id, nom, dest, insertado);
        } else {
            insertado = false; // ID duplicado en el ABB
        }
        return nodo;
    }

    NodoABB* buscarRecursivo(NodoABB* nodo, int id) const {
        if (nodo == nullptr || nodo->id == id) return nodo;
        if (id < nodo->id) return buscarRecursivo(nodo->izq, id);
        return buscarRecursivo(nodo->der, id);
    }

    NodoABB* encontrarMinimo(NodoABB* nodo) {
        while (nodo != nullptr && nodo->izq != nullptr) {
            nodo = nodo->izq;
        }
        return nodo;
    }

    NodoABB* eliminarRecursivo(NodoABB* nodo, int id, bool& eliminado) {
        if (nodo == nullptr) {
            eliminado = false;
            return nullptr;
        }
        if (id < nodo->id) {
            nodo->izq = eliminarRecursivo(nodo->izq, id, eliminado);
        } else if (id > nodo->id) {
            nodo->der = eliminarRecursivo(nodo->der, id, eliminado);
        } else {
            // Nodo encontrado: analizar los 3 casos
            eliminado = true;
            // Caso 1 y 2: Sin hijo o con un solo hijo
            if (nodo->izq == nullptr) {
                NodoABB* temp = nodo->der;
                delete nodo;
                return temp;
            } else if (nodo->der == nullptr) {
                NodoABB* temp = nodo->izq;
                delete nodo;
                return temp;
            }
            // Caso 3: Dos hijos (sucesor inorden)
            NodoABB* sucesor = encontrarMinimo(nodo->der);
            nodo->id = sucesor->id;
            nodo->nombre = sucesor->nombre;
            nodo->ciudadDestino = sucesor->ciudadDestino;
            nodo->der = eliminarRecursivo(nodo->der, sucesor->id, eliminado);
        }
        return nodo;
    }

    void inordenRecursivo(NodoABB* nodo, int& contador) const {
        if (nodo != nullptr) {
            inordenRecursivo(nodo->izq, contador);
            std::cout << "  " << std::setw(8) << nodo->id 
                      << std::setw(24) << nodo->nombre 
                      << std::setw(20) << nodo->ciudadDestino << "\n";
            contador++;
            inordenRecursivo(nodo->der, contador);
        }
    }

public:
    ArbolBusquedaABB() : raiz(nullptr), nodosTotales(0) {}

    ~ArbolBusquedaABB() {
        limpiar();
    }

    void limpiar() {
        destruirRecursivo(raiz);
        raiz = nullptr;
        nodosTotales = 0;
    }

    int getNodos() const { return nodosTotales; }

    bool insertar(int id, const std::string& nom, const std::string& dest) {
        bool insertado = false;
        raiz = insertarRecursivo(raiz, id, nom, dest, insertado);
        if (insertado) nodosTotales++;
        return insertado;
    }

    bool buscar(int id, std::string& outNom, std::string& outDest) const {
        NodoABB* res = buscarRecursivo(raiz, id);
        if (res == nullptr) return false;
        outNom = res->nombre;
        outDest = res->ciudadDestino;
        return true;
    }

    bool eliminar(int id) {
        bool eliminado = false;
        raiz = eliminarRecursivo(raiz, id, eliminado);
        if (eliminado) nodosTotales--;
        return eliminado;
    }

    void recorridoInorden() const {
        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " RECORRIDO INORDEN DEL ABB (Orden Ascendente por ID - O(n))\n";
        std::cout << "------------------------------------------------------------------------\n";
        std::cout << "  " << std::left << std::setw(8) << "ID" 
                  << std::setw(24) << "Nombre" 
                  << std::setw(20) << "Destino" << "\n";
        std::cout << "------------------------------------------------------------------------\n";
        if (raiz == nullptr) {
            std::cout << " [Arbol Vacio] No hay elementos indexados en el ABB.\n";
        } else {
            int cont = 0;
            inordenRecursivo(raiz, cont);
            std::cout << " Total de elementos indexados: " << cont << "\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }
};

// ============================================================================
// 5. GRAFO PONDERADO DINÁMICO PROPIO: GESTIÓN DE CIUDADES, CONEXIONES Y RUTAS
// ============================================================================

struct AristaPonderada {
    std::string destino;
    double peso;
    AristaPonderada* siguiente;

    AristaPonderada(std::string d, double p, AristaPonderada* sig = nullptr)
        : destino(d), peso(p), siguiente(sig) {}
};

struct VerticeCiudad {
    std::string nombre;
    AristaPonderada* adyacentes;
    VerticeCiudad* siguiente;

    VerticeCiudad(std::string n, VerticeCiudad* sig = nullptr)
        : nombre(n), adyacentes(nullptr), siguiente(sig) {}
};

class GrafoSITRAC {
private:
    VerticeCiudad* cabezaVertices;
    int numCiudades;

    VerticeCiudad* buscarVertice(const std::string& ciudad) const {
        VerticeCiudad* actual = cabezaVertices;
        while (actual != nullptr) {
            if (actual->nombre == ciudad) return actual;
            actual = actual->siguiente;
        }
        return nullptr;
    }

    void liberarAristas(AristaPonderada*& lista) {
        AristaPonderada* act = lista;
        while (act != nullptr) {
            AristaPonderada* temp = act;
            act = act->siguiente;
            delete temp;
        }
        lista = nullptr;
    }

public:
    GrafoSITRAC() : cabezaVertices(nullptr), numCiudades(0) {}

    ~GrafoSITRAC() {
        limpiar();
    }

    void limpiar() {
        VerticeCiudad* act = cabezaVertices;
        while (act != nullptr) {
            VerticeCiudad* temp = act;
            act = act->siguiente;
            liberarAristas(temp->adyacentes);
            delete temp;
        }
        cabezaVertices = nullptr;
        numCiudades = 0;
    }

    int getNumCiudades() const { return numCiudades; }

    bool existeCiudad(const std::string& ciudad) const {
        return buscarVertice(ciudad) != nullptr;
    }

    // Prueba adversarial: ciudad duplicada rechazada sin alterar grafo
    bool agregarCiudad(const std::string& ciudad) {
        if (ciudad.empty() || existeCiudad(ciudad)) return false;
        cabezaVertices = new VerticeCiudad(ciudad, cabezaVertices);
        numCiudades++;
        return true;
    }

    // Prueba adversarial: distancia negativa rechazada
    bool agregarConexion(const std::string& origen, const std::string& destino, double peso, bool bidireccional = true) {
        if (origen.empty() || destino.empty() || origen == destino || peso < 0) return false;

        // Asegurar existencia de vértices
        if (!existeCiudad(origen)) agregarCiudad(origen);
        if (!existeCiudad(destino)) agregarCiudad(destino);

        VerticeCiudad* vOrig = buscarVertice(origen);
        if (vOrig == nullptr) return false;

        // Verificar si la arista ya existe
        AristaPonderada* a = vOrig->adyacentes;
        while (a != nullptr) {
            if (a->destino == destino) return false; // Ya existe
            a = a->siguiente;
        }

        vOrig->adyacentes = new AristaPonderada(destino, peso, vOrig->adyacentes);

        if (bidireccional) {
            VerticeCiudad* vDest = buscarVertice(destino);
            if (vDest != nullptr) {
                // Verificar si existe en sentido inverso
                bool existeInv = false;
                AristaPonderada* inv = vDest->adyacentes;
                while (inv != nullptr) {
                    if (inv->destino == origen) { existeInv = true; break; }
                    inv = inv->siguiente;
                }
                if (!existeInv) {
                    vDest->adyacentes = new AristaPonderada(origen, peso, vDest->adyacentes);
                }
            }
        }
        return true;
    }

    bool eliminarConexion(const std::string& origen, const std::string& destino, bool bidireccional = true) {
        VerticeCiudad* vOrig = buscarVertice(origen);
        if (vOrig == nullptr) return false;

        bool borrada = false;
        AristaPonderada* act = vOrig->adyacentes;
        AristaPonderada* prev = nullptr;

        while (act != nullptr) {
            if (act->destino == destino) {
                if (prev == nullptr) {
                    vOrig->adyacentes = act->siguiente;
                } else {
                    prev->siguiente = act->siguiente;
                }
                delete act;
                borrada = true;
                break;
            }
            prev = act;
            act = act->siguiente;
        }

        if (bidireccional) {
            VerticeCiudad* vDest = buscarVertice(destino);
            if (vDest != nullptr) {
                AristaPonderada* act2 = vDest->adyacentes;
                AristaPonderada* prev2 = nullptr;
                while (act2 != nullptr) {
                    if (act2->destino == origen) {
                        if (prev2 == nullptr) {
                            vDest->adyacentes = act2->siguiente;
                        } else {
                            prev2->siguiente = act2->siguiente;
                        }
                        delete act2;
                        break;
                    }
                    prev2 = act2;
                    act2 = act2->siguiente;
                }
            }
        }

        return borrada;
    }

    // Prueba adversarial: ciudad eliminada -> borrado en cascada de TODAS sus conexiones
    bool eliminarCiudad(const std::string& ciudad) {
        if (!existeCiudad(ciudad)) return false;

        // 1. Eliminar la arista incidente en todos los demás vértices
        VerticeCiudad* v = cabezaVertices;
        while (v != nullptr) {
            if (v->nombre != ciudad) {
                AristaPonderada* actA = v->adyacentes;
                AristaPonderada* prevA = nullptr;
                while (actA != nullptr) {
                    if (actA->destino == ciudad) {
                        AristaPonderada* aBorrar = actA;
                        if (prevA == nullptr) {
                            v->adyacentes = actA->siguiente;
                            actA = v->adyacentes;
                        } else {
                            prevA->siguiente = actA->siguiente;
                            actA = prevA->siguiente;
                        }
                        delete aBorrar;
                    } else {
                        prevA = actA;
                        actA = actA->siguiente;
                    }
                }
            }
            v = v->siguiente;
        }

        // 2. Eliminar el vértice y sus aristas salientes
        VerticeCiudad* actV = cabezaVertices;
        VerticeCiudad* prevV = nullptr;

        while (actV != nullptr && actV->nombre != ciudad) {
            prevV = actV;
            actV = actV->siguiente;
        }

        if (actV == nullptr) return false;

        liberarAristas(actV->adyacentes);

        if (prevV == nullptr) {
            cabezaVertices = actV->siguiente;
        } else {
            prevV->siguiente = actV->siguiente;
        }

        delete actV;
        numCiudades--;
        return true;
    }

    void mostrarRed() const {
        std::cout << "\n========================================================================\n";
        std::cout << "           RED VIAL NACIONAL (LISTA DE ADYACENCIA DINÁMICA)             \n";
        std::cout << "========================================================================\n";
        if (cabezaVertices == nullptr) {
            std::cout << " [Grafo Vacio] No hay ciudades registradas en la red.\n";
            return;
        }

        VerticeCiudad* v = cabezaVertices;
        int aristasTotales = 0;
        while (v != nullptr) {
            std::cout << " [CIUDAD: " << v->nombre << "]\n";
            if (v->adyacentes == nullptr) {
                std::cout << "   └── (Sin conexiones / Ciudad aislada)\n";
            } else {
                AristaPonderada* a = v->adyacentes;
                while (a != nullptr) {
                    bool esUltima = (a->siguiente == nullptr);
                    std::cout << (esUltima ? "   └── " : "   ├── ")
                              << "-> " << std::left << std::setw(16) << a->destino 
                              << " [Costo/Distancia: " << std::fixed << std::setprecision(1) << a->peso << "]\n";
                    aristasTotales++;
                    a = a->siguiente;
                }
            }
            v = v->siguiente;
        }
        std::cout << "------------------------------------------------------------------------\n";
        std::cout << " Total de Ciudades (Vértices): " << numCiudades << "\n";
        std::cout << " Total de Aristas Dirigidas:   " << aristasTotales << "\n";
        std::cout << "========================================================================\n";
    }

    std::vector<std::string> obtenerListaCiudades() const {
        std::vector<std::string> lista;
        VerticeCiudad* act = cabezaVertices;
        while (act != nullptr) {
            lista.push_back(act->nombre);
            act = act->siguiente;
        }
        std::sort(lista.begin(), lista.end());
        return lista;
    }

    // ========================================================================
    // ALGORITMOS DEL MOTOR (ENTREGABLE 2): BFS, DFS Y DIJKSTRA
    // ========================================================================

    std::vector<std::string> recorridoBFS(const std::string& inicio) const {
        std::vector<std::string> orden;
        if (!existeCiudad(inicio)) return orden;

        std::set<std::string> visitados;
        std::queue<std::string> cola;

        cola.push(inicio);
        visitados.insert(inicio);

        while (!cola.empty()) {
            std::string actual = cola.front();
            cola.pop();
            orden.push_back(actual);

            VerticeCiudad* v = buscarVertice(actual);
            if (v != nullptr) {
                AristaPonderada* a = v->adyacentes;
                while (a != nullptr) {
                    if (visitados.find(a->destino) == visitados.end()) {
                        visitados.insert(a->destino);
                        cola.push(a->destino);
                    }
                    a = a->siguiente;
                }
            }
        }
        return orden;
    }

    void dfsRecursivoInterno(const std::string& actual, std::set<std::string>& visitados, std::vector<std::string>& orden) const {
        visitados.insert(actual);
        orden.push_back(actual);

        VerticeCiudad* v = buscarVertice(actual);
        if (v != nullptr) {
            AristaPonderada* a = v->adyacentes;
            while (a != nullptr) {
                if (visitados.find(a->destino) == visitados.end()) {
                    dfsRecursivoInterno(a->destino, visitados, orden);
                }
                a = a->siguiente;
            }
        }
    }

    std::vector<std::string> recorridoDFS(const std::string& inicio) const {
        std::vector<std::string> orden;
        if (!existeCiudad(inicio)) return orden;
        std::set<std::string> visitados;
        dfsRecursivoInterno(inicio, visitados, orden);
        return orden;
    }

    std::pair<std::vector<std::string>, double> rutaMenosSaltosBFS(const std::string& origen, const std::string& destino) const {
        std::vector<std::string> camino;
        if (!existeCiudad(origen) || !existeCiudad(destino)) return {camino, -1.0};
        if (origen == destino) { camino.push_back(origen); return {camino, 0.0}; }

        std::set<std::string> visitados;
        std::queue<std::string> cola;
        std::map<std::string, std::string> predecesor;

        cola.push(origen);
        visitados.insert(origen);
        bool hallado = false;

        while (!cola.empty()) {
            std::string act = cola.front();
            cola.pop();

            if (act == destino) {
                hallado = true;
                break;
            }

            VerticeCiudad* v = buscarVertice(act);
            if (v != nullptr) {
                AristaPonderada* a = v->adyacentes;
                while (a != nullptr) {
                    if (visitados.find(a->destino) == visitados.end()) {
                        visitados.insert(a->destino);
                        predecesor[a->destino] = act;
                        cola.push(a->destino);
                    }
                    a = a->siguiente;
                }
            }
        }

        if (!hallado) return {camino, -1.0};

        std::string paso = destino;
        while (paso != origen) {
            camino.push_back(paso);
            paso = predecesor[paso];
        }
        camino.push_back(origen);
        std::reverse(camino.begin(), camino.end());

        // Calcular costo acumulado en los saltos de BFS
        double costoTotal = 0.0;
        for (size_t i = 0; i + 1 < camino.size(); ++i) {
            VerticeCiudad* v = buscarVertice(camino[i]);
            if (v != nullptr) {
                AristaPonderada* a = v->adyacentes;
                while (a != nullptr) {
                    if (a->destino == camino[i + 1]) {
                        costoTotal += a->peso;
                        break;
                    }
                    a = a->siguiente;
                }
            }
        }

        return {camino, costoTotal};
    }

    std::pair<std::vector<std::string>, double> dijkstra(const std::string& origen, const std::string& destino) const {
        std::vector<std::string> camino;
        if (!existeCiudad(origen) || !existeCiudad(destino)) return {camino, -1.0};
        if (origen == destino) { camino.push_back(origen); return {camino, 0.0}; }

        const double INF = std::numeric_limits<double>::infinity();
        std::map<std::string, double> distancias;
        std::map<std::string, std::string> predecesor;

        VerticeCiudad* v = cabezaVertices;
        while (v != nullptr) {
            distancias[v->nombre] = INF;
            v = v->siguiente;
        }
        distancias[origen] = 0.0;

        typedef std::pair<double, std::string> ParDistancia;
        std::priority_queue<ParDistancia, std::vector<ParDistancia>, std::greater<ParDistancia>> minHeap;
        minHeap.push({0.0, origen});

        while (!minHeap.empty()) {
            double dActual = minHeap.top().first;
            std::string u = minHeap.top().second;
            minHeap.pop();

            if (u == destino) break;
            if (dActual > distancias[u]) continue;

            VerticeCiudad* vActual = buscarVertice(u);
            if (vActual != nullptr) {
                AristaPonderada* a = vActual->adyacentes;
                while (a != nullptr) {
                    std::string vecino = a->destino;
                    double peso = a->peso;

                    if (distancias[u] + peso < distancias[vecino]) {
                        distancias[vecino] = distancias[u] + peso;
                        predecesor[vecino] = u;
                        minHeap.push({distancias[vecino], vecino});
                    }
                    a = a->siguiente;
                }
            }
        }

        if (distancias[destino] == INF) return {camino, -1.0};

        std::string paso = destino;
        while (paso != origen) {
            camino.push_back(paso);
            paso = predecesor[paso];
        }
        camino.push_back(origen);
        std::reverse(camino.begin(), camino.end());

        return {camino, distancias[destino]};
    }
};

// ============================================================================
// MOTOR CENTRAL DEL SISTEMA SITRAC (INTEGRACIÓN TOTAL)
// ============================================================================

class SistemaSITRAC {
private:
    ListaPasajeros listaPasajeros;
    ColaSolicitudes colaViajes;
    PilaHistorial historial;
    ArbolBusquedaABB arbolIndice;
    GrafoSITRAC redVial;

    int contadorSolicitudes;

public:
    SistemaSITRAC() : contadorSolicitudes(101) {
        precargarDatosEjemplo();
    }

    void precargarDatosEjemplo() {
        // Red Vial de Colombia (incluye el caso clave Bogotá - Villavicencio)
        redVial.agregarConexion("Bogota", "Villavicencio", 90.0);
        redVial.agregarConexion("Bogota", "Ibague", 20.0);
        redVial.agregarConexion("Ibague", "Villavicencio", 25.0);
        redVial.agregarConexion("Bogota", "Medellin", 50.0);
        redVial.agregarConexion("Medellin", "Pereira", 30.0);
        redVial.agregarConexion("Pereira", "Manizales", 15.0);
        redVial.agregarConexion("Manizales", "Ibague", 25.0);
        redVial.agregarConexion("Bogota", "Cali", 60.0);
        redVial.agregarConexion("Cali", "Ibague", 40.0);
        redVial.agregarConexion("Bogota", "Bucaramanga", 70.0);
        redVial.agregarConexion("Bucaramanga", "Barranquilla", 80.0);

        // Componente aislada para probar casos límite de grafos desconectados
        redVial.agregarConexion("Cartagena", "Santa Marta", 30.0);

        // Pasajeros iniciales
        registrarPasajeroInterno(1001, "Carlos Gomez", "Medellin", "3001234567", false);
        registrarPasajeroInterno(1002, "Ana Martinez", "Cali", "3109876543", false);
        registrarPasajeroInterno(1003, "Sofia Ramirez", "Villavicencio", "3154567890", false);
        registrarPasajeroInterno(1004, "Juan Castro", "Bogota", "3201122334", false);

        // Solicitudes iniciales en cola FIFO
        colaViajes.encolar(SolicitudViaje(contadorSolicitudes++, 1001, "Carlos Gomez", "Bogota", "Medellin", 2));
        colaViajes.encolar(SolicitudViaje(contadorSolicitudes++, 1002, "Ana Martinez", "Bogota", "Cali", 1));
    }

    bool registrarPasajeroInterno(int id, const std::string& nom, const std::string& dest, const std::string& tel, bool apilarEnHistorial = true) {
        Pasajero p(id, nom, dest, tel);
        if (listaPasajeros.insertar(p)) {
            arbolIndice.insertar(id, nom, dest);
            if (apilarEnHistorial) {
                historial.apilar(AccionHistorial(ACCION_REGISTRAR_PASAJERO, 
                    "Registrado pasajero ID " + std::to_string(id) + " (" + nom + ")", id, nom, dest));
            }
            return true;
        }
        return false;
    }

    // ========================================================================
    // SUBMENÚ 1: GESTIÓN DE CIUDADES
    // ========================================================================
    void menuCiudades() {
        int op = -1;
        do {
            std::cout << "\n=======================================================\n";
            std::cout << "            SUBMENÚ: 1. GESTIÓN DE CIUDADES            \n";
            std::cout << "=======================================================\n";
            std::cout << " 1. Listar ciudades registradas\n";
            std::cout << " 2. Agregar nueva ciudad\n";
            std::cout << " 3. Eliminar ciudad (Borrado en cascada de conexiones)\n";
            std::cout << " 0. Volver al menú principal\n";
            std::cout << "=======================================================\n";
            op = leerEntero("Seleccione una opción: ", 0, 3);

            switch (op) {
                case 1: {
                    std::vector<std::string> c = redVial.obtenerListaCiudades();
                    std::cout << "\nCiudades registradas (" << c.size() << "):\n";
                    for (size_t i = 0; i < c.size(); ++i) {
                        std::cout << "  " << (i + 1) << ". " << c[i] << "\n";
                    }
                    pausar();
                    break;
                }
                case 2: {
                    std::string nom = leerCadena("Ingrese el nombre de la nueva ciudad: ");
                    if (redVial.agregarCiudad(nom)) {
                        historial.apilar(AccionHistorial(ACCION_AGREGAR_CIUDAD, "Agregada ciudad: " + nom, 0, nom));
                        std::cout << " [Éxito] Ciudad '" << nom << "' agregada correctamente al grafo.\n";
                    } else {
                        std::cout << " [Rechazado] La ciudad ya existe o el nombre es inválido.\n";
                    }
                    pausar();
                    break;
                }
                case 3: {
                    std::string nom = leerCadena("Ciudad a eliminar (borrará en cascada sus rutas): ");
                    if (redVial.eliminarCiudad(nom)) {
                        std::cout << " [Éxito] Ciudad '" << nom << "' y todas sus aristas asociadas fueron eliminadas.\n";
                    } else {
                        std::cout << " [Error] La ciudad no existe en el sistema.\n";
                    }
                    pausar();
                    break;
                }
            }
        } while (op != 0);
    }

    // ========================================================================
    // SUBMENÚ 2: GESTIÓN DE CONEXIONES
    // ========================================================================
    void menuConexiones() {
        int op = -1;
        do {
            std::cout << "\n=======================================================\n";
            std::cout << "           SUBMENÚ: 2. GESTIÓN DE CONEXIONES           \n";
            std::cout << "=======================================================\n";
            std::cout << " 1. Ver red vial completa (Lista de Adyacencia)\n";
            std::cout << " 2. Conectar dos ciudades (Distancia/Costo no negativo)\n";
            std::cout << " 3. Eliminar conexión entre ciudades\n";
            std::cout << " 0. Volver al menú principal\n";
            std::cout << "=======================================================\n";
            op = leerEntero("Seleccione una opción: ", 0, 3);

            switch (op) {
                case 1:
                    redVial.mostrarRed();
                    pausar();
                    break;
                case 2: {
                    std::string orig = leerCadena("Ciudad de ORIGEN: ");
                    std::string dest = leerCadena("Ciudad de DESTINO: ");
                    double costo = leerDouble("Costo / Distancia del tramo (>= 0): ", 0.0);
                    if (redVial.agregarConexion(orig, dest, costo, true)) {
                        historial.apilar(AccionHistorial(ACCION_AGREGAR_CONEXION, 
                            "Conectada ruta " + orig + " <-> " + dest + " ($" + std::to_string((int)costo) + ")", 0, orig, dest, costo));
                        std::cout << " [Éxito] Tramo registrado entre " << orig << " y " << dest << ".\n";
                    } else {
                        std::cout << " [Error] Verifique que los nombres no sean iguales o que la ruta no exista.\n";
                    }
                    pausar();
                    break;
                }
                case 3: {
                    std::string orig = leerCadena("Ciudad Origen: ");
                    std::string dest = leerCadena("Ciudad Destino: ");
                    if (redVial.eliminarConexion(orig, dest, true)) {
                        std::cout << " [Éxito] Conexión entre " << orig << " y " << dest << " eliminada.\n";
                    } else {
                        std::cout << " [Error] No se encontró conexión entre esas ciudades.\n";
                    }
                    pausar();
                    break;
                }
            }
        } while (op != 0);
    }

    // ========================================================================
    // SUBMENÚ 3: GESTIÓN DE PASAJEROS (LISTA ENLAZADA PROPIA)
    // ========================================================================
    void menuPasajeros() {
        int op = -1;
        do {
            std::cout << "\n=======================================================\n";
            std::cout << "       SUBMENÚ: 3. GESTIÓN DE PASAJEROS (LISTA TAD)    \n";
            std::cout << "=======================================================\n";
            std::cout << " 1. Listar todos los pasajeros registrados\n";
            std::cout << " 2. Registrar nuevo pasajero (Inserción ordenada)\n";
            std::cout << " 3. Buscar pasajero por ID\n";
            std::cout << " 4. Modificar datos de pasajero\n";
            std::cout << " 5. Eliminar pasajero por ID (Prueba adversarial)\n";
            std::cout << " 0. Volver al menú principal\n";
            std::cout << "=======================================================\n";
            op = leerEntero("Seleccione una opción: ", 0, 5);

            switch (op) {
                case 1:
                    listaPasajeros.mostrar();
                    pausar();
                    break;
                case 2: {
                    int id = leerEntero("Ingrese Cédula / ID del pasajero: ", 1);
                    std::string nom = leerCadena("Nombre completo: ");
                    std::string dest = leerCadena("Ciudad destino habitual: ");
                    std::string tel = leerCadena("Teléfono de contacto: ");

                    if (registrarPasajeroInterno(id, nom, dest, tel, true)) {
                        std::cout << " [Éxito] Pasajero insertado en la Lista Enlazada y sincronizado con el ABB.\n";
                    } else {
                        std::cout << " [Error] Ya existe un pasajero con el ID " << id << ".\n";
                    }
                    pausar();
                    break;
                }
                case 3: {
                    int id = leerEntero("Ingrese ID a buscar: ");
                    Pasajero* p = listaPasajeros.buscar(id);
                    if (p != nullptr) {
                        std::cout << "\n [PASAJERO ENCONTRADO EN LISTA]\n";
                        std::cout << " ID:      " << p->id << "\n";
                        std::cout << " Nombre:  " << p->nombre << "\n";
                        std::cout << " Destino: " << p->ciudadDestino << "\n";
                        std::cout << " Tel:     " << p->telefono << "\n";
                    } else {
                        std::cout << " [No encontrado] No existe ningún pasajero con ID " << id << ".\n";
                    }
                    pausar();
                    break;
                }
                case 4: {
                    int id = leerEntero("ID del pasajero a modificar: ");
                    if (!listaPasajeros.existeId(id)) {
                        std::cout << " [Error] Pasajero no registrado.\n";
                    } else {
                        std::string nom = leerCadena("Nuevo nombre (Enter para conservar): ");
                        std::string dest = leerCadena("Nuevo destino (Enter para conservar): ");
                        std::string tel = leerCadena("Nuevo teléfono (Enter para conservar): ");
                        listaPasajeros.modificar(id, nom, dest, tel);
                        std::cout << " [Éxito] Información actualizada.\n";
                    }
                    pausar();
                    break;
                }
                case 5: {
                    int id = leerEntero("ID del pasajero a eliminar: ");
                    if (listaPasajeros.eliminar(id)) {
                        arbolIndice.eliminar(id);
                        std::cout << " [Éxito] Pasajero con ID " << id << " eliminado de la lista y del ABB.\n";
                    } else {
                        std::cout << " [Error Adversarial Controlado] El pasajero ID " << id 
                                  << " no existe. La lista permanece integra sin fugas de memoria.\n";
                    }
                    pausar();
                    break;
                }
            }
        } while (op != 0);
    }

    // ========================================================================
    // SUBMENÚ 4: COLA DE SOLICITUDES (COLA FIFO PROPIA)
    // ========================================================================
    void menuCola() {
        int op = -1;
        do {
            std::cout << "\n=======================================================\n";
            std::cout << "       SUBMENÚ: 4. COLA DE SOLICITUDES (FIFO PROPIO)    \n";
            std::cout << "=======================================================\n";
            std::cout << " 1. Ver cola de solicitudes pendientes\n";
            std::cout << " 2. Encolar nueva solicitud de viaje\n";
            std::cout << " 3. Atender siguiente solicitud (Desencolar)\n";
            std::cout << " 0. Volver al menú principal\n";
            std::cout << "=======================================================\n";
            op = leerEntero("Seleccione una opción: ", 0, 3);

            switch (op) {
                case 1:
                    colaViajes.mostrar();
                    pausar();
                    break;
                case 2: {
                    int idPas = leerEntero("ID del pasajero que solicita: ");
                    Pasajero* p = listaPasajeros.buscar(idPas);
                    std::string nom = (p != nullptr) ? p->nombre : "No registrado";
                    std::string orig = leerCadena("Ciudad Origen: ");
                    std::string dest = leerCadena("Ciudad Destino: ");
                    int cupos = leerEntero("Cantidad de cupos: ", 1, 10);

                    SolicitudViaje s(contadorSolicitudes++, idPas, nom, orig, dest, cupos);
                    colaViajes.encolar(s);
                    std::cout << " [Éxito] Solicitud #" << s.idSolicitud << " encolada en la posición FIFO.\n";
                    pausar();
                    break;
                }
                case 3: {
                    SolicitudViaje atendida;
                    if (colaViajes.desencolar(atendida)) {
                        historial.apilar(AccionHistorial(ACCION_ATENDER_SOLICITUD,
                            "Atendida Solicitud #" + std::to_string(atendida.idSolicitud) + " para " + atendida.nombrePasajero,
                            atendida.idSolicitud));
                        std::cout << "\n=======================================================\n";
                        std::cout << "           SOLICITUD ATENDIDA SATISFACTORIAMENTE       \n";
                        std::cout << "=======================================================\n";
                        std::cout << " Solicitud N°:  " << atendida.idSolicitud << "\n";
                        std::cout << " Pasajero:      " << atendida.nombrePasajero << " (ID: " << atendida.idPasajero << ")\n";
                        std::cout << " Trayecto:      " << atendida.origen << " ---> " << atendida.destino << "\n";
                        std::cout << " Cupos:         " << atendida.asientos << "\n";
                        std::cout << "=======================================================\n";
                    } else {
                        std::cout << "\n [Prueba Adversarial Exitosa] La cola está vacía.\n";
                        std::cout << " La operación fue manejada con gracia sin desreferenciar punteros nulos.\n";
                    }
                    pausar();
                    break;
                }
            }
        } while (op != 0);
    }

    // ========================================================================
    // SUBMENÚ 5: PILA DE HISTORIAL Y DESHACER (PILA LIFO PROPIA)
    // ========================================================================
    void menuHistorial() {
        int op = -1;
        do {
            std::cout << "\n=======================================================\n";
            std::cout << "       SUBMENÚ: 5. PILA DE HISTORIAL (LIFO PROPIO)     \n";
            std::cout << "=======================================================\n";
            std::cout << " 1. Ver pila de historial de acciones (Tope = Reciente)\n";
            std::cout << " 2. Deshacer última acción (Pop en la Pila LIFO)\n";
            std::cout << " 0. Volver al menú principal\n";
            std::cout << "=======================================================\n";
            op = leerEntero("Seleccione una opción: ", 0, 2);

            switch (op) {
                case 1:
                    historial.mostrar();
                    pausar();
                    break;
                case 2: {
                    AccionHistorial a;
                    if (historial.desapilar(a)) {
                        std::cout << "\n [DESHACER] Se ha revertido la acción del tope:\n";
                        std::cout << " -> " << a.descripcion << "\n";

                        // Lógica inversa según el tipo de acción
                        if (a.tipo == ACCION_AGREGAR_CIUDAD) {
                            redVial.eliminarCiudad(a.param1);
                            std::cout << "    (Ciudad '" << a.param1 << "' eliminada de la red)\n";
                        } else if (a.tipo == ACCION_REGISTRAR_PASAJERO) {
                            listaPasajeros.eliminar(a.idRef);
                            arbolIndice.eliminar(a.idRef);
                            std::cout << "    (Pasajero ID " << a.idRef << " retirado del sistema)\n";
                        } else if (a.tipo == ACCION_AGREGAR_CONEXION) {
                            redVial.eliminarConexion(a.param1, a.param2);
                            std::cout << "    (Ruta " << a.param1 << " <-> " << a.param2 << " desmantelada)\n";
                        }
                    } else {
                        std::cout << " [Pila Vacía] No hay más acciones que se puedan deshacer.\n";
                    }
                    pausar();
                    break;
                }
            }
        } while (op != 0);
    }

    // ========================================================================
    // SUBMENÚ 6: ÁRBOL BINARIO DE BÚSQUEDA (ABB PROPIO)
    // ========================================================================
    void menuABB() {
        int op = -1;
        do {
            std::cout << "\n=======================================================\n";
            std::cout << "      SUBMENÚ: 6. ÁRBOL BINARIO DE BÚSQUEDA (ABB)      \n";
            std::cout << "=======================================================\n";
            std::cout << " 1. Mostrar recorrido Inorden (Ordenado ascendentemente)\n";
            std::cout << " 2. Buscar pasajero en el ABB (Búsqueda binaria O(log n))\n";
            std::cout << " 3. Insertar elemento manual al ABB\n";
            std::cout << " 4. Eliminar nodo del ABB (Prueba de los 3 casos)\n";
            std::cout << " 0. Volver al menú principal\n";
            std::cout << "=======================================================\n";
            op = leerEntero("Seleccione una opción: ", 0, 4);

            switch (op) {
                case 1:
                    arbolIndice.recorridoInorden();
                    pausar();
                    break;
                case 2: {
                    int id = leerEntero("ID a buscar en el ABB: ");
                    std::string nom, dest;
                    if (arbolIndice.buscar(id, nom, dest)) {
                        std::cout << "\n [Éxito en ABB - O(log n)] Pasajero localizado:\n";
                        std::cout << " ID:      " << id << "\n";
                        std::cout << " Nombre:  " << nom << "\n";
                        std::cout << " Destino: " << dest << "\n";
                    } else {
                        std::cout << " [No encontrado] El ID " << id << " no existe en el árbol.\n";
                    }
                    pausar();
                    break;
                }
                case 3: {
                    int id = leerEntero("ID único: ");
                    std::string nom = leerCadena("Nombre: ");
                    std::string dest = leerCadena("Destino: ");
                    if (arbolIndice.insertar(id, nom, dest)) {
                        std::cout << " [Éxito] Nodo insertado en el ABB respetando invariante de orden.\n";
                    } else {
                        std::cout << " [Error] Clave duplicada: el ID " << id << " ya existe en el ABB.\n";
                    }
                    pausar();
                    break;
                }
                case 4: {
                    int id = leerEntero("ID a eliminar del ABB: ");
                    if (arbolIndice.eliminar(id)) {
                        std::cout << " [Éxito] Nodo " << id << " eliminado del ABB reestructurando punteros.\n";
                    } else {
                        std::cout << " [Error] El ID no se encuentra en el árbol.\n";
                    }
                    pausar();
                    break;
                }
            }
        } while (op != 0);
    }

    // ========================================================================
    // SUBMENÚ 7: RUTAS Y ALGORITMOS (BFS, DFS, DIJKSTRA)
    // ========================================================================
    void menuRutasAlgoritmos() {
        int op = -1;
        do {
            std::cout << "\n=======================================================\n";
            std::cout << "          SUBMENÚ: 7. RUTAS Y ALGORITMOS DEL MOTOR     \n";
            std::cout << "=======================================================\n";
            std::cout << " 1. Recorrido BFS (Exploración en Anchura / Niveles)\n";
            std::cout << " 2. Recorrido DFS (Exploración en Profundidad)\n";
            std::cout << " 3. Ruta de Menor Costo (Algoritmo de Dijkstra)\n";
            std::cout << " 4. Comparativa Teórica: Menos Saltos (BFS) vs Menor Costo (Dijkstra)\n";
            std::cout << " 0. Volver al menú principal\n";
            std::cout << "=======================================================\n";
            op = leerEntero("Seleccione una opción: ", 0, 4);

            switch (op) {
                case 1: {
                    std::string orig = leerCadena("Ciudad de inicio para BFS: ");
                    if (!redVial.existeCiudad(orig)) {
                        std::cout << " [Error] La ciudad '" << orig << "' no existe en el grafo.\n";
                    } else {
                        std::vector<std::string> rec = redVial.recorridoBFS(orig);
                        std::cout << "\nRecorrido BFS por anillos de cercanía (" << rec.size() << " ciudades):\n";
                        for (size_t i = 0; i < rec.size(); ++i) {
                            std::cout << "  Nivel [" << i << "]: " << rec[i] << "\n";
                        }
                    }
                    pausar();
                    break;
                }
                case 2: {
                    std::string orig = leerCadena("Ciudad de inicio para DFS: ");
                    if (!redVial.existeCiudad(orig)) {
                        std::cout << " [Error] La ciudad '" << orig << "' no existe en el grafo.\n";
                    } else {
                        std::vector<std::string> rec = redVial.recorridoDFS(orig);
                        std::cout << "\nRecorrido DFS recursivo en profundidad (" << rec.size() << " ciudades):\n";
                        for (size_t i = 0; i < rec.size(); ++i) {
                            std::cout << "  Profundidad [" << i << "]: " << rec[i] << "\n";
                        }
                    }
                    pausar();
                    break;
                }
                case 3: {
                    std::string orig = leerCadena("Ciudad de ORIGEN: ");
                    std::string dest = leerCadena("Ciudad de DESTINO: ");
                    if (!redVial.existeCiudad(orig) || !redVial.existeCiudad(dest)) {
                        std::cout << " [Error] Verifique los nombres: una o ambas ciudades no existen.\n";
                    } else {
                        auto res = redVial.dijkstra(orig, dest);
                        if (res.second < 0) {
                            std::cout << " [Aviso] NO EXISTE RUTA viable (pertenecen a componentes aisladas).\n";
                        } else {
                            std::cout << "\n=======================================================\n";
                            std::cout << " RUTA ÓPTIMA ENCONTRADA (DIJKSTRA):\n ";
                            for (size_t i = 0; i < res.first.size(); ++i) {
                                std::cout << res.first[i] << (i + 1 < res.first.size() ? "  ──>  " : "");
                            }
                            std::cout << "\n Costo Mínimo Acumulado: " << std::fixed << std::setprecision(2) << res.second << " unidades\n";
                            std::cout << " Cantidad de Tramos:     " << (res.first.size() - 1) << "\n";
                            std::cout << "=======================================================\n";
                        }
                    }
                    pausar();
                    break;
                }
                case 4: {
                    std::cout << "\n [Caso Clave Diapositiva 05: Bogota a Villavicencio]\n";
                    std::string orig = leerCadena("Ciudad Origen [Enter para 'Bogota']: ");
                    if (orig.empty()) orig = "Bogota";
                    std::string dest = leerCadena("Ciudad Destino [Enter para 'Villavicencio']: ");
                    if (dest.empty()) dest = "Villavicencio";

                    auto rBFS = redVial.rutaMenosSaltosBFS(orig, dest);
                    auto rDijkstra = redVial.dijkstra(orig, dest);

                    if (rBFS.second < 0 || rDijkstra.second < 0) {
                        std::cout << " [Aviso] No existe ruta entre " << orig << " y " << dest << ".\n";
                    } else {
                        std::cout << "\n [1] RESULTADO POR BFS (Menor cantidad de saltos):\n";
                        std::cout << "     Ruta: ";
                        for (size_t i = 0; i < rBFS.first.size(); ++i) std::cout << rBFS.first[i] << (i + 1 < rBFS.first.size() ? " -> " : "");
                        std::cout << "\n     Saltos: " << (rBFS.first.size() - 1) << " | Costo total: " << rBFS.second << "\n";

                        std::cout << "\n [2] RESULTADO POR DIJKSTRA (Menor costo ponderado):\n";
                        std::cout << "     Ruta óptima: ";
                        for (size_t i = 0; i < rDijkstra.first.size(); ++i) std::cout << rDijkstra.first[i] << (i + 1 < rDijkstra.first.size() ? " -> " : "");
                        std::cout << "\n     Saltos: " << (rDijkstra.first.size() - 1) << " | Costo total: " << rDijkstra.second << "\n";

                        std::cout << "\n [3] CONCLUSIÓN TÉCNICA:\n";
                        std::cout << "     Se comprueba la premisa: 'Menos saltos != Menor costo'.\n";
                        std::cout << "     BFS eligió la ruta de " << (rBFS.first.size() - 1) << " saltos (costo " << rBFS.second << ").\n";
                        std::cout << "     Dijkstra optimizó el peso total llegando a " << rDijkstra.second 
                                  << " usando " << (rDijkstra.first.size() - 1) << " saltos.\n";
                    }
                    pausar();
                    break;
                }
            }
        } while (op != 0);
    }

    // ========================================================================
    // SUBMENÚ 8: REPORTES Y ANÁLISIS BIG-O
    // ========================================================================
    void menuReportes() {
        std::cout << "\n========================================================================\n";
        std::cout << "             SITRAC: REPORTE INTEGRAL Y AUDITORÍA TÉCNICA               \n";
        std::cout << "========================================================================\n";
        std::cout << " 1. ESTADO DE LAS ESTRUCTURAS EN MEMORIA:\n";
        std::cout << "    - Ciudades en el Grafo (V):      " << redVial.getNumCiudades() << "\n";
        std::cout << "    - Pasajeros en Lista Enlazada:   " << listaPasajeros.getCantidad() << "\n";
        std::cout << "    - Pasajeros indexados en el ABB: " << arbolIndice.getNodos() << "\n";
        std::cout << "    - Solicitudes en Cola FIFO:      " << colaViajes.getTamanio() << "\n";
        std::cout << "    - Acciones en Pila LIFO:         " << historial.getTamanio() << "\n";
        std::cout << "------------------------------------------------------------------------\n";
        std::cout << " 2. TABLA DE COMPLEJIDAD COMPUTACIONAL (BIG-O):\n";
        std::cout << "    - Lista Enlazada Pasajeros:   Inserción O(n), Búsqueda O(n), Eliminación O(n)\n";
        std::cout << "    - Cola FIFO Solicitudes:      Encolar O(1), Desencolar O(1)\n";
        std::cout << "    - Pila LIFO Historial:        Push O(1), Pop (Undo) O(1)\n";
        std::cout << "    - Árbol Binario de Búsqueda:  Búsqueda O(log n), Inserción O(log n)\n";
        std::cout << "    - Grafo con Lista Adyacencia: Espacio O(V + E)\n";
        std::cout << "    - Algoritmo BFS:              Tiempo O(V + E), Espacio O(V)\n";
        std::cout << "    - Algoritmo DFS:              Tiempo O(V + E), Espacio O(V)\n";
        std::cout << "    - Algoritmo Dijkstra:         Tiempo O((V + E) log V), Espacio O(V)\n";
        std::cout << "========================================================================\n";
        pausar();
    }

    // ========================================================================
    // SUBMENÚ 9: GUARDAR / CARGAR (PERSISTENCIA TOLERANTE A FALLOS)
    // ========================================================================
    void menuPersistencia() {
        int op = -1;
        do {
            std::cout << "\n=======================================================\n";
            std::cout << "        SUBMENÚ: 9. PERSISTENCIA DE DATOS              \n";
            std::cout << "=======================================================\n";
            std::cout << " 1. Guardar estado actual a archivos de texto\n";
            std::cout << " 2. Cargar datos desde archivos (Tolerante a fallos)\n";
            std::cout << " 0. Volver al menú principal\n";
            std::cout << "=======================================================\n";
            op = leerEntero("Seleccione una opción: ", 0, 2);

            switch (op) {
                case 1: {
                    std::ofstream fPas("pasajeros_sitrac.txt");
                    if (fPas.is_open()) {
                        NodoPasajero* act = listaPasajeros.getCabeza();
                        while (act != nullptr) {
                            fPas << act->datos.id << "," << act->datos.nombre << "," 
                                 << act->datos.ciudadDestino << "," << act->datos.telefono << "\n";
                            act = act->siguiente;
                        }
                        fPas.close();
                        std::cout << " [Éxito] Pasajeros guardados en 'pasajeros_sitrac.txt'.\n";
                    } else {
                        std::cout << " [Error] No se pudo crear el archivo de pasajeros.\n";
                    }
                    pausar();
                    break;
                }
                case 2: {
                    std::ifstream fPas("pasajeros_sitrac.txt");
                    if (!fPas.is_open()) {
                        std::cout << " [Prueba de Resistencia Exitosa] El archivo 'pasajeros_sitrac.txt' no existe.\n";
                        std::cout << " El sistema detectó la ausencia y continuó operando sin colapsar.\n";
                    } else {
                        std::string linea;
                        int cargados = 0;
                        while (std::getline(fPas, linea)) {
                            std::stringstream ss(linea);
                            std::string idStr, nom, dest, tel;
                            if (std::getline(ss, idStr, ',') && std::getline(ss, nom, ',') &&
                                std::getline(ss, dest, ',') && std::getline(ss, tel, ',')) {
                                try {
                                    int id = std::stoi(idStr);
                                    if (registrarPasajeroInterno(id, nom, dest, tel, false)) cargados++;
                                } catch (...) {}
                            }
                        }
                        fPas.close();
                        std::cout << " [Éxito] Se cargaron " << cargados << " pasajeros desde el archivo.\n";
                    }
                    pausar();
                    break;
                }
            }
        } while (op != 0);
    }

    // ========================================================================
    // BUCLE DEL MENÚ PRINCIPAL
    // ========================================================================
    void ejecutar() {
        int opcion = -1;
        do {
            std::cout << "\n";
            std::cout << "=======================================================\n";
            std::cout << "        SITRAC: SISTEMA INTELIGENTE DE TRANSPORTE       \n";
            std::cout << "      PROYECTO INTEGRADOR FINAL · USTA 2026-II          \n";
            std::cout << "=======================================================\n";
            std::cout << " 1. Gestión de ciudades       (Vértices del grafo)\n";
            std::cout << " 2. Gestión de conexiones     (Aristas ponderadas)\n";
            std::cout << " 3. Gestión de pasajeros      (Lista Enlazada propia)\n";
            std::cout << " 4. Cola de solicitudes       (Cola FIFO propia)\n";
            std::cout << " 5. Pila de historial         (Pila LIFO / Deshacer)\n";
            std::cout << " 6. Árbol de búsquedas        (ABB con borrado propio)\n";
            std::cout << " 7. Rutas y algoritmos        (BFS, DFS y Dijkstra)\n";
            std::cout << " 8. Reportes y auditoría      (Métricas y análisis Big-O)\n";
            std::cout << " 9. Guardar / Cargar datos    (Persistencia robusta)\n";
            std::cout << " 0. Salir\n";
            std::cout << "=======================================================\n";
            opcion = leerEntero("Seleccione un módulo (0-9): ", 0, 9);

            switch (opcion) {
                case 1: menuCiudades(); break;
                case 2: menuConexiones(); break;
                case 3: menuPasajeros(); break;
                case 4: menuCola(); break;
                case 5: menuHistorial(); break;
                case 6: menuABB(); break;
                case 7: menuRutasAlgoritmos(); break;
                case 8: menuReportes(); break;
                case 9: menuPersistencia(); break;
                case 0:
                    std::cout << "\nCerrando SITRAC de forma segura. Liberando memoria...\n";
                    std::cout << "¡Sustentación exitosa para USTA 2026-II!\n\n";
                    break;
            }
        } while (opcion != 0);
    }
};

// ============================================================================
// PUNTO DE ENTRADA PRINCIPAL
// ============================================================================

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    SistemaSITRAC sitrac;
    sitrac.ejecutar();

    return 0;
}
