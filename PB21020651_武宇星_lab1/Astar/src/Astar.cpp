#include<vector>
#include<iostream>
#include<queue>
#include<map>
#include<fstream>
#include<sstream>
#include<string>
#include <chrono>

using namespace std;

struct Map_Cell
{
    int type;
    // TODO: 定义地图信息

};

struct Search_Cell
{
    int h;
    int g;
    // TODO: 定义搜索状态
    int x;
    int y;
    int supply;
    Search_Cell *parent;
    // int step;
};

// 自定义比较函数对象，按照 Search_Cell 结构体的 g + h 属性进行比较
struct CompareF {
    bool operator()(const Search_Cell *a, const Search_Cell *b) const {
        return (a->g + a->h) > (b->g + b->h); // 较小的 g + h 值优先级更高
    }
};

// TODO: 定义启发式函数
int Heuristic_Funtion(const Search_Cell &current, const pair<int, int> &end_point)
{
    // return 0;
    return abs(current.x - end_point.first) + abs(current.y - end_point.second);
}

void Astar_search(const string input_file, int &step_nums, string &way)
{
    ifstream file(input_file);
    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        return;
    }

    string line;
    getline(file, line); // 读取第一行
    stringstream ss(line);
    string word;
    vector<string> words;
    while (ss >> word) {
        words.push_back(word);
    }
    int M = stoi(words[0]);
    int N = stoi(words[1]);
    int T = stoi(words[2]);

    pair<int, int> start_point; // 起点
    pair<int, int> end_point;   // 终点
    Map_Cell **Map = new Map_Cell *[M];
    // 加载地图
    for(int i = 0; i < M; i++)
    {
        Map[i] = new Map_Cell[N];
        getline(file, line);
        stringstream ss(line);
        string word;
        vector<string> words;
        while (ss >> word) {
            words.push_back(word);
        }
        for(int j = 0; j < N; j++)
        {
            Map[i][j].type = stoi(words[j]);
            if(Map[i][j].type == 3)
            {
                start_point = {i, j};
            }
            else if(Map[i][j].type == 4)
            {
                end_point = {i, j};
            }
        }
    }
    // 以上为预处理部分
    // ------------------------------------------------------------------

    Search_Cell *search_cell = new Search_Cell;
    search_cell->g = 0;
    search_cell->h = Heuristic_Funtion(*search_cell, end_point);
    search_cell->x = start_point.first;
    search_cell->y = start_point.second;
    search_cell->parent = nullptr;
    search_cell->supply = T;
    // search_cell->step = 0;

    priority_queue<Search_Cell *, vector<Search_Cell *>, CompareF> open_list;
    vector<Search_Cell *> close_list;
    open_list.push(search_cell);

    int dx[4] = {0, 0, 1, -1};
    int dy[4] = {1, -1, 0, 0};

    while(!open_list.empty())
    {
        // TODO: A*搜索过程实现
        Search_Cell *current = open_list.top();
        open_list.pop();
        if(current->x == end_point.first && current->y == end_point.second)
        {
            step_nums = current->g;
            Search_Cell *temp = current;
            while(temp->parent != nullptr)
            {
                if(temp->x - temp->parent->x == 1)
                {
                    way = "D" + way;
                }
                else if(temp->x - temp->parent->x == -1)
                {
                    way = "U" + way;
                }
                else if(temp->y - temp->parent->y == 1)
                {
                    way = "R" + way;
                }
                else if(temp->y - temp->parent->y == -1)
                {
                    way = "L" + way;
                }
                temp = temp->parent;
            }
            break;
        }
        close_list.push_back(current);
        for(int i = 0; i < 4; i++)
        {
            int new_x = current->x + dx[i];
            int new_y = current->y + dy[i];
            if(new_x < 0 || new_x >= M || new_y < 0 || new_y >= N)
            {
                continue;
            }
            if(Map[new_x][new_y].type == 1)
            {
                continue;
            }
            Search_Cell *new_cell = new Search_Cell;
            new_cell->x = new_x;
            new_cell->y = new_y;
            new_cell->g = current->g + 1;
            new_cell->h = Heuristic_Funtion(*new_cell, end_point);
            new_cell->parent = current;
            if(Map[new_x][new_y].type == 2)
            {
                new_cell->supply = T;
            }
            else
            {
                new_cell->supply = current->supply - 1;
            }
            if(new_cell->supply = 0)
            {
                continue;
            }
            bool flag = false;
            for(int j = 0; j < close_list.size(); j++)
            {
                if(new_cell->x == close_list[j]->x && new_cell->y == close_list[j]->y)
                {
                    flag = true;
                    break;
                }
            }
            if(flag)
            {
                continue;
            }
            open_list.push(new_cell);
        }
    }

    // ------------------------------------------------------------------
    // 释放动态内存
    for(int i = 0; i < M; i++)
    {
        delete[] Map[i];
    }
    delete[] Map;
    while(!open_list.empty())
    {
        auto temp = open_list.top();
        delete[] temp;
        open_list.pop();
    }
    for(int i = 0; i < close_list.size(); i++)
    {
        delete[] close_list[i];
    }

    return;
}

void output(const string output_file, int &step_nums, string &way)
{
    ofstream file(output_file);
    if(file.is_open())
    {
        file << step_nums << endl;
        if(step_nums >= 0)
        {
            file << way << endl;
        }

        file.close();
    }
    else
    {
        cerr << "Can not open file: " << output_file << endl;
    }
    return;
}

int main(int argc, char *argv[])
{
    string input_base = "../input/input_";
    string output_base = "../output/output_";
    // input_0为讲义样例，此处不做测试
    for(int i = 0; i < 11; i++)
    {
        int step_nums = -1;
        string way = "";
        auto start = std::chrono::steady_clock::now();
        Astar_search(input_base + to_string(i) + ".txt", step_nums, way);
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        cout << "Input " << i << " time cost: " << double(duration.count()) << "us" << endl;
        output(output_base + to_string(i) + ".txt", step_nums, way);
    }
    return 0;
}