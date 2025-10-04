#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>
#include <iomanip> // Included to satisfy print_board TODO
#include <iterator> // Included for back_inserter

using namespace std;

// Forward declaration
int compute_score(const std::vector<std::vector<int>>& board);

// TODO: Compress a row: remove zeros, pad with zeros at the end
std::vector<int> compress_row(const std::vector<int>& row) {
    // TODO: Use copy_if to filter non-zero values, then pad with zeros
    vector<int> result_row;
    copy_if(row.begin(), row.end(), std::back_inserter(result_row), [](int val){ return val != 0; });
    result_row.resize(4, 0); // Pad with zeros to ensure size is always 4
    return result_row;
}

// TODO: Merge a row (assumes already compressed)
std::vector<int> merge_row(std::vector<int> row) {
    // TODO: Implement merging logic - combine adjacent equal tiles
    // This version includes the i++ fix to prevent double merges (e.g., [2,2,4,0] -> [4,4,0,0])
    for (int i = 0; i < 3; ++i) {
        if (row[i] != 0 && row[i] == row[i+1]) {
            row[i] *= 2;
            row[i+1] = 0;
            i++; // Skip the next tile since it has been merged
        }
    }
    return compress_row(row);
}

void write_board_csv(const vector<vector<int>>& board, bool first, const string& stage) {
    ios_base::openmode mode = ios::app;
    if (first) mode = ios::trunc;
    ofstream fout("game_output.csv", mode);
    if (!fout) return;

    // Write stage identifier
    fout << stage << ",";

    // Write board data
    for (int r=0;r<4;r++){
        for (int c=0;c<4;c++){
            fout<<board[r][c];
            if (!(r==3 && c==3)) fout<<",";
        }
    }
    fout<<"\n";
}

void read_board_csv(vector<vector<int>>& board) {
    ifstream fin("game_input.csv");

    string line;
    int r = 0;
    while (getline(fin, line) && r < 4) {
        stringstream ss(line);
        string cell;
        int c = 0;
        while (getline(ss, cell, ',') && c < 4) {
            try {
                board[r][c] = stoi(cell);
            } catch (...) {
                board[r][c] = 0;  // Default to 0 for invalid input
            }
            c++;
        }
        r++;
    }
}

void print_board(const vector<vector<int>>& board) {
    // TODO: Print the score using compute_score(board)
    cout << "Score: " << compute_score(board) << endl;
    // TODO: Print the board in a 4x4 format
    // TODO: Use dots (.) for empty cells (value 0)
    // TODO: Use tabs (\t) to separate values for alignment
    for (const auto& current_row : board) { 
        for (int tile_value : current_row) {
            if (tile_value == 0) {
                cout << ".\t";
            } else {
                cout << tile_value << "\t";
            }
        }
        cout << "\n";
    }
}

void spawn_tile(std::vector<std::vector<int>>& board) {
    std::vector<std::pair<int,int>> empty_spots; 
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            if (board[r][c] == 0) empty_spots.emplace_back(r,c);

    if (empty_spots.empty()) return;

    static std::mt19937 gen(42);  // Fixed seed for deterministic behavior
    std::uniform_int_distribution<> pos_dist(0, empty_spots.size()-1);
    std::uniform_int_distribution<> val_dist(1, 10);

    auto [r, c] = empty_spots[pos_dist(gen)];
    board[r][c] = (val_dist(gen) == 1 ? 4 : 2); // 10% chance of 4
}

// TODO: Implement move_left using compress_row and merge_row
bool move_left(std::vector<std::vector<int>>& board) {
    bool moved = false;
    // TODO: For each row:
    //   1. Compress the row (remove zeros)
    //   2. Merge adjacent equal tiles
    //   3. Check if the row changed
    for (int i = 0; i < 4; ++i) {
        vector<int> original_row = board[i]; 
        vector<int> processed_row = merge_row(compress_row(original_row)); 
        if (original_row != processed_row) {
            moved = true;
            board[i] = processed_row;
        }
    }
    return moved;
}

// TODO: Implement move_right (hint: reverse, compress, merge, reverse)
bool move_right(std::vector<std::vector<int>>& board) {
    bool moved = false;
    // TODO: Similar to move_left but with reversal
    for (int i = 0; i < 4; ++i) {
        vector<int> original_row = board[i];
        vector<int> temp_row = original_row; 
        reverse(temp_row.begin(), temp_row.end());
        
        temp_row = merge_row(compress_row(temp_row));
        
        reverse(temp_row.begin(), temp_row.end());

        if (original_row != temp_row) {
            moved = true;
            board[i] = temp_row;
        }
    }
    return moved;
}

// TODO: Implement move_up (work with columns)
bool move_up(std::vector<std::vector<int>>& board) {
    // This is a completely different implementation.
    // It transposes the board, moves left, and transposes back.
    bool moved = false;
    // TODO: Extract column, compress, merge, write back
    vector<vector<int>> original_board = board;

    // Transpose the board
    vector<vector<int>> transposed_board(4, vector<int>(4));
    for(int r=0; r<4; ++r) for(int c=0; c<4; ++c) transposed_board[r][c] = board[c][r];

    // Apply move_left logic to the transposed board
    move_left(transposed_board);

    // Transpose back
    for(int r=0; r<4; ++r) for(int c=0; c<4; ++c) board[r][c] = transposed_board[c][r];

    // Check if the board has changed
    if(original_board != board) {
        moved = true;
    }
    return moved;
}

// TODO: Implement move_down (columns with reversal)
bool move_down(std::vector<std::vector<int>>& board) {
    // It transposes the board, moves right, and transposes back.
    bool moved = false;
    // TODO: Similar to move_up but with reversal
    vector<vector<int>> original_board = board;

    // Transpose the board
    vector<vector<int>> transposed_board(4, vector<int>(4));
    for(int r=0; r<4; ++r) for(int c=0; c<4; ++c) transposed_board[r][c] = board[c][r];

    // Apply move_right logic to the transposed board
    move_right(transposed_board);
    
    // Transpose back
    for(int r=0; r<4; ++r) for(int c=0; c<4; ++c) board[r][c] = transposed_board[c][r];

    // Check if the board has changed
    if(original_board != board) {
        moved = true;
    }
    return moved;
}

int compute_score(const std::vector<std::vector<int>>& board) {
    int score = 0;
    for (const auto& row : board)
        for (int val : row)
            score += val;
    return score;
}

int main(){
    vector<vector<int>> board(4, vector<int>(4,0));

    // Read initial board from CSV
    read_board_csv(board);

    stack<vector<vector<int>>> history;
    bool first=true;

    while(true){
        print_board(board);
        if (first) {
            write_board_csv(board, true, "initial");
            first = false;
        }

        cout<<"Move (w=up, a=left, s=down, d=right), u=undo, q=quit: ";
        char move_char;
        if (!(cin>>move_char)) break;
        if (move_char=='q') break;

        if (move_char=='u') {
            // TODO: Check if history stack is not empty using !history.empty()
            // If not empty:
            //   1. Set board = history.top() to restore the previous state
            //   2. Remove the top element with history.pop()
            //   3. Call print_board(board) to show the restored board
            //   4. Call write_board_csv(board, false, "undo") to log the undo
            // Use 'continue' to skip the rest of the loop iteration
            if (!history.empty()) {
                board = history.top();
                history.pop();
                print_board(board);
                write_board_csv(board, false, "undo");
            }
            continue;
        }

        vector<vector<int>> board_before_move = board; 
        bool was_moved=false; 
        if (move_char=='a') was_moved=move_left(board);
        else if (move_char=='d') was_moved=move_right(board);
        else if (move_char=='w') was_moved=move_up(board);
        else if (move_char=='s') was_moved=move_down(board);

        if (was_moved) {
            // TODO: Push the previous board state to history stack
            // Use: history.push(prev)
            history.push(board_before_move);

            // Write board after merge but before spawn
            write_board_csv(board, false, "merge");

            spawn_tile(board);
            // Write board after spawn
            write_board_csv(board, false, "spawn");
        } else {
            // No move was made
            write_board_csv(board, false, "invalid");
        }
    }
    return 0;
}