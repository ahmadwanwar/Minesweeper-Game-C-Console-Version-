#include <iostream>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <fstream>

using namespace std;

class Cell {
protected:
    bool mine;
    bool revealed;
    bool flagged;
    int adjmines;

public:
    Cell() : mine(false), revealed(false), flagged(false), adjmines(0) {}

    void setmine(bool isMine) {
        mine = isMine;
    }

    bool ismine() const {
        return mine;
    }

    void setrevealed(bool isRevealed) {
        revealed = isRevealed;
    }

    bool isrevealed() const {
        return revealed;
    }

    void setflagged(bool isFlagged) {
        flagged = isFlagged;
    }

    bool isflagged() const {
        return flagged;
    }

    void setadjmines(int count) {
        adjmines = count;
    }

    int getadjmines() const {
        return adjmines;
    }
};
const int sizefr = 10;

class minesweeper {
private:
    Cell board[sizefr][sizefr];
    int nmines;
    bool gameover;
    chrono::time_point<chrono::system_clock> startTime;
    string wname;

public:
    minesweeper(int nm) : nmines(nm), gameover(false) {
        initialiseboard();
        placemines();
        calc_adjmines();
        startTime = chrono::system_clock::now();
    }

    void initialiseboard() {
        for (int i = 0; i < sizefr; i++) {
            for (int j = 0; j < sizefr; j++) {
                board[i][j] = Cell();
            }
        }
    }

    void placemines() {
        srand(time(NULL));
        int minesplaced = 0;
        while (minesplaced < nmines) {
            int x = rand() % sizefr;
            int y = rand() % sizefr;
            if (!(board[x][y].ismine())) {
                board[x][y].setmine(true);
                minesplaced++;
            }
        }
    }

    void calc_adjmines() {
        for (int i = 0; i < sizefr; i++) {
            for (int j = 0; j < sizefr; j++) {
                if (!(board[i][j].ismine())) {
                    int count = 0;
                    for (int nx = -1; nx <= 1; nx++) {
                        for (int ny = -1; ny <= 1; ny++) {
                            int ni = i + nx;
                            int nj = j + ny;
                            if ((ni >= 0) && (ni < sizefr) && (nj >= 0) && (nj < sizefr) && (board[ni][nj].ismine())) {
                                count++;
                            }
                        }
                    }
                    board[i][j].setadjmines(count);
                }
            }
        }
    }

    void display() {
        cout << "   ";
        for (int i = 0; i < sizefr; i++) {
            cout << i << "  ";
        }
        cout << endl;
        for (int j = 0; j < sizefr; j++) {
            cout << j << "|";
            for (int i = 0; i < sizefr; i++) {
                if (board[j][i].isrevealed()) {
                    if (board[j][i].ismine()) {
                        cout << " * ";
                    }
                    else {
                        cout << " "<< board[j][i].getadjmines() << " ";
                    }
                }
                else {
                    if (board[j][i].isflagged()) {
                        cout << " F ";
                    }
                    else {
                        cout << " # ";
                    }
                }
            }
            cout << endl;
        }
    }

    void leftclick(int r, int c) {
        if ((r < 0) || (r >= sizefr) || (c < 0) || (c >= sizefr) || (board[r][c].isrevealed()) || (board[r][c].isflagged())) {
            return;
        }
        else {
            if (board[r][c].ismine()) {
                gameover = true;
                revealmines();
            }
            else {
                board[r][c].setrevealed(true);
                if (board[r][c].getadjmines() == 0) {
                    exposeemptycell(r, c);
                }
            }
        }
    }

    void rightclick(int r, int c) {
        if ((r < 0) || (r >= sizefr) || (c < 0) || (c >= sizefr) || (board[r][c].isrevealed())) {
            return;
        }
        else {
            if (!(board[r][c].isflagged())) {
                board[r][c].setflagged(true);
            }
            else if (board[r][c].isflagged()) {
                board[r][c].setflagged(false);
            }
        }
    }

    void exposeemptycell(int r, int c) {
        for (int nx = -1; nx <= 1; nx++) {
            for (int ny = -1; ny <= 1; ny++) {
                int ni = r + nx;
                int nj = c + ny;
                if ((ni >= 0) && (ni < sizefr) && (nj >= 0) && (nj < sizefr) && (!(board[ni][nj].isrevealed()))) {
                    board[ni][nj].setrevealed(true);
                    if (board[ni][nj].getadjmines() == 0) {
                        exposeemptycell(ni, nj);
                    }
                }
            }
        }
    }

    void revealmines() {
        for (int i = 0; i < sizefr; i++) {
            for (int j = 0; j < sizefr; j++) {
                if (board[i][j].ismine()) {
                    board[i][j].setrevealed(true);
                }
            }
        }
    }

    bool isgameover() const {
        return gameover;
    }

    bool isgamewon() const {
        int flagsPlaced = 0;
        for (int i = 0; i < sizefr; i++) {
            for (int j = 0; j < sizefr; j++) {
                if (board[i][j].isflagged() && board[i][j].ismine()) {
                    flagsPlaced++;
                }
            }
        }
        return flagsPlaced == nmines;
    }

    double timerstop() {
        auto endTime = chrono::system_clock::now();
        chrono::duration<double> elapsedSeconds = endTime - startTime;
        return elapsedSeconds.count();
    }

    void timershow() {
        auto endTime = chrono::system_clock::now();
        chrono::duration<double> elapsedSeconds = endTime - startTime;
        cout << "time: "<< elapsedSeconds.count() << " seconds." << endl;
    }

    void setwinnername(string& name) {
        wname = name;
    }

    string getwinner()const {
        return wname;
    }

    void addwinner(const string& name, double elapsedtime) {
        ofstream outfile("winners.txt", ios::app);
        if (outfile.is_open()) {
            outfile << "winner: " << name << " time: " << elapsedtime << " seconds" << endl;
            outfile.close();
            cout << "winner's name and time added" << endl;
        }
        else {
            cout << "cannot open file" << endl;
        }
    }
};

int main() {
    int row, col, mines, difficultylvl;
    char action;
    string name;
    cout << "enter your name" << endl;
    cin >> name;
    cout << "select the difficulty level:" << endl;
    cout << "1. easy" << endl;
    cout << "2. intermediate" << endl;
    cout << "3. advanced" << endl;
    cin >> difficultylvl;
    if (difficultylvl == 1) {
        mines = 10;
    }
    else if (difficultylvl == 2) {
        mines = 40;
    }
    else if (difficultylvl == 3) {
        mines = 99;
    }
    else {
        cout << "invalid input" << endl;
        return -1;
    }
    minesweeper game(mines);
    while (!(game.isgameover()) && (!(game.isgamewon()))) {
        game.display();
        cout << "enter the row, column, and the action (L or F):" << endl;
        cin >> row >> col >> action;
        if (action == 'L') {
            game.leftclick(row, col);
        }
        else if (action == 'F') {
            game.rightclick(row, col);
        }
        game.timershow();

    }
    double elapsedtime = game.timerstop();
    if (game.isgamewon()) {
        cout << name<<" won in ";
        game.timershow();
        game.setwinnername(name);
        game.addwinner(name, elapsedtime);

    }
    else {
        cout << "you've lost. ";
    }
    game.timerstop();
    return 0;
}
