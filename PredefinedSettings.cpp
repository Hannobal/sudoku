/******************************************************************************
 * NOTE: Initially, these static variables were defined in the respective
 * cpp files of the SudokuSolver/Generator classes. However, since the
 * SudokuGenerator settings depend on the solver settings and in the cpp file
 * of the SudokuGenerator, the values of the predefined settings were unknown
 * this lead to the Generator settings having only defaults for their Solver
 * Thus, the definitions were externalized here to occur in the right order
 *****************************************************************************/

#include "SudokuGenerator.h"

SudokuSolver::Settings SudokuSolver::Settings::easy(
		true,  // allowNakedSingle
		true,  // allowHiddenSingle
		false, // allowBlockRowColumn
		false, // allowBlockBlock
		false, // allowNakedTuples
		false, // allowHiddenTuples
		false, // nextIterOnChange
		GuessMode::Deterministic,
		0, // maxTupleSize
		0, // maxNbGuesses
		std::numeric_limits<size_t>::max() // maxResults
);

SudokuSolver::Settings SudokuSolver::Settings::medium(
		true,  // allowNakedSingle
		true,  // allowHiddenSingle
		true,  // allowBlockRowColumn
		true,  // allowBlockBlock
		true,  // allowNakedTuples
		true,  // allowHiddenTuples
		false, // nextIterOnChange
		GuessMode::Deterministic,
		2, // maxTupleSize
		0, // maxNbGuesses
		std::numeric_limits<size_t>::max() // maxResults
);

SudokuSolver::Settings SudokuSolver::Settings::hard(
		true,  // allowNakedSingle
		true,  // allowHiddenSingle
		true,  // allowBlockRowColumn
		true,  // allowBlockBlock
		true,  // allowNakedTuples
		true,  // allowHiddenTuples
		false, // nextIterOnChange
		GuessMode::Deterministic,
		4, // maxTupleSize
		0, // maxNbGuesses
		std::numeric_limits<size_t>::max() // maxResults
);

SudokuSolver::Settings SudokuSolver::Settings::extreme(
		true,  // allowNakedSingle
		true,  // allowHiddenSingle
		true,  // allowBlockRowColumn
		true,  // allowBlockBlock
		true,  // allowNakedTuples
		true,  // allowHiddenTuples
		false, // nextIterOnChange
		GuessMode::Deterministic,
		4, // maxTupleSize
		1, // maxNbGuesses
		std::numeric_limits<size_t>::max() // maxResults
);

SudokuGenerator::Settings SudokuGenerator::Settings::easy(
		SudokuSolver::Settings::easy,
		0.4f, 0.5f, 0
);

SudokuGenerator::Settings SudokuGenerator::Settings::medium(
		SudokuSolver::Settings::medium,
		0.3f, 0.4f, 0
);

SudokuGenerator::Settings SudokuGenerator::Settings::hard(
		SudokuSolver::Settings::hard,
		0.3f, 0.4f, 1
);

SudokuGenerator::Settings SudokuGenerator::Settings::extreme(
		SudokuSolver::Settings::extreme,
		0.1f, 0.3f, 1
);
