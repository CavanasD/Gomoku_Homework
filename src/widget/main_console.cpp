#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <windows.h>

#include "widget/gomokuLogic.h"
#include "widget/aibrain.h"

using namespace std;

bool isPvE = true;

vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// 随机兜底
std::pair<int, int> getRandomMove(const GomokuLogic& game) {
    auto board = game.getBoard();
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (board[i][j] == 0) return {i, j};
        }
    }
    return {-1, -1};
}

int main() {
    SetConsoleOutputCP(65001);
    setvbuf(stdout, NULL, _IONBF, 0);

    GomokuLogic game;
    // 保持你的 AI 参数不变
    AlphaBeta ai(1000, 10000, 1.414, true, 2);

    while (true) {
        string line;
        if (!getline(cin, line)) break;

        vector<string> parts = split(line, ' ');
        if (parts.empty()) continue;
        string command = parts[0];

        // --- 1. 切换模式 ---
        if (command == "SET_MODE") {
            if (parts.size() >= 2) {
                string mode = parts[1];
                isPvE = (mode == "PVE");

                game.reset(); // 重置棋盘
                cout << "GAME_STARTED" << endl;

                // ★★★ 核心修改：如果是 PVE，流萤酱(黑)直接先手 ★★★
                if (isPvE) {
                    game.placePiece(7, 7); // 必走天元
                    cout << "MOVED 7,7,1" << endl; // 1=黑
                }
            }
        }
        // --- 2. 重开 ---
        else if (command == "RESTART") {
            game.reset();
            cout << "GAME_STARTED" << endl;

            // ★★★ 核心修改：重开时流萤酱也要先手 ★★★
            if (isPvE) {
                game.placePiece(7, 7);
                cout << "MOVED 7,7,1" << endl;
            }
        }
        // --- 3. 落子 ---
        else if (command == "MOVE") {
            if (parts.size() < 2) continue;
            vector<string> coords = split(parts[1], ',');
            int x = stoi(coords[0]);
            int y = stoi(coords[1]);

            // 玩家(白) 或 PVP对手 落子
            if (!game.placePiece(x, y)) {
                cout << "INVALID_MOVE" << endl;
                continue;
            }

            int pieceColor = game.getBoard()[x][y];
            cout << "MOVED " << x << "," << y << "," << pieceColor << endl;

            if (game.state() == GomokuLogic::BlackWin) { cout << "WINNER BLACK" << endl; continue; }
            else if (game.state() == GomokuLogic::WhiteWin) { cout << "WINNER WHITE" << endl; continue; }
            else if (game.state() == GomokuLogic::Draw) { cout << "WINNER DRAW" << endl; continue; }

            // ★★★ 核心修改：AI 触发时机 ★★★
            // 只有当“刚才下的是白棋(2)”时，AI(黑)才行动
            if (isPvE && pieceColor == 2 && game.state() == GomokuLogic::InProgress) {

                cout << "AI_THINKING" << endl;

                std::pair<int, int> aiMove = ai.getBestMove(game.getBoard());

                if (aiMove.first < 0 || aiMove.first >= 15 || aiMove.second < 0 || aiMove.second >= 15) {
                    aiMove = getRandomMove(game);
                }

                if (game.placePiece(aiMove.first, aiMove.second)) {
                    // AI 执黑(1)
                    cout << "MOVED " << aiMove.first << "," << aiMove.second << ",1" << endl;

                    if (game.state() == GomokuLogic::WhiteWin) { cout << "WINNER WHITE" << endl; }
                    else if (game.state() == GomokuLogic::BlackWin) { cout << "WINNER BLACK" << endl; }
                    else if (game.state() == GomokuLogic::Draw) { cout << "WINNER DRAW" << endl; }
                }
            }
        }
    }
    return 0;
}