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
 *
 * Terminology:
 *
 * column: 0  1   2    3  4  5    6  7  8
 *       +----------+----------+----------+
 * row 0 | 0  9  18 | 27 36 45 | 54 63 72 |
 * row 1 | 1 10  19 | 28 37 46 | 55 64 73 | block row 0
 * row 2 | 2 11  20 | 29 38 47 | 56 65 74 |
 *       +----------+----------+----------+
 * row 3 | 3 12  21 | 30 39 48 | 57 66 75 |
 * row 4 | 4 13  22 | 31 40 49 | 58 67 76 | block row 1
 * row 5 | 5 14  23 | 32 41 50 | 59 68 77 |
 *       +----------+----------+----------+
 * row 6 | 6 15  24 | 33 42 51 | 60 69 78 |
 * row 7 | 7 16  25 | 34 43 52 | 61 70 79 | block row 2
 * row 8 | 8 17  26 | 35 44 53 | 62 71 80 |
 *       +----------+----------+----------+
 *          block      block      block
 *         column 0   column 1   column 2
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

	/** Returns the total number of fields
	 * (=sideLength*sideLength). */
	size_t nbFields() const {return m_sideLength*m_sideLength;}

	/** Returns the width of each block. */
	size_t blockWidth() const {return m_blockWidth;}

	/** Returns the height of each block. */
	size_t blockHeight() const {return m_blockHeight;}

	/** Returns the number of block rows. */
	size_t nbBlockRows() const {return m_nbBlockRows;}

	/** Returns the number of block rows. */
	size_t nbBlockColumns() const {return m_nbBlockColumns;}

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

	/* The number of fields that already contain a solution */
	size_t nbSolved() { return m_nbSolved; }

	/*
	 * Sets the number of guesses. In a solution process, there may be
	 * situations, where there is no definite solution and guessing may
	 * be necessary. A solver may set this to keep track of how many guesses
	 * were required.
	 */
	void nbGuesses(int guesses) { m_nbGuesses = guesses; }

	/*
	 * Returns the number of guesses. In a solution process, there may be
	 * situations, where there is no definite solution and guessing may
	 * be necessary. A solver may set this to keep track of how many guesses
	 * were required.
	 */
	int nbGuesses() { return m_nbGuesses; }

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
	void enterSolution(size_t fieldIndex, size_t number, bool guessed=false);

	/** Enter a number into a field. Throws if the number conflicts the rules */
	void enterSolution(GridPoint const& p, size_t number, bool guessed=false) {
		enterSolution(xyToIndex(p),number, guessed);
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

	void transpose();
	void invert();
	void flipHorizontal();
	void flipVertical();
	void rotateRight();
	void rotateLeft();

	/** swap two entire rows y1 and y2. Throws if the two
	 * rows are in different block rows*/
	void swapRows(size_t r1, size_t r2);

	/** swap two entire columns x1 and x2. Throws if the two
	 * columns are in different block columns*/
	void swapColumns(size_t c1, size_t c2);

	/** swap two entire block rows y1 and y2.*/
	void swapBlockRows(size_t br1, size_t br2);

	/** swap two entire block columns x1 and x2.*/
	void swapBlockColumns(size_t bc1, size_t bc2);

	bool checkSanity() const;

	/**
	 * Read a Sudoku from a stream. Format is as follows:
	 * - Sudoku must be a square array of fields
	 * - each line corresponds to one row
	 * - fields in each row/line separated by whitespace
	 * - unsolved fields as "?"
	 * - no comments allowed
	 */
    friend std::istream& operator>>( std::istream  &input, Sudoku &sudoku );
    friend std::ostream& operator<<( std::ostream  &output, Sudoku const& sudoku );

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

	void swapFields(size_t fieldIndex1, size_t fieldIndex2);

	void swapBlockOrientation();

	bool checkSanity(size_t fieldIndex, FieldGroup const& group) const;

	size_t m_sideLength; /**< The length of each row/column */
	size_t m_blockWidth; /**< The width of a block */
	size_t m_blockHeight; /**< The height of a block */
	size_t m_nbBlockRows; /**< The number of block rows */
	size_t m_nbBlockColumns; /**< The number of block columns */

	size_t m_nbSolved; /**< The number of fields that have already been solved */
	int m_nbGuesses; /**< The number of ambiguities, that have been resolved */

	// each field has a bitset of length "size" which for each number
	// states whether it's still possible to enter this number
	std::vector<DynamicBitset<>> m_possible;

	// the actual solution of the Sudoku
	// unsolved fields have value 0
	std::vector<size_t> m_solution;

};

#endif /* SUDOKU_H_ */
