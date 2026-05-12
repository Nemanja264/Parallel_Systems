// Lab1_Practice.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <mpi.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;
#define N 9

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

	int A[N][N];

	int root = 2;
	int per_process = N / size;
	int blocks = N / 2;

	if (rank == root)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				A[i][j] = (rand() % 81) + 20;;
			}
		}

		cout << "Matrix A:" << endl;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++)
				cout << A[i][j] << "\t";
			cout << endl;
		}
	}

	MPI_Datatype vector_type, resized_type;
	MPI_Type_vector(blocks, per_process, 2*N, MPI_INT, &vector_type);
	MPI_Type_commit(&vector_type);
	MPI_Type_create_resized(vector_type, 0, per_process * sizeof(int), &resized_type);
	MPI_Type_commit(&resized_type);

	int* loc_A = new int[blocks * per_process];
	MPI_Scatter(&A[1], 1, resized_type, loc_A, blocks* per_process, MPI_INT, root, MPI_COMM_WORLD);

	min_in.value = loc_A[0];
	min_in.rank = rank;
	for (int i = 1; i < blocks * per_process; i++)
	{
		if(loc_A[i] < min_in.value)
		{
			min_in.value = loc_A[i];
		}
	}

	MPI_Reduce(&min_in, &min_out, 1, MPI_2INT, MPI_MINLOC, root, MPI_COMM_WORLD);
	if (rank == root)
	{
		cout << "Minimum value: " << min_out.value << " found at process " << min_out.rank << endl;
	}

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
