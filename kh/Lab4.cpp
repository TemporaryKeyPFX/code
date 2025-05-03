#include <mpi.h>
#include <stdio.h>
#include <iostream>
using namespace std;

int main(int argc, char** argv) {
	int size, rank;
	MPI_Init(&argc, &argv);                     // Initialize MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);       // Get rank
	MPI_Comm_size(MPI_COMM_WORLD, &size);       // Get total number of processes

	const int arr_size = 5;
	float full_array[arr_size] = { 1.0, 2.0, 3.0, 4.0, 5.0 }; // fixed array is processed curr at rank 0
	float result_array[arr_size]; // will be present in rank 3

	if (size != 4) {
		if (rank == 0) {
			cout << "This program must be run with 4 processes.\n";
		}
		MPI_Finalize();
		return 0;
	}

	if (rank == 0) {
		for (int i = 0; i < arr_size; i++) {
			float partial_step = full_array[i] - 4; // sub 4
			MPI_Send(&partial_step, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD); // send to rahnk 1 
		}
	}
	else if (rank == 1) {
		for (int i = 0; i < arr_size; i++) {
			float partial_step = 0.0;
			MPI_Recv(&partial_step, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recv from rank 0
			partial_step = partial_step / 5; // div by 5
			MPI_Send(&partial_step, 1, MPI_FLOAT, 2, 0, MPI_COMM_WORLD); // send to rank 2
		}
	}
	else if (rank == 2) {
		for (int i = 0; i < arr_size; i++) {
			float partial_step = 0.0;
			MPI_Recv(&partial_step, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recv from rank 1
			partial_step = partial_step + 3; //  add 3
			MPI_Send(&partial_step, 1, MPI_FLOAT, 3, 0, MPI_COMM_WORLD); // send to rank 3
		}
	}
	else if (rank == 3) {
		cout << "Final result:\n";
		for (int i = 0; i < arr_size; i++) {
			float partial_step = 0.0;
			MPI_Recv(&partial_step, 1, MPI_FLOAT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recv from rank 2
			result_array[i] = partial_step * 7; // ,mult 7
			cout << result_array[i] << " ";
		}
		cout << endl;
	}

	MPI_Finalize();
	return 0;
}
