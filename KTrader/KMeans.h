#pragma once
#include <vector>
#include <limits>
#include <random>
#include <omp.h>
#include "DataSet.h"
#include "Cluster.h"

class KMeans {
public:
	KMeans(int k, DataSet& dataSet, Clusters& clusters, bool verbose = false)
		: K(k), ROWS(static_cast<int>(dataSet.size())), x(dataSet), clusters(clusters), iteration(0), cluster_changed(0), verbose(verbose)
	{
		omp_init_lock(&omp_lock);

		auto div = ROWS / K;
		#pragma omp parallel for
		for (auto i = 0; i < ROWS; ++i) {
			x[i].label = ((i % K) ^ (i / div)) % K;
		}

		clusters.resize(K);
		for (auto i = 0; i < K; ++i) {
			clusters[i].label = i;
			clusters[i] = 0;
		}

		UpdateClusterCenter();
	}

	const Clusters& CalculateClusters() {
		while (true) {
			++iteration;

			cluster_changed = 0;
			#pragma omp parallel for
			for (auto i = 0; i < ROWS; ++i) {
				auto min_dist = std::numeric_limits<double>::max();
				auto new_label_index = std::numeric_limits<int>::max();
				for (auto j = 0; j < K; ++j) {
					auto dist = Dist(clusters[j], x[i]);
					if (dist < min_dist) {
						min_dist = dist;
						new_label_index = j;
					}
				}
				auto new_label = clusters[new_label_index].label;
				if (x[i].label != new_label) {
					omp_set_lock(&omp_lock);
					++cluster_changed;
					omp_unset_lock(&omp_lock);
				}
				x[i].label = new_label;
			}

			UpdateClusterCenter();

			if (cluster_changed == 0) {
				break;
			}
		}

		for (size_t i = 0; i < clusters.size(); ++i) {
			if (clusters[i].Count() == 0) {
				clusters.erase(clusters.begin() + i);
			}
		}

		return clusters;
	}

private:
	void UpdateClusterCenter() {
		#pragma omp parallel for
		for (int i = 0; i < K; ++i) {
			clusters[i].Clear();
		}

		for (auto i = 0; i < ROWS; ++i) {
			auto label1 = x[i].label;
			clusters[label1] += x[i];
		}

		#pragma omp parallel for
		for (auto i = 0; i < K; ++i) {
			clusters[i].Mean();
		}

		if (verbose) {
			std::cout << iteration << " iteration: " << cluster_changed << " vectors cluster changed" << std::endl;
			for (auto i = 0; i < K; ++i) {
				std::cout << clusters[i];
			}
		}
	}

private:
	int K;
	const int ROWS;
	DataSet& x;
	Clusters& clusters;
	int iteration;
	int cluster_changed;
	bool verbose;
	omp_lock_t omp_lock;
};
