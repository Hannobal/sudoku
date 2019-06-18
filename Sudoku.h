#ifndef SUDOKU_H_
#define SUDOKU_H_

#include "DynamicBitset.h"
#include <set>
#include <iostream>
#include <iomanip>
#include <vector>

/**
 * A point on a 2D grid with integral coordinates
 */

struct GridPoint {

	/**
	 * Constructor to initialize both coordinates
	 */
	GridPoint(
			size_t px, /**< x-coordinate */
			size_t py  /**< y-coordinate */
	) :
		x(px), y(py) {}

	/**
	 * Default constructor initializes to (0,0)
	 */
	GridPoint() :
			x(0), y(0) {}

	size_t x; /**< x-coordinate */
	size_t y; /**< y-coordinate */
};

/**
 * The principal Sudoku class that implements the rules.
 * The Sudoku may have an arbitrary length N provided that it
 * is factorizable into a block width and height (i.e. not a prime number).
 * Fields are indexed column-major.
 */

class Sudoku {

public:

	/*! A field group may be a row, column or block */
	typedef std::vector<size_t> FieldGroup;

	/*! Default constructor creates sudoku of size 0 */
	Sudoku();

	/**
	 * Constructor to create a (length x length) Sudoku.
	 * The block width is chosen automatically, so that width and height
	 * are as close to sqrt(length) as possible with width >= height.
	 * Will throw an exception if length is a prime number, since an
	 * N*1 block would be the same as a row.
	 */
	Sudoku(size_t length);

	/**
	 * Constructor to create a (length x length) Sudoku with explicit
	 * block length (and thus block height).
	 * Will throw an exception if blockWidth is not a divisor of length or
	 * if either block width or height is one, since an N*1 block would
	 * be the same as a row.
	 */
	Sudoku(size_t length, size_t blockWidth);

	/** Returns the side length of the Sudoku. */
	size_t sideLength() const {return m_sideLength;}

	/// Returns the total number of fields in the sudoku
	/// (=sideLength*sideLength).
	size_t nbFields() const {return m_sideLength*m_sideLength;}

	/*
	 * Check if a given number can be entered into a field.
	 * Returns false if the field already has a number entered.
	 */
	bool isPossible(size_t fieldIndex, size_t i) const {
		return m_possible[fieldIndex][i-1];
	}

	/*
	 * Check if a given number can be entered into a field.
	 * Returns false if the field already has a number entered.
	 */
	bool isPossible(GridPoint const& p, size_t i) const {
		return isPossible(xyToIndex(p),i);
	}

	/*
	 * Count the numbers that could potentially still be entered
	 * into a field. Returns 0 if the field already has a number entered.
	 */
	size_t nbPossible(size_t fieldIndex) const {
		return m_possible[fieldIndex].count();
	}

	/*
	 * Count the numbers that could potentially still be entered
	 * into a field. Returns 0 if the field already has a number entered.
	 */
	size_t nbPossible(GridPoint const& p) const {
		return nbPossible(xyToIndex(p));
	}

	/*
	 * Count the numbers that could potentially still be entered
	 * into a field. Returns 0 if the field already has a number entered.
	 */
	size_t nbSolved() { return m_nbSolved; }

	 /** Check if the field already has a number entered. */
	bool isSolved(size_t fieldIndex) const {
		return m_solution[fieldIndex]!=0;
	}

	 /** Check if the field already has a number entered. */
	bool isSolved(GridPoint const& p) const {
		return isSolved(xyToIndex(p));
	}

	/** returns true only if all fields are solved */
	bool isSolved() {
		return m_nbSolved == m_sideLength*m_sideLength;
	}

	/** returns a vector containing all the numbers that can potentially
	 * still be entered into a field */
	void getPossibleNumbers(size_t fieldIndex, std::vector<size_t>& numbers);

	/** Prints the sudoku to stdout */
	void print(size_t indent=0) const;

	/** Prints a boolean map of fields where the number
	 * can potentially still be entered */
	void printPossible(size_t number, size_t indent=0) const;

	/** Returns the number that has been entered into the field.
	 * Returns zero for empty fields */
	size_t getSolution(size_t fieldIndex) {return m_solution[fieldIndex];}

	/** Returns the number that has been entered into the field.
	 * Returns zero for empty fields */
	size_t getSolution(GridPoint const& p) {return getSolution(xyToIndex(p));}

	/** Enter a number into a field. Throws if the number conflicts the rules */
	void enterSolution(size_t fieldIndex, size_t number);

	/** Enter a number into a field. Throws if the number conflicts the rules */
	void enterSolution(GridPoint const& p, size_t number) {
		enterSolution(xyToIndex(p),number);
	}

	/** Removes the number entered in a certain field and re-evaluates the
	 * possibilities for all numbers for affected fields (self, column, row, block) */
	void clearSolution(size_t fieldIndex);

	/** Removes the number entered in a certain field and re-evaluates the
	 * possibilities for all numbers for affected fields (self, column, row, block) */
	void clearSolution(GridPoint const& p) {
		clearSolution(xyToIndex(p));
	}

	/** Replaces the content with a trivial Sudoku. */
	void trivialSolution();

	/** get the field indices for a row */
	void getRow(size_t fieldIndex, FieldGroup& row) const;
	/** get the field indices for a row */
	void getRow(GridPoint p, FieldGroup& row) const;

	/** get the field indices for a column */
	void getColumn(size_t fieldIndex, FieldGroup& col) const;
	/** get the field indices for a column */
	void getColumn(GridPoint p, FieldGroup& col) const;

	/** get the field indices for the block containing the field fieldIndex */
	void getBlock(size_t fieldIndex, FieldGroup& block) const;

	/** get the field indices for the block containing the field at GridPoint p */
	void getBlock(GridPoint p, FieldGroup& block) const;

	/**
	 * Read a Sudoku from a stream. Format is as follows:
	 * - Sudoku must be a square array of fields
	 * - each line corresponds to one row
	 * - fields in each row/line separated by whitespace
	 * - unsolved fields as "?"
	 * - no comments allowed
	 */
    friend std::istream& operator>>( std::istream  &input, Sudoku &sudoku );
    friend std::ostream& operator<<( std::ostream  &output, Sudoku &sudoku );

private:

	/** convert GridPoint p to the corresponding field index */
	size_t xyToIndex(GridPoint const& p) const {
		return p.x*m_sideLength + p.y;
	}

	/** convert a field index to the corresponding GridPoint */
	GridPoint indexToXY(size_t index) const {
		return GridPoint(
				index/m_sideLength,
				index%m_sideLength
		);
	}

	/** check possibility based on current solution */
	bool checkPossible(size_t fieldIndex, size_t i);

	/** change the possibility to enter number i into the
	 * field with index fieldIndex to true */
	void makePossible(size_t fieldIndex, size_t i) {
		if(checkPossible(fieldIndex,i))
			m_possible[fieldIndex].set(i-1);
	}

	/** change the possibility to enter number i into the
	 * field with index fieldIndex to false */
	void makeImpossible(size_t fieldIndex, size_t i) {
		m_possible[fieldIndex].reset(i-1);
	}

	/** Factorizes an integer number n into two integer factors
	 *  f1, f2 that are closest to the square root of the input number
	 *  with f1 <= f2 */
	void nearSquareFactors(size_t n, size_t& f1, size_t &f2);

	size_t m_sideLength; /**< The length of each row/column in the Sudoku */
	size_t m_blockWidth; /**< The width of a block in the Sudoku */
	size_t m_blockHeight; /**< The height of a block in the Sudoku */

	size_t m_nbSolved; /**< The number of fields that have already been solved */

	// each field has a bitset of length "size" which for each number
	// states whether it's still possible to enter this number
	std::vector<DynamicBitset<>> m_possible;

	// the actual solution of the Sudoku
	// unsolved fields have value 0
	std::vector<size_t> m_solution;

};

#endif /* SUDOKU_H_ */
