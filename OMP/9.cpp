#include <iostream>
#include <ctime>
#include <iomanip>
#include <chrono>
using namespace std;


int main() {
    int rows = 1000;
    int cols = 1000;
    int mx[rows][cols];
    int v[cols][1];
    int r[rows][1];
    srand(time(NULL));

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            mx[i][j] = rand() % 30 + 1;

    for (int c = 0; c < cols; c++)
        v[c][0] = c * 3;


    auto start = chrono::system_clock::now();
    for (int i = 0; i < rows; i++) {
        r[i][0] = 0;
        for (int j = 0; j < cols; j++)
            r[i][0] += mx[i][j] * v[j][0];
    }
    auto end = chrono::system_clock::now();
    chrono::duration<double> elapsed = end - start;
    printf("[Sequential time]: %f\n", elapsed.count());


    start = chrono::system_clock::now();
#pragma omp parallel for num_threads(8) shared(mx, v, r) schedule (static)
    for (int i = 0; i < rows; i++)
    {
        r[i][0] = 0;
        for (int j = 0; j < cols; j++)
            r[i][0] += mx[i][j] * v[j][0];
    }
    end = chrono::system_clock::now();
    elapsed = end - start;
    printf("[Parallel time][schedule=static]: %f\n", elapsed.count());


    start = chrono::system_clock::now();
#pragma omp parallel for num_threads(8) shared(mx, v, r) schedule (dynamic)
    for (int i = 0; i < rows; i++)
    {
        r[i][0] = 0;
        for (int j = 0; j < cols; j++)
            r[i][0] += mx[i][j] * v[j][0];
    }
    end = chrono::system_clock::now();
    elapsed = end - start;
    printf("[Parallel time][schedule=dynamic]: %f\n", elapsed.count());


    start = chrono::system_clock::now();
#pragma omp parallel for num_threads(8) shared(mx, v, r) schedule (guided)
    for (int i = 0; i < rows; i++)
    {
        r[i][0] = 0;
        for (int j = 0; j < cols; j++)
            r[i][0] += mx[i][j] * v[j][0];
    }
    end = chrono::system_clock::now();
    elapsed = end - start;
    printf("[Parallel time][schedule=guided]: %f\n", elapsed.count());


    start = chrono::system_clock::now();
#pragma omp parallel for num_threads(8) shared(mx, v, r) schedule (auto)
    for (int i = 0; i < rows; i++)
    {
        r[i][0] = 0;
        for (int j = 0; j < cols; j++)
            r[i][0] += mx[i][j] * v[j][0];
    }
    end = chrono::system_clock::now();
    elapsed = end - start;
    printf("[Parallel time][schedule=auto]: %f\n", elapsed.count());



    return 0;
}
