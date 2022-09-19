#pragma once
#include "Vector.h"

typedef int Label;
typedef std::vector<Label> Labels;

class Cluster : public Vector {
public:
	Cluster(Label label = -1) : count(0) {
		Vector& x(*this);
		x = 0;
		this->label = label;
	}

	Cluster& operator=(const double a) {
		Vector& x(*this);
		x = a;
		return *this;
	}

	Cluster& operator+=(const Vector& a) {
		total_center += a;
		++count;
		return *this;
	}

	Cluster& Mean() {
		Vector& x(*this);
		x = total_center;
		x /= count;
		return *this;
	}

	int Count() const {
		return count;
	}

	void Clear() {
		Vector& x(*this);
		total_center = x;
		x = 0.0;
		count = 0;
	}

private:
	int count;
	Vector total_center;
};

class Clusters : public std::vector<Cluster> {
public:
	Clusters() { }

	Clusters& operator=(const Clusters& a) {
		*static_cast<std::vector<Cluster>*>(this) = *static_cast<const std::vector<Cluster>*>(&a);
		return *this;
	}

	void Write(std::ostream& ostream) {
		std::vector<Cluster>& x(*this);
		for (size_t i = 0; i < x.size(); ++i) {
			x[i].Write(ostream);
		}
	}

	friend std::ostream& operator<<(std::ostream& ostream, Clusters& xx) {
		for (size_t i = 0; i < xx.size(); ++i) {
			ostream << xx[i];
		}
		return ostream;
	}
};
