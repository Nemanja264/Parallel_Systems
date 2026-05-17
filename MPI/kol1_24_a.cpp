// Lab1_Practice.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS
#include <mpi.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;
#define N 3
#define M 6
#define K 3

int main(int argc, char** argv)
{
	srand(time(0));
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int root = 0;
	
	int A[N][K];
	int B[K][M];
	int cols_per_proc = M / size;
	struct {
		int value;
		int rank;
	} in, out;


	MPI_Datatype col_type, resized_type;
	MPI_Type_vector(K, cols_per_proc, M, MPI_INT, &col_type);
	MPI_Type_commit(&col_type);
	MPI_Type_create_resized(col_type, 0, cols_per_proc * sizeof(int), &resized_type);
	MPI_Type_commit(&resized_type);

	if (rank == root)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < K; j++)
			{
				A[i][j] = i*j + 1;
			}
		}

		cout << "Matrix A:" << endl;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < K; j++)
				cout << A[i][j] << "\t";
			cout << endl;
		}

		for (int i = 0; i < K; i++)
		{
			for (int j = 0; j < M; j++)
			{
				B[i][j] = i*j + 5;
			}
		}

		cout << "Matrix B:" << endl;
		for (int i = 0; i < K; i++) {
			for (int j = 0; j < M; j++)
				cout << B[i][j] << "\t";
			cout << endl;
		}
		
		
	}
	
	MPI_Bcast(A, N * K, MPI_INT, root, MPI_COMM_WORLD);

	int* cols_B = new int[K * cols_per_proc];
	MPI_Scatter(B, 1, resized_type, cols_B, K*cols_per_proc, MPI_INT, root, MPI_COMM_WORLD);

	in.value = 0;
	in.rank = rank;
	for (int i = 0; i < K; i++)
		for (int j = 0; j < cols_per_proc; j++)
			if (in.value < cols_B[i * cols_per_proc + j])
				in.value = cols_B[i * cols_per_proc + j];

	MPI_Reduce(&in, &out, 1, MPI_2INT, MPI_MAXLOC, root, MPI_COMM_WORLD);

	int dest;
	if (rank == root)
		dest = out.rank;

	MPI_Bcast(&dest, 1, MPI_INT, root, MPI_COMM_WORLD);

	int partial_C[N][M];
	memset(partial_C, 0, sizeof(partial_C));
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < cols_per_proc; j++)
			for (int k = 0; k < K; k++)
				partial_C[i][rank * cols_per_proc + j] += A[i][k] * cols_B[k * cols_per_proc + j];
	}

	int C[N][M];
	MPI_Reduce(partial_C, C, N*M, MPI_INT, MPI_SUM, dest, MPI_COMM_WORLD);

	if (rank == dest)
	{
		cout << "Process " << rank << " has the maximum value: " << in.value << endl;
		cout << "Matrix C:" << endl;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++)
				cout << C[i][j] << "\t";
			cout << endl;
		}
	}

	delete[] cols_B;
	MPI_Type_free(&resized_type);
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
