#include <iostream>      // For input/output
#include <omp.h>         // For OpenMP timing and parallelism

using namespace std;

int main() {
	int n;
	cout << "Enter a number: ";
	cin >> n;

	// -------- Serial Version --------
	long long serial_fact = 1;
	double start_serial = omp_get_wtime(); // Start timer

	for (int i = 1; i <= n; i++) {
		serial_fact *= i;
	}

	double end_serial = omp_get_wtime(); // End timer
	cout << "Serial Factorial: " << serial_fact << endl;
	cout << "Serial Time: " << (end_serial - start_serial) << " seconds\n";

	// -------- Parallel Version --------
	long long parallel_fact = 1;
	double start_parallel = omp_get_wtime(); // Start timer

#pragma omp parallel for reduction(*:parallel_fact) // OpenMP directive: run loop in parallel and reduce the product into one variable safely.
	//"Use multiple threads to run the for-loop in parallel, and combine their results using multiplication (*) into the variable parallel_fact safely."
	// #pragma- directive to give compiler instructions
	// omp - stands for open mp 
	// for- for loop after this line should run in parallel
	// reduction- reduces all results from multiple threads by multiplyign into parallel fact
	for (int i = 1; i <= n; i++) {
		parallel_fact *= i;
	}

	double end_parallel = omp_get_wtime(); // End timer
	cout << "Parallel Factorial: " << parallel_fact << endl;
	cout << "Parallel Time: " << (end_parallel - start_parallel) << " seconds\n";

	return 0;
}

//Each thread gets its own temporary copy of parallel_fact.After all threads finish their portion of the loop, 
//OpenMP multiplies all their results together into the final parallel_fact.
//This avoids race conditions and makes the result correct.
// A race condition happens when two or more threads access the same memory (like a variable) at the same time
// to avoid we can use mutex