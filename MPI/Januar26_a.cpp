// Lab1_Practice.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <mpi.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;
#define N 6
#define S 3
#define K 4

int main(int argc, char** argv)
{
	srand(time(0));
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	struct {
		int value;
		int rank;
	} min_in, min_out;

	int A[N][S];
	int B[S][K];

	int root = 0;

	if (rank == root)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < S; j++)
			{
				A[i][j] = (rand() % 51) + 5;
			}
		}

		cout << "Matrix A:" << endl;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < S; j++)
				cout << A[i][j] << "\t";
			cout << endl;
		}

		for (int i = 0; i < S; i++)
		{
			for (int j = 0; j < K; j++)
			{
				B[i][j] = (rand() % 20) + 5;
			}
		}

		cout << "Matrix B:" << endl;
		for (int i = 0; i < S; i++) {
			for (int j = 0; j < K; j++)
				cout << B[i][j] << "\t";
			cout << endl;
		}
	}

	MPI_Bcast(A, S * N, MPI_INT, root, MPI_COMM_WORLD);

	int per_row = K / size;
	int blocks = per_row * S;
	MPI_Datatype vector_type, resized_type;
	MPI_Type_vector(blocks, 1, size, MPI_INT, &vector_type);
	MPI_Type_commit(&vector_type);
	MPI_Type_create_resized(vector_type, 0, sizeof(int), &resized_type);
	MPI_Type_commit(&resized_type);

	int* loc_B = new int[S*per_row];
	MPI_Scatter(B, 1, resized_type, loc_B, blocks, MPI_INT, root, MPI_COMM_WORLD);

	int loc_prod[S];
	for (int i = 0; i < S; i++) {
		loc_prod[i] = 1;
		for (int j = 0; j < per_row; j++)
			loc_prod[i] *= loc_B[i * per_row + j];
	}

	int row_prods[S];
	MPI_Reduce(loc_prod, row_prods, S, MPI_INT, MPI_PROD, root, MPI_COMM_WORLD);

	int partial_C[N][K];
	memset(partial_C, 0, sizeof(partial_C));
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < per_row; j ++)
		{
			int sum = 0;
			for (int k = 0; k < S; k++)
				sum += A[i][k] * loc_B[k * per_row + j];
			partial_C[i][rank + j * size] = sum;
		}
	}

	int C[N][K];
	MPI_Reduce(partial_C, C, N*K, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);

	if (rank == root)
	{
		cout << endl << "Matrix B rows productions:" << endl;
		for (int i = 0; i < S; i++) {
			cout << "Row" << i << ": " << row_prods[i] << "\t";
			cout << endl;
		}

		cout << endl << "Matrix C:" << endl;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < K; j++)
				cout << C[i][j] << "\t";
			cout << endl;
		}
	}

	delete[] loc_B;
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
