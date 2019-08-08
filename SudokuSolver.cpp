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
		m_depth(depth+1)
{}

SudokuSolver::SudokuSolver(
		SudokuSolver const& other
) :
				m_sudoku(other.m_sudoku),
				m_mode(other.m_mode),
				m_maxAmbiguities(other.m_maxAmbiguities),
				m_maxResults(other.m_maxResults),
				m_depth(other.m_depth+1)
{}

SudokuSolver::Result SudokuSolver::solve() {

	Result iterationResult;
	do {
		std::cout << "NEXT ITERATION\n";
		iterationResult=solveIteration();
	} while(iterationResult==Result::ambiguos && m_changed);

	if(iterationResult==Result::solved)
		return Result::solved;
	else if(iterationResult==Result::impossible)
		return Result::impossible;
	else if(m_maxAmbiguities>=0 && m_sudoku.nbGuesses()>=m_maxAmbiguities) {
//		std::cout << "MAX GUESSES REACHED\n" << m_sudoku.nbGuesses() << " vs " << m_maxAmbiguities << std::endl;
		return Result::ambiguos;
	}

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

	InteractionStorage blockInfo;
	findPossibleRowsAndColumns(blockInfo);

	// now check and eliminated candidate pairs
	std::cout << "CHECKING BLOCK-ROW/COLUMN\n";
	checkBlockRowColInteractions(blockInfo);

	// now checkTuples
	std::cout << "CHECKING TWINS\n";
	checkTuples(2);
	std::cout << "CHECKING TRIPLETS\n";
	checkTuples(3);

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
	GridPoint p;
	for(p.x=0; p.x<m_sudoku.sideLength(); p.x+=m_sudoku.blockWidth()) {
		for(p.y=0; p.y<m_sudoku.sideLength(); p.y+=m_sudoku.blockHeight()) {
			workGroup(group);
		}
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

void SudokuSolver::findPossibleRowsAndColumns(InteractionStorage & interactions) {
	interactions.clear();
	interactions.resize(m_sudoku.sideLength());
	size_t i(0);
	for(GridPoint p(0,0); p.x<m_sudoku.sideLength(); p.x+=m_sudoku.blockWidth()) {
		for(p.y=0; p.y<m_sudoku.sideLength(); p.y+=m_sudoku.blockHeight(),++i) {
			interactions[i].fields.resize(m_sudoku.sideLength());
			m_sudoku.getBlock(p, interactions[i].fields);
			findPossibleRowsAndColumns(interactions[i]);
		}
	}
}

void SudokuSolver::findPossibleRowsAndColumns(InteractionBlock & block) {
	m_sudoku.getMissingNumbers(block.fields,block.missingNumbers);
	block.possibleCols.clear();
	block.possibleCols.resize(m_sudoku.sideLength());
	block.possibleRows.clear();
	block.possibleRows.resize(m_sudoku.sideLength());
	block.referenceField.clear();
	block.referenceField.resize(m_sudoku.sideLength());
	for(auto number : block.missingNumbers) {
		for(auto fieldIndex : block.fields) {
			GridPoint p(m_sudoku.indexToXY(fieldIndex));
			if(m_sudoku.isSolved(fieldIndex))
				continue;
			if(m_sudoku.isCandidate(fieldIndex, number)) {
				size_t index(number-1);
				block.possibleCols[index].insert(p.x);
				block.possibleRows[index].insert(p.y);
				block.referenceField[index]=fieldIndex;
			}
		}
	}
}

void SudokuSolver::checkBlockRowColInteractions(InteractionStorage const& interactions) {
	for(auto const& block : interactions) {
		for(auto number : block.missingNumbers) {
			size_t index(number-1);
			if(block.possibleRows[index].size()==1)
				applyBlockRowColInteractions(block.fields, block.referenceField[index], number, true);
			if(block.possibleCols[index].size()==1)
				applyBlockRowColInteractions(block.fields, block.referenceField[index], number, false);
		}
	}
}

void SudokuSolver::applyBlockRowColInteractions(
		Sudoku::FieldGroup const& origGroup,
		size_t refFieldIndex,
		size_t number,
		bool row
) {
	Sudoku::FieldGroup group;
	if(row)
		m_sudoku.getRow(refFieldIndex,group);
	else
		m_sudoku.getColumn(refFieldIndex,group);
	for(auto fieldIndex : group) {
		if(contains(origGroup, fieldIndex)) continue;
		if(m_sudoku.isCandidate(fieldIndex,number)) {
			m_changed = true;
			m_sudoku.makeImpossible(fieldIndex, number);
		}
	}
}

void SudokuSolver::checkTuples(
		size_t nbTupleElements
){
	// find all candidate tuples with correct number of elements
	TupleLookup tuples;
	size_t nbFields=m_sudoku.nbFields();
	for(size_t fieldIndex(0); fieldIndex<nbFields; fieldIndex++) {
		if(m_sudoku.nbCandidates(fieldIndex)!=nbTupleElements) continue;

		CandidateList candidates;
		m_sudoku.getCandidates(fieldIndex, candidates);
		tuples[std::move(candidates)].push_back(fieldIndex);
	}

	for(TupleLookup::const_iterator it=tuples.begin(); it!=tuples.end(); ++it) {
		// we need at least N fields with an identical N-tuple to make any decision
		if(it->second.size() < it->first.size()) continue;
		for(size_t i(0); i<it->second.size(); ++i) {

			FieldList tupleIndices({i});
			Sudoku::FieldGroup group(m_sudoku.sideLength());

			m_sudoku.getRow(it->second[i], group);
			checkTuples(it,tupleIndices,group);

			m_sudoku.getColumn(it->second[i], group);
			checkTuples(it,tupleIndices,group);

			m_sudoku.getBlock(it->second[i], group);
			checkTuples(it,tupleIndices,group);
		}
	}
}

void SudokuSolver::checkTuples(
		TupleLookup::const_iterator tuple,
		FieldList const& tupleIndices,
		Sudoku::FieldGroup const& group)
{
	if(tupleIndices.size()>1 && ! contains(group, tuple->second[tupleIndices.back()])) {
		return;
	}
	// we need at least N fields with an identical N-tuple to make any decision
	if(tupleIndices.size()<tuple->first.size()) {
		for(size_t tupleIndex(tupleIndices.back()+1); tupleIndex<tuple->second.size(); ++tupleIndex) {
			FieldList newList(tupleIndices);
			newList.push_back(tupleIndex);
			checkTuples(tuple, newList, group);
		}
	} else {
		// We've got a match!
		FieldList tupleFields(tupleIndices.size());
		for(size_t i(0); i<tupleIndices.size(); ++i) {
			tupleFields[i]=tuple->second[tupleIndices[i]];
		}
		for(size_t fieldIndex : group) {
			if(contains(tupleFields, fieldIndex)) continue;
			std::vector<size_t> numbers;
			m_sudoku.getCandidates(fieldIndex, numbers);
			for(size_t number : numbers) {
				if(! contains(tuple->first, number)) continue;
				m_sudoku.makeImpossible(fieldIndex, number);
				m_changed=true;
			}
		}
	}
}

void SudokuSolver::educatedGuess()
{
//	std::cout << "DETERMINISTIC GUESS\n";
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
//		Sudoku tmp(m_sudoku);
		SudokuSolver solver(*this);
		solver.m_sudoku.enterSolution(fieldIndex, i, true);
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
//	std::cout << "SudokuSolver::randomGuess " << m_depth << std::endl;
	// check if the current sudoku is solvable at all
	{
		SudokuSolver solver(m_sudoku,Mode::Deterministic,m_maxAmbiguities,m_depth);
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
			SudokuSolver solver(*this);
			solver.m_sudoku.enterSolution(fieldIndex, nb, true);
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
