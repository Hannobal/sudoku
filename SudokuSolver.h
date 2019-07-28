#include "Sudoku.h"
#include <memory>
#include <deque>
#include <limits>

#ifndef SUDOKUSOLVER_H_
#define SUDOKUSOLVER_H_

class SudokuSolver {

public:

	typedef std::deque<Sudoku> ResultList;

	enum class Mode : char {
		Deterministic,
		RandomGuessing
	};
	enum class Result : char {
		solved, impossible, ambiguos
	};

	SudokuSolver(
			Sudoku const& sudoku,
			Mode mode = Mode::Deterministic,
			int maxAmbiguities=-1,
			size_t maxResults=std::numeric_limits<size_t>::max(),
			size_t depth=0
	);

	Result solve();

	ResultList getSolved() {return m_results;}

	Sudoku const& getWorkingVersion() const {return m_sudoku;}

	Sudoku & getWorkingVersion() {return m_sudoku;}

private:

	Sudoku m_sudoku;
	Mode m_mode;
	int m_maxAmbiguities;
	size_t m_maxResults;
	size_t m_depth;
	bool m_changed;
	ResultList m_results;

	Result solveIteration();

	// check for naked singles
	void workFields();
	void workField(size_t fieldIndex);

	// check for hidden singles
	void workRows();
	void workColumns();
	void workBlocks();
	void workGroup(Sudoku::FieldGroup const& group);

	// check for block-row/column/block interactions
	// should only be called with a block as group
	void checkInteractions(Sudoku::FieldGroup const& block);
	void applyBlockRowColInteractions(
			std::set<size_t> const& allFieldCoords,
			std::set<size_t> const& possibleFieldCoords,
			Sudoku::FieldGroup const& origGroup,
			size_t number,
			bool row);

	struct TwinField {
		size_t fieldIndex;
		std::vector<size_t> candidates;
	};

	//TODO: Work in progress
	void checkTuples(size_t nbTupleElements);
	void checkTuples(
			size_t maxTupleElements,
			Sudoku::FieldGroup const& group
	);

	void educatedGuess();

	void randomGuess();

	void moveResults(SudokuSolver & other);

};


std::ostream& operator << (std::ostream& os, const SudokuSolver::Result& res);

#endif /* SUDOKUSOLVER_H_ */
