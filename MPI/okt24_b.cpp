// Lab1_Practice.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS
#include <mpi.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;
#define N 6

int main(int argc, char** argv)
{
	srand(time(0));
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int root = 0;
	
	int A[N][N];

	MPI_Datatype pattern_type;
	int blocklengths[] = { 2, 1, 2, 1, 2, 1, 2, 1, 2, 1 };
	int offsets[10];
	int offset = 1;
	for (int i = 0; i < 10; i++)
	{
		offsets[i] = offset;
		if (i % 2 == 0)
			offset += 3;
		else
			offset += 4;
	}
	
	MPI_Type_indexed(10, blocklengths, offsets, MPI_INT, &pattern_type);
	MPI_Type_commit(&pattern_type);

	if (rank == root)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				A[i][j] = i*j + 1;
			}
		}

		cout << "Matrix A:" << endl;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++)
				cout << A[i][j] << "\t";
			cout << endl;
		}		
		
	}
	
	int in_group = (rank == 0 || rank == 1 || rank == 3 || rank == 5) ? 1 : MPI_UNDEFINED;
	MPI_Comm sub_comm;
	MPI_Comm_split(MPI_COMM_WORLD, in_group, rank, &sub_comm);

	if (in_group == 1)
	{
		int sub_rank;
		MPI_Comm_rank(MPI_COMM_WORLD, &sub_rank);

		int pattern_elems[15];
		if (sub_rank == 0)
			MPI_Bcast(A, 1, pattern_type, 0, sub_comm);
		else
		{
			MPI_Bcast(pattern_elems, 15, MPI_INT, 0, sub_comm);
			for (int i = 0; i < 15; i++)
				cout << "Process rank=" << rank << " subrank=" << sub_rank << " received: " << pattern_elems[i] << endl;
		}

		MPI_Comm_free(&sub_comm);
	}
	MPI_Type_free(&pattern_type);
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
