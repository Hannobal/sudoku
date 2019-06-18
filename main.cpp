#include "SudokuSolver.h"
#include "DynamicBitset.h"
#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
//	Sudoku sudoku(9);
	try {

		Sudoku sudoku(9);
		sudoku.trivialSolution();
		std::cout << sudoku;
		std::cout << "is sane: " << sudoku.checkSanity() << std::endl;
		sudoku.swapBlockRows(0, 2);
		std::cout << std::endl << sudoku;
		std::cout << "is sane: " << sudoku.checkSanity() << std::endl;


//		std::ofstream output(argv[1]);
//		output << sudoku;
//		output.close();
//
//		std::ifstream input(argv[1]);
//		Sudoku sudokuIn;
//		input >> sudokuIn;
//		sudokuIn.print();
//
//		SudokuSolver solver(
//				sudokuIn,
//				SudokuSolver::Mode::RandomGuessing,
//				1
//		);
//		SudokuSolver::Result result = solver.solve();
//		if(result==SudokuSolver::Result::solved) {
//			std::cout << "found " << solver.getSolved().size() << " solution(s):" << std::endl;
//			for(size_t i=0; i<solver.getSolved().size(); i++) {
//				std::cout << "solution " << i+1 << ":" << std::endl;
//				solver.getSolved()[i].print();
//			}
//		} else {
//			std::cout << "Solve result: " << result << std::endl;
//			solver.getWorkingVersion().print();
//		}

	} catch(std::exception &e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		return 1;
	}
}
