#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define DIM 20
#define N_THREADS 8

double euclideanDistance(double v1[], double v2[], int dim)
{
    double sum = 0.0;
    for (int i = 0; i < dim; i++)
    {
        double diff = v1[i] - v2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

int main()
{

    // Abre el archivo en modo lectura
    FILE *archivo;

    archivo = fopen("test.txt", "r");

    if (archivo == NULL)
    {
        perror("Error al abrir el archivo");
        return 1;
    }

    // Declara un array para almacenar los datos
    double *datos;
    int capacidad = 10; // Capacidad inicial del array
    int num_datos = 0;  // Número actual de datos
    datos = (double *)malloc(capacidad * sizeof(double));

    if (datos == NULL)
    {
        perror("Error al asignar memoria");
        return 1;
    }

    // Lee los datos del archivo y los almacena en el array
    double dato;
    while (fscanf(archivo, "%lf", &dato) == 1)
    {
        // Verifica si hay suficiente espacio en el array
        if (num_datos >= capacidad)
        {
            // Si no hay suficiente espacio, expande el array
            capacidad *= 2;
            datos = (double *)realloc(datos, capacidad * sizeof(double));
            if (datos == NULL)
            {
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

    int N = datos[0];
    double M = datos[1];
    double alfa = 0.4;
    double criterio = (M * alfa);

    // Leer y almacenar los componentes de cada vector
    double vectores[N][DIM];
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < DIM; j++)
        {
            vectores[i][j] = datos[2 + i * DIM + j];
        }
    }

    //Crear la lista de elementos y de pivotes, para ser llenada mas adelante con el criterio
    int pivotes[N];
    int num_pivotes = 1;
    pivotes[0]=0;

    int elementos[N];
    // Aca empezamos a usar hilos, se reparten el trabajo de calcular la distancia euclidiana
    // para descubrir los pivotes
    omp_set_num_threads(N_THREADS);
    #pragma omp parallel for shared(vectores, criterio)
    // Primero hacemos un barrido desde el P0 para encontrar los pivotes
    for (int i = 0; i < N; i++) {
       /* for (int j = 0; i < num_pivotes; i++)
        {
        }*/
        
        double distancias = euclideanDistance(vectores[0], vectores[i], DIM);
        if (distancias < (criterio))
        { //Si no cumple el criterio de estar al menos a M*a
            printf("Pivote: vector %d\n", (i + 1));
            // se añada a la lista de pivote
        }
        else
        { //Si cumple el criterio se añade a la lista
            printf("No pivote: vector %d\n", (i + 1));
            // se añade a la lista de no pivote
        }
    }

    // Libera la memoria asignada al array
    free(datos);
    free(vectores);

    return 0;
}