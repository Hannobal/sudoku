#include "SudokuGenerator.h"
#include "SudokuSolver.h"
#include "DynamicBitset.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>

int main(int argc, char** argv) {
	try {
		if(argc<2) {
			std::cout << "input format:" << std::endl;
			std::cout << "solve <file>" << std::endl;
			std::cout << "generate <size> <difficulty>" << std::endl;
			return 1;
		}

		if(std::string(argv[1])=="test") {
			int size=atoi(argv[2]);
			Sudoku sudoku(size);
			Sudoku::FieldGroup group(sudoku.sideLength());
			for(GridPoint p(0,0); p.x<sudoku.sideLength(); p.x+=sudoku.blockWidth()) {
				for(p.y=0; p.y<sudoku.sideLength(); p.y+=sudoku.blockHeight()) {
					sudoku.getBlock(p, group);
					std::cout << p << "   ";
					for(auto f : group) std::cout << " " << f;
					std::cout << std::endl;
				}
			}
		} else if(std::string(argv[1])=="generate") {

			if(argc<4)
				throw std::runtime_error("generate needs two additional arguments: <size> <difficulty>");
			int size=atoi(argv[2]);
			if(size<2)
				throw std::runtime_error("sudoku size must be at least 2");
			SudokuGenerator::Settings settings;
			if(std::string(argv[3])=="easy")
				settings = SudokuGenerator::Settings::easy;
			else if(std::string(argv[3])=="medium")
				settings = SudokuGenerator::Settings::medium;
			else if(std::string(argv[3])=="hard")
				settings = SudokuGenerator::Settings::hard;
			else if(std::string(argv[3])=="extreme")
				settings = SudokuGenerator::Settings::extreme;
			else
				throw std::runtime_error("could not interpret "+std::string(argv[3])+" as difficulty");

			SudokuGenerator generator(
					settings,
					Sudoku(static_cast<size_t>(size))
			);

			if(generator.generate()) {
				std::cout << "# attempts: " << generator.nbAttempts() << std::endl;
				std::cout << "Sudoku:" << std::endl;
				std::cout << generator.getSudoku();
				std::cout << "Solution:" << std::endl;
				std::cout << generator.getSolution();
			} else {
				std::cout << "no success\n";
			}

		} else if(std::string(argv[1])=="solve") {

			if(argc<3)
				throw std::runtime_error("solve needs the filename as additional argument");
			std::ifstream file(argv[2]);
			if(!file)
				throw std::runtime_error("could not open file "+std::string(argv[2]));
			Sudoku sudoku;
			file >> sudoku;
			SudokuSolver solver(sudoku, SudokuSolver::Mode::Deterministic);
			SudokuSolver::Result result = solver.solve();
			if(result==SudokuSolver::Result::solved) {
				std::cout << "found " << solver.getSolved().size() << " solution(s):" << std::endl;
				for(size_t i=0; i<solver.getSolved().size(); i++) {
					std::cout << "solution " << i+1 << " with " <<
							solver.getSolved()[i].nbGuesses() << " guesses :" << std::endl;
					solver.getSolved()[i].print();
				}
			} else {
				std::cout << "No solution found. Final state: " << std::endl;
				solver.getWorkingVersion().print();
			}
		} else {
			throw std::runtime_error("unknown keyword "+std::string(argv[1]));
		}

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
