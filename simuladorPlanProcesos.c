#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESOS 200   // por si quieren simular mas de 100

typedef struct {
    int id;   			// ID del proceso
    int arrival;                // Tiempo de llegada (t0, t1... tn)
    int burst;                  // Ráfaga de CPU original (tiempo cpu)
    int priority;               // Tiempo restante (para RR, SRTN)
    int remaining;		// Para algoritmos con prioridades
    int start_time;		// Primer instante en que se ejecuta
    int finish_time;		// Momento en que termina
    int waiting_time;		// Tiempo total de espera
    int turnaround;		// finish_time - arrival
    int response_time;          // primer instante en que el proceso recibe CPU − arrival
} Process;

// First-Come-First-Served 
void simulate_fcfs(Process *p, int n) {
    // copiamos arreglo original
    Process arr[MAX_PROCESOS];
    for (int i = 0; i < n; i++) arr[i] = p[i];

    // ordenamos con insertion sort por arrival
    for (int i = 1; i < n; i++) {
        Process key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j].arrival > key.arrival) {
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = key;
    }

    int tiempo_actual = 0;
    double wait_total = 0.0;
    double turnaround_total = 0.0;
    double response_total = 0.0;

    printf("\n Simulación de FCFS \n");
    printf("P     arrival  burst  priority  start  finish  waiting  turnaround  response\n");

    for (int i = 0; i < n; i++) {
        // CPU espera hasta llegada
        if (tiempo_actual < arr[i].arrival) tiempo_actual = arr[i].arrival; 

        arr[i].start_time = tiempo_actual;
        arr[i].response_time = arr[i].start_time - arr[i].arrival;
        arr[i].waiting_time = arr[i].start_time - arr[i].arrival;

        tiempo_actual += arr[i].burst;
        arr[i].finish_time = tiempo_actual;
        arr[i].turnaround = arr[i].finish_time - arr[i].arrival;

        wait_total += arr[i].waiting_time;
        turnaround_total += arr[i].turnaround;
        response_total += arr[i].response_time;
        // imprimimos fila
        printf("P%2d   %6d   %4d   %4d   %5d   %6d   %7d   %9d   %8d\n",
               arr[i].id,
               arr[i].arrival,
               arr[i].burst,
               arr[i].priority,
               arr[i].start_time,
               arr[i].finish_time,
               arr[i].waiting_time,
               arr[i].turnaround,
               arr[i].response_time);
    }

    // imprimimos métricas de resultados
    printf("\nPromedios:\n");
        
    printf("waiting=%.2f \n", wait_total / n);
    printf("turnaround=%.2f \n", turnaround_total / n);
    printf("response=%.2f\n", response_total / n);
}

// Priority Scheduling 
// Suponemos que menor valor de priority => mayor prioridad, con 0 la máxima
// cuando tenemos empates de prioridad desempatamos con FCFS
void simulate_priority(Process *p, int n) {

    // copiamos por arrival
    Process arr[MAX_PROCESOS];
    for (int i = 0; i < n; i++) arr[i] = p[i];
    for (int i = 1; i < n; i++) {
        Process key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j].arrival > key.arrival) {
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = key;
    }

    // arreglo de listos
    Process ready[MAX_PROCESOS];
    int r_cont = 0;

    int tiempo_actual = 0;
    int sig = 0; 
    int completado = 0;
    double wait_total = 0.0;
    double turnaround_total = 0.0;
    double response_total = 0.0;

    printf("\n Simulación Priority Scheduling \n");
    printf("P     arrival  burst  priority  start  finish  waiting  turnaround  response\n");

    while (completado < n) {
        // insertar llegadas hasta tiempo_actual
        while (sig < n && arr[sig].arrival <= tiempo_actual) {
            Process pr = arr[sig];
            // encontrar posición de inserción
            int pos = 0;
                while (pos < r_cont) {
                    if (pr.priority < ready[pos].priority) break;
                    if (pr.priority == ready[pos].priority) {
                        if (pr.arrival < ready[pos].arrival) break;
                    }
                    pos++;
                }
                // desplazar a la derecha
                for (int k = r_cont; k > pos; k--) ready[k] = ready[k-1];
                ready[pos] = pr;
            r_cont++;
            sig++;
        }

        if (r_cont == 0) {
            // CPU espera hasta la siguiente llegada
            if (sig < n) {
                tiempo_actual = arr[sig].arrival;
                continue;
            } else break;
        }

        // tomar el de mayor prioridad (índice 0)
        Process pr = ready[0];
        // desplazar hacia la izquierda
        for (int k = 1; k < r_cont; k++) ready[k-1] = ready[k];
        r_cont--;

        if (tiempo_actual < pr.arrival) tiempo_actual = pr.arrival;
        pr.start_time = tiempo_actual;
        pr.response_time = pr.start_time - pr.arrival;
        pr.waiting_time = pr.start_time - pr.arrival;

        tiempo_actual += pr.burst; 
        pr.finish_time = tiempo_actual;
        pr.turnaround = pr.finish_time - pr.arrival;

        wait_total += pr.waiting_time;
        turnaround_total += pr.turnaround;
        response_total += pr.response_time;

        printf("P%2d   %6d   %4d   %4d   %5d   %6d   %7d   %9d   %8d\n",
               pr.id,
               pr.arrival,
               pr.burst,
               pr.priority,
               pr.start_time,
               pr.finish_time,
               pr.waiting_time,
               pr.turnaround,
               pr.response_time);

        completado++;

        // insertar llegadas que ocurrieron mientras se ejecutaba este proceso
        while (sig < n && arr[sig].arrival <= tiempo_actual) {
            Process pr2 = arr[sig];
            int pos = 0;
            while (pos < r_cont) {
                if (pr2.priority < ready[pos].priority) break;
                if (pr2.priority == ready[pos].priority) {
                    if (pr2.arrival < ready[pos].arrival) break;
                }
                pos++;
            }
            for (int k = r_cont; k > pos; k--) ready[k] = ready[k-1];
            ready[pos] = pr2;
            r_cont++;
            sig++;
        }
    }

    // imprimimos métricas de resultados
    printf("\nPromedios:\n");
        
    printf("waiting=%.2f \n", wait_total / n);
    printf("turnaround=%.2f \n", turnaround_total / n);
    printf("response=%.2f\n", response_total / n);
}

int main(void) {
    Process procesos[MAX_PROCESOS];
    int n = 0;

    // Lee desde stdin hasta EOF: cada línea debe tener id arrival burst priority
    while (scanf("%d %d %d %d",
                 &procesos[n].id,
                 &procesos[n].arrival,
                 &procesos[n].burst,
                 &procesos[n].priority) == 4) {

        procesos[n].remaining    = procesos[n].burst;
        procesos[n].start_time   = -1;
        procesos[n].finish_time  = 0;
        procesos[n].waiting_time = 0;
        procesos[n].turnaround   = 0;
        procesos[n].response_time= -1;
        n++;

        if (n >= MAX_PROCESOS) {
            fprintf(stderr, "Se alcanzó el límite MAX_PROCESOS=%d\n", MAX_PROCESOS);
            break;
        }
    }

    printf("Se leyeron %d procesos:\n", n);
    for (int i = 0; i < n; i++) {
        printf("P%3d: arrival=%3d burst=%2d priority=%d\n",
               procesos[i].id,
               procesos[i].arrival,
               procesos[i].burst,
               procesos[i].priority);
    }

    // Llamada a la simulación FCFS 
    simulate_fcfs(procesos, n);
    // Llamada a la simulación Priority (no preemptiva)
    simulate_priority(procesos, n);

    return 0;
}
