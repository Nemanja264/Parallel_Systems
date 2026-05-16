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
#define LEN 20

struct Student
{
	int index;
	char ime[LEN];
	char prezime[LEN];
	float avg_grade;
};

int main(int argc, char** argv)
{
	srand(time(0));
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int root = 0;

	MPI_Datatype student_type;
	int blocklengths[4] = { 1, LEN, LEN, 1 };
	MPI_Datatype types[4] = { MPI_INT, MPI_CHAR, MPI_CHAR, MPI_FLOAT };
	MPI_Aint offsets[4];
	offsets[0] = offsetof(Student, index);
	offsets[1] = offsetof(Student, ime);
	offsets[2] = offsetof(Student, prezime);
	offsets[3] = offsetof(Student, avg_grade);

	MPI_Type_create_struct(4, blocklengths, offsets, types, &student_type);
	MPI_Type_commit(&student_type);

	Student students[5];
	if (root == rank)
	{
		students[0] = { 1001, "Marko", "Markovic", 8.5f };
		students[1] = { 1002, "Ana", "Jovanovic", 9.2f };
		students[2] = { 1003, "Petar", "Petrovic", 7.8f };
		students[3] = { 1004, "Jelena", "Nikolic", 9.5f };
		students[4] = { 1005, "Stefan", "Djordjevic", 8.1f };
	}

	Student student;
	MPI_Scatter(students, 1, student_type, &student, 1, student_type, root, MPI_COMM_WORLD);
	cout << "Process " << rank << " received student: " << student.index << ", " << student.ime
		<< " " << student.prezime << ", average grade: " << student.avg_grade << endl;
	
	MPI_Type_free(&student_type);
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
