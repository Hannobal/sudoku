#include "SudokuGenerator.h"
#include "SudokuScrambler.h"
#include <algorithm>

SudokuGenerator::SudokuGenerator(Settings const& settings, Sudoku && sudoku) :
		m_settings(settings),
		m_sudoku(sudoku),
		m_targetFilledRatio(0.0f),
		m_nbAttempts(0),
		m_randomEngine(m_randomDevice())
{

}

SudokuGenerator::SudokuGenerator(Settings && settings, Sudoku && sudoku) :
		m_settings(settings),
		m_sudoku(sudoku),
		m_targetFilledRatio(0.0f),
		m_nbAttempts(0),
		m_randomEngine(m_randomDevice())
{}

bool SudokuGenerator::generate() {
	m_settings.nextIterOnChange(true);
	m_settings.maxResults(2);
	m_sudoku.trivialSolution();
	scramble();
	std::uniform_real_distribution<float> randomFloat(
			m_settings.minFilledRatio(), m_settings.maxFilledRatio());
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

//	std::cout << "depth=" << depth << " " <<  "solvedFields.size()="<< solvedFields.size() << std::endl;
//	for(size_t fieldIndex : solvedFields)
//		std::cout << " " << fieldIndex;
//	std::cout << std::endl;

	std::shuffle(solvedFields.begin(), solvedFields.end(), m_randomEngine);
	for(size_t fieldIndex : solvedFields) {
//		std::cout << "remove field " << fieldIndex << std::endl;
		sudoku.clearSolution(fieldIndex);
		m_nbAttempts++;
//		std::cout << sudoku;
		SudokuSolver solver(
				m_settings,
				sudoku);
		// check if it's still possible to solve this
		SudokuSolver::Result result = solver.solve();
		std::cout << "nsolved="<<sudoku.nbSolved() << " SudokuSolver::Result=" << result << std::endl;

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
		} else if(solver.getSolved()[0].nbGuesses()<=m_settings.maxNbGuesses()) {
			m_solution = solver.getSolved()[0];
			m_sudoku = sudoku;
			return true;
		}
	}
	return false;
}
