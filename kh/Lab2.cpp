#include <iostream>     // For input and output
#include <omp.h>        // For OpenMP parallelism

using namespace std;

// Define the function f(x) you want to integrate
double f(double x) {
	return x * x;  // Example: f(x) = x^2
}

int main() {
	double a, b;    // Integration interval [a, b]
	int n;          // Number of subintervals (must be divisible by num_threads)

	cout << "Enter lower limit a: ";
	cin >> a;
	cout << "Enter upper limit b: ";
	cin >> b;
	cout << "Enter number of subintervals n: ";
	cin >> n;

	double h = (b - a) / n;  // Width of each subinterval
	double sum = 0.0;

	double start_time = omp_get_wtime();

	// Parallel region using reduction to accumulate partial sums
#pragma omp parallel for reduction(+:sum) //sum var gets summed up at the end for reduction into one thread
	for (int i = 1; i < n; i++) { // loops over interior points 
		double x = a + i * h; // calculates the x-coordinate of the i-th subinterval point on the x-axis.
		sum += f(x); // f(x) at each interior point and then added 

	}

	// Final result using trapezoidal rule formula
	double result = (h / 2) * (f(a) + 2 * sum + f(b));

	double end_time = omp_get_wtime();

	cout << "Estimated integral = " << result << endl;
	cout << "Computation Time = " << (end_time - start_time) << " seconds" << endl;

	return 0;
}

//trapezoidal rule => integral = h/2​[f(a) + 2∑f(xi​) + f(b)]
// h = width of each interval
// n = number of intervals
// f(a) = function value at the start of the interval 
// f(b) = function value at the end of the interval 
// 2∑f(xi​) = interior points, and this appears on each side therefore 2*
