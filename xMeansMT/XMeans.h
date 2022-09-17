#pragma once
#include "KMeans.h"

class XMeans {
public:
	XMeans(DataSet& dataSet, Clusters& clusters, const Label target_label)
		: ROWS(dataSet.ROWS), dataSet(dataSet), clusters(clusters), root_clusters(dataSet),label(0)
	{
		clusters.clear();

		KMeans kmeans(K, 0, dataSet, root_clusters, target_label);
		kmeans.CalculateClusters();
		label += 2;

		Clusters children0(dataSet);
		CalculateClusters(root_clusters[0], children0);
		Clusters children1(dataSet);
		CalculateClusters(root_clusters[1], children1);
	}

	static const int K = 2;

	void CalculateClusters() {
	}

private:
	void CalculateClusters(const Cluster& parent, Clusters& chlidren) {
		Clusters children0(dataSet);
		KMeans kmeans0(K, label++,  dataSet, children0, parent.label);
		kmeans0.CalculateClusters();

		if (parent.AverageDist() > children0[0].AverageDist() + children0[1].AverageDist()) {
			if (children0[0].Count() > 1) {
				Clusters children(dataSet);
				CalculateClusters(children0[0], children);
			}
			else {
				clusters.push_back(children0[0]);
			}
			if (children0[1].Count() > 1) {
				Clusters children(dataSet);
				CalculateClusters(children0[1], children);
			}
			else {
				clusters.push_back(children0[1]);
			}
		}
		else {
			clusters.push_back(parent);
		}
	}

private:
	const int ROWS;
	DataSet& dataSet;
	Clusters& clusters;
	Clusters root_clusters;
	int label;
};