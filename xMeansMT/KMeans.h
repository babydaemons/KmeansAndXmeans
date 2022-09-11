#pragma once
#include <vector>
#include <limits>
#include "DataSet.h"

class KMeans {
public:
	KMeans(int k, DataSet& dataSet, double eps = 0.025) : K(k), ROWS(dataSet.ROWS), x(dataSet), EPS(eps), center(k), prev_center(k) {
		for (auto i = 0; i < ROWS; ++i) {
			x[i].label = i / K;
		}

		for (auto i = 0; i < K; ++i) {
			center[i] = 0;
			center[i].label = i;
			prev_center[i] = std::numeric_limits<double>::max();
			prev_center[i].label = i;
		}

		Update();
	}

	void Calculate() {
		while (!IsTerminated()) {
			#pragma omp parallel for
			for (auto i = 0; i < ROWS; ++i) {
				auto min_dist = std::numeric_limits<double>::max();
				auto new_label = std::numeric_limits<int>::max();
				for (auto j = 0; j < K; ++j) {
					auto dist = Dist(center[j], x[i]);
					if (dist < min_dist) {
						min_dist = dist;
						new_label = j;
					}
				}
				x[i].label = new_label;
			}

			prev_center = center;
			Update();
		}
	}

	bool IsTerminated() const {
		auto max_dist = 0.0;
		for (auto i = 0; i < K; ++i) {
			auto dist = Dist(prev_center[i], center[i]);
			if (dist > max_dist) {
				max_dist = dist;
			}
		}
		return max_dist < EPS;
	}

private:
	void Update() {
		std::vector<int> count(K);
		for (auto i = 0; i < ROWS; ++i) {
			auto label = x[i].label;
			center[label] += x[i];
			++count[label];
		}
		for (auto i = 0; i < K; ++i) {
			center[i] /= count[i];
		}
	}

private:
	const int K;
	const int ROWS;
	DataSet& x;
	const double EPS;
	std::vector<Vector> center;
	std::vector<Vector> prev_center;
};
