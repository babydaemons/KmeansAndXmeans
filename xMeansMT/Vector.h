#pragma once
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>

class Vector : public std::vector<double> {
public:
	static void Initialize(int colums, int excludes = 0) {
		COLUMNS = colums;
		EXCLUDES = excludes;
		DIMENSION = colums - excludes;
	}

	Vector() : std::vector<double>(COLUMNS), index(-1), label(-1) { }

	Vector(double x0) : std::vector<double>(COLUMNS), index(-1), label(-1) {
		std::vector<double>& x(*this);
		#pragma omp parallel for
		for (auto i = 0; i < COLUMNS; ++i) {
			x[i] = x0;
		}
	}

	void  Read(std::istream& istream) {
		std::vector<double>& x(*this);
		istream.read(reinterpret_cast<char*>(&(x[0])), Vector::COLUMNS * sizeof(double));
	}

	friend std::istream& operator>>(std::istream& istream, Vector& vector) {
		for (auto i = 0; i < COLUMNS; ++i) {
			istream >> vector[i];
		}
		return istream;
	}

	void Write(std::ostream& ostream) const {
		const std::vector<double>& x(*this);
		ostream.write(reinterpret_cast<const char*>(&(x[0])), Vector::COLUMNS * sizeof(double));
	}

	friend std::ostream& operator<<(std::ostream& ostream, const Vector& vector) {
		ostream << std::fixed;
		for (auto i = 0; i < COLUMNS; ++i) {
			ostream << std::setprecision(8) << vector[i];
			ostream << (i < COLUMNS - 1 ? '\t' : '\n');
		}
		return ostream;
	}

	friend double Dist(const Vector& a, const Vector& b) {
		double sum = 0;
		for (int i = 0; i < Vector::DIMENSION; ++i) {
			sum += std::pow(a[i] - b[i], 2.0);
		}
		return std::sqrt(sum);
	}

	friend double Norm(const Vector& a) {
		double sum = 0;
		for (int i = 0; i < Vector::DIMENSION; ++i) {
			sum += a[i] * a[i];
		}
		return std::sqrt(sum);
	}

	Vector& operator+=(const Vector& a0) {
		std::vector<double>& x(*this);
		#pragma omp parallel for
		for (int i = 0; i < Vector::DIMENSION; ++i) {
			x[i] += a0[i];
		}
		return *this;
	}

	Vector& operator-=(const Vector& a0) {
		std::vector<double>& x(*this);
		#pragma omp parallel for
		for (int i = 0; i < Vector::DIMENSION; ++i) {
			x[i] -= a0[i];
		}
		return *this;
	}

	Vector& operator/=(double x0) {
		std::vector<double>& x(*this);
		#pragma omp parallel for
		for (int i = 0; i < Vector::DIMENSION; ++i) {
			x[i] /= x0;
		}
		return *this;
	}

	Vector& operator/=(const Vector& x0) {
		std::vector<double>& x(*this);
		#pragma omp parallel for
		for (int i = 0; i < Vector::DIMENSION; ++i) {
			x[i] /= x0[i];
		}
		return *this;
	}

	Vector& operator*=(const Vector& x0) {
		std::vector<double>& x(*this);
		#pragma omp parallel for
		for (int i = 0; i < Vector::DIMENSION; ++i) {
			x[i] *= x0[i];
		}
		return *this;
	}

public:
	int index;
	int label;

public:
	static int COLUMNS;
	static int EXCLUDES;
	static int DIMENSION;
};

int Vector::COLUMNS;
int Vector::EXCLUDES;
int Vector::DIMENSION;
