#include "SudokuSolver.h"
#include "Utility.h"
#include <iomanip>
#include <random>
#include <algorithm>
#include <set>

SudokuSolver::SudokuSolver(
		Sudoku const& sudoku,
		Mode mode,
		int maxAmbiguities,
		size_t maxResults,
		size_t depth
) :
		m_sudoku(sudoku),
		m_mode(mode),
		m_maxAmbiguities(maxAmbiguities),
		m_maxResults(maxResults),
		m_depth(depth+1),
		m_changed(false)
{}

SudokuSolver::Result SudokuSolver::solve() {

	Result iterationResult;
	do {
		iterationResult=solveIteration();
	} while(iterationResult==Result::ambiguos && m_changed);

	if(iterationResult==Result::solved)
		return Result::solved;
	else if(iterationResult==Result::impossible)
		return Result::impossible;
	else if(m_maxAmbiguities>=0 && m_sudoku.nbGuesses()>=m_maxAmbiguities)
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

SudokuSolver::Result SudokuSolver::solveIteration() {

	m_changed = false;
	// sole and unique candidates
	workFields();
	workRows();
	workColumns();
	workBlocks();

	if(m_sudoku.isSolved()) {
		m_results.push_back(m_sudoku);
		return Result::solved;
	} else if(m_changed)
		return Result::ambiguos;

	// now check and eliminated candidate pairs
	for(size_t i=0; i<m_sudoku.sideLength(); i++) {
		Sudoku::FieldGroup group(m_sudoku.sideLength());
		m_sudoku.getBlock(i*m_sudoku.sideLength(), group);
		checkInteractions(group);
	}

	// now checkTuples
//	checkTuples(2);

	return Result::ambiguos;
}

void SudokuSolver::workFields() {
	size_t nbFields=m_sudoku.nbFields();
	for(size_t field=0; field<nbFields; field++)
		workField(field);
}

void SudokuSolver::workRows()
{
	Sudoku::FieldGroup group(m_sudoku.sideLength());
	for(size_t i=0; i<m_sudoku.sideLength(); i++) {
		m_sudoku.getRow(i*m_sudoku.sideLength(), group);
		workGroup(group);
	}
}

void SudokuSolver::workColumns()
{
	Sudoku::FieldGroup group(m_sudoku.sideLength());
	for(size_t i=0; i<m_sudoku.sideLength(); i++) {
		m_sudoku.getColumn(i, group);
		workGroup(group);
	}
}

void SudokuSolver::workBlocks()
{
	Sudoku::FieldGroup group(m_sudoku.sideLength());
	for(size_t i=0; i<m_sudoku.sideLength(); i++) {
		m_sudoku.getBlock(i*m_sudoku.sideLength(), group);
		workGroup(group);
	}
}

void SudokuSolver::workField(size_t fieldIndex)
{
	if(m_sudoku.nbCandidates(fieldIndex)==1) {
		for(size_t i=1; i<=m_sudoku.sideLength(); i++) {
			if(m_sudoku.isCandidate(fieldIndex, i)) {
				m_sudoku.enterSolution(fieldIndex, i);
				m_changed = true;
				return;
			}
		}
		throw std::runtime_error("SudokuSolver::workField: did not find the number that should be possible in field "+
				std::to_string(fieldIndex));
	}
}

void SudokuSolver::workGroup(const Sudoku::FieldGroup& group)
{
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
			if(m_sudoku.isCandidate(field, i)) {
				if(++nbPossible>1) break;
				fieldIndex=field;
			}
		}
		// if there is only one possibility to palace the number
		// we can safely enter it as the solution
		if(nbPossible==1) {
			m_sudoku.enterSolution(fieldIndex, i);
			m_changed = true;
		}
	}
}

void SudokuSolver::checkInteractions(Sudoku::FieldGroup const& block) {
	// TODO: This is buggy (for the 9x9 very hard it succeeds once and fails once,
	// resulting in an almost but wrongly solved sudoku (compare with brute force
	// solution via educated guess)
	std::set<size_t> possibleNumbers;
	m_sudoku.getMissingNumbers(block,possibleNumbers);
	for(auto number : possibleNumbers) {
		// find rows/columns
		std::set<size_t> allCols, allRows, possibleCols, possibleRows;
		for(auto fieldIndex : block) {
			GridPoint p(m_sudoku.indexToXY(fieldIndex));
			allCols.insert(p.x);
			allRows.insert(p.y);
			if(m_sudoku.isSolved(fieldIndex))
				continue;
			if(m_sudoku.isCandidate(fieldIndex, number)) {
				possibleCols.insert(p.x);
				possibleRows.insert(p.y);
			}
		}
//		if(possibleRows.size()!=allRows.size())
		if(possibleRows.size()==1)
			applyBlockRowColInteractions(allRows, possibleRows, block, number, true);
		if(possibleCols.size()==1)
			applyBlockRowColInteractions(allCols, possibleCols, block, number, false);
	}
}

void SudokuSolver::applyBlockRowColInteractions(
		std::set<size_t> const& allFieldCoords,
		std::set<size_t> const& possibleFieldCoords,
		Sudoku::FieldGroup const& origGroup,
		size_t number,
		bool row
) {
	for(auto coord : possibleFieldCoords) {
		Sudoku::FieldGroup group;
		if(row)
			m_sudoku.getRow(coord,group);
		else
			m_sudoku.getColumn(coord,group);
		for(auto fieldIndex : group) {
			if(contains(origGroup, fieldIndex)) continue;
			if(m_sudoku.isCandidate(fieldIndex,number)) {
				m_changed = true;
				m_sudoku.makeImpossible(fieldIndex, number);
			}
		}
	}
}

void SudokuSolver::checkTuples(
		size_t nbTupleElements
){
	std::cout << "SudokuSolver::checkTuples" << std::endl;
	// find all tuples
	std::vector<TwinField> tuples;
	size_t nbFields=m_sudoku.nbFields();
	for(size_t fieldIndex(0); fieldIndex<nbFields; fieldIndex++) {
		if(m_sudoku.nbCandidates(fieldIndex)==nbTupleElements) {
			TwinField field;
			field.fieldIndex=fieldIndex;
			m_sudoku.getCandidates(fieldIndex, field.candidates);
			std::cout << field.fieldIndex << "  ";
			for(auto i : field.candidates) std::cout << " " << i;
			std::cout << std::endl;
			tuples.push_back(std::move(field));
		}
	}
	// check pairwise TODO: This is wrong. Should be pairwise for twins
	// but groups of three for tiplets and so forth
	for(size_t t1(0); t1<tuples.size(); t1++) {
		for(size_t t2(t1+1); t2<tuples.size(); t2++) {
			if(tuples[t1].candidates!=tuples[t2].candidates)
				continue;
			GridPoint p1(m_sudoku.indexToXY(tuples[t1].fieldIndex));
			GridPoint p2(m_sudoku.indexToXY(tuples[t2].fieldIndex));
			if(m_sudoku.sameBlock(p1, p2)) {

			}
			if(m_sudoku.sameRow(p1, p2)) {

			} else if(m_sudoku.sameColumn(p1, p2)) {

			}
		}
	}
}

void SudokuSolver::checkTuples(
		size_t maxTupleElements,
		Sudoku::FieldGroup const& group)
{
}

void SudokuSolver::educatedGuess()
{
	// find field with minimum number of possibilities
	size_t fieldIndex(999);
	size_t minPossible(m_sudoku.sideLength()+1);
	size_t nbFields=m_sudoku.nbFields();
	for(size_t f=0; f<nbFields; f++) {
		if(m_sudoku.isSolved(f))
			continue;
		if(m_sudoku.nbCandidates(f) < minPossible) {
			minPossible=m_sudoku.nbCandidates(f);
			fieldIndex=f;
		}
	}
	if(fieldIndex==999) {
		throw std::runtime_error("SudokuSolver::educatedGuess min not found");
	}
	// get the possiblities;
	std::vector<size_t> numbers;
	m_sudoku.getCandidates(fieldIndex, numbers);
	for(auto i : numbers) {
		Sudoku tmp(m_sudoku);
		tmp.enterSolution(fieldIndex, i, true);
		SudokuSolver solver(tmp,m_mode,m_maxResults,m_depth);
		if(solver.solve() == Result::solved) {
			// move results from child solver
			moveResults(solver);
			if(m_results.size()>=m_maxResults)
				return;
		}
	}
}

void SudokuSolver::randomGuess()
{
	std::cout << "SudokuSolver::randomGuess " << m_depth << std::endl;
	// check if the current sudoku is solvable at all
	{
		SudokuSolver solver(m_sudoku,Mode::Deterministic,1,m_depth);
		if(solver.solve()==Result::impossible)
			return;
	}
	// find fields with missing entries
	size_t nbFields=m_sudoku.nbFields();
	Sudoku::FieldGroup fields;
	for(size_t f=0; f<nbFields; f++) {
		if((!m_sudoku.isSolved(f)) && m_sudoku.nbCandidates(f)>0)
			fields.push_back(f);
	}
    std::random_device r;
    std::default_random_engine randEngine(r());
    std::shuffle(fields.begin(),fields.end(),randEngine);
    for(auto fieldIndex : fields) {
    	std::vector<size_t> numbers;
    	m_sudoku.getCandidates(fieldIndex, numbers);
        std::shuffle(numbers.begin(),numbers.end(),randEngine);
        for(auto nb : numbers) {
			Sudoku tmp(m_sudoku);
			tmp.enterSolution(fieldIndex, nb, true);
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

void SudokuSolver::moveResults(SudokuSolver & other)
{
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
