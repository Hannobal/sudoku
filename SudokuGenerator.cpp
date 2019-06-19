#include "SudokuGenerator.h"
#include "SudokuScrambler.h"
#include "SudokuSolver.h"
#include <algorithm>

SudokuGenerator::Settings::Settings() :
m_minFilledRatio(0.0f),
m_maxFilledRatio(1.0f),
m_minAmbiguities(0),
m_maxAmbiguities(0)
{}

SudokuGenerator::Settings::Settings(
		float minFilledRatio,
		float maxFilledRatio,
		int minAmbiguities,
		int maxAmbiguities) :
		m_minFilledRatio(minFilledRatio),
		m_maxFilledRatio(maxFilledRatio),
		m_minAmbiguities(minAmbiguities),
		m_maxAmbiguities(maxAmbiguities)
{}

SudokuGenerator::Settings SudokuGenerator::Settings::easy(
		0.35f, 0.5f, 0, 0
);

SudokuGenerator::Settings SudokuGenerator::Settings::medium(
		0.2f, 0.4f, 0, 0
);

SudokuGenerator::Settings SudokuGenerator::Settings::hard(
		0.2f, 0.4f, 1, 1
);

SudokuGenerator::Settings SudokuGenerator::Settings::extreme(
		0.1f, 0.3f, 1, 2
);


SudokuGenerator::SudokuGenerator(Settings const& settings, Sudoku && sudoku) :
		m_settings(settings),
		m_sudoku(sudoku),
		m_randomEngine(m_randomDevice())
{}

SudokuGenerator::SudokuGenerator(Settings && settings, Sudoku && sudoku) :
		m_settings(settings),
		m_sudoku(sudoku),
		m_randomEngine(m_randomDevice())
{}

void SudokuGenerator::generate() {
	m_sudoku.trivialSolution();
	SudokuScrambler scrambler(m_sudoku);
	scrambler.scramble();
	m_sudoku=scrambler.sudoku();
}

bool SudokuGenerator::tryRemoveSolution(bool allowAmbiguity) {
	std::vector<size_t> solvedFields(m_sudoku.nbSolved());
	size_t nFields = m_sudoku.nbFields();
	size_t f(0);
	for(size_t fieldIndex(0); fieldIndex < nFields; fieldIndex++)
		if(!m_sudoku.isSolved(fieldIndex))
			solvedFields[f] = fieldIndex;

	std::shuffle(solvedFields.begin(), solvedFields.end(), m_randomEngine);
	for(size_t fieldIndex : solvedFields) {
		SudokuSolver solver(m_sudoku,SudokuSolver::Mode::Deterministic,0);
		solver.getWorkingVersion().clearSolution(fieldIndex);
		// check if it's still possible to solve this
		SudokuSolver::Result result = solver.solve();
		if(result==SudokuSolver::Result::solved)
			break;
	}
}
