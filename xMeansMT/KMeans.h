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
	KMeans(DataSet& dataSet, Clusters& clusters, const Label label)
		: K(static_cast<int>(clusters.size())), ROWS(dataSet.ROWS), x(dataSet), label(label), clusters(clusters), iteration(0)
	{
		omp_init_lock(&omp_lock);

		std::random_device rnd;								// 非決定的な乱数生成器を生成
		std::mt19937 mt(rnd());								// メルセンヌ・ツイスタの32ビット版、引数は初期シード値
		std::uniform_int_distribution<> randK(0, K - 1);	// [0, K] 範囲の一様乱数
		for (auto i = 0; i < ROWS; ++i) {
			if (x[i].label == label) {
				x[i].label = clusters[randK(mt)].label;
			}
		}

		prev_clusters = clusters;
		for (auto i = 0; i < K; ++i) {
			auto label1 = clusters[i].label;
			clusters[i] = 0;
			prev_clusters[i] = std::numeric_limits<double>::max();
		}

		UpdateClusterCenter();
	}

	void CalculateClusters() {
		while (true) {
			++iteration;
			UpdateLabelIndex();

			auto cluster_changed = 0;
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

			prev_clusters = clusters;
			UpdateClusterCenter();

			if (cluster_changed == 0) {
				break;
			}
		}
	}

private:
	void UpdateClusterCenter() {
		UpdateLabelIndex();

		#pragma omp parallel for
		for (auto i = 0; i < ROWS; ++i) {
			auto label1 = x[i].label;
			if (x[i].label != label && std::find(clusters.begin(), clusters.end(), label1) == clusters.end()) {
				continue;
			}
			auto index = r[label1];
			clusters[index] += x[i];
		}

		#pragma omp parallel for
		for (auto i = 0; i < K; ++i) {
			clusters[i].Average();
		}

#if 0
		auto it1 = clusters.end();
		auto it2 = prev_clusters.end();
		for (auto i = 1; i <= K; ++i) {
			if (clusters[K - i].Count() > 0) {
				continue;
			}
			clusters.erase(it1 - i);
			prev_clusters.erase(it2 - i);
		}

		K = static_cast<int>(clusters.size());
#endif
	}

	void UpdateLabelIndex() {
		r.clear();
		for (auto i = 0; i < K; ++i) {
			auto label1 = clusters[i].label;
			clusters[i].Clear();
			r[label1] = i;
		}
	}

private:
	int K;
	const int ROWS;
	DataSet& x;
	const Label label;
	Clusters& clusters;
	Clusters prev_clusters;
	std::map<Label, Label> r;
	int iteration;
	omp_lock_t omp_lock;
};
