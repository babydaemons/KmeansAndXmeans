#pragma once
#include <vector>
#include <limits>
#include <map>
#include <random>
#include <omp.h>
#include "DataSet.h"
#include "Cluster.h"

class KMeans {
public:
	KMeans(int k, int k0, DataSet& dataSet, Clusters& clusters, const Label target_label, bool verbose = false)
		: K(k), ROWS(dataSet.ROWS), x(dataSet), target_label(target_label), clusters(clusters), iteration(0), cluster_changed(0), verbose(verbose)
	{
		omp_init_lock(&omp_lock);

		auto div = ROWS / K;
		for (auto i = 0; i < ROWS; ++i) {
			if (x[i].label == target_label) {
				x[i].label = k0 + (((i % K) ^ (i / div)) % K);
			}
		}

		clusters.resize(K);
		for (auto i = 0; i < K; ++i) {
			clusters[i].label = k0 + i;
		}

		for (auto i = 0; i < K; ++i) {
			auto label1 = clusters[i].label;
			clusters[i] = 0;
		}

		UpdateClusterCenter();
	}

	const Clusters& CalculateClusters() {
		while (true) {
			++iteration;
			UpdateLabelIndex();

			cluster_changed = 0;
			#pragma omp parallel for
			for (auto i = 0; i < ROWS; ++i) {
				if (std::find(clusters.begin(), clusters.end(), x[i].label) == clusters.end()) {
					continue;
				}
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

		return clusters;
	}

private:
	void UpdateClusterCenter() {
		UpdateLabelIndex();

		#pragma omp parallel for
		for (int i = 0; i < K; ++i) {
			clusters[i].Clear();
		}

		#pragma omp parallel for
		for (auto i = 0; i < ROWS; ++i) {
			auto label1 = x[i].label;
			if (x[i].label != target_label && std::find(clusters.begin(), clusters.end(), label1) == clusters.end()) {
				continue;
			}
			auto index = r[label1];
			clusters[index] += x[i];
		}

		#pragma omp parallel for
		for (auto i = 0; i < K; ++i) {
			clusters[i].Mean();
		}

		auto it1 = clusters.end();
		for (auto i = 1; i <= K; ++i) {
			if (clusters[K - i].Count() > 0) {
				continue;
			}
			clusters.erase(it1 - i);
		}
		K = static_cast<int>(clusters.size());

		if (verbose) {
			std::cout << iteration << " iteration: " << cluster_changed << " vectors cluster changed" << std::endl;
			for (auto i = 0; i < K; ++i) {
				std::cout << clusters[i];
			}
		}
	}

	void UpdateLabelIndex() {
		r.clear();
		for (auto i = 0; i < K; ++i) {
			auto label1 = clusters[i].label;
			r[label1] = i;
		}
	}

private:
	int K;
	const int ROWS;
	DataSet& x;
	const Label target_label;
	Clusters& clusters;
	std::map<Label, Label> r;
	int iteration;
	int cluster_changed;
	bool verbose;
	omp_lock_t omp_lock;
};
