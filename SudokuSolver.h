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
		RandomGuessing,
		NoAmbiguityResolution
	};
	enum class Result : char {
		solved, impossible, ambiguos
	};

	SudokuSolver(
			Sudoku const& sudoku,
			Mode mode = Mode::Deterministic,
			size_t maxResults=std::numeric_limits<size_t>::max(),
			size_t depth=0
	);

	Result solve();

	ResultList getSolved() {return m_results;}
	Sudoku const& getWorkingVersion() {return m_sudoku;}

private:

	Sudoku m_sudoku;
	Mode m_mode;
	size_t m_maxResults;
	size_t m_depth;
	ResultList m_results;

	 // work functions return true if a change was made

	bool workFields();
	bool workRows();
	bool workColumns();
	bool workBlocks();

	bool workField(size_t fieldIndex);
	bool workGroup(Sudoku::FieldGroup const& group);

	void educatedGuess();

	void randomGuess();

	void moveResults(SudokuSolver & other);

};


std::ostream& operator << (std::ostream& os, const SudokuSolver::Result& res);

#endif /* SUDOKUSOLVER_H_ */
