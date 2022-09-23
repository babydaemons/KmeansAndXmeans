#pragma once
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>

void Check(double x) {
#if defined(__NUMERIC_DEBUG)
	if (isinf(x)) {
		std::cerr << "ERROR: Inf detected!" << std::endl;
	}
	if (isnan(x)) {
		std::cerr << "ERROR: NaN detected!" << std::endl;
	}
#endif
}

class Vector : public std::vector<double> {
public:
	static void Initialize(int colums, int excludes) {
		COLUMNS = colums;
		EXCLUDES = excludes;
		DIMENSION = colums - excludes;
	}

	Vector() : std::vector<double>(COLUMNS) { }

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
		for (int i = 0; i < DIMENSION; ++i) {
			Check(a[i]);
			Check(b[i]);
			sum += (a[i] - b[i]) * (a[i] - b[i]);
		}
		return std::sqrt(sum);
	}

	Vector& operator+=(const Vector& a0) {
		std::vector<double>& x(*this);
		#pragma omp parallel for
		for (int i = 0; i < COLUMNS; ++i) {
			Check(x[i]);
			Check(a0[i]);
			x[i] += a0[i];
		}
		return *this;
	}

	Vector& operator/=(double d0) {
		std::vector<double>& x(*this);
		#pragma omp parallel for
		for (int i = 0; i < COLUMNS; ++i) {
			x[i] /= d0;
			Check(x[i]);
		}
		return *this;
	}

	Vector& operator=(double x0) {
		std::vector<double>& x(*this);
		#pragma omp parallel for
		for (auto i = 0; i < COLUMNS; ++i) {
			x[i] = x0;
			Check(x[i]);
		}
		return *this;
	}

private:
	static int COLUMNS;
	static int EXCLUDES;
	static int DIMENSION;
};

int Vector::COLUMNS;
int Vector::EXCLUDES;
int Vector::DIMENSION;
