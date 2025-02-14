#include <fstream>
#include "node.h"

using namespace ChineseChess;

//博弈树搜索，depth为搜索深度
int alphaBeta(GameTreeNode node, int alpha, int beta, int depth, bool isMaximizer) {
    if (depth == 0) {
        return node.getEvaluationScore();
    }
    ChessBoard board = node.getBoardClass();
    if (board.judgeTermination()) {
        // int red_king = 0;
        // std::vector<ChessPiece> pieces = board.getChessPiece();
        // for (int i = 0; i < pieces.size(); i++) {
        //     if (pieces[i].name == 'K') {
        //         red_king++;
        //         break;
        //     }
        // }
        // return red_king == 0 ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
        return board.evaluateNode();
    }
    //TODO alpha-beta剪枝过程
    if (isMaximizer) {
        int maxEval = std::numeric_limits<int>::min();
        for (Move move: board.getMoves(true)) {
            GameTreeNode* childptr = node.updateBoard(board.getBoard(), move, false);
            GameTreeNode child = *childptr;
            delete childptr;
            int eval = alphaBeta(child, alpha, beta, depth - 1, false);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta < alpha) {
                break;
            }
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (Move move: board.getMoves(false)) {
            GameTreeNode* childptr = node.updateBoard(board.getBoard(), move, true);
            GameTreeNode child = *childptr;
            delete childptr;
            int eval = alphaBeta(child, alpha, beta, depth - 1, true);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) {
                break;
            }
        }
        return minEval;
    }
    
}

int main() {
    std::ifstream file("../input/8.txt");
    std::vector<std::vector<char>> board;

    std::string line;
    int n = 0;
    while (std::getline(file, line)) {
        std::vector<char> row;

        for (char ch : line) {
            row.push_back(ch);
        }

        board.push_back(row);
        n = n + 1;
        if (n >= 10) break;
    }

    file.close();
    GameTreeNode root(true, board, std::numeric_limits<int>::min());
    bool player = true;
    int depth = 3;
    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();
    int step = 0;

    // Move test_move;
    // test_move.init_x = 3;
    // test_move.init_y = 5;
    // test_move.next_x = 3;
    // test_move.next_y = 0;
    // test_move.score = 0;
    // ChessBoard _board = root.getBoardClass();
    // std::vector<std::vector<char>> test_board = _board.getBoard();
    // GameTreeNode test_node = *root.updateBoard(test_board, test_move, true);
    // int score = alphaBeta(test_node, alpha, beta, depth, false);
    // root = *root.updateBoard(test_board, test_move, true);
    // player = !player;

    while (true) {
        ChessBoard cur_board = root.getBoardClass();
        board = cur_board.getBoard();
        //代码测试
        ChessBoard _board = root.getBoardClass();
        std::vector<std::vector<char>> test_board = _board.getBoard();

        for (int i = 0; i < test_board.size(); i++) {
            for (int j = 0; j < test_board[0].size(); j++) {
                std::cout << test_board[i][j];
            }
            std::cout << std::endl;
        }

        // std::vector<Move> red_moves = _board.getMoves(true);
        // // print all moves
        // for (int i = 0; i < red_moves.size(); i++) {
        //     std::cout << board[red_moves[i].init_y][red_moves[i].init_x] << std::endl;
        //     std::cout << "init: " << red_moves[i].init_x <<  " " << red_moves[i].init_y << std::endl;
        //     std::cout << "next: " << red_moves[i].next_x <<  " " << red_moves[i].next_y << std::endl;
        // }

        // Move test_move;
        // test_move.init_x = 3;
        // test_move.init_y = 5;
        // test_move.next_x = 3;
        // test_move.next_y = 0;
        // test_move.score = 0;
        // GameTreeNode test_node = *root.updateBoard(test_board, test_move, true);
        // int score = alphaBeta(test_node, alpha, beta, depth, false);
        // std::cout << "score: " << score << std::endl;
        // root = *root.updateBoard(test_board, test_move, true);
        // _board = root.getBoardClass();
        // test_board = _board.getBoard();
        // player = !player;

        // std::vector<Move> red_moves = _board.getMoves(true);
        // std::vector<Move> black_moves = _board.getMoves(false);

        // for (int i = 0; i < red_moves.size(); i++) {
        //     std::cout << "init: " << red_moves[i].init_x <<  " " << red_moves[i].init_y << std::endl;
        //     std::cout << "next: " << red_moves[i].next_x <<  " " << red_moves[i].next_y << std::endl;
        //     std::cout << "score " << red_moves[i].score << std::endl;
        // }
        // for (int i = 0; i < black_moves.size(); i++) {
        //     std::cout << "init: " << black_moves[i].init_x <<  " " << black_moves[i].init_y << std::endl;
        //     std::cout << "next: " << black_moves[i].next_x <<  " " << black_moves[i].next_y << std::endl;
        //     std::cout << "score " << black_moves[i].score << std::endl;
        // }
        if (cur_board.judgeTermination() || step >= 100) {
            std::cout << "step: " << step << std::endl;
            break;
        }
        int best_score = player ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
        Move best_move;
        std::vector<Move> next_moves = cur_board.getMoves(player);
        for (Move move: next_moves) {
            std::cout << "move:" << board[move.init_y][move.init_x] << " (" << move.init_x 
                    << "," << move.init_y << ") (" << move.next_x << "," << move.next_y 
                    << ")" <<std::endl;
            GameTreeNode* childptr = root.updateBoard(cur_board.getBoard(), move, player);
            GameTreeNode child = *childptr;
            delete childptr;
            int score = alphaBeta(child, alpha, beta, depth, !player);
            std::cout << "score: " << score << std::endl;
            if (player) {
                if(score > 9000) {
                    best_score = score;
                    best_move = move;
                    break;
                }
                if (score > best_score) {
                    best_score = score;
                    best_move = move;
                }
            } else {
                if(score < -9000) {
                    best_score = score;
                    best_move = move;
                    break;
                }
                if (score < best_score) {
                    best_score = score;
                    best_move = move;
                }
            }
        }
        std::cout << "best_move:" << board[best_move.init_y][best_move.init_x] << " (" << best_move.init_x 
        << "," << best_move.init_y << ") (" << best_move.next_x << "," << best_move.next_y 
        << ")" <<std::endl;
        board = cur_board.getBoard();
        root = *root.updateBoard(board, best_move, player);
        std::ofstream output_file("../output/8.txt", std::ios::app);
        output_file << board[best_move.init_y][best_move.init_x] << " (" << best_move.init_x 
        << "," << best_move.init_y << ") (" << best_move.next_x << "," << best_move.next_y 
        << ")" <<std::endl;
        output_file.close();
        player = !player;
        step++;
    }


    //TODO
    // alphaBeta(root, )


    return 0;
}