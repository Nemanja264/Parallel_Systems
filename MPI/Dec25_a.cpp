// Lab1_Practice.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <mpi.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;
#define N 6
#define M 6
#define K 4

int main(int argc, char** argv)
{
	srand(time(0));
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);


	int A[N][K];
	int B[K][M];

	int root = 0;

	if (rank == root)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < K; j++)
			{
				A[i][j] = (rand() % 51) + 5;
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
				B[i][j] = (rand() % 20) + 5;
			}
		}

		cout << "Matrix B:" << endl;
		for (int i = 0; i < K; i++) {
			for (int j = 0; j < M; j++)
				cout << B[i][j] << "\t";
			cout << endl;
		}
	}

	int r = N / size;
	MPI_Datatype vector_type, resized_type;
	MPI_Type_vector(r, K, K*size, MPI_INT, &vector_type);
	MPI_Type_commit(&vector_type);
	MPI_Type_create_resized(vector_type, 0, K*sizeof(int), &resized_type);
	MPI_Type_commit(&resized_type);

	MPI_Bcast(B, K*M, MPI_INT, root, MPI_COMM_WORLD);
	int* loc_A = new int[r*K];
	MPI_Scatter(A, 1, resized_type,loc_A, r*K,MPI_INT, root, MPI_COMM_WORLD);



	int partial_C[N][M];
	memset(partial_C, 0, sizeof(partial_C));
	for (int i = 0; i < r; i++) {
		for (int j = 0; j < M; j++)
			for (int k = 0; k < K; k++)
				partial_C[rank+size*i][j] += loc_A[K * i + k] * B[k][j];
		
	}

	int C[N][M];
	MPI_Reduce(partial_C, C, N * M, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);

	if (rank == root)
	{

		cout << endl << "Matrix C:" << endl;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++)
				cout << C[i][j] << "\t";
			cout << endl;
		}
	}
	delete[] loc_A;
	MPI_Type_free(&vector_type);
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
