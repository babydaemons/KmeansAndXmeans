#pragma once
#include <iostream>
#include "Vector.h"

class DataSet : public std::vector<Vector> {
public:
	DataSet() { }

	DataSet(int rows) : std::vector<Vector>(rows) {
		std::vector<Vector>& x(*this);
		#pragma omp parallel for
		for (auto i = 0; i < static_cast<int>(size()); ++i) {
			x[i].index = i;
			x[i].label = -1;
		}
	}

	DataSet& ZScore(DataSet& z) const {
		const std::vector<Vector>& x(*this);
		auto N = static_cast<int>(x.size());
		Vector mean = 0.0;
		#pragma omp parallel for
		for (auto i = 0; i < N; ++i) {
			mean += x[i];
		}
		mean /= N;

		z = *this;
		#pragma omp parallel for
		for (auto i = 0; i < N; ++i) {
			z[i] -= mean;
		}

		Vector var = 0.0;
		#pragma omp parallel for
		for (auto i = 0; i < N; ++i) {
			Vector sq = z[i];
			sq *= z[i];
			var += sq;
		}

		#pragma omp parallel for
		for (auto i = 0; i < Vector::DIMENSION; ++i) {
			var[i] = std::sqrt(var[i]);
		}

		#pragma omp parallel for
		for (auto i = 0; i < N; ++i) {
			z[i] /= var;
		}

		return z;
	}

	void Read(std::istream& istream) {
		std::vector<Vector>& x(*this);
		for (auto i = 0; i < static_cast<int>(size()); ++i) {
			x[i].Read(istream);
			x[i].index = i;
			x[i].label = -1;
		}
	}

	friend std::istream& operator>>(std::istream& istream, DataSet& x) {
		for (auto i = 0; i < static_cast<int>(x.size()); ++i) {
			istream >> x[i];
			x[i].index = i;
			x[i].label = -1;
		}
		return istream;
	}

	void Write(std::ostream& ostream) const {
		const std::vector<Vector>& x(*this);
		for (auto i = 0; i < size(); ++i) {
			x[i].Write(ostream);
		}
	}

	friend std::ostream& operator<<(std::ostream& ostream, const DataSet& x) {
		for (auto i = 0; i < x.size(); ++i) {
			ostream << x[i];
		}
		return ostream;
	}
};