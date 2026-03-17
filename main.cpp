#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "tools.h"

using namespace std;

// ansi escape codes
#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define DIM         "\033[2m"
#define GREEN       "\033[32m"
#define RED         "\033[31m"
#define YELLOW      "\033[33m"
#define CYAN        "\033[36m"
#define MAGENTA     "\033[35m"
#define WHITE       "\033[37m"

// box width (inner visible chars between the two ║)
#define BOX_W 56

// game state
struct GameState{
    bool tiles[10];      // tiles[1..9]
    int turns = 0;
    int dice1 = 0;
    int dice2 = 0;
    int diceTotal = 0;
    int numDice = 2;
    bool endless = false;
    bool reopenMode = false;
    string message = "";
    string messageColor = WHITE;
};

// clear screen
void clearScreen(){
    system("clear");
}

// print a row: ║ <content padded to BOX_W> ║
// visLen = how many visible characters are in content (excluding ANSI codes)
void boxRow(const string& content, int visLen){
    cout << CYAN << "  ║" << RESET << content;
    int pad = BOX_W - visLen;
    for(int i = 0; i < pad; i++) cout << " ";
    cout << CYAN << "║" << RESET << endl;
}

void boxTop(){
    cout << CYAN << "  ╔";
    for(int i = 0; i < BOX_W; i++) cout << "═";
    cout << "╗" << RESET << endl;
}

void boxMid(){
    cout << CYAN << "  ╠";
    for(int i = 0; i < BOX_W; i++) cout << "═";
    cout << "╣" << RESET << endl;
}

void boxBot(){
    cout << CYAN << "  ╚";
    for(int i = 0; i < BOX_W; i++) cout << "═";
    cout << "╝" << RESET << endl;
}

// dice
int rollDie(){
    return (rand() % 6) + 1;
}

bool anyTileAbove6(bool tiles[]){
    for(int i = 7; i <= 9; i++){
        if(tiles[i]) return true;
    }
    return false;
}

// combinations
void findCombinations(vector<int>& pool, int idx, int target,
                      vector<int>& current, vector<vector<int>>& results){
    if(target == 0){
        results.push_back(current);
        return;
    }
    for(int i = idx; i < (int)pool.size(); i++){
        if(pool[i] <= target){
            current.push_back(pool[i]);
            findCombinations(pool, i + 1, target - pool[i], current, results);
            current.pop_back();
        }
    }
}

vector<vector<int>> getValidCombinations(int roll, bool tiles[], int mode){
    vector<int> pool;
    for(int i = 1; i <= 9; i++){
        if(mode == 0 && tiles[i])  pool.push_back(i);        // open only
        else if(mode == 1 && !tiles[i])  pool.push_back(i);   // closed only
        else if(mode == 2) pool.push_back(i);                // all tiles
    }
    vector<vector<int>> results;
    vector<int> current;
    findCombinations(pool, 0, roll, current, results);
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

// score
int calculateScore(bool tiles[]){
    int s = 0;
    for(int i = 1; i <= 9; i++){
        if(tiles[i]) s += i;
    }
    return s;
}

bool allClosed(bool tiles[]){
    for(int i = 1; i <= 9; i++){
        if(tiles[i]) return false;
    }
    return true;
}

// draw UI
void drawUI(GameState& gs){
    clearScreen();

    string modeLabel = gs.endless ? "ENDLESS" : "CLASSIC";

    // title
    boxTop();
    // center "S H U T   T H E   B O X"  (25 visible chars)
    string title = "S H U T   T H E   B O X";
    int titlePad = (BOX_W - (int)title.size()) / 2;
    string titleLine = "";
    for(int i = 0; i < titlePad; i++) titleLine += " ";
    titleLine += string(BOLD) + WHITE + title + RESET;
    boxRow(titleLine, titlePad + (int)title.size());

    // mode label
    string modeLine = "[ " + modeLabel + " MODE ]";
    int modePad = (BOX_W - (int)modeLine.size()) / 2;
    string modeStr = "";
    for(int i = 0; i < modePad; i++) modeStr += " ";
    modeStr += string(DIM) + modeLine + RESET;
    boxRow(modeStr, modePad + (int)modeLine.size());

    boxMid();

    // tiles - each tile is "┌───┐" = 5 visible chars, with 1 space between = 53 + some padding
    // 9 tiles * 5 = 45, + 8 spaces = 53
    // left pad = (BOX_W - 53) / 2 = 1 (with BOX_W=56, pad=1, right=2)
    int tileAreaW = 9 * 5 + 8; // 53
    int tileLPad = (BOX_W - tileAreaW) / 2;

    // top border row
    string topRow = "";
    for(int i = 0; i < tileLPad; i++) topRow += " ";
    for(int i = 1; i <= 9; i++){
        if(gs.tiles[i]) topRow += string(GREEN) + "┌───┐" + RESET;
        else            topRow += string(DIM) + RED + "┌───┐" + RESET;
        if(i < 9) topRow += " ";
    }
    boxRow(topRow, tileLPad + tileAreaW);

    // value row
    string valRow = "";
    for(int i = 0; i < tileLPad; i++) valRow += " ";
    for(int i = 1; i <= 9; i++){
        if(gs.tiles[i]) valRow += string(GREEN) + BOLD + "│ " + to_string(i) + " │" + RESET;
        else            valRow += string(DIM) + RED + "│ X │" + RESET;
        if(i < 9) valRow += " ";
    }
    boxRow(valRow, tileLPad + tileAreaW);

    // bottom border row
    string botRow = "";
    for(int i = 0; i < tileLPad; i++) botRow += " ";
    for(int i = 1; i <= 9; i++){
        if(gs.tiles[i]) botRow += string(GREEN) + "└───┘" + RESET;
        else            botRow += string(DIM) + RED + "└───┘" + RESET;
        if(i < 9) botRow += " ";
    }
    boxRow(botRow, tileLPad + tileAreaW);

    // number row
    string numRow = "";
    for(int i = 0; i < tileLPad; i++) numRow += " ";
    for(int i = 1; i <= 9; i++){
        numRow += "  " + to_string(i) + "  ";
        if(i < 9) numRow += " ";
    }
    boxRow(numRow, tileLPad + tileAreaW);

    boxMid();

    // dice section
    if(gs.diceTotal > 0){
        // build dice display
        // each die: ┌───┐ / │ N │ / └───┘ = 5 visible chars
        // total line: "  Rolled: " + dice + "  =  N" 
        // "  Rolled: " = 10,  "  =  " = 5,  digit = 1-2
        string totalStr = to_string(gs.diceTotal);

        // line 1: top of dice
        string d1 = "  " + string(CYAN) + BOLD + "Rolled: " + RESET;
        d1 += string(YELLOW) + "┌───┐" + RESET;
        int visLen1 = 10 + 5;
        if(gs.numDice == 2){
            d1 += " ";
            d1 += string(YELLOW) + "┌───┐" + RESET;
            visLen1 += 1 + 5;
        }
        boxRow(d1, visLen1);

        // line 2: value of dice + total
        string d2 = "          "; // 10 spaces to align under "Rolled: "
        d2 += string(YELLOW) + "│ " + BOLD + to_string(gs.dice1) + RESET + YELLOW + " │" + RESET;
        int visLen2 = 10 + 5;
        if(gs.numDice == 2){
            d2 += " ";
            d2 += string(YELLOW) + "│ " + BOLD + to_string(gs.dice2) + RESET + YELLOW + " │" + RESET;
            visLen2 += 1 + 5;
        }
        d2 += "  =  " + string(BOLD) + WHITE + totalStr + RESET;
        visLen2 += 5 + (int)totalStr.size();
        boxRow(d2, visLen2);

        // line 3: bottom of dice
        string d3 = "          "; // 10 spaces
        d3 += string(YELLOW) + "└───┘" + RESET;
        int visLen3 = 10 + 5;
        if(gs.numDice == 2){
            d3 += " ";
            d3 += string(YELLOW) + "└───┘" + RESET;
            visLen3 += 1 + 5;
        }
        boxRow(d3, visLen3);
    }else{
        string rolling = "  Rolling " + to_string(gs.numDice) + " dice...";
        boxRow(rolling, (int)rolling.size());
    }

    // message section
    boxMid();
    if(!gs.message.empty()){
        string msg = gs.message;
        if((int)msg.size() > BOX_W - 4) msg = msg.substr(0, BOX_W - 4);
        string line = "  " + string(gs.messageColor.c_str()) + msg + RESET;
        boxRow(line, 2 + (int)msg.size());
    }else{
        boxRow("", 0);
    }

    boxBot();
    cout << endl;
}

// validate choice
bool validateChoice(const vector<int>& chosen, int roll, bool tiles[], bool endlessToggle, string& errMsg){
    for(int t : chosen){
        if(t < 1 || t > 9){
            errMsg = "Tile " + to_string(t) + " is out of range (1-9)";
            return false;
        }
        if(!endlessToggle){
            if(!tiles[t]){
                errMsg = "Tile " + to_string(t) + " is already closed";
                return false;
            }
        }
    }
    vector<int> sorted = chosen;
    sort(sorted.begin(), sorted.end());
    for(int i = 1; i < (int)sorted.size(); i++){
        if(sorted[i] == sorted[i-1]){
            errMsg = "Duplicate tile " + to_string(sorted[i]);
            return false;
        }
    }
    int sum = 0;
    for(int t : chosen) sum += t;
    if(sum != roll){
        errMsg = "Tiles sum to " + to_string(sum) + ", need " + to_string(roll);
        return false;
    }
    return true;
}

// menu
int showMenu(){
    clearScreen();
    boxTop();

    string title = "S H U T   T H E   B O X";
    int titlePad = (BOX_W - (int)title.size()) / 2;
    string titleLine = "";
    for(int i = 0; i < titlePad; i++) titleLine += " ";
    titleLine += string(BOLD) + WHITE + title + RESET;
    boxRow(titleLine, titlePad + (int)title.size());

    boxMid();
    boxRow("", 0);

    string opt1 = "    " + string(GREEN) + BOLD + "[1]" + RESET + "  Classic Mode";
    boxRow(opt1, 4 + 3 + 14); // 21
    string sub1 = "         " + string(DIM) + "Shut all 9 tiles to win" + RESET;
    boxRow(sub1, 9 + 23); // 32
    boxRow("", 0);

    string opt2 = "    " + string(MAGENTA) + BOLD + "[2]" + RESET + "  Endless Mode";
    boxRow(opt2, 4 + 3 + 14);
    string sub2 = "         " + string(DIM) + "Play forever, reopen tiles" + RESET;
    boxRow(sub2, 9 + 26); // 35
    boxRow("", 0);

    string opt3 = "    " + string(RED) + BOLD + "[q]" + RESET + "  Quit";
    boxRow(opt3, 4 + 3 + 6); // 13
    boxRow("", 0);

    boxBot();
    cout << endl;
    cout << "  Choose: ";

    string input;
    if(!getline(cin, input)) return 0;
    if(input == "1") return 1;
    if(input == "2") return 2;
    if(input == "q" || input == "Q") return 0;
    return -1;
}

// game over screen
void showGameOver(GameState& gs, bool won){
    drawUI(gs);
    cout << endl;
    if(won){
        cout << GREEN << BOLD << "  ★ SHUT THE BOX! Perfect score: 0 ★" << RESET << endl;
    }else{
        int sc = calculateScore(gs.tiles);
        cout << RED << BOLD << "  Game Over!" << RESET << " Final Score: " << BOLD << sc << RESET << endl;
        cout << DIM << "  (Lower is better. 0 = perfect)" << RESET << endl;
    }
    cout << WHITE << "  Total turns: " << gs.turns << RESET << endl;
    cout << endl;
    cout << "  Press ENTER to continue...";
    string dummy;
    getline(cin, dummy);
}

// classic mode
void playClassic(){
    GameState gs;
    gs.endless = false;
    for(int i = 1; i <= 9; i++) gs.tiles[i] = true;

    while(true){
        gs.numDice = anyTileAbove6(gs.tiles) ? 2 : 1;
        gs.reopenMode = false;

        // roll immediately, no enter needed
        gs.dice1 = rollDie();
        gs.dice2 = (gs.numDice == 2) ? rollDie() : 0;
        gs.diceTotal = gs.dice1 + gs.dice2;
        gs.turns++;

        // check valid combos
        vector<vector<int>> combos = getValidCombinations(gs.diceTotal, gs.tiles, 0);
        if(combos.empty()){
            gs.message = "No valid moves!";
            gs.messageColor = RED;
            showGameOver(gs, false);
            return;
        }

        // input loop
        while(true){
            gs.message = "Close tiles that sum to " + to_string(gs.diceTotal);
            gs.messageColor = GREEN;
            drawUI(gs);

            cout << "  > ";
            string line;
            if(!getline(cin, line)){
                cout << endl;
                return;
            }
            if(line.empty()) continue;

            vector<int> chosen = parseTiles(line);
            if(chosen.empty()){
                gs.message = "No tiles entered";
                gs.messageColor = YELLOW;
                continue;
            }

            string errMsg;
            if(!validateChoice(chosen, gs.diceTotal, gs.tiles, false, errMsg)){
                gs.message = errMsg;
                gs.messageColor = RED;
                continue;
            }

            for(int t : chosen) gs.tiles[t] = false;
            break;
        }

        // win check
        if(allClosed(gs.tiles)){
            gs.message = "ALL TILES SHUT!";
            gs.messageColor = GREEN;
            showGameOver(gs, true);
            return;
        }
    }
}

// endless mode
void playEndless(){
    GameState gs;
    gs.endless = true;
    for(int i = 1; i <= 12; i++) gs.tiles[i] = true;

    while(true){
        // gs.numDice = anyTileAbove6(gs.tiles) ? 2 : 1;
        gs.numDice = 2;
        gs.reopenMode = false;

        // roll immediately
        gs.dice1 = rollDie();
        gs.dice2 = (gs.numDice == 2) ? rollDie() : 0;
        gs.diceTotal = gs.dice1 + gs.dice2;
        gs.turns++;

        // check combos (using all tiles)
        vector<vector<int>> combos = getValidCombinations(gs.diceTotal, gs.tiles, 2);

        if(combos.empty()){
            gs.message = "No moves at all! Rolling again...";
            gs.messageColor = YELLOW;
            drawUI(gs);
            cout << "  Press ENTER to continue...";
            string d;
            getline(cin, d);
            continue;
        }

        // toggle mode
        while(true){
            gs.message = "Toggle tiles that sum to " + to_string(gs.diceTotal);
            gs.messageColor = CYAN;
            drawUI(gs);

            cout << "  > ";
            string line;
            if(!getline(cin, line)){
                cout << endl;
                return;
            }
            if(line == "q" || line == "Q") return;
            if(line.empty()) continue;

            vector<int> chosen = parseTiles(line);
            if(chosen.empty()){
                gs.message = "No tiles entered";
                gs.messageColor = YELLOW;
                continue;
            }

            string errMsg;
            if(!validateChoice(chosen, gs.diceTotal, gs.tiles, true, errMsg)){
                gs.message = errMsg;
                gs.messageColor = RED;
                continue;
            }

            for(int t : chosen) gs.tiles[t] = !gs.tiles[t];  // toggle state
            break;
        }

        // all closed in endless = reset and keep going
        if(allClosed(gs.tiles)){
            gs.diceTotal = 0;
            gs.message = "ALL SHUT! Tiles reset! Finished in " + to_string(gs.turns) + " turns";
            gs.messageColor = GREEN;
            drawUI(gs);
            cout << GREEN << BOLD << "  ★ SHUT THE BOX! Resetting tiles... ★" << RESET << endl;
            cout << "  Press ENTER to continue...";
            string d;
            getline(cin, d);
            for(int i = 1; i <= 12; i++) gs.tiles[i] = true;
        }
    }
}

int main(){
    randomizeSeed();

    while(true){
        int choice = showMenu();
        if(choice == 0) break;
        if(choice == 1) playClassic();
        else if(choice == 2) playEndless();
    }

    clearScreen();
    cout << CYAN << BOLD << "  Thanks for playing Shut the Box!" << RESET << endl;
    cout << endl;
    return 0;
}
