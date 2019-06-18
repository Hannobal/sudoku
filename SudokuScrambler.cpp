#include "SudokuScrambler.h"
#include <random>

SudokuScrambler::SudokuScrambler(const Sudoku& sudoku) :
	m_sudoku(sudoku),
	m_totSteps(0),
	m_randomDevice(),
	m_randomEngine(m_randomDevice()),
	m_uniformDistRowCol(0,m_sudoku.sideLength()-1),
	m_uniformDistBlockRows(0,m_sudoku.nbBlockRows()-1),
	m_uniformDistBlockColumns(0,m_sudoku.nbBlockColumns()-1)
{}

void SudokuScrambler::scramble() {
	 scramble(m_sudoku.nbFields());
}

void SudokuScrambler::scramble(size_t steps)
{
    std::uniform_int_distribution<int> uniformDistMove(0,7);

	for(size_t s(0); s<steps; s++) {

		int random(uniformDistMove(m_randomEngine));

		if(random<3) {
			swapRows();
		} else if(random<6) {
			swapColumns();
		} else if(random<7) {
			swapBlockRows();
		} else {
			swapBlockColumns();
		}
	}
	m_totSteps += steps;
}

void SudokuScrambler::swapRows()
{
	size_t r1(m_uniformDistRowCol(m_randomEngine)), r2;
	size_t b1=r1/m_sudoku.nbBlockColumns();
	do {
		r2=m_uniformDistRowCol(m_randomEngine);
	} while(r2/m_sudoku.nbBlockColumns() != b1);
	m_sudoku.swapRows(r1,r2);
}

void SudokuScrambler::swapColumns()
{
	size_t c1(m_uniformDistRowCol(m_randomEngine)), c2;
	size_t b1=c1/m_sudoku.nbBlockRows();
	do {
		c2=m_uniformDistRowCol(m_randomEngine);
	} while(c2/m_sudoku.nbBlockRows() != b1);
	m_sudoku.swapColumns(c1,c2);
}

void SudokuScrambler::swapBlockRows()
{
	size_t br1(m_uniformDistBlockRows(m_randomEngine)), br2;
	do {
		br2=m_uniformDistBlockRows(m_randomEngine);
	} while(br1==br2);
	m_sudoku.swapBlockRows(br1,br2);
}

void SudokuScrambler::swapBlockColumns()
{
	size_t bc1(m_uniformDistBlockColumns(m_randomEngine)), bc2;
	do {
		bc2=m_uniformDistBlockColumns(m_randomEngine);
	} while(bc1==bc2);
	m_sudoku.swapBlockColumns(bc1,bc2);

}
