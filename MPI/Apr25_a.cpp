// Lab1_Practice.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <mpi.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;
#define N 3

int main(int argc, char** argv)
{
	srand(time(0));
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);


	int A[N][N];
	int B[N][N];
	int row_A[N];
	int col_B[N];

	int root = 0;
	int grid_row = rank / N;
	int grid_col = rank % N;

	MPI_Datatype col_type, resized_type;
	MPI_Type_vector(N, 1, N, MPI_INT, &col_type);
	MPI_Type_commit(&col_type);
	if (rank == root)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				A[i][j] = (rand() % 51) + 5;
			}
		}

		cout << "Matrix A:" << endl;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++)
				cout << A[i][j] << "\t";
			cout << endl;
		}

		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				B[i][j] = (rand() % 20) + 5;
			}
		}

		cout << "Matrix B:" << endl;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++)
				cout << B[i][j] << "\t";
			cout << endl;
		}
		
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
			{
				int dest = i * N + j;
				if (root == dest)
					continue;
				MPI_Send(&A[i][0], N, MPI_INT, dest, 0, MPI_COMM_WORLD);
				MPI_Send(&B[0][j], 1, col_type, dest, 0, MPI_COMM_WORLD);
			}

		for (int i = 0; i < N; i++)
		{
			row_A[i] = A[grid_row][i];
			col_B[i] = B[i][grid_col];
		}
	}
	else
	{
		MPI_Recv(row_A, N, MPI_INT, root, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(col_B, N, MPI_INT, root, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}



	

	int partial_C[N][N];
	memset(partial_C, 0, sizeof(partial_C));
	for (int i = 0; i < N; i++) {
		partial_C[grid_row][grid_col] += row_A[i]*col_B[i];
	}

	int C[N][N];
	MPI_Reduce(partial_C, C, N * N, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);

	if (rank == root)
	{
		cout << endl << "Matrix C:" << endl;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++)
				cout << C[i][j] << "\t";
			cout << endl;
		}
	}
	MPI_Type_free(&col_type);
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
