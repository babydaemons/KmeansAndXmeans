#pragma once
#include <vector>
#include <limits>
#include <random>
#include <thread>
#include "DataSet.h"
#include "Cluster.h"

class KMeans {
public:
	KMeans(int k, DataSet& dataSet, Clusters& clusters, int verbose = 0)
		: K(k), ROWS(static_cast<int>(dataSet.size())), label(ROWS), x(dataSet), clusters(clusters), iteration(0), cluster_changed(0), max_cluster_changed(ROWS / 1024), verbose(verbose)
	{
		auto div = ROWS / K;
		#pragma omp parallel for
		for (auto i = 0; i < ROWS; ++i) {
			label[i] = ((i % K) ^ (i / div)) % K;
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
			EraseEmptyCluster();

			cluster_changed = 0;
#if defined(_DEBUG)
			ThreadEntry(this, 0, 1, std::ref(cluster_changed));
#else
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
#endif

#if defined(VERBOSE)
			std::cout << "done!" << std::endl;
#endif

			if (cluster_changed <= max_cluster_changed) {
				break;
			}
		}

		EraseEmptyCluster();

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
			auto label_index = std::numeric_limits<int>::max();
			for (auto j = 0; j < K; ++j) {
				auto dist = Dist(clusters[j], x[i]);
				if (dist < min_dist) {
					min_dist = dist;
					label_index = j;
				}
			}
			auto new_label = clusters[label_index].label;
			if (label[i] != new_label) {
				++part_cluster_changed;
			}
			label[i] = new_label;
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
			auto label1 = label[i];
			clusters[label1] += x[i];
		}

		EraseEmptyCluster();

		#pragma omp parallel for
		for (auto i = 0; i < K; ++i) {
			clusters[i].Mean();
		}

		if (verbose > 0) {
			if (cluster_changed == 0) {
				cluster_changed = ROWS;
			}
			std::cout << "#### " << clusters.size() << " clusters: " << iteration << " iteration: " << cluster_changed << "/" << max_cluster_changed << " vectors cluster changed " << std::endl;
			if (verbose > 1) {
				for (auto i = 0; i < K; ++i) {
					if (clusters[i].Count() == 0) {
						continue;
					}
					std::cout << clusters[i].label << '\t' << clusters[i].Count() << std::endl;
				}
			}
		}
	}

	void EraseEmptyCluster() {
		for (size_t k = 0; k < clusters.size(); ++k) {
			if (clusters[k].Count() == 0) {
				clusters.erase(clusters.begin() + k);
				for (size_t i = 0; i < x.size(); ++i) {
					if (label[i] > k) {
						--label[i];
					}
				}
			}
		}
		K = static_cast<int>(clusters.size());
	}

private:
	int K;
	const int ROWS;
	std::vector<int> label;
	DataSet& x;
	Clusters& clusters;
	int iteration;
	int cluster_changed;
	int max_cluster_changed;
	int verbose;
};
