#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "tools.h"

using namespace std;

// board

void displayBoard(bool tiles[]){
    cout << "\n  +---+---+---+---+---+---+---+---+---+" << endl;
    cout << "  |";
    for(int i = 1; i <= 9; i++){
        if(tiles[i]) cout << " " << i << " |";
        else         cout << " X |";
    }
    cout << endl;
    cout << "  +---+---+---+---+---+---+---+---+---+" << endl;
    cout << "    1   2   3   4   5   6   7   8   9" << endl;
}

// dice

int rollDie(){
    return (rand() % 6) + 1;
}

int rollDice(int n){
    int total = 0;
    cout << "  Rolled: ";
    for(int i = 0; i < n; i++){
        int d = rollDie();
        cout << "[" << d << "]";
        total += d;
    }
    cout << "  =  " << total << endl;
    return total;
}

bool anyTileAbove6(bool tiles[]){
    for(int i = 7; i <= 9; i++){
        if(tiles[i]) return true;
    }
    return false;
}

// combinations

// Find all subsets of open tiles that sum to target
void findCombinations(vector<int>& open, int idx, int target,
                      vector<int>& current, vector<vector<int>>& results){
    if(target == 0){
        results.push_back(current);
        return;
    }
    for(int i = idx; i < (int)open.size(); i++){
        if(open[i] <= target){
            current.push_back(open[i]);
            findCombinations(open, i + 1, target - open[i], current, results);
            current.pop_back();
        }
    }
}

vector<vector<int>> getValidCombinations(int roll, bool tiles[]){
    vector<int> open;
    for(int i = 1; i <= 9; i++){
        if(tiles[i]) open.push_back(i);
    }
    vector<vector<int>> results;
    vector<int> current;
    findCombinations(open, 0, roll, current, results);
    return results;
}

// input

vector<int> parseTiles(const string& input){
    vector<int> chosen;
    istringstream ss(input);
    int n;
    while(ss >> n){
        chosen.push_back(n);
    }
    return chosen;
}

bool validateChoice(const vector<int>& chosen, int roll, bool tiles[]){
    // check range and uniqueness
    for(int t : chosen){
        if(t < 1 || t > 9){
            cout << "  ! Tile " << t << " is out of range (1-9)." << endl;
            return false;
        }
        if(!tiles[t]){
            cout << "  ! Tile " << t << " is already closed." << endl;
            return false;
        }
    }
    // check for duplicates
    vector<int> sorted = chosen;
    sort(sorted.begin(), sorted.end());
    for(int i = 1; i < (int)sorted.size(); i++){
        if(sorted[i] == sorted[i-1]){
            cout << "  ! Duplicate tile " << sorted[i] << "." << endl;
            return false;
        }
    }
    // check sum
    int sum = 0;
    for(int t : chosen) sum += t;
    if(sum != roll){
        cout << "  ! Tiles sum to " << sum << ", but you need " << roll << "." << endl;
        return false;
    }
    return true;
}

void closeTiles(bool tiles[], const vector<int>& chosen){
    for(int t : chosen) tiles[t] = false;
}

// score

int calculateScore(bool tiles[]){
    int score = 0;
    for(int i = 1; i <= 9; i++){
        if(tiles[i]) score += i;
    }
    return score;
}

bool allClosed(bool tiles[]){
    for(int i = 1; i <= 9; i++){
        if(tiles[i]) return true;
    }
    return false;
}

// round

void playRound(){
    bool tiles[10];  // tiles[1..9]
    for(int i = 1; i <= 9; i++) tiles[i] = true;

    cout << "\n  ============================================" << endl;
    cout << "         S H U T   T H E   B O X" << endl;
    cout << "  ============================================" << endl;

    while(true){
        displayBoard(tiles);

        // decide how many dice
        int numDice = anyTileAbove6(tiles) ? 2 : 1;
        if(numDice == 1) cout << "\n  (All tiles 7-9 are closed — rolling 1 die)" << endl;
        cout << endl;

        int roll = rollDice(numDice);

        // check if any valid move exists
        vector<vector<int>> combos = getValidCombinations(roll, tiles);
        if(combos.empty()){
            displayBoard(tiles);
            int score = calculateScore(tiles);
            cout << "\n  No valid moves for a roll of " << roll << "." << endl;
            cout << "  ----------------------------------------" << endl;
            if(score == 0){
                cout << "  *** SHUT THE BOX! Perfect score! ***" << endl;
            }else{
                cout << "  Game over!  Your score: " << score << endl;
                cout << "  (Lower is better. 0 = perfect)" << endl;
            }
            cout << "  ----------------------------------------" << endl;
            return;
        }

        // player input loop
        while(true){
            cout << "  Choose tiles to close (space-separated): ";
            string line;
            if(!getline(cin, line)){
                cout << endl;
                return;  // EOF / stream closed
            }
            if(line.empty()) continue;

            vector<int> chosen = parseTiles(line);
            if(chosen.empty()){
                cout << "  ! No tiles entered." << endl;
                continue;
            }
            if(!validateChoice(chosen, roll, tiles)) continue;

            closeTiles(tiles, chosen);
            break;
        }

        // win check
        if(!allClosed(tiles)){
            displayBoard(tiles);
            cout << "\n  *** SHUT THE BOX! You win with a perfect score! ***" << endl;
            return;
        }
    }
}

int main(){
    randomizeSeed();

    while(true){
        playRound();

        cout << "\n  Play again? (y/n): ";
        string answer;
        getline(cin, answer);
        if(answer != "y" && answer != "Y") break;
    }

    cout << "\n  Thanks for playing Shut the Box!\n" << endl;
    return 0;
}
