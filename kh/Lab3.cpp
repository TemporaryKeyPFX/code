#include <mpi.h>
#include <iostream>
using namespace std;

int main(int argc, char** argv) {
	int rank, size; // process id (rank) 0,1,2 size = total no. of processes
	MPI_Init(&argc, &argv); // init MPI  
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // gets id of calling process
	MPI_Comm_size(MPI_COMM_WORLD, &size); // gets total number of processes launched 

	if (size != 3) {
		if (rank == 0)
			cout << "This program must be run with exactly 3 processes.\n";
		MPI_Finalize();
		return 0;
	}

	int ARRAY_SIZE;
	int* full_array = nullptr; // used by process 0 
	int* sub_array = nullptr;  // used by process 0 and 1 to store their halves 

	if (rank == 0) {
		cout << "Enter the length (must be divisible by 2): ";
		cin >> ARRAY_SIZE;
		if (ARRAY_SIZE % 2 != 0) {
			cout << "Array size must be divisible by 2.\n";
			MPI_Abort(MPI_COMM_WORLD, 1); // MPI Comm is used to communicate thru the processes
		}
		full_array = new int[ARRAY_SIZE];
		cout << "Enter elements of array:\n";
		for (int i = 0; i < ARRAY_SIZE; i++) {
			cin >> full_array[i];
		}
	}

	// Broadcast the array size to all processes
	MPI_Bcast(&ARRAY_SIZE, 1, MPI_INT, 0, MPI_COMM_WORLD);  // broadcasts array size to all processes

	int half_size = ARRAY_SIZE / 2;
	sub_array = new int[half_size];

	// Scatter the array to rank 0 and 1
	MPI_Scatter(full_array, half_size, MPI_INT, // scatters to rank 0, 1
		sub_array, half_size, MPI_INT,
		0, MPI_COMM_WORLD);

	int partial_sum = 0;
	for (int i = 0; i < half_size; i++) {
		partial_sum += sub_array[i];
	}

	if (rank == 0) {
		double start_time = MPI_Wtime();
		MPI_Send(&partial_sum, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
		MPI_Send(&start_time, 1, MPI_DOUBLE, 2, 1, MPI_COMM_WORLD); // p0 sends its partial sum to process 2 
	}
	else if (rank == 1) {
		MPI_Send(&partial_sum, 1, MPI_INT, 2, 2, MPI_COMM_WORLD); // p1 sends its partial sum to process 2 
	}
	else if (rank == 2) {
		int sum1, sum2;
		double start_time, end_time;
		MPI_Recv(&sum1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // partial sum from rank 0 
		MPI_Recv(&start_time, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // start time
		MPI_Recv(&sum2, 1, MPI_INT, 1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //partial sum from rank 0 
		int final_sum = sum1 + sum2; // final sum 
		end_time = MPI_Wtime(); // stopwatch end
		cout << "Final sum: " << final_sum << endl;
		cout << "Processing time: " << end_time - start_time << " seconds" << endl;
	}

	if (full_array) delete[] full_array;
	delete[] sub_array; // freeing up mem

	MPI_Finalize(); // deconstructor 
	return 0;
}