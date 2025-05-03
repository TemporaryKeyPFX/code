#include <mpi.h>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

void printArray(const vector<int>& arr, int rank) {
	cout << "Rank " << rank << " has: ";
	for (int num : arr)
		cout << num << " ";
	cout << endl;
}

int main(int argc, char** argv) {
	int rank, size;
	const int total_size = 8;
	int full_array[total_size] = { 23, 12, 4, 56, 9, 1, 44, 17 };
	vector<int> local_array;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int chunk_size = total_size / size;  // Divide equally among processes
	local_array.resize(chunk_size);  // Divide equally among processes

	// Scatter the array to all processes
	MPI_Scatter(full_array, chunk_size, MPI_INT, local_array.data(), chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

	// Sort the local chunk
	sort(local_array.begin(), local_array.end());

	// Odd-even transposition sort
	for (int phase = 0; phase < size; ++phase) { // Every phase does either even or odd neighbor communication.
		if ((phase % 2 == 0 && rank % 2 == 0) || (phase % 2 == 1 && rank % 2 == 1)) {
			if (rank + 1 < size) { // even phase, rank sends to rank++1
				vector<int> neighbor_array(chunk_size);
				MPI_Sendrecv(local_array.data(), chunk_size, MPI_INT, rank + 1, 0, // Send own data to rank+1 and receive theirs simultaneously.
					neighbor_array.data(), chunk_size, MPI_INT, rank + 1, 0,
					MPI_COMM_WORLD, MPI_STATUS_IGNORE);

				vector<int> merged(chunk_size * 2); // Merge the two arrays and keep the smaller half.
				merge(local_array.begin(), local_array.end(),
					neighbor_array.begin(), neighbor_array.end(), merged.begin());

				copy(merged.begin(), merged.begin() + chunk_size, local_array.begin());
			}
		}
		else if (rank - 1 >= 0) { // odd rank sends to rank+1
			// Odd rank receives from rank-1 and merges.
			vector<int> neighbor_array(chunk_size);
			MPI_Sendrecv(local_array.data(), chunk_size, MPI_INT, rank - 1, 0,
				neighbor_array.data(), chunk_size, MPI_INT, rank - 1, 0,
				MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			vector<int> merged(chunk_size * 2);
			merge(neighbor_array.begin(), neighbor_array.end(),
				local_array.begin(), local_array.end(), merged.begin());

			copy(merged.begin() + chunk_size, merged.end(), local_array.begin());
		}
	}

	// Gather sorted data to rank 0
	MPI_Gather(local_array.data(), chunk_size, MPI_INT, full_array, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		cout << "\nFinal Sorted Array:\n";
		for (int i = 0; i < total_size; ++i)
			cout << full_array[i] << " ";
		cout << endl;
	}

	MPI_Finalize();
	return 0;
}
