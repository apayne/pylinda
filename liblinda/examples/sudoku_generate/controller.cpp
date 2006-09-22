#include "linda.h"

class Sudoku {
public:
    Sudoku{}

    int grid[9][9];
}

linda::Value encodeSudoku(Sudoku& s, linda::Memo memo) {
    linda::Value rows[9];
    for(int y=0; y<9; y++) {
        linda::Value v;
        if(s.grid[y][x] > 0)
            v = linda::Value(s.grid[y][x]);
        else
            v = linda::Nil;
        for(int x=7; x>=0; x-) {
            if(s.grid[y][x] > 0)
                v = linda::Value(s.grid[y][x], v);
            else
                v = linda::Value(linda::Nil, v);
        }
        rows[y] = v;
    }

    for(int y=0; y<9; y++) {
    }
}

linda::decodeSudoku(linda::Value v, linda::Memo memo) {
}

char* sudoku_spec = "sudoku: (row*row*row*row*row*row*row*row*row); square: Nil + int; row: (square*square*square*square*square*square*square*square*square);";

int main(int argc, char* argv[]) {
    int cout;

    std::cout << "Starting Generation Of A New Sudoku Puzzle..." << std::endl;

    linda.connect();

    linda:Type typeSudoku = linda::registerType(sudoku_spec, encodeSudoku, decodeSudoku);

    linda::TupleSpace ts;
    linda::Tuple query;
    query.push_back(typeSudoku);
    query.push_back(5);
    while(count == 0) {
        count = linda::uts.collect(ts, query);
    }

    linda::Tuple stop;
    stop.push_back("stop");
    linda::uts._out(stop);

    linda::Tuple result = ts._in(query);

    Sudoku s = typeSudoku.decode(result[1]);

    std::cout << s << std::endl;
}
