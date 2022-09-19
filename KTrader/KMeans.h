#pragma once
#include <vector>
#include <limits>
#include <random>
#include <thread>
#include "DataSet.h"
#include "Cluster.h"

class KMeans {
public:
	KMeans(int k, DataSet& dataSet, Clusters& clusters, bool verbose = false)
		: K(k), ROWS(static_cast<int>(dataSet.size())), x(dataSet), clusters(clusters), iteration(0), cluster_changed(0), max_cluster_changed(ROWS / 2048), verbose(verbose)
	{
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
			auto cores = static_cast<int>(std::thread::hardware_concurrency());
			std::vector<int> part_cluster_changed(cores);
			std::vector<std::thread*> threads(cores);
			for (auto i = 0; i < cores; ++i) {
				threads[i] = new std::thread(&ThreadEntry, this, i, cores, std::ref(part_cluster_changed[i]));
			}
			for (auto i = 0; i < cores; ++i) {
				threads[i]->join();
				delete threads[i];
				cluster_changed += part_cluster_changed[i];
			}
#if defined(VERBOSE)
			std::cout << "done!" << std::endl;
#endif

			UpdateClusterCenter();

			if (cluster_changed < max_cluster_changed) {
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
	static void ThreadEntry(KMeans* self, int thread, int cores, int& part_cluster_changed) {
		self->AssignNearestCluster(thread, cores, part_cluster_changed);
	}

	void AssignNearestCluster(int thread, int cores, int& part_cluster_changed) {
		auto partition = ROWS / cores;
		auto start = thread * partition;
		auto end = (thread + 1) * partition;
		if (thread == cores - 1) {
			end = ROWS;
		}

		part_cluster_changed = 0;
		for (auto i = start; i < end; ++i) {
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
				++part_cluster_changed;
			}
			x[i].label = new_label;
#if defined(VERBOSE)
			std::cout << '.' << std::flush;
#endif
		}
	}

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
			std::cout << "#### " << iteration << " iteration: " << cluster_changed << "/" << max_cluster_changed << " vectors cluster changed" << std::endl;
			for (auto i = 0; i < K; ++i) {
				if (clusters[i].Count() == 0) {
					continue;
				}
				std::cout << clusters[i].label << '\t' << clusters[i].Count() << std::endl;
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
	int max_cluster_changed;
	bool verbose;
};
