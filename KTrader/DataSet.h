#pragma once
#include <fstream>
#include "Vector.h"

class DataSet : public std::vector<Vector> {
public:
	DataSet() {	}

	void Read(std::istream& istream) {
		while (!istream.eof()) {
			Vector a;
			a.Read(istream);
			push_back(a);
		}
	}

	friend std::istream& operator>>(std::istream& istream, DataSet& xx) {
		while (!istream.eof()) {
			Vector a;
			istream >> a;
			xx.push_back(a);
		}
		return istream;
	}

	void Write(std::ostream& ostream) {
		for (size_t i = 0; i < size(); ++i) {
			at(i).Write(ostream);
		}
	}

	friend std::ostream& operator<<(std::ostream& ostream, DataSet& xx) {
		for (size_t i = 0; i < xx.size(); ++i) {
			ostream << xx[i];
		}
		return ostream;
	}
};