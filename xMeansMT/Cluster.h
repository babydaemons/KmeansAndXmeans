#pragma once
#include "Vector.h"

typedef int Label;
typedef std::vector<Label> Labels;

class Cluster : public Vector {
public:
	Cluster(Label label) : count(0) {
		*static_cast<Vector*>(this) = 0.0;
		this->label = label;
	}

	Cluster& operator=(const double a) {
		*static_cast<Vector*>(this) = a;
		return *this;
	}

	Cluster& operator+=(const Vector& a) {
		*static_cast<Vector*>(this) += a;
		++count;
		return *this;
	}

	bool operator==(Label label) const {
		return this->label == label;
	}

	Cluster& Average() {
		*static_cast<Vector*>(this) /= count;
		return *this;
	}

	void Clear() {
		*static_cast<Vector*>(this) = 0.0;
		count = 0;
	}

	int Count() const {
		return count;
	}

private:
	int count;
};

typedef std::vector<Cluster> Clusters;

