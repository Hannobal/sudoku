#ifndef SUDOKU_H_
#define SUDOKU_H_

#include "DynamicBitset.h"
#include <set>
#include <iostream>
#include <iomanip>
#include <vector>

/************************************
 *
 ************************************/

struct GridPoint {

	GridPoint(size_t px, size_t py) :
		x(px), y(py) {}

	GridPoint() :
			x(0), y(0) {}

	size_t x;
	size_t y;
};

class Sudoku {

public:

	typedef std::vector<size_t> FieldGroup;

	Sudoku();
	Sudoku(size_t length);
	Sudoku(size_t length, size_t blockLength);

	size_t size() const {return m_size;}

	bool isPossible(size_t fieldIndex, size_t i) const {
		return m_possible[fieldIndex][i-1];
	}

	bool isPossible(GridPoint const& p, size_t i) const {
		return isPossible(xyToIndex(p),i);
	}

	size_t nbPossible(size_t fieldIndex) const {
		return m_possible[fieldIndex].count();
	}

	size_t nbPossible(GridPoint const& p) const {
		return nbPossible(xyToIndex(p));
	}

	size_t nbSolved() { return m_nbSolved; }

	bool isSolved(size_t fieldIndex) const {
		return m_solution[fieldIndex]!=0;
	}

	bool isSolved(GridPoint const& p) const {
		return isSolved(xyToIndex(p));
	}

	// returns true only if all fields are solved
	bool isSolved() {
		return m_nbSolved == m_size*m_size;
	}

	void getPossibleNumbers(size_t fieldIndex, std::vector<size_t>& numbers);

	void print(size_t indent=0) const;
	void printPossible(size_t number, size_t indent=0) const;

	size_t getSolution(size_t fieldIndex) {return m_solution[fieldIndex];}
	size_t getSolution(GridPoint const& p) {return getSolution(xyToIndex(p));}

	void enterSolution(size_t fieldIndex, size_t number);
	void enterSolution(GridPoint const& p, size_t number) {
		enterSolution(xyToIndex(p),number);
	}

	void clearSolution(size_t fieldIndex);

	void clearSolution(GridPoint const& p) {
		clearSolution(xyToIndex(p));
	}

	// get the field indices for a row
	void getRow(size_t fieldIndex, FieldGroup& row) const;
	void getRow(GridPoint p, FieldGroup& row) const;

	// get the field indices for a column
	void getColumn(size_t fieldIndex, FieldGroup& col) const;
	void getColumn(GridPoint p, FieldGroup& col) const;

	// get the field indices for the block containing the field
	void getBlock(size_t fieldIndex, FieldGroup& block) const;
	void getBlock(GridPoint p, FieldGroup& block) const;

    friend std::istream& operator>>( std::istream  &input, Sudoku &sudoku );

private:

	size_t xyToIndex(GridPoint const& p) const {
		return p.x*m_size + p.y;
	}

	GridPoint indexToXY(size_t index) const {
		return GridPoint(
				index/m_size,
				index%m_size
		);
	}

	// check possibility based on current solution
	bool checkPossible(size_t fieldIndex, size_t i);

	void makePossible(size_t fieldIndex, size_t i) {
		if(checkPossible(fieldIndex,i))
			m_possible[fieldIndex].set(i-1);
	}

	void makeImpossible(size_t fieldIndex, size_t i) {
		m_possible[fieldIndex].reset(i-1);
	}

	// factorizes an integer number into two integers that are
	// closest to the square root of the input number
	// f1 is smaller or equal f2
	void nearSquareFactors(size_t n, size_t& f1, size_t &f2);

	// the length of each row/column in the Sudoku
	// number of fields is size squared
	size_t m_size;
	size_t m_blockLength;
	size_t m_blockHeight;

	size_t m_nbSolved;

	// each field has a bitset of length "size" which for each number
	// states whether it's still possible to enter this number
	std::vector<DynamicBitset<>> m_possible;

	// the actual solution of the Sudoku
	// unsolved fields have value 0
	std::vector<size_t> m_solution;

};

#endif /* SUDOKU_H_ */
