#include <stdio.h>
#include <stdlib.h>

#define DIM 20
#define N_THREADS 8

int main() {
    // Abre el archivo en modo lectura
    FILE *archivo;
    archivo = fopen("test.txt", "r");

    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }

    // Declara un array para almacenar los datos
    double *datos;
    int capacidad = 10; // Capacidad inicial del array
    int num_datos = 0;  // Número actual de datos
    datos = (double*)malloc(capacidad * sizeof(double));

    if (datos == NULL) {
        perror("Error al asignar memoria");
        return 1;
    }

    // Lee los datos del archivo y los almacena en el array
    double dato;
    while (fscanf(archivo, "%lf", &dato) == 1) {
        // Verifica si hay suficiente espacio en el array
        if (num_datos >= capacidad) {
            // Si no hay suficiente espacio, expande el array
            capacidad *= 2;
            datos = (double*)realloc(datos, capacidad * sizeof(double));
            if (datos == NULL) {
                perror("Error al asignar memoria");
                return 1;
            }
        }
        // Almacena el dato en el array
        datos[num_datos] = dato;
        num_datos++;
    }

    // Cierra el archivo
    fclose(archivo);

    // Imprime los datos almacenados en el array
    printf("Datos leídos del archivo:\n");
    for (int i = 0; i < num_datos; i++) {
        printf("%lf\n", datos[i]);
    }
    int N = datos[0];
    int M = datos[1];
    int alfa = 0.4;
    
    // Leer y almacenar los componentes de cada vector
    double vectores[N][DIM];
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < DIM; j++) {
            vectores[i][j] = datos[2 + i * DIM + j];
        }
    }
    printf("Vector 1 Elemento 7: %lf\n", vectores[0][6]);


    printf("Numero de vectores: %lf\n", datos[0]);
    printf("M: %lf\n", datos[1]);
    // Libera la memoria asignada al array
    free(datos);

    return 0;
}