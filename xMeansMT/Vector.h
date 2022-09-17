#pragma once
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>

class Vector {
public:
	static void Initialize(int colums, int excludes) {
		COLUMNS = colums;
		EXCLUDES = excludes;
		DIMENSION = colums - excludes;
	}

	Vector() : label(-1), x(COLUMNS) { }

	void  Read(std::istream& istream) {
		istream.read(reinterpret_cast<char*>(&(x[0])), Vector::COLUMNS * sizeof(double));
		label = -1;
	}

	friend std::istream& operator>>(std::istream& istream, Vector& vector) {
		for (auto i = 0; i < COLUMNS; ++i) {
			istream >> vector.x[i];
		}
		vector.label = -1;
		return istream;
	}

	void Write(std::ostream& ostream) {
		ostream.write(reinterpret_cast<const char*>(&(x[0])), Vector::COLUMNS * sizeof(double));
	}

	friend std::ostream& operator<<(std::ostream& ostream, Vector& vector) {
		ostream << std::fixed;
		for (auto i = 0; i < COLUMNS; ++i) {
			ostream << std::setprecision(8) << vector.x[i];
			ostream << (i < COLUMNS - 1 ? '\t' : '\n');
		}
		return ostream;
	}

	friend double Dist(const Vector& a, const Vector& b) {
		double sum = 0;
		for (int i = 0; i < Vector::DIMENSION; ++i) {
			sum += (a[i] - b[i]) * (a[i] - b[i]);
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
		#pragma omp parallel for
		for (int i = 0; i < Vector::COLUMNS; ++i) {
			x[i] += a0[i];
		}
		return *this;
	}

	Vector& operator/=(double d0) {
		#pragma omp parallel for
		for (int i = 0; i < Vector::COLUMNS; ++i) {
			x[i] /= d0;
		}
		return *this;
	}

	Vector& operator=(double x0) {
		#pragma omp parallel for
		for (auto i = 0; i < COLUMNS; ++i) {
			x[i] = x0;
		}
		return *this;
	}

	double operator[](int i) const {
		return x[i];
	}

	double& operator[](int i) {
		return x[i];
	}

public:
	int label;

private:
	std::vector<double> x;

	static int COLUMNS;
	static int EXCLUDES;
	static int DIMENSION;
};

int Vector::COLUMNS;
int Vector::EXCLUDES;
int Vector::DIMENSION;
