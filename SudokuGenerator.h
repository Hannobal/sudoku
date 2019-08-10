#ifndef SUDOKUGENERATOR_H_
#define SUDOKUGENERATOR_H_

#include "Sudoku.h"
#include "DataContainerMacro.h"
#include <random>

class SudokuGenerator {

public:

	DATA_CONTAINER(Settings,
			((minFilledRatio, float, 0.0, float))
			((maxFilledRatio, float, 1.0, float))
			((minAmbiguities, size_t, 0, size_t))
			((maxAmbiguities, size_t, 0, size_t)),
			(static Settings easy;)
			(static Settings medium;)
			(static Settings hard;)
			(static Settings extreme;))

	SudokuGenerator(Settings const& settings, Sudoku && sudoku);

	SudokuGenerator(Settings && settings, Sudoku && sudoku);

	bool generate();

	Sudoku & getSudoku() { return m_sudoku; }

	Sudoku & getSolution() { return m_solution; }

	Sudoku const& sudoku() const { return m_sudoku; }

	size_t nbAttempts() const { return m_nbAttempts; }

private:
	void scramble();

	bool tryRemoveSolution(Sudoku sudoku, int depth);

	Settings m_settings;
	Sudoku m_sudoku;
	Sudoku m_solution;
    float m_targetFilledRatio;
    size_t m_nbAttempts;

    std::random_device m_randomDevice;
    std::default_random_engine m_randomEngine;
};

#endif /* SUDOKUGENERATOR_H_ */
