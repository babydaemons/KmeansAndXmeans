#pragma once
#include "KMeans.h"

class XMeans {
public:
	XMeans(DataSet& xx, Cluster& x, Clusters& clusters,double eps = 0.01, bool verbose = false)
		: ROWS(xx.size()), xx(xx), x(x), clusters(clusters), root_clusters(xx),eps(eps), verbose(verbose) {
		clusters.clear();

		KMeans kmeans(K, x, root_clusters, eps, verbose);
		kmeans.CalculateClusters();
	}

	static const int K = 2;

	void CalculateClusters() {
		Clusters children0(xx);
		CalculateClusters(root_clusters[0], children0);
		Clusters children1(xx);
		CalculateClusters(root_clusters[1], children1);
	}

private:
	void CalculateClusters(Cluster& parent, Clusters& chlidren) {
		auto parent_error = parent.MeanDistance();
		if (parent_error < eps) {
			AddCluster(parent);
			return;
		}

		Clusters child_pair(xx);
		KMeans kmeans(K, parent, child_pair, eps, verbose);
		kmeans.CalculateClusters();
		if (child_pair.size() == 1) {
			AddCluster(parent);
			return;
		}

#if 1
		auto child_error0 = child_pair[0].MeanDistance();
		auto child_error1 = child_pair[1].MeanDistance();
		auto child_error = child_error0 + child_error1;
#else
		auto parent_error = -parent.MeanDistance();
		auto child_error0 = -child_pair[0].MeanDistance();
		auto child_error1 = -child_pair[1].MeanDistance();
#endif
		bool divide = parent_error > child_error;
		if (divide) {
			if (child_pair[0].Count() > 1) {
				Clusters children(xx);
				CalculateClusters(child_pair[0], children);
			}
			else {
				AddCluster(child_pair[0]);
			}
			if (child_pair[1].Count() > 1) {
				Clusters children(xx);
				CalculateClusters(child_pair[1], children);
			}
			else {
				AddCluster(child_pair[1]);
			}
		}
		else {
			AddCluster(parent, child_pair);
		}
	}

	void AddCluster(Cluster& parent, Clusters& child_pair) {
		auto label0 = parent.label;
		auto label1 = child_pair[0].label;
		auto label2 = child_pair[1].label;

#pragma omp parallel for
		for (auto i = 0; i < ROWS; ++i) {
			if (x[i].label == label1) {
				x[i].label = label0;
			}
		}
#pragma omp parallel for
		for (auto i = 0; i < ROWS; ++i) {
			if (x[i].label == label2) {
				x[i].label = label0;
			}
		}
		AddCluster(parent);
	}

	void AddCluster(Cluster& cluster) {
		clusters.push_back(cluster);
#if defined(_DEBUG)
		std::cout << cluster;
#else
		std::cout << '.' << std::flush;
#endif
	}

private:
	const size_t ROWS;
	DataSet& xx;
	Cluster& x;
	Clusters& clusters;
	Clusters root_clusters;
	double eps;
	bool verbose;
};