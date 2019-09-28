#ifndef SUDOKUGENERATOR_H_
#define SUDOKUGENERATOR_H_

#include "Sudoku.h"
#include "DataContainerMacro.h"
#include "SudokuSolver.h"
#include <random>

class SudokuGenerator {

public:

	DATA_CONTAINER_DERIVED(Settings,
			((solverSettings,SudokuSolver::Settings, public)),
			((minFilledRatio, float, 0.0, float))
			((maxFilledRatio, float, 1.0, float))
			((minNbGuesses, size_t, 0, size_t)),
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

	/*
	 * iterates recursively over all combinations of solved/unsolved fields
	 * and returns true on the first sudoku it finds that matches the settings.
	 * The number of possible combinations can be calculated by the binomial
	 * coefficient nbFields!/(targetSolved!*(nbFields-targetSolved)!)
	 * Despite being deterministic, this may not be best way to go
	 */
	bool tryRemoveSolutionDeterministic(
			Sudoku sudoku,
			size_t index,
			size_t depth); // depth is for debugging only

	/*
	 * randomly clears m_targetNbSolvedFields from the filled sudoku and checks
	 * if it can be solved with the given settings. Works well for 9x9 sudokus
	 * and appears to be faster than the deterministic method.
	 */
	bool tryRemoveSolutionRandom();

	Settings m_settings;
	Sudoku m_sudoku;
	Sudoku m_solution;
    float m_targetNbSolvedFields;
    size_t m_nbAttempts;
    std::vector<size_t> m_processingOrder;

    std::random_device m_randomDevice;
    std::default_random_engine m_randomEngine;
};

#endif /* SUDOKUGENERATOR_H_ */
