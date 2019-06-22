#include "Sudoku.h"
#include <math.h>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

Sudoku::Sudoku() :
	m_sideLength(0),
	m_blockWidth(0),
	m_blockHeight(0),
	m_nbBlockRows(0),
	m_nbBlockColumns(0),
	m_nbSolved(0),
	m_nbGuesses(0) {}

Sudoku::Sudoku(size_t length) :
	m_sideLength(length),
	m_nbSolved(0),
	m_nbGuesses(0),
	m_possible(length*length),
	m_solution(length*length)
{
	nearSquareFactors(length, m_blockHeight, m_blockWidth);
	if(m_blockHeight==1)
		throw std::runtime_error("invalid block size"+std::to_string(m_blockWidth)+"x"+std::to_string(m_blockHeight));
	m_nbBlockRows = m_sideLength/m_blockHeight;
	m_nbBlockColumns = m_sideLength/m_blockWidth;
	for(size_t i=0; i<m_possible.size(); ++i) {
		m_possible[i].resize(length);
		m_possible[i].set();
	}
}

Sudoku::Sudoku(size_t length, size_t blockWidth) :
	m_sideLength(length),
	m_blockWidth(blockWidth),
	m_blockHeight(length/blockWidth),
	m_nbSolved(0),
	m_nbGuesses(0),
	m_possible(length*length),
	m_solution(length*length)
{
	if(length%blockWidth!=0)
		throw std::runtime_error("length "+std::to_string(m_sideLength)+" cannot be factorized with "+std::to_string(m_blockWidth));
	if(length==blockWidth || blockWidth==1)
		throw std::runtime_error("invalid block size"+std::to_string(m_blockWidth)+"x"+std::to_string(m_blockHeight));
	m_nbBlockRows = m_sideLength/m_blockHeight;
	m_nbBlockColumns = m_sideLength/m_blockWidth;
	for(size_t i=0; i<m_possible.size(); ++i) {
		m_possible[i].resize(length);
		m_possible[i].set();
	}
}

void Sudoku::getPossibleNumbers(size_t fieldIndex, std::vector<size_t>& numbers) {
	numbers.resize(m_possible[fieldIndex].count());
	size_t idx=0;
	for(size_t i=1; i<=m_sideLength; ++i)
		if(isPossible(fieldIndex,i))
			numbers[idx++]=i;
	if(idx!=numbers.size())
		throw std::runtime_error("idx="+std::to_string(idx)+" != size="+std::to_string(numbers.size()));
}

void Sudoku::print() const
{
	GridPoint p;
	for(p.y=0; p.y<m_sideLength; ++p.y)  {
		if(p.y % m_blockHeight == 0)
			printFrameLine();
		for(p.x=0; p.x<m_sideLength; ++p.x) {
			if(p.x % m_blockWidth == 0)
				std::cout << "|";
			std::cout << std::setw(4);
			size_t s=m_solution[xyToIndex(p)];
			if(s>0)
				std::cout << s;
			else
				std::cout << '?';
		}
		std::cout << "|" << std::endl;
	}
	printFrameLine();
}

void Sudoku::printFrameLine() const {
	int blockw(static_cast<int>(m_blockWidth)*4);
	std::cout << std::setfill('-');
	for(size_t i(0); i<m_nbBlockColumns; i++)
		std::cout << "+" << std::setw(blockw) << "";
	std::cout << std::setfill(' ') << "+" << std::endl;
}

void Sudoku::printPossible(size_t number, size_t indent) const
{
	GridPoint p;
	number--;
	for(p.y=0; p.y<m_sideLength; ++p.y)  {
		if(indent>0)
			std::cout << std::setw(indent) << "";
		for(p.x=0; p.x<m_sideLength; ++p.x) {
			std::cout << " " << m_possible[xyToIndex(p)][number];
		}
		std::cout << std::endl;
	}
}

void Sudoku::enterSolution(size_t fieldIndex, size_t number, bool guessed)
{
	if(number==0)
		return clearSolution(fieldIndex);

	if(isSolved(fieldIndex)) {
		if(number==m_solution[fieldIndex])
			return;
		else {
			std::cout << "ERROR:\n" << *this << std::endl;
			throw std::runtime_error("Sudoku::enterSolution: Field "
					+std::to_string(fieldIndex)+" is already solved");
		}
	}

	if(!isPossible(fieldIndex,number))
		throw std::runtime_error("Sudoku::enterSolution: Invalid number " + std::to_string(number));

	FieldGroup group(m_sideLength);
	getRow(fieldIndex,group);
	for(auto field : group)
		makeImpossible(field,number);
	getColumn(fieldIndex,group);
	for(auto field : group)
		makeImpossible(field,number);
	getBlock(fieldIndex,group);
	for(auto field : group)
		makeImpossible(field,number);

	m_solution[fieldIndex]=number;
	m_possible[fieldIndex].reset();
	++m_nbSolved;
	if(guessed)
		++m_nbGuesses;
}

void Sudoku::clearSolution(size_t fieldIndex)
{
	if(!isSolved(fieldIndex)) return;

	size_t number=getSolution(fieldIndex);
	m_solution[fieldIndex]=0;

	for(size_t i=1; i<=m_sideLength; ++i)
		makePossible(fieldIndex,i);

	FieldGroup group(m_sideLength);
	getRow(fieldIndex,group);
	for(auto field : group)
		makePossible(field,number);
	getColumn(fieldIndex,group);
	for(auto field : group)
		makePossible(field,number);
	getBlock(fieldIndex,group);
	for(auto field : group)
		makePossible(field,number);
	m_nbSolved--;
}

void Sudoku::trivialSolution() {
	GridPoint p;
	size_t n(0);
	for(p.y=0; p.y<m_sideLength; ++p.y)  {
		size_t blockY = p.y / m_blockHeight;
		for(p.x=0; p.x<m_sideLength; ++p.x) {
			size_t f=xyToIndex(p);
			m_possible[f].reset();
			m_solution[f] = 1 + (n+m_blockWidth*p.y+blockY) % m_sideLength;
			++n;
		}
	}
	m_nbSolved=nbFields();
}

void Sudoku::getSolvedOrUnsolvedFields(FieldGroup& solvedFields, bool solved) const {
	size_t nFields = nbFields();
	solvedFields.resize(nbSolved());
	size_t fieldIndex(0);
	for(size_t nsolved=0; nsolved<nbSolved();nsolved++) {
		while(solved != isSolved(fieldIndex)) {
			if(fieldIndex++==nFields)
				throw std::runtime_error("Sudoku::getSolvedFields reached the end of the field list");
		}
		solvedFields[nsolved]=fieldIndex++;
	}
}

void Sudoku::getRow(GridPoint p, FieldGroup& row) const
{
	row.resize(m_sideLength);
	for(p.x=0; p.x<m_sideLength; ++p.x)
		row[p.x] = xyToIndex(p);
}

void Sudoku::getRow(size_t fieldIndex, FieldGroup& row) const
{
	row.resize(m_sideLength);
	size_t min=m_sideLength*(fieldIndex/m_sideLength);
	for(size_t x=0; x<m_sideLength; ++x)
		row[x] = min+x;
}

void Sudoku::getColumn(GridPoint p, FieldGroup& col) const
{
	col.resize(m_sideLength);
	for(p.y=0; p.y<m_sideLength; ++p.y)
		col[p.y] = xyToIndex(p);
}

void Sudoku::getColumn(size_t fieldIndex, FieldGroup& col) const
{
	col.resize(m_sideLength);
	size_t offset=fieldIndex%m_sideLength;
	for(size_t y=0; y<m_sideLength; ++y)
		col[y] = y*m_sideLength+offset;
}

// get the field indices for the block containing the field
void Sudoku::getBlock(size_t fieldIndex, FieldGroup& block) const {
	GridPoint p(indexToXY(fieldIndex));
	return getBlock(p,block);
}

void Sudoku::getBlock(GridPoint p, FieldGroup& block) const
{
//	block.resize(m_size);
//	size_t blockSize=(size_t) (sqrt(m_size)+0.1); // add 0.1 for rounding errors
//	if(blockSize*blockSize!=m_size)
//		throw std::runtime_error("non-square blocks are not implemented yet!");
	size_t xMin=m_blockWidth*(p.x/m_blockWidth); size_t xMax=xMin+m_blockWidth;
	size_t yMin=m_blockHeight*(p.y/m_blockHeight); size_t yMax=yMin+m_blockHeight;
	size_t i(0);
	for(p.x=xMin; p.x<xMax; ++p.x) {
		for(p.y=yMin; p.y<yMax; ++p.y) {
			block[i++] = xyToIndex(p);
		}
	}
}

bool Sudoku::checkPossible(size_t fieldIndex, size_t i) {
	if(m_solution[fieldIndex]!=0 && m_solution[fieldIndex]!=i)
		return false;
	FieldGroup group(m_sideLength);
	getRow(fieldIndex,group);
	for(auto field : group)
		if(m_solution[field]==i)
			return false;

	getColumn(fieldIndex,group);
	for(auto field : group)
		if(m_solution[field]==i)
			return false;

	getBlock(fieldIndex,group);
	for(auto field : group)
		if(m_solution[field]==i)
			return false;

	return true;
}

void Sudoku::swapRows(size_t r1, size_t r2)
{
	if(r1 >= m_sideLength || r2 >= m_sideLength)
		throw std::runtime_error("Sudoku::swapRows: row index out of range");

	if(r1==r2) return;

	if(r1/m_blockHeight != r2/m_blockHeight)
		throw std::runtime_error("Sudoku::swapRows: rows are not in the same block row");

	size_t f1(xyToIndex(GridPoint(0,r1)));
	size_t f2(xyToIndex(GridPoint(0,r2)));
	for(size_t x=0;x<m_sideLength; ++x, f1+=m_sideLength, f2+=m_sideLength)
		swapFields(f1,f2);
}

void Sudoku::swapColumns(size_t c1, size_t c2)
{
	if(c1 >= m_sideLength || c2 >= m_sideLength)
		throw std::runtime_error("Sudoku::swapColumns: column index out of range");

	if(c1==c2) return;

	if(c1/m_blockWidth != c2/m_blockWidth)
		throw std::runtime_error("Sudoku::swapColumns: columns are not in the same block column");

	size_t f1(xyToIndex(GridPoint(c1,0)));
	size_t f2(xyToIndex(GridPoint(c2,0)));
	for(size_t y=0; y<m_sideLength; ++y, ++f1, ++f2)
		swapFields(f1,f2);
}

void Sudoku::swapBlockRows(size_t br1, size_t br2)
{
	if(br1 >= m_nbBlockRows || br2 >= m_nbBlockRows)
		throw std::runtime_error("Sudoku::swapBlockRows: block row index out of range");
	if(br1==br2) return;
	for(size_t x=0; x<m_sideLength; ++x) {
		size_t f1(xyToIndex(GridPoint(x,br1*m_blockHeight)));
		size_t f2(xyToIndex(GridPoint(x,br2*m_blockHeight)));
		for(size_t y=0; y<m_blockHeight; ++y, ++f1, ++f2) {
			swapFields(f1,f2);
		}
	}
}

void Sudoku::swapBlockColumns(size_t bc1, size_t bc2)
{
	if(bc1 >= m_nbBlockColumns || bc2 >= m_nbBlockColumns)
		throw std::runtime_error("Sudoku::swapBlockColumns: block column index out of range");
	if(bc1==bc2) return;

	size_t f1(xyToIndex(GridPoint(bc1*m_blockWidth,0)));
	size_t f2(xyToIndex(GridPoint(bc2*m_blockWidth,0)));
	for(size_t x=0; x<m_blockWidth; ++x) {
		for(size_t y=0; y<m_sideLength; ++y, ++f1, ++f2) {
			swapFields(f1,f2);
		}
	}
}

void Sudoku::transpose() {
	GridPoint p1;
	for(p1.x=0; p1.x<m_sideLength; ++p1.x) {
		for(p1.y=0; p1.y<p1.x; ++p1.y) {
			GridPoint p2(p1.y, p1.x);
			swapFields(xyToIndex(p1),xyToIndex(p2));
		}
	}
	swapBlockOrientation();
}

void Sudoku::invert() {
	size_t max(nbFields());
	size_t maxHalf(max/2);
	for(size_t f1(0); f1<maxHalf; f1++) {
		swapFields(f1, max-f1-1);
	}
}

void Sudoku::flipHorizontal() {
	size_t xMax(m_sideLength/2);
	GridPoint p1;
	for(p1.x=0; p1.x<xMax; ++p1.x) {
		for(p1.y=0; p1.y<m_sideLength; ++p1.y) {
			GridPoint p2(m_sideLength-1-p1.x, p1.y);
			swapFields(xyToIndex(p1),xyToIndex(p2));
		}
	}
}

void Sudoku::flipVertical() {
	size_t yMax(m_sideLength/2);
	GridPoint p1;
	for(p1.x=0; p1.x<m_sideLength; ++p1.x) {
		for(p1.y=0; p1.y<yMax; ++p1.y) {
			GridPoint p2(p1.x, m_sideLength-1-p1.y);
			swapFields(xyToIndex(p1),xyToIndex(p2));
		}
	}
}

void Sudoku::rotateRight() {
	// not very efficient, but does the trick
	transpose();
	flipHorizontal();
}

void Sudoku::rotateLeft() {
	// not very efficient, but does the trick
	transpose();
	flipVertical();
}

void Sudoku::swapFields(size_t fieldIndex1, size_t fieldIndex2) {
	std::swap(m_solution[fieldIndex1],m_solution[fieldIndex2]);
	std::swap(m_possible[fieldIndex1],m_possible[fieldIndex2]);
}

void Sudoku::swapBlockOrientation() {
	std::swap(m_blockHeight,m_blockWidth);
	std::swap(m_nbBlockRows,m_nbBlockColumns);
}

bool Sudoku::checkSanity() const {
	size_t max(nbFields());
	for(size_t fieldIndex(0); fieldIndex<max; fieldIndex++) {
		if(! isSolved(fieldIndex)) continue;
		FieldGroup group;
		getRow(fieldIndex, group);
		if(! checkSanity(fieldIndex, group)) return false;
		getColumn(fieldIndex, group);
		if(! checkSanity(fieldIndex, group)) return false;
		getBlock(fieldIndex, group);
		if(! checkSanity(fieldIndex, group)) return false;
	}
	return true;
}

bool Sudoku::checkSanity(size_t fieldIndex, FieldGroup const& group) const {
	for(auto groupField : group) {
		if(! isSolved(groupField)) continue;
		if(groupField == fieldIndex) continue;
		if(m_solution[groupField] == m_solution[fieldIndex])
			return false;
	}
	return true;
}

bool Sudoku::operator==(Sudoku const& other) {
	if(m_sideLength != other.m_sideLength) return false;
	if(m_blockWidth != other.m_blockWidth) return false;
	// blockHeight, nbBlockRows and nbBlockColumns are dependent on the above
	// and thus do not need to be checked here

	size_t max(nbFields());
	for(size_t fieldIndex(0); fieldIndex<max; fieldIndex++)
		if(m_solution[fieldIndex] != other.m_solution[fieldIndex]) return false;

	return true;
}

std::istream &operator>>( std::istream  &input, Sudoku &sudoku )
{
	std::string line;
	size_t size=0;
	GridPoint p;
	std::shared_ptr<Sudoku> tmp;
	while(getline(input,line)) {
		std::vector<std::string> parts;
		boost::trim(line);
		boost::split(parts,line, boost::is_any_of("\t "), boost::token_compress_on);

		if(size!=parts.size()) {
			if(size==0) {
				size=parts.size();
				tmp.reset(new Sudoku(size));
			} else
				throw std::runtime_error("number of elements is not the same on each line");
		}
		if(size==0)
			throw std::runtime_error("empty sudoku");

		for(p.x=0; p.x<size; ++p.x) {
			if(parts[p.x]=="?") continue;
			size_t nb = boost::lexical_cast<size_t>(parts[p.x]);
			tmp->enterSolution(p, nb);
		}

		if(++p.y == size) break;
	}

	sudoku=*tmp;
	return input;
}

std::ostream &operator<<( std::ostream  &output, Sudoku const& sudoku ) {
	GridPoint p;
	for(p.y=0; p.y<sudoku.m_sideLength; ++p.y)  {
		for(p.x=0; p.x<sudoku.m_sideLength; ++p.x) {
			output << std::setw(4);
			size_t s=sudoku.m_solution[sudoku.xyToIndex(p)];
			if(s>0)
				output << s;
			else
				output << '?';
		}
		output << std::endl;
	}
	return output;
}

void Sudoku::nearSquareFactors(size_t n, size_t& f1, size_t &f2)
{
	f1=sqrt(n);
	while(f1>1 && n%f1!=0) f1--;
	f2=n/f1;
}
