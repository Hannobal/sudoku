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
		0.3f, 0.4f, 0, 0
);

SudokuGenerator::Settings SudokuGenerator::Settings::hard(
		0.3f, 0.4f, 1, 1
);

SudokuGenerator::Settings SudokuGenerator::Settings::extreme(
		0.1f, 0.3f, 1, 2
);


SudokuGenerator::SudokuGenerator(Settings const& settings, Sudoku && sudoku) :
		m_settings(settings),
		m_sudoku(sudoku),
		m_targetFilledRatio(0.0f),
		m_randomEngine(m_randomDevice())
{}

SudokuGenerator::SudokuGenerator(Settings && settings, Sudoku && sudoku) :
		m_settings(settings),
		m_sudoku(sudoku),
		m_targetFilledRatio(0.0f),
		m_randomEngine(m_randomDevice())
{}

bool SudokuGenerator::generate() {
	m_sudoku.trivialSolution();
	scramble();
	std::uniform_real_distribution<float> randomFloat(
			m_settings.m_minFilledRatio, m_settings.m_maxFilledRatio);
	m_targetFilledRatio = randomFloat(m_randomEngine);
	std::cout << "m_targetFilledRatio = " << m_targetFilledRatio << std::endl;
	return tryRemoveSolution(m_sudoku, 0);
}

void SudokuGenerator::scramble() {
	SudokuScrambler scrambler(m_sudoku);
	scrambler.scramble();
	m_sudoku=scrambler.sudoku();
}

/**
 * Recursive function to iteratively remove one entry. Each recursion
 * corresponds to one removed entry. Returns true on success and false
 * if the criteria from the settings could not be met.
 */
bool SudokuGenerator::tryRemoveSolution(Sudoku sudoku, int depth) {
	std::vector<size_t> solvedFields(sudoku.nbSolved());
	sudoku.getSolvedOrUnsolvedFields(solvedFields, true);

	std::cout << "depth=" << depth << " " <<  "solvedFields.size()="<< solvedFields.size() << std::endl;
	for(size_t fieldIndex : solvedFields)
		std::cout << " " << fieldIndex;
	std::cout << std::endl;
	if(depth>60)
		throw std::runtime_error("DEBUG: recursion too deep");

	std::shuffle(solvedFields.begin(), solvedFields.end(), m_randomEngine);
	for(size_t fieldIndex : solvedFields) {
		std::cout << "remove field " << fieldIndex << std::endl;
		sudoku.clearSolution(fieldIndex);
		std::cout << sudoku;
		SudokuSolver solver(
				sudoku,
				SudokuSolver::Mode::Deterministic,
				m_settings.m_maxAmbiguities);
		// check if it's still possible to solve this
		SudokuSolver::Result result = solver.solve();

		// no solution => discard and next try;
		if(result!=SudokuSolver::Result::solved)
			continue;

		// multiple solutions => discard and next try;
		if(solver.getSolved().size()>1)
			continue;

		if(sudoku.solvedRatio()>m_targetFilledRatio) {
			// if the next removal cannot fulfill the target conditions
//			if(static_cast<float>(sudoku.nbSolved()-1)/
//					static_cast<float>(sudoku.nbFields())<=m_targetFilledRatio &&
//					m_settings.m_minAmbiguities - solver.getSolved()[0].nbGuesses() > 1)
//				return false;
			if(tryRemoveSolution(sudoku, depth+1))
				return true;
		} else if(solver.getSolved()[0].nbGuesses()<=m_settings.m_maxAmbiguities) {
			m_solution = solver.getSolved()[0];
			m_sudoku = sudoku;
			return true;
		}
	}
	return false;
}
