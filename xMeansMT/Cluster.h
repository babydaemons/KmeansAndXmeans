#pragma once
#include "Vector.h"

typedef int Label;
typedef std::vector<Label> Labels;

class Cluster : public Vector {
public:
	Cluster(Label label = -1) : a(*static_cast<Vector*>(this)), count(0), total_dist(0) {
		a = 0.0;
		this->label = label;
	}

	Cluster& operator=(const double b) {
		a = b;
		return *this;
	}

	Cluster& operator=(const Cluster& b) {
		a = *static_cast<const Vector*>(&b);
		return *this;
	}

	Cluster& operator+=(const Vector& b) {
		a += b;
		++count;
		total_dist += Dist(a, b);
		return *this;
	}

	bool operator==(Label label) const {
		return this->label == label;
	}

	Cluster& Mean() {
		a /= count;
		return *this;
	}

	int Count() const {
		return count;
	}

	double AverageDist() const {
		return total_dist / count;
	}

	void Clear() {
		a = 0.0;
		count = 0;
		total_dist = 0.0;
	}

private:
	Vector& a;
	int count;
	double total_dist;
};

class Clusters : public std::vector<Cluster> {
public:
	Clusters(DataSet& dataSet) : dataSet(dataSet) { }

	Clusters& operator=(const Clusters& a) {
		*static_cast<std::vector<Cluster>*>(this) = *static_cast<const std::vector<Cluster>*>(&a);
		return *this;
	}

private:
	DataSet& dataSet;
};
