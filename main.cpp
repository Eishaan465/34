#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <string>
#include <limits>
using namespace std;

// Software Design Final Project
// Tic Tac Toe vs Local AI
// Player is X and Ollama AI is O

void showBoard(vector<char>& board) {
    cout << endl;
    cout << " " << board[0] << " | " << board[1] << " | " << board[2] << endl;
    cout << "---+---+---" << endl;
    cout << " " << board[3] << " | " << board[4] << " | " << board[5] << endl;
    cout << "---+---+---" << endl;
    cout << " " << board[6] << " | " << board[7] << " | " << board[8] << endl;
    cout << endl;
}

bool checkWin(vector<char>& board, char player) {
    if (board[0] == player && board[1] == player && board[2] == player) return true;
    if (board[3] == player && board[4] == player && board[5] == player) return true;
    if (board[6] == player && board[7] == player && board[8] == player) return true;
    if (board[0] == player && board[3] == player && board[6] == player) return true;
    if (board[1] == player && board[4] == player && board[7] == player) return true;
    if (board[2] == player && board[5] == player && board[8] == player) return true;
    if (board[0] == player && board[4] == player && board[8] == player) return true;
    if (board[2] == player && board[4] == player && board[6] == player) return true;
    return false;
}

bool boardFull(vector<char>& board) {
    for (int i = 0; i < board.size(); i++) {
        if (board[i] != 'X' && board[i] != 'O') return false;
    }
    return true;
}

bool spotOpen(vector<char>& board, int spot) {
    if (spot < 0 || spot > 8) return false;
    if (board[spot] == 'X' || board[spot] == 'O') return false;
    return true;
}

string getOpenSpots(vector<char>& board) {
    string spots = "";
    for (int i = 0; i < board.size(); i++) {
        if (board[i] != 'X' && board[i] != 'O') {
            spots += to_string(i + 1) + " ";
        }
    }
    return spots;
}

string getBoardText(vector<char>& board) {
    string text = "";
    text += board[0]; text += " "; text += board[1]; text += " "; text += board[2]; text += " / ";
    text += board[3]; text += " "; text += board[4]; text += " "; text += board[5]; text += " / ";
    text += board[6]; text += " "; text += board[7]; text += " "; text += board[8];
    return text;
}

int getAIMove(vector<char>& board) {
    string prompt = "";
    prompt += "You are playing Tic Tac Toe as O. ";
    prompt += "The human is X. ";
    prompt += "The board spots are numbered 1 2 3 on the top row, 4 5 6 in the middle, 7 8 9 on the bottom. ";
    prompt += "Current board: ";
    prompt += getBoardText(board);
    prompt += ". Open spots: ";
    prompt += getOpenSpots(board);
    prompt += ". Pick one open spot number. ";
    prompt += "Win if you can. If X is about to win, block X. ";
    prompt += "Reply with only a single digit number, nothing else.";

    string command =
        "curl -s http://localhost:11434/api/generate "
        "-H \"Content-Type: application/json\" "
        "-d \"{\\\"model\\\":\\\"gemma3:4b\\\",\\\"prompt\\\":\\\"" + prompt +
        "\\\",\\\"stream\\\":false}\" > ai_response.txt";

    system(command.c_str());

    ifstream file("ai_response.txt");
    if (!file) {
        cout << "Could not read AI response." << endl;
        return -1;
    }

    string answer = "";
    string line;
    while (getline(file, line)) {
        answer += line;
    }
    file.close();

    // Look for the number inside the "response":"..." field only
    string responseTag = "\"response\":\"";
    int tagPos = answer.find(responseTag);
    if (tagPos != string::npos) {
        int numStart = tagPos + responseTag.length();
        for (int i = numStart; i < numStart + 5; i++) {
            if (answer[i] >= '1' && answer[i] <= '9') {
                int move = answer[i] - '1';
                if (spotOpen(board, move)) {
                    return move;
                }
            }
        }
    }

    // Fallback: take the first open spot
    for (int i = 0; i < board.size(); i++) {
        if (spotOpen(board, i)) return i;
    }

    return -1;
}

int main() {
    vector<char> board = {'1','2','3','4','5','6','7','8','9'};
    int choice;
    bool gameOver = false;

    cout << "Tic Tac Toe vs Local AI" << endl;
    cout << "You are X. The AI is O." << endl;
    cout << "Enter a number 1-9 to pick your spot." << endl;

    while (gameOver == false) {
        showBoard(board);

        cout << "Your move: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a number." << endl;
            continue;
        }

        choice = choice - 1;

        if (!spotOpen(board, choice)) {
            cout << "That spot is taken. Try again." << endl;
            continue;
        }

        board[choice] = 'X';

        if (checkWin(board, 'X')) {
            showBoard(board);
            cout << "You win!" << endl;
            gameOver = true;
        }
        else if (boardFull(board)) {
            showBoard(board);
            cout << "It's a tie." << endl;
            gameOver = true;
        }
        else {
            cout << "AI is thinking..." << endl;
            int aiMove = getAIMove(board);

            if (aiMove == -1) {
                cout << "AI failed to move." << endl;
                gameOver = true;
            }
            else {
                board[aiMove] = 'O';
                cout << "AI picked spot " << aiMove + 1 << "." << endl;

                if (checkWin(board, 'O')) {
                    showBoard(board);
                    cout << "AI wins." << endl;
                    gameOver = true;
                }
                else if (boardFull(board)) {
                    showBoard(board);
                    cout << "It's a tie." << endl;
                    gameOver = true;
                }
            }
        }
    }

    cout << "Thanks for playing." << endl;
    return 0;
}