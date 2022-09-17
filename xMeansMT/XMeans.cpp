#include "XMeans.h"

#include <random>

void TestKMeans(int N)
{
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::normal_distribution<double> r0(  0.0, 50.0);
	std::normal_distribution<double> r1(-200.0, 50.0);
	std::normal_distribution<double> r2(+200.0, 50.0);

	Vector::Initialize(2, 0);
	const int K = 5;
	DataSet x(K * N);

	auto N0 = 0; auto N1 = 1;
	for (auto i = N0 * N; i < N1 * N; ++i) {
		x[i][0] = r0(mt);
		x[i][1] = r0(mt);
	}
	++N0; ++N1;

	for (auto i = N0 * N; i < N1 * N; ++i) {
		x[i][0] = r1(mt);
		x[i][1] = r1(mt);
	}
	++N0; ++N1;

	for (auto i = N0 * N; i < N1 * N; ++i) {
		x[i][0] = r2(mt);
		x[i][1] = r1(mt);
	}
	++N0; ++N1;

	for (auto i = N0 * N; i < N1 * N; ++i) {
		x[i][0] = r2(mt);
		x[i][1] = r2(mt);
	}
	++N0; ++N1;

	for (auto i = N0 * N; i < N1 * N; ++i) {
		x[i][0] = r1(mt);
		x[i][1] = r2(mt);
	}
	++N0; ++N1;

	Clusters clusters(x);
	KMeans kmeans(K, 10, x, clusters, -1, true);
	kmeans.CalculateClusters();
}

void TestXMeans(int N)
{
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::normal_distribution<double> r1(-100.0, 25.0);
	std::normal_distribution<double> r2(+100.0, 25.0);

	Vector::Initialize(2, 0);
	DataSet x(4 * N);

	auto N0 = 0; auto N1 = 1;
	for (auto i = N0 * N; i < N1 * N; ++i) {
		x[i][0] = r1(mt);
		x[i][1] = r1(mt);
	}

	for (auto i = N0 * N; i < N1 * N; ++i) {
		x[i][0] = r2(mt);
		x[i][1] = r1(mt);
	}
	++N0; ++N1;

	for (auto i = N0 * N; i < N1 * N; ++i) {
		x[i][0] = r2(mt);
		x[i][1] = r2(mt);
	}
	++N0; ++N1;

	for (auto i = N0 * N; i < N1 * N; ++i) {
		x[i][0] = r1(mt);
		x[i][1] = r2(mt);
	}
	++N0; ++N1;

	Clusters clusters(x);
	XMeans xmeans(x, clusters, -1);
	xmeans.CalculateClusters();
}

int main()
{
	const int N = 1024 * 1024;
	TestKMeans(N);

	return 0;
}