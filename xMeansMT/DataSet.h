#pragma once
#include <fstream>
#include "Vector.h"

class DataSet {
public:
	DataSet(int rows) : ROWS(rows), x(rows) {	}

	void Read(std::istream& istream) {
		for (auto i = 0; i < ROWS; ++i) {
			x[i].Read(istream);
		}
	}

	friend std::istream& operator>>(std::istream& istream, DataSet& x) {
		for (auto i = 0; i < x.ROWS; ++i) {
			istream >> x[i];
		}
		return istream;
	}

	void Write(std::ostream& ostream) {
		for (auto i = 0; i < ROWS; ++i) {
			x[i].Write(ostream);
		}
	}

	friend std::ostream& operator<<(std::ostream& ostream, DataSet& x) {
		for (auto i = 0; i < x.ROWS; ++i) {
			ostream << x[i];
		}
		return ostream;
	}

	const Vector& operator[](int i) const {
		return x[i];
	}

	Vector& operator[](int i) {
		return x[i];
	}

public:
	const int ROWS;

private:
	std::vector<Vector> x;
};