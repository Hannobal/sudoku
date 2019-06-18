#ifndef SUDOKUSCRAMBLER_H_
#define SUDOKUSCRAMBLER_H_

#include "Sudoku.h"
#include <random>

class SudokuScrambler {
public:
	SudokuScrambler(Sudoku const& sudoku);

	Sudoku const& sudoku() { return m_sudoku; }

	size_t totSteps() { return m_totSteps; }

	void scramble();

	void scramble(size_t steps);

private:

	void swapRows();
	void swapColumns();
	void swapBlockRows();
	void swapBlockColumns();

	Sudoku m_sudoku;
	size_t m_totSteps;

    std::random_device m_randomDevice;
    std::default_random_engine m_randomEngine;
    std::uniform_int_distribution<size_t> m_uniformDistRowCol;
    std::uniform_int_distribution<size_t> m_uniformDistBlockRows;
    std::uniform_int_distribution<size_t> m_uniformDistBlockColumns;
};

#endif /* SUDOKUSCRAMBLER_H_ */
