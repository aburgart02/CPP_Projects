using namespace std;
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <tuple>
#include <cmath>
#include <random>
#include <windows.h>

class Maze {
public:
	Maze(int height, int width) {
		this->height = height;
		this->width = width;
		maze = new int* [height];
		for (int i = 0; i < height; i++)
			maze[i] = new int[width];
	}

	~Maze() {
		for (int i = 0; i < height; i++)
			delete[] maze[i];
		delete[] maze;
	}

	void CreateMaze() {
		int rnd = 0;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				maze[i][j] = wall;
			}
		}
		for (int i = 0; i < height * width; i++) {
			maze[y][x] = pass;
			while (true) {
				rnd = rand() % 4;
				switch (rnd) {
				case 0:
					if (x != 1) {
						if (maze[y][x - 2] == wall) {
							maze[y][x - 1] = pass;
							maze[y][x - 2] = pass;
							x -= 2;
						}
					};
				case 1:
					if (x != width - 2) {
						if (maze[y][x + 2] == wall) {
							maze[y][x + 1] = pass;
							maze[y][x + 2] = pass;
							x += 2;
						}
					};
				case 2:
					if (y != 1) {
						if (maze[y - 2][x] == wall) {
							maze[y - 1][x] = pass;
							maze[y - 2][x] = pass;
							y -= 2;
						}
					};
				case 3:
					if (y != height - 2) {
						if (maze[y + 2][x] == wall) {
							maze[y + 1][x] = pass;
							maze[y + 2][x] = pass;
							y += 2;
						}
					};
				}
				if (IsDeadEnd())
					break;
			}
			if (IsDeadEnd()) {
				do {
					x = 2 * (rand() % ((width - 1) / 2)) + 1;
					y = 2 * (rand() % ((height - 1) / 2)) + 1;
				} while (maze[y][x] != pass);
			}
		}
	}

	void AStar() {
		map<tuple<int, int>, vector<tuple<tuple<int, int>, int>>> grid;
		MakeAdjacencyList(grid);
		tuple<int, int> start_v = make_tuple(1, 1);
		tuple<int, int> target_v = make_tuple(height - 2, width - 2);
		map<tuple<int, int>, int> D;
		D[start_v] = 0;
		map<tuple<int, int>, tuple<int, int>> P;
		P[start_v] = make_tuple(-1, -1);
		priority_queue<tuple<int, tuple<int, int>>, vector<tuple<int, tuple<int, int>>>, greater<tuple<int, tuple<int, int>>>> pq;
		pq.push(make_tuple(Heuristic(start_v, target_v), start_v));
		while (!pq.empty()) {
			int priority;
			tuple<int, int> v;
			tie(priority, v) = pq.top();
			pq.pop();
			int px, py;
			tie(px, py) = v;
			if (py == width - 2 && px == height - 2)
				break;
			for (auto nv : grid[v]) {
				tuple<int, int> neighbour;
				int cost;
				tie(neighbour, cost) = nv;
				int new_cost = D[v] + cost;
				if (D.count(neighbour) == 0 || new_cost < D[neighbour]) {
					D[neighbour] = new_cost;
					P[neighbour] = v;
					priority = new_cost + Heuristic(target_v, neighbour);
					pq.push(make_tuple(priority, neighbour));
				}
			}
		}
		RestorePath(target_v, P);
	}

private:
	bool IsDeadEnd() {
		int count = 0;
		if (x != 1) {
			if (maze[y][x - 2] == pass)
				count += 1;
		}
		else
			count += 1;
		if (x != width - 2) {
			if (maze[y][x + 2] == pass)
				count += 1;
		}
		else
			count += 1;
		if (y != 1) {
			if (maze[y - 2][x] == pass)
				count += 1;
		}
		else
			count += 1;
		if (y != height - 2) {
			if (maze[y + 2][x] == pass)
				count += 1;
		}
		else
			count += 1;
		if (count == 4)
			return true;
		return false;
	}

	void MakeAdjacencyList(map<tuple<int, int>, vector<tuple<tuple<int, int>, int>>>& grid)
	{
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				vector<tuple<tuple<int, int>, int>> list;
				if (0 <= i - 1 && i - 1 < height && 0 <= j && j < width)
					list.push_back(make_tuple(make_tuple(i - 1, j), maze[i - 1][j] * 10000));
				if (0 <= i && i < height && 0 <= j - 1 && j - 1 < width)
					list.push_back(make_tuple(make_tuple(i, j - 1), maze[i][j - 1] * 10000));
				if (0 <= i + 1 && i + 1 < height && 0 <= j && j < width)
					list.push_back(make_tuple(make_tuple(i + 1, j), maze[i + 1][j] * 10000));
				if (0 <= i && i < height && 0 <= j + 1 && j + 1 < width)
					list.push_back(make_tuple(make_tuple(i, j + 1), maze[i][j + 1] * 10000));
				grid[make_tuple(i, j)] = list;
			}
		}
	}

	int Heuristic(tuple<int, int> target_v, tuple<int, int> neighbour) {
		int tx, ty, nx, ny;
		tie(tx, ty) = target_v;
		tie(nx, ny) = neighbour;
		return abs(tx - nx) + abs(ty - ny);
	}

	void RestorePath(tuple<int, int> target_v, map<tuple<int, int>, tuple<int, int>> P)
	{
		vector<tuple<int, int>> path;
		while (target_v != make_tuple(-1, -1))
		{
			path.push_back(target_v);
			target_v = P[target_v];
		}
		reverse(path.begin(), path.end());
		for (auto v : path) {
			int x, y;
			tie(x, y) = v;
			maze[x][y] = point;
			PrintMaze();
			Sleep(100);
		}
	}

	void PrintMaze() {
		DWORD dword;
		HANDLE handle = (HANDLE)GetStdHandle(STD_OUTPUT_HANDLE);
		string s;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (maze[i][j] == pass)
					s += ' ';
				if (maze[i][j] == wall)
					s += '0';
				if (maze[i][j] == point)
					s += '*';
			}
		}
		wstring stemp = wstring(s.begin(), s.end());
		LPCWSTR sw = stemp.c_str();
		WriteConsoleOutputCharacter(handle, sw, height * width, COORD{ 0, 0 }, &dword);
	}

	int height = 0;
	int width = 0;
	enum Cell { pass, wall, point };
	int x = 1;
	int y = 1;
	int** maze;
};

int main() {
	system("mode con cols=101 lines=51");
	srand((unsigned)time(NULL));
	Maze maze(51, 101);
	maze.CreateMaze();
	maze.AStar();
	return 0;
}