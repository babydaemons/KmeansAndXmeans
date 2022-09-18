#include "XMeans.h"
#include <fstream>
#include <random>

void Seed(int K, int N, DataSet& x)
{
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::normal_distribution<double> r0(0.0, 50.0);

	auto N0 = 0; auto N1 = 1;
	for (auto i = N0 * N; i < N1 * N; ++i) {
		x[i][0] = r0(mt) + 0;
		x[i][1] = r0(mt) + 0;
	}
	++N0; ++N1;

	for (auto i = N0 * N; i < N1 * N; ++i) {
		x[i][0] = r0(mt) + 2500;
		x[i][1] = r0(mt) + 3500;
	}
	++N0; ++N1;

	for (auto i = N0 * N; i < N1 * N; ++i) {
		x[i][0] = r0(mt) + 3600;
		x[i][1] = r0(mt) - 4000;
	}
	++N0; ++N1;

	for (auto i = N0 * N; i < N1 * N; ++i) {
		x[i][0] = r0(mt) - 4000;
		x[i][1] = r0(mt) + 3200;
	}
	++N0; ++N1;

	for (auto i = N0 * N; i < N1 * N; ++i) {
		x[i][0] = r0(mt) - 3400;
		x[i][1] = r0(mt) - 2800;
	}
	++N0; ++N1;
}

void SaveResult(const std::string& path, const DataSet& x)
{
	std::ofstream file(path);

	for (auto i = 0; i < x.size(); ++i) {
		file << x[i].label << '\t' << x[i];
	}
}
void TestKMeans(int K, int N, double eps, double verbose)
{
	DataSet x(K * N);
	Seed(K, N, x);
	DataSet z(K * N);
	x.ZScore(z);
	Clusters clusters(z);
	Cluster cluster;
	cluster.Fill();
	//SaveResult("KMeans1.tsv", x);
	KMeans kmeans(K, cluster, clusters, 0.0, verbose);
	kmeans.CalculateClusters();
	//SaveResult("KMeans2.tsv", x);
	std::cout << clusters;
}

void TestXMeans(int K, int N, double eps, bool verbose)
{
	DataSet x(K * N);
	Seed(K, N, x);
	DataSet z(K * N);
	x.ZScore(z);
	Clusters clusters(z);
	Cluster cluster;
	cluster.Fill();
	//SaveResult("XMeans1.tsv", x);
	XMeans xmeans(x, cluster, clusters, 0.0005, verbose);
	xmeans.CalculateClusters();
	//SaveResult("XMeans2.tsv", x);
	std::cout << clusters;
}

int main()
{
	const int K = 5;
	const int N = 100000;
	const  double eps = std::pow(50.0, 2.0);
#if defined(_DEBUG)
	const bool verbose = true;
#else
	const bool verbose = false;
#endif

	Vector::Initialize(2, 0);
	TestKMeans(K, N, eps, verbose);
	TestXMeans(K, N, eps, verbose);

	return 0;
}