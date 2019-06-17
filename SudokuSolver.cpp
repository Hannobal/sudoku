#include "SudokuSolver.h"
#include <iomanip>
#include <random>
#include <algorithm>

SudokuSolver::SudokuSolver(
		Sudoku const& sudoku,
		Mode mode,
		size_t maxResults,
		size_t depth
) :
		m_sudoku(sudoku),
		m_mode(mode),
		m_maxResults(maxResults),
		m_depth(depth+1)
{}

SudokuSolver::Result SudokuSolver::solve() {

	bool changed(true);
	Sudoku::FieldGroup group;

	while(changed) {
		changed=false;
		changed |= workFields();
		changed |= workRows();
		changed |= workColumns();
		changed |= workBlocks();

		if(m_sudoku.isSolved()) {
			m_results.push_back(m_sudoku);
			return Result::solved;
		}
	}

	if(m_mode==Mode::NoAmbiguityResolution)
		return Result::ambiguos;

	if(m_mode==Mode::Deterministic)
		educatedGuess();
	else if(m_mode==Mode::RandomGuessing)
		randomGuess();

	if(m_results.empty())
		return Result::impossible;
	else
		return Result::solved;
}

bool SudokuSolver::workFields() {
	bool changed(false);
	size_t nbFields=m_sudoku.sideLength()*m_sudoku.sideLength();
	for(size_t field=0; field<nbFields; field++)
		changed |= workField(field);
	return changed;
}

bool SudokuSolver::workRows()
{
	bool changed(false);
	Sudoku::FieldGroup group(m_sudoku.sideLength());
	for(size_t i=0; i<m_sudoku.sideLength(); i++) {
		m_sudoku.getRow(i*m_sudoku.sideLength(), group);
		changed |= workGroup(group);
	}
	return changed;
}

bool SudokuSolver::workColumns()
{
	bool changed(false);
	Sudoku::FieldGroup group(m_sudoku.sideLength());
	for(size_t i=0; i<m_sudoku.sideLength(); i++) {
		m_sudoku.getColumn(i, group);
		changed |= workGroup(group);
	}
	return changed;
}

bool SudokuSolver::workBlocks()
{
	bool changed(false);
	Sudoku::FieldGroup group(m_sudoku.sideLength());
	for(size_t i=0; i<m_sudoku.sideLength(); i++) {
		m_sudoku.getBlock(i*m_sudoku.sideLength(), group);
		changed |= workGroup(group);
	}
	return changed;
}

bool SudokuSolver::workField(size_t fieldIndex)
{
	if(m_sudoku.nbPossible(fieldIndex)==1) {
		for(size_t i=1; i<=m_sudoku.sideLength(); i++) {
			if(m_sudoku.isPossible(fieldIndex, i)) {
				m_sudoku.enterSolution(fieldIndex, i);
				return true;
			}
		}
		throw std::runtime_error("SudokuSolver::workField: did not find the number that should be possible in field "+
				std::to_string(fieldIndex));
	}
	return false;
}

bool SudokuSolver::workGroup(const Sudoku::FieldGroup& group)
{
	bool changed(false);
	// loop all numbers
	for(size_t i=1; i<=m_sudoku.sideLength(); i++) {
		size_t fieldIndex;
		size_t nbPossible(0);
		for(auto field : group) {
			if(m_sudoku.getSolution(field)==i) {
				// the number is alredy solved in the group
				nbPossible=0;
				break;
			}
			if(m_sudoku.isPossible(field, i)) {
				if(++nbPossible>1) break;
				fieldIndex=field;
			}
		}
		// if there is only one possibility to palace the number
		// we can safely enter it as the solution
		if(nbPossible==1) {
			m_sudoku.enterSolution(fieldIndex, i);
			changed=true;
		}
	}
	return changed;
}

void SudokuSolver::educatedGuess() {
	// find field with minimum number of possibilities
	size_t fieldIndex(999);
	size_t minPossible(m_sudoku.sideLength()+1);
	size_t nbFields=m_sudoku.sideLength()*m_sudoku.sideLength();
	for(size_t f=0; f<nbFields; f++) {
		if(m_sudoku.isSolved(f))
			continue;
		if(m_sudoku.nbPossible(f) < minPossible) {
			minPossible=m_sudoku.nbPossible(f);
			fieldIndex=f;
		}
	}
	if(fieldIndex==999) {
		throw std::runtime_error("SudokuSolver::educatedGuess min not found");
	}
	// get the possiblities;
	std::vector<size_t> numbers;
	m_sudoku.getPossibleNumbers(fieldIndex, numbers);
	for(auto i : numbers) {
		Sudoku tmp(m_sudoku);
		tmp.enterSolution(fieldIndex, i);
		SudokuSolver solver(tmp,m_mode,m_maxResults,m_depth);
		if(solver.solve() == Result::solved) {
			// move results from child solver
			moveResults(solver);
			if(m_results.size()>=m_maxResults)
				return;
		}
	}
}

void SudokuSolver::randomGuess() {
	std::cout << "SudokuSolver::randomGuess " << m_depth << std::endl;
	// check if the current sudoku is solvable at all
	{
		SudokuSolver solver(m_sudoku,Mode::Deterministic,1,m_depth);
		if(solver.solve()==Result::impossible)
			return;
	}
	// find fields with missing entries
	size_t nbFields=m_sudoku.sideLength()*m_sudoku.sideLength();
	Sudoku::FieldGroup fields;
	for(size_t f=0; f<nbFields; f++) {
		if((!m_sudoku.isSolved(f)) && m_sudoku.nbPossible(f)>0)
			fields.push_back(f);
	}
    std::random_device r;
    std::default_random_engine randEngine(r());
    std::shuffle(fields.begin(),fields.end(),randEngine);
    for(auto fieldIndex : fields) {
    	std::vector<size_t> numbers;
    	m_sudoku.getPossibleNumbers(fieldIndex, numbers);
        std::shuffle(numbers.begin(),numbers.end(),randEngine);
        for(auto nb : numbers) {
			Sudoku tmp(m_sudoku);
			tmp.enterSolution(fieldIndex, nb);
			SudokuSolver solver(tmp,m_mode,m_maxResults,m_depth);
			Result res = solver.solve();
			if(res == Result::solved) {
				moveResults(solver);
				if(m_results.size()>=m_maxResults)
					return;
			}
        }
    }
}

void SudokuSolver::moveResults(SudokuSolver & other) {
	m_results.insert(
			m_results.end(),
			std::make_move_iterator(other.m_results.begin()),
			std::make_move_iterator(other.m_results.end()));
}

std::ostream& operator << (std::ostream& os, const SudokuSolver::Result& res)
{
	if(res==SudokuSolver::Result::ambiguos)
		os << "ambiguous";
	else if(res==SudokuSolver::Result::impossible)
		os << "impossible";
	else if(res==SudokuSolver::Result::solved)
		os << "solved";
	return os;
}
