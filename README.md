# Sudoku
- a solver and generator for Sudokus in C++
- can (theoretically) handle square sudokus of arbitrary size provided that the side length is not a prime number.
- blocks are sized such that they are as close to square as possible. e.g. for a 12x12 Sudoku they will be 3x4 rather than 2x6.

<a name="fileSyntax"></a>
### syntax for sudoku files:
- fields separated by whitespace
- empty fields as questionmark

example :
```
   7   ?   8   ?   1   ?   6   5   ?  
   1   3   2   ?   4   ?   9   8   7  
   4   6   ?   ?   7   8   ?   2   1  
   5   7   6   1   ?   ?   4   3   2  
   ?   4   ?   7   5   6   ?   9   ?  
   8   ?   9   4   2   ?   7   6   ?  
   ?   5   4   8   ?   7   ?   ?   9  
   ?   ?   7   2   9   1   5   4   3  
   9   2   1   5   ?   ?   8   7   ?  
```
## usage

### solve a sudoku:
```
SudokuSolver solve <filename> [difficulty]
```
- filename: the name of the file containing the sudoku (see [file syntax](#fileSyntax))
- difficulty: either of `easy`, `medium`, `hard`, or `extreme`.

### generate a sudoku:
```
SudokuSolver generate <sideLength> <difficulty>
```
- sideLength: the side length of the resulting sudoku
- difficulty: either of `easy`, `medium`, `hard`, or `extreme`.

**NOTE:** currently, the generation works reliably only for sudokus up to size 10.
It appears, that the requrired number of filled fields increases with the size of the sudoku.
