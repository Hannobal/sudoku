#include "Sudoku.h"
#include <memory>
#include <deque>
#include <limits>
#include <map>

#ifndef SUDOKUSOLVER_H_
#define SUDOKUSOLVER_H_

class SudokuSolver {

public:

	typedef std::deque<Sudoku> ResultList;

	typedef std::vector<size_t> CandidateList;
	typedef std::deque<size_t> FieldList;
	typedef std::map<CandidateList,FieldList> TupleLookup;

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

	SudokuSolver(
			SudokuSolver const& other
	);

	Result solve();

	ResultList getSolved() {return m_results;}

	Sudoku const& getWorkingVersion() const {return m_sudoku;}

	Sudoku & getWorkingVersion() {return m_sudoku;}

private:

	struct InteractionBlock {
		Sudoku::FieldGroup fields;
		// vector index: candidate/number
		// set contains row/column indices where the candidate is possible
		std::set<size_t> missingNumbers;
		std::vector<size_t> referenceField;
		std::vector<std::set<size_t>> possibleCols;
		std::vector<std::set<size_t>> possibleRows;
	};

	//index: BlockID
	typedef std::vector<InteractionBlock> InteractionStorage;

	Sudoku m_sudoku;
	Mode m_mode;
	int m_maxAmbiguities;
	size_t m_maxResults;
	size_t m_depth;
	bool m_changed = false;
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

	void findPossibleRowsAndColumns(InteractionStorage & blockInfo);
	void findPossibleRowsAndColumns(InteractionBlock & block);

	// check for block-row/column/block interactions
	// should only be called with a block as group
	void checkBlockRowColInteractions(InteractionStorage const& blockInfo);
	void applyBlockRowColInteractions(
			Sudoku::FieldGroup const& origGroup,
			size_t refFieldIndex,
			size_t number,
			bool row);

	// for block-block interaction, a candidate must be possible in only the
	// same two rows/columns in the two blocks of the same block row/column
	// then, the candidate is impossible for all fields of the same two
	// rows/columns in all other blocks of that block row/column
	void checkBlockBlockInteractions(InteractionStorage const& blockInfo);
	void applyBlockBlockInteractions(
			InteractionBlock const& block1,
			InteractionBlock const& block2,
			std::set<size_t> const& rowOrColumnIndices,
			size_t number,
			bool row);

	void checkTuples(size_t nbTupleElements);
	void checkTuples(
			TupleLookup::const_iterator tuple,
			FieldList const& tupleIndices,
			Sudoku::FieldGroup const& group
	);

	void educatedGuess();

	void randomGuess();

	void moveResults(SudokuSolver & other);

};


std::ostream& operator << (std::ostream& os, const SudokuSolver::Result& res);

#endif /* SUDOKUSOLVER_H_ */
