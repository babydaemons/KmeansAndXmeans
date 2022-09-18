#pragma once
#include <vector>
#include <limits>
#include <map>
#include <numeric>
#include <cmath>
#include <omp.h>
#include "DataSet.h"
#include "Cluster.h"

class KMeans {
public:
	KMeans(int K, Cluster& x, Clusters& clusters, double eps = 0.0, bool verbose = false)
		: K(K), ROWS(x.Count()), x(x), clusters(clusters), prev_clusters(clusters)
		, iteration(0), max_iteration(0), verbose(verbose), eps(eps), max_move(0)
	{
		clusters.Clear(K);
		auto div = ROWS / K;
		for (auto i = 0; i < ROWS; ++i) {
			auto label = ((i % K) ^ (i / div)) % K;
			clusters[label] += x[i];
		}
		clusters.Mean();
		prev_clusters = clusters;
#if defined(_DEBUG)
		std::cout << clusters;
#endif

		max_iteration = static_cast<int>(std::ceil(16 * std::log2(ROWS)));

		if (eps == 0.0) {
			this->eps = std::numeric_limits<double>::max();
			for (auto i = 0; i < K; ++i) {
				auto new_eps = Norm(clusters[i].Center()) / 256;
				if (new_eps < this->eps) {
					this->eps = new_eps;
				}
			}
		}
	}

	const Clusters& CalculateClusters() {
		while (true) {
			++iteration;
			clusters.Clear(K);

			#pragma omp parallel for
			for (auto i = 0; i < ROWS; ++i) {
				auto min_dist = std::numeric_limits<double>::max();
				auto label = std::numeric_limits<size_t>::max();
				for (size_t j = 0; j < K; ++j) {
					auto dist = Dist(prev_clusters[j].Center(), x[i]);
					if (dist < min_dist) {
						min_dist = dist;
						label = j;
					}
				}
				clusters[label] += x[i];
			}

			if (UpdateClusterCenter()) {
				break;
			}
		}

		return clusters;
	}

private:
	bool UpdateClusterCenter() {
		for (auto i = 0; i < clusters.size(); ++i) {
			if (clusters[i].Count() > 0) {
				continue;
			}
			clusters.erase(clusters.begin() + i);
		}
		K = static_cast<int>(clusters.size());
		clusters.Mean();

		bool terminated = iteration > max_iteration || IsTerminated();
		if (verbose && iteration > 0) {
			std::cout << iteration << "/" << max_iteration << " iteration, " << "max center move = " << max_move << "/" << eps << std::endl;
			for (auto i = 0; i < K; ++i) {
				std::cout << clusters[i];
			}
		}

		return terminated;
	}

	bool IsTerminated() {
		max_move = -std::numeric_limits<double>::max();
		for (auto i = 0; i < K; ++i) {
			auto move = Dist(clusters[i].Center(), prev_clusters[i].Center());
			if (move > max_move) {
				max_move = move;
			}
		}
		prev_clusters = clusters;
		return max_move < eps;
	}

private:
	int K;
	const int ROWS;
	Cluster& x;
	Clusters& clusters;
	Clusters prev_clusters;
	std::map<Label, Label> r;
	int iteration;
	int max_iteration;
	bool verbose;
	double eps;
	double max_move;
};
