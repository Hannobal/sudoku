#include "SudokuSolver.h"
#include "DynamicBitset.h"
#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
//	Sudoku sudoku(9);
	try {
		std::ifstream input(argv[1]);
		Sudoku sudokuIn;
		input >> sudokuIn;
		sudokuIn.print();
//		for(int i=1;i<=s.size();i++) {
//			std::cout << "For " << i << std::endl;
//			s.printPossible(i);
//		}
		SudokuSolver solver(
				sudokuIn,
				SudokuSolver::Mode::RandomGuessing,
				1
		);
		SudokuSolver::Result result = solver.solve();
		if(result==SudokuSolver::Result::solved) {
			std::cout << "found " << solver.getSolved().size() << " solution(s):" << std::endl;
			for(size_t i=0; i<solver.getSolved().size(); i++) {
				std::cout << "solution " << i+1 << ":" << std::endl;
				solver.getSolved()[i].print();
			}
		} else {
			std::cout << "Solve result: " << result << std::endl;
			solver.getWorkingVersion().print();
		}

	} catch(std::exception &e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		return 1;
	}
}
