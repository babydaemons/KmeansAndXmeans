#pragma once
#include "DataSet.h"

//#define GEOMETRIC_MEAN

#if defined(GEOMETRIC_MEAN)
const double INIT_TOTAL = 1.0;
#else
const double INIT_TOTAL = 0.0;
#endif

typedef int Label;

class Cluster;

class Clusters : public std::vector<Cluster> {
public:
	Clusters(DataSet& dataSet) {
		Clusters::clusters = this;
		Clusters::dataSet = &dataSet;
	}

	Clusters& operator=(const Clusters& a) {
		*static_cast<std::vector<Cluster>*>(this) = *static_cast<const std::vector<Cluster>*>(&a);
		return *this;
	}

	void Clear(int K);

	void Mean();

	static Clusters& GetInstance() {
		return *clusters;
	}

	static DataSet& GetDataSet() {
		return *dataSet;
	}

public:
	static Label label;

private:
	static Clusters* clusters;
	static DataSet* dataSet;
};

Label Clusters::label;
Clusters* Clusters::clusters;
DataSet* Clusters::dataSet;

class Cluster : private std::vector<Vector*> {
public:
	Cluster() : label(Clusters::label++), total_x(0.0), x(0.0), total_dist(INIT_TOTAL), mean_dist(INIT_TOTAL) {
		omp_init_lock(&omp_lock);
	}

	void Fill() {
		std::vector<Vector*>& ptr(*this);
		auto& xx(Clusters::GetDataSet());

		auto count = static_cast<int>(xx.size());
		ptr.resize(count);

		#pragma omp parallel for
		for (auto i = 0; i < count; ++i) {
			ptr[i] = &xx[i];
		}
	}

	Cluster& operator=(const double b) {
		x = b;
		return *this;
	}

	Cluster& operator+=(Vector& a) {
		omp_set_lock(&omp_lock);
		total_x += a;
#if defined(GEOMETRIC_MEAN)
		total_dist += std::log(Dist(x, a));
#else
		total_dist += Dist(x, a);
#endif
		push_back(&a);
		omp_unset_lock(&omp_lock);
		a.label = label;
		return *this;
	}

	bool operator==(Label label) const {
		return this->label == label;
	}

	void Mean() {
#if defined(GEOMETRIC_MEAN)
		mean_dist = std::exp(total_dist / static_cast<int>(size()));
#else
		mean_dist = total_dist / static_cast<int>(size());
#endif
		x = total_x;
		x /= static_cast<int>(size());
	}

	int Count() const {
		return static_cast<int>(size());
	}

	double BIC() const {
		auto& clusters(Clusters::GetInstance());
		const auto K = clusters.size();
		auto& xx(Clusters::GetDataSet());
		const auto ROWS = xx.size();
		auto score = std::numeric_limits<double>::max();
		auto dimension = (double)x.size();
		auto sigma = total_dist;
		auto N = 0.0;

		for (auto i = 0; i < K; i++) {
			N += static_cast<double>(clusters[i].Count());
		}

		if (N != K) {
			std::vector<double> scores(K, 0.0);

			sigma /= N - K;
			auto p = (K - 1) + dimension * K + 1;

			/* splitting criterion */
			const double PI = 3.1415926535897932384626433832795028841971;
			for (auto i = 0; i < K; ++i) {
				double n = (double)clusters[i].Count();
				double L = n * std::log(n) - n * std::log(N) - n * std::log(2.0 * PI) / 2.0 - n * dimension * std::log(sigma) / 2.0 - (n - K) / 2.0;

				scores[i] = L - p * 0.5 * std::log(N);
			}

			score = std::accumulate(scores.begin(), scores.end(), 0.0);
		}

		return score;
	}

	double MeanDistance() {
#if defined(GEOMETRIC_MEAN)
		mean_dist = std::exp(total_dist / count);
#else
		mean_dist = total_dist / static_cast<int>(size());
#endif
		return mean_dist;
	}

	void Clear() {
		clear();
		total_x = 0.0;
		x = 0.0;
		total_dist = INIT_TOTAL;
		mean_dist = INIT_TOTAL;
	}

	const Vector& Center() const {
		return x;
	}

	const Vector& operator[](int i) const {
		const std::vector<Vector*>& ptr(*this);
		return *(ptr[i]);
	}

	Vector& operator[](int i) {
		std::vector<Vector*>& ptr(*this);
		return *(ptr[i]);
	}

	friend std::ostream& operator<<(std::ostream& ostream, const Cluster& cluster) {
		ostream << cluster.label << '\t' << cluster.mean_dist << '\t' << cluster.size() << '\t' << cluster.x;
		return ostream;
	}

public:
	Label label;

private:
	Vector total_x;
	Vector x;
	double total_dist;
	double mean_dist;
	omp_lock_t omp_lock;
};

void Clusters::Clear(int K) {
	Clusters& clusters(*this);
	if (clusters.size() != K) {
		clusters.resize(K);
	}
	for (auto i = 0; i < K; ++i) {
		clusters[i].Clear();
		clusters[i].label = i;
	}
}

void Clusters::Mean() {
	auto K = static_cast<int>(size());
	std::vector<Cluster>& clusters(*this);

	#pragma omp parallel for
	for (auto i = 0; i < K; ++i) {
		clusters[i].Mean();
	}
}

std::ostream& operator<<(std::ostream& ostream, const Clusters& x) {
	ostream << "========== clusters infomation ==========" << std::endl;
	for (auto i = 0; i < x.size(); ++i) {
		ostream << x[i];
	}
	ostream << "========== clusters infomation ==========" << std::endl;
	return ostream;
}
