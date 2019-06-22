#ifndef SUDOKUGENERATOR_H_
#define SUDOKUGENERATOR_H_

#include "Sudoku.h"
#include <random>

class SudokuGenerator {

public:

	class Settings {
	public:
		Settings();
		Settings(
			float minFilledRatio,
			float maxFilledRatio,
			int minAmbiguities,
			int maxAmbiguities
		);

		float m_minFilledRatio;
		float m_maxFilledRatio;
		int m_minAmbiguities;
		int m_maxAmbiguities;

		static Settings easy;
		static Settings medium;
		static Settings hard;
		static Settings extreme;
	};

	SudokuGenerator(Settings const& settings, Sudoku && sudoku);

	SudokuGenerator(Settings && settings, Sudoku && sudoku);

	bool generate();

	Sudoku & getSudoku() { return m_sudoku; }

	Sudoku & getSolution() { return m_solution; }

	Sudoku const& sudoku() const { return m_sudoku; }



private:
	void scramble();

	bool tryRemoveSolution(Sudoku sudoku, int depth);

	Settings m_settings;
	Sudoku m_sudoku;
	Sudoku m_solution;
    float m_targetFilledRatio;

    std::random_device m_randomDevice;
    std::default_random_engine m_randomEngine;
};

#endif /* SUDOKUGENERATOR_H_ */
