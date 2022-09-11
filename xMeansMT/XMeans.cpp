#include "KMeans.h"

/*
void KMeansExecute(const wchar_t* input_path, const wchar_t* output_path)
{
}
*/

#include <random>

int main()
{
	const int N = 100000;

	Vector::Initialize(2, 0);
	DataSet x(4 * N);

	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::normal_distribution<double> r1(-50.0, 15.0);
	std::normal_distribution<double> r2(+50.0, 15.0);

	for (auto i = 0 * N; i < 1 * N; ++i) {
		x[i][0] = r1(mt);
		x[i][1] = r1(mt);
	}

	for (auto i = 1 * N; i < 2 * N; ++i) {
		x[i][0] = r2(mt);
		x[i][1] = r1(mt);
	}

	for (auto i = 2 * N; i < 3 * N; ++i) {
		x[i][0] = r2(mt);
		x[i][1] = r2(mt);
	}

	for (auto i = 3 * N; i < 4 * N; ++i) {
		x[i][0] = r1(mt);
		x[i][1] = r2(mt);
	}

	KMeans kmeans(4, x);
	kmeans.Calculate();

	return 0;
}