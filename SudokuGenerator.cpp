#include "SudokuGenerator.h"
#include "SudokuScrambler.h"
#include <algorithm>

SudokuGenerator::SudokuGenerator(Settings const& settings, Sudoku && sudoku) :
		m_settings(settings),
		m_sudoku(sudoku),
		m_targetNbSolvedFields(0),
		m_nbAttempts(0),
		m_processingOrder(m_sudoku.nbFields()),
		m_randomEngine(m_randomDevice())
{
}

SudokuGenerator::SudokuGenerator(Settings && settings, Sudoku && sudoku) :
		m_settings(settings),
		m_sudoku(sudoku),
		m_targetNbSolvedFields(0),
		m_nbAttempts(0),
		m_processingOrder(m_sudoku.nbFields()),
		m_randomEngine(m_randomDevice())
{
}

bool SudokuGenerator::generate() {
	m_settings.nextIterOnChange(true);
	m_settings.maxResults(2);
	m_sudoku.trivialSolution();
	scramble();

	std::uniform_real_distribution<float> randomFloat(
			m_settings.minFilledRatio(), m_settings.maxFilledRatio());
	m_targetNbSolvedFields = static_cast<size_t>(
			randomFloat(m_randomEngine)*
			static_cast<float>(m_sudoku.nbFields())
	);

	std::cout << "m_targetNbSolvedFields = " << m_targetNbSolvedFields << std::endl;

	std::iota (std::begin(m_processingOrder), std::end(m_processingOrder), 0);
	std::shuffle(m_processingOrder.begin(), m_processingOrder.end(), m_randomEngine);

	while(!tryRemoveSolutionRandom()) {
		if(m_nbAttempts%10000==0) std::cout << "attempt "<<m_nbAttempts << std::endl;
	}
	return true;
//	return tryRemoveSolution(m_sudoku, 0, 0);
}

void SudokuGenerator::scramble() {
	SudokuScrambler scrambler(m_sudoku);
	scrambler.scramble();
	m_sudoku=scrambler.sudoku();
}

bool SudokuGenerator::tryRemoveSolutionDeterministic(
		Sudoku sudoku,
		size_t index,
		size_t depth) {
	// check if we still have enough numbers left to clear to the target
	if(static_cast<double>((sudoku.nbSolved())-(m_processingOrder.size() - index))
			> m_targetNbSolvedFields) return false;

	if(depth<4)
	std::cout << std::setw(depth*2) << m_processingOrder[index] << std::endl;

	sudoku.clearSolution(m_processingOrder[index]);
	m_nbAttempts++;

	// check if it's still possible to solve this
	SudokuSolver solver(
			m_settings,
			sudoku);
	SudokuSolver::Result result = solver.solve();

	// no solution => discard and next try;
	if(result!=SudokuSolver::Result::solved)
		return false;

	// multiple solutions => discard and next try;
	if(solver.getSolved().size()>1)
		return false;

	if(sudoku.nbSolved() == m_targetNbSolvedFields) {
		m_sudoku = sudoku;
		m_solution = solver.getSolved()[0];
		return true;
	}

	size_t max=m_processingOrder.size()-index;
	for(size_t i=1; i<max; i++)
		if(tryRemoveSolutionDeterministic(sudoku, index+i, depth+1))
			return true;

	return false;
}

bool SudokuGenerator::tryRemoveSolutionRandom() {
	m_nbAttempts++;
	Sudoku sudoku(m_sudoku);
	std::shuffle(m_processingOrder.begin(), m_processingOrder.end(), m_randomEngine);
	for(int i=0; i<m_targetNbSolvedFields; i++) {
		sudoku.clearSolution(m_processingOrder[i]);
	}
	// check if it's still possible to solve this
	SudokuSolver solver(
			m_settings,
			sudoku);
	SudokuSolver::Result result = solver.solve();

	// no solution => discard and next try;
	if(result!=SudokuSolver::Result::solved)
		return false;

	// multiple solutions => discard and next try;
	if(solver.getSolved().size()>1)
		return false;

	m_sudoku = sudoku;
	m_solution = solver.getSolved()[0];
	return true;
}
