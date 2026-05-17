// Lab1_Practice.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS
#include <mpi.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;
#define N 3
#define LEN 20
#define EMPSIZE 6

struct Employee
{
	int id;
	char name[LEN];
	char surname[LEN];
	float salary;
};

int main(int argc, char** argv)
{
	srand(time(0));
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int root = 0;
	int grid_row = rank / N;
	int grid_col = rank % N;
	int lower = (grid_col < grid_row) ? 1 : 0;

	MPI_Comm lower_comm;
	MPI_Comm_split(MPI_COMM_WORLD, lower ? 0 : MPI_UNDEFINED, rank, &lower_comm);

	MPI_Datatype employee_type;
	int blocklengths[4] = { 1, LEN, LEN, 1 };
	MPI_Datatype types[4] = { MPI_INT, MPI_CHAR, MPI_CHAR, MPI_FLOAT };
	MPI_Aint offsets[4];
	offsets[0] = offsetof(Employee, id);
	offsets[1] = offsetof(Employee, name);
	offsets[2] = offsetof(Employee, surname);
	offsets[3] = offsetof(Employee, salary);

	MPI_Type_create_struct(4, blocklengths, offsets, types, &employee_type);
	MPI_Type_commit(&employee_type);

	if (lower)
	{
		int lower_rank, lower_size;
		MPI_Comm_rank(lower_comm, &lower_rank);
		MPI_Comm_size(lower_comm, &lower_size);

		int per_proc = EMPSIZE / lower_size;

		Employee employees[EMPSIZE];
		if (lower_rank == 0) {
			employees[0] = { 101, "Marko", "Markovic", 85000.0f };
			employees[1] = { 102, "Ana", "Jovanovic", 92000.0f };
			employees[2] = { 103, "Petar", "Petrovic", 78000.0f };
			employees[3] = { 104, "Jelena", "Nikolic", 95000.0f };
			employees[4] = { 105, "Stefan", "Djordjevic", 81000.0f };
			employees[5] = { 106, "Milica", "Ilic", 88000.0f };
		}

		Employee* local_emps = new Employee[per_proc];
		MPI_Scatter(employees, per_proc, employee_type, local_emps, per_proc, employee_type, 0, lower_comm);

		for (int i = 0; i < per_proc; i++)
		{
			cout << "Process world_rank=" << rank << ", lower_rank " << lower_rank << " ID: " << local_emps[i].id << ", Name: " << local_emps[i].name
				<< " " << local_emps[i].surname << ", Salary: " << local_emps[i].salary << endl;
		}
		delete[] local_emps;
		MPI_Comm_free(&lower_comm);
	}
	
	MPI_Type_free(&employee_type);
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
