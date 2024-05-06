/*
Autores:

Alfredo Becerra
Cristobal Ramirez

Asignatura: Sistemas Distribuidos
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define DIM 20
#define N_THREADS 8


//Funcion de distancia euclidiana
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

    // Se realiza lectura de archivo
    FILE *archivo;

    char nombre_archivo[100]; // Declara un array para almacenar el nombre
    printf("Ingrese nombre del archivo: ");
    scanf("%s", nombre_archivo); // Captura la cadena desde el teclado para ser usado como el nombre

    archivo = fopen(nombre_archivo, "r");

    if (archivo == NULL)
    {
        perror("Error al abrir el archivo");
        return 1;
    }

    // Declara un array para almacenar los datos del archivo
    double *datos;
    int capacidad = 10; // Capacidad inicial del array
    int num_datos = 0;  // Número actual de datos
    datos = (double *)malloc(capacidad * sizeof(double));

    if (datos == NULL)
    {
        perror("Error al asignar memoria");
        return 1;
    }

    // Lee los datos del archivo y los almacena en el array datos
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

    // Definimos variables a usar para realizar las operaciones
    int N = datos[0];
    double M = datos[1];
    double alfa = 0.4;
    double criterio = (M * alfa);
    double distancias;

    // Leer y almacenar los componentes de cada vector de un array 
    double vectores[N][DIM];
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < DIM; j++)
        {
            vectores[i][j] = datos[2 + i * DIM + j];
        }
    }

    // Crear la lista de elementos (no pivotes) y de pivotes, para ser llenada mas adelante con el criterio
    int pivotes[N];
    int num_pivotes = 1;
    pivotes[0] = 0;

    int no_pivotes[N];
    int num_no_pivotes = 0;
    // Aca empezamos a usar hilos, se reparten el trabajo de calcular la distancia euclidiana
    // para descubrir los pivotes
    omp_set_num_threads(N_THREADS);

    // parallel for shared indica que para el proximo for los hilos se dividiran el trabajo de ejecutar
    // el bucle que contiene el calculo de distancia euclidiana, shared especificamente indica que existe
    // transferencia de informacion entre hilos para evitar algun problema
#pragma omp parallel for shared(vectores, criterio, num_pivotes, pivotes)
    // Primero hacemos un barrido desde el P0 para encontrar los pivotes
    for (int i = 0; i < N; i++)
    {
        int j;
        for (j = 0; j < num_pivotes; j++)
        {
            distancias = euclideanDistance(vectores[pivotes[j]], vectores[i], DIM); // Llamado a la funcion que calcula la distancia euclidiana entre dos vectores i j
            if (distancias < (criterio))
            {
                if(i==0 && j==0){ // Condicion para evitar que P0 sea añadido como elemento no pivote debido a que su distancia con sigo mismo es cero
                    break;
                }
        // Si no cumple el criterio de estar al menos a distancia M*a se añade a lista de elementos (no pivotes)
                #pragma omp critical
                {   
                    no_pivotes[num_no_pivotes++] = i;
                }
                break;
            }
        }
        if (j == num_pivotes)
        {
            pivotes[num_pivotes++] = i;
        }
        // Si cumple el criterio se añade a la lista de pivotes
    }

    //Ahora se debe realiza una matriz distancia entre los pivotes y los elementos
    double matriz_distancia[num_pivotes][num_no_pivotes];
    #pragma omp parallel for shared(num_pivotes, num_no_pivotes, vectores, pivotes, matriz_distancia)
    for (int i = 0; i < (num_pivotes); i++)
    {
        for (int j = 0; j < num_no_pivotes; j++)
        {
            matriz_distancia[i][j] = euclideanDistance(vectores[pivotes[i]], vectores[no_pivotes[j]] ,DIM);
        }
        
    }
    // Se muestra en pantalla la matriz distancia, las distancias separadas por un espacio
    // y se indica a que pivote corresponde para mayor claridad al leer
    printf("--------------------------------------------------------------------------Matriz de distancias--------------------------------------------------------------------------\n");
    for (int k = 0; k < num_pivotes; k++) {
        printf("\nPivote %d:\n", k); // Imprimir el número del pivote
        for (int l = 0; l < num_no_pivotes; l++) {
            printf(" %lf ", matriz_distancia[k][l]); // Imprimir la distancia
        }
        printf("\n");
    }

    // Libera la memoria asignada a los array
    free(datos);
    free(vectores);
    free(pivotes);
    free(no_pivotes);
    free(matriz_distancia);

    return 0;
}