# SITRAC: Sistema Inteligente de Transporte y Gestión de Rutas 🚍🇨🇴
### Proyecto Integrador Final · Estructura de Datos (2026-II)
**Universidad Santo Tomás (USTA) · División de Ingenierías**

* **Autor:** **Jerónimo Martínez Daza**
* **Lenguaje:** C++ (Estándar C++17)
* **Entorno:** Consola Multiplataforma (Windows / Linux / macOS)
* **Repositorio GitHub:** [https://github.com/Jeronimomdaza-spec/rutas-colombia-](https://github.com/Jeronimomdaza-spec/rutas-colombia-)

---

## 1. Cumplimiento de Criterios de Evaluación y Calidad

> ⚠️ **Criterio de Rechazo Técnico Superado:**  
> *"Las estructuras de datos esenciales están implementadas desde cero mediante punteros y nodos dinámicos (TADs propios), sin depender de la STL para los componentes evaluados."*  
> *"La aplicación realiza operaciones reales sobre memoria con inserción, consulta y eliminación garantizando integridad y resistencia ante pruebas adversariales."*

---

## 2. Estructuras de Datos Implementadas Desde Cero (Punteros y Nodos)

| TAD Propio | Estructura de Punteros | Módulo en SITRAC | Operaciones Implementadas | Justificación Técnica y Complejidad |
| :--- | :--- | :--- | :--- | :--- |
| **Lista Enlazada Simple** | `NodoPasajero* siguiente;` | **3. Gestión de Pasajeros** | • Inserción ordenada por ID<br>• Búsqueda por puntero<br>• Modificación de campos<br>• Eliminación sin memory leaks | Tamaño dinámico en tiempo de ejecución. Inserción: $O(n)$, Búsqueda: $O(n)$, Eliminación: $O(n)$. |
| **Cola FIFO Dinámica** | `NodoCola* frente;`<br>`NodoCola* fin;` | **4. Cola de Solicitudes** | • Encolar al final (`fin`)<br>• Atender al inicio (`frente`)<br>• Consulta de turno | Orden de atención justo (primero en llegar, primero en ser atendido). Encolar: $O(1)$, Desencolar: $O(1)$. |
| **Pila LIFO Dinámica** | `NodoPila* tope;` | **5. Pila de Historial** | • Apilar acción (`push`)<br>• Desapilar (`pop`)<br>• **Deshacer (Undo)** | Almacena el historial cronológico inverso para revertir la última acción del sistema en $O(1)$. |
| **Árbol Binario de Búsqueda (ABB)** | `NodoABB* izq;`<br>`NodoABB* der;` | **6. Árbol de Búsquedas** | • Inserción balanceada<br>• Búsqueda binaria en $O(\log n)$<br>• **Eliminación con 3 casos**<br>• Recorrido Inorden | Indexación ultra rápida por Cédula/ID y visualización ordenada ascendente en $O(n)$. |
| **Grafo Ponderado Dinámico** | `VerticeCiudad*`<br>`AristaPonderada*` | **1, 2 y 7. Ciudades y Rutas** | • Inserción de vértices<br>• Conexiones ponderadas<br>• **Borrado en cascada**<br>• BFS, DFS y Dijkstra | Modela redes viales dispersas ($|E| \ll |V|^2$). Ahorro masivo de memoria frente a matrices: $O(V + E)$. |

---

## 3. Pruebas Adversariales y Casos Límite Controlados

El sistema implementa defensas estrictas contra errores de usuario y pruebas sorpresa de auditoría:

1. **Ciudad duplicada:** La inserción detecta si el vértice ya existe y rechaza la operación sin alterar los punteros del grafo.
2. **Atender cola vacía:** Si la cola no tiene solicitudes, la función devuelve `false` con un mensaje amigable y **no desreferencia punteros nulos** (previene `Segmentation Fault`).
3. **Distancia negativa:** Se rechaza de inmediato cualquier conexión con peso menor a cero.
4. **Pasajero inexistente:** La eliminación recorre la lista enlazada y el ABB; si el ID no existe, notifica al usuario sin dañar los enlaces contiguos.
5. **Ciudad eliminada (Borrado en Cascada):** Al eliminar una ciudad (ej. "Bogotá"), el algoritmo borra el vértice y recorre automáticamente todos los demás vértices para destruir cualquier carretera que entraba o salía de ella.
6. **Entrada alfabética en campo numérico:** Se implementó `leerEntero()` y `leerDouble()` con limpieza de búfer (`std::cin.clear()`, `std::cin.ignore()`), evitando que el menú entre en bucle infinito.
7. **Persistencia resistente:** Al intentar cargar desde un archivo inexistente, el programa no colapsa y notifica la ausencia.

---

## 4. Menú Principal y Submenús Requeridos

```text
=======================================================
        SITRAC: SISTEMA INTELIGENTE DE TRANSPORTE       
      PROYECTO INTEGRADOR FINAL · USTA 2026-II          
=======================================================
 1. Gestión de ciudades       (Vértices del grafo)
 2. Gestión de conexiones     (Aristas ponderadas)
 3. Gestión de pasajeros      (Lista Enlazada propia)
 4. Cola de solicitudes       (Cola FIFO propia)
 5. Pila de historial         (Pila LIFO / Deshacer)
 6. Árbol de búsquedas        (ABB con borrado propio)
 7. Rutas y algoritmos        (BFS, DFS y Dijkstra)
 8. Reportes y auditoría      (Métricas y análisis Big-O)
 9. Guardar / Cargar datos    (Persistencia robusta)
 0. Salir
=======================================================
```

---

## 5. Instrucciones de Compilación y Ejecución

### Requisitos:
* Compilador C++17 (`g++`, `clang++`, o MSVC).

### Compilación:
```powershell
cd "C:\Users\jeron\.gemini\antigravity\scratch\rutas-colombia"
g++ -std=c++17 src/main.cpp -o rutas_colombia.exe
```

### Ejecución:
```powershell
.\rutas_colombia.exe
```

---

## 6. Sustentación Técnica: Preguntas Clave para la Auditoría

1. **¿Cómo se maneja la eliminación en el Árbol Binario de Búsqueda (ABB)?**  
   * **Caso 1 (Nodo hoja):** Se elimina el nodo y se anula el puntero del padre.
   * **Caso 2 (Un solo hijo):** Se sustituye el nodo por su único hijo.
   * **Caso 3 (Dos hijos):** Se busca el **sucesor inorden** (el valor más pequeño del subárbol derecho), se copian sus datos al nodo actual y se elimina recursivamente el sucesor en el subárbol derecho.

2. **¿Cómo funciona el borrado en cascada en el Grafo?**  
   Al borrar una ciudad, primero se iteran todos los demás vértices de la lista principal para desvincular y liberar la memoria (`delete`) de cualquier arista que tenga como destino la ciudad eliminada. Luego se liberan las aristas salientes del vértice y se desenlaza el vértice de la lista principal.

3. **¿Por qué la Cola de Solicitudes usa punteros `frente` y `fin`?**  
   Tener un puntero directo al `fin` permite encolar en tiempo constante **$O(1)$** sin necesidad de recorrer toda la cola. Desencolar desde el `frente` también toma **$O(1)$**.

4. **¿Por qué Dijkstra supera a BFS en redes ponderadas?**  
   BFS asume saltos unitarios (todas las aristas pesan lo mismo). Dijkstra utiliza una **cola de prioridad (Min-Heap)** para relajar aristas vorazmente, encontrando la ruta que minimiza la suma acumulada de costos ($O((V+E)\log V)$). En la Diapositiva 05, Bogotá $\rightarrow$ Villavicencio directo toma 1 salto pero cuesta 90; la ruta Bogotá $\rightarrow$ Ibagué $\rightarrow$ Villavicencio toma 2 saltos pero cuesta solo 45.
