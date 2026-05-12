// Lab1_Practice.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <mpi.h>
#include <iostream>
using namespace std;
#define N 10

int main(int argc, char** argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	int A[N];
	int B[N];

	int root = 0;

	int k = N / size;
	if (rank == root)
	{
		for (int i = 0; i < N; i++)
		{
			A[i] = N + i;
			B[i] = N * 2 - i;
			cout << "A"<<i << ": " << A[i] << " B"<<i << ": " << B[i] << endl;
		}
	}
	MPI_Datatype vector_type, resized_type;
	MPI_Type_vector(k, 1, size, MPI_INT, &vector_type);
	MPI_Type_commit(&vector_type);
	MPI_Type_create_resized(vector_type, 0, sizeof(int), &resized_type);
	MPI_Type_commit(&resized_type);

	int* loc_A = new int[k];
	int* loc_B = new int[k];
	MPI_Scatter(A, 1, resized_type, loc_A, k, MPI_INT, root, MPI_COMM_WORLD);
	MPI_Scatter(B, 1, resized_type, loc_B, k, MPI_INT, root, MPI_COMM_WORLD);

	int partial = 0;
	for (int i = 0; i < k; i++)
	{
		partial += loc_A[i] * loc_B[i];
	}

	int result;
	MPI_Reduce(&partial, &result, 1, MPI_INT, MPI_SUM, 1, MPI_COMM_WORLD);
	if(rank == 1)
	{
		cout << "The scalar product is: " << result << endl;
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
