// Lab1_Practice.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS
#include <mpi.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;
#define N 4
#define M 6

int main(int argc, char** argv)
{
	srand(time(0));
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);


	int A[N][M];
	int B[M];
	int cols_per_proc = M / size;

	int root = 0;

	MPI_Datatype col_type, resized_type;
	MPI_Type_vector(N, cols_per_proc, M, MPI_INT, &col_type);
	MPI_Type_commit(&col_type);
	MPI_Type_create_resized(col_type, 0, cols_per_proc * sizeof(int), &resized_type);
	MPI_Type_commit(&resized_type);
	if (rank == root)
	{
		FILE* f = fopen("april23.dar", "r");
		for (int i = 0; i < N; i++)
			for (int j = 0; j < M; j++)
				fscanf(f, "%d", &A[i][j]);
		fclose(f);

		cout << "Matrix A:" << endl;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++)
				cout << A[i][j] << "\t";
			cout << endl;
		}		
		
	}
	
	for (int i = 0; i < M; i++)
		B[i] = i+1;

	int* cols_A = new int[N * cols_per_proc];
	MPI_Scatter(A, 1, resized_type, cols_A, N*cols_per_proc, MPI_INT, root, MPI_COMM_WORLD);

	int partial_C[N];
	memset(partial_C, 0, sizeof(partial_C));
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < cols_per_proc; j++)
			partial_C[i] += cols_A[cols_per_proc * i + j] * B[rank * cols_per_proc + j];
	}

	int C[N];
	MPI_Reduce(partial_C, C, N, MPI_INT, MPI_SUM, size-1, MPI_COMM_WORLD);

	if (rank == (size-1))
	{
		cout << "Matrix B:" << endl;
		for (int i = 0; i < M; i++) {
			cout << B[i] << "\t";
			cout << endl;
		}
		cout << endl << "Matrix C:" << endl;
		for (int i = 0; i < N; i++) {
				cout << C[i] << "\t";
				cout << endl;
		}
	}
	MPI_Type_free(&col_type);
	MPI_Type_free(&resized_type);
	delete[] cols_A;
	MPI_Finalize();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
