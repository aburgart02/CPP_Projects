using namespace std;
#include <string>
#include <vector>
#include <random>
#include <windows.h>

class Tetris {
public:
	Tetris() : game_map(20, 20) { }

	void StartGame() {
		GenerateFigure();
		while (game_state) {
			time++;
			if (GetAsyncKeyState(VK_LEFT)) {
				UpdateMap(-1, 0);
				ProcessAction();
			}
			if (GetAsyncKeyState(VK_RIGHT)) {
				UpdateMap(1, 0);
				ProcessAction();
			}
			if (GetAsyncKeyState(VK_DOWN)) {
				UpdateMap(0, 1);
				ProcessAction();
			}
			if (GetAsyncKeyState(VK_UP)) {
				figure->Rotate(game_map, next_figure);
				UpdateMap(0, 0);
				ProcessAction(2);
			}
			if (next_figure) {
				delete figure;
				GenerateFigure();
				next_figure = false;
			}
			if (time > speed) {
				time = 0;
				UpdateMap(0, 1);
				PrintMap();
			}
		}
	}

private:
	struct Map {
		Map(int height, int width) {
			this->height = height;
			this->width = width;
			map = new int* [height];
			for (int i = 0; i < height; i++) {
				map[i] = new int[width];
			}
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					map[i][j] = space;
				}
			}
		}
		int height;
		int width;
		int** map;
	};

	struct Figure
	{
		void Rotate(Map& game_map, bool& next_figure) {
			if (!rotatable)
				return;
			pair<int, int> pivot = cells[0];
			int px = pivot.first, py = pivot.second;
			vector<pair<int, int>> temp_cells;
			for (auto cell : cells)
				temp_cells.push_back(make_pair(px + py - cell.second, cell.first + py - px));
			for (auto cell : temp_cells) {
				if ((game_map.map[cell.second][cell.first] == block && CheckCell(cells, cell.second, cell.first))
					|| cell.first == -1 || cell.first == game_map.width)
					return;
			}
			for (auto cell : cells)
				game_map.map[cell.second][cell.first] = space;
			cells = temp_cells;
			for (auto cell : cells)
				game_map.map[cell.second][cell.first] = block;
		}

		bool rotatable = true;
		bool mobile = true;
		vector<pair<int, int>> cells;
	};

	struct TFigure : Figure
	{
		TFigure() {
			cells.push_back(make_pair(5, 1));
			cells.push_back(make_pair(4, 1));
			cells.push_back(make_pair(5, 0));
			cells.push_back(make_pair(6, 1));
		}
	};

	struct Square : Figure
	{
		Square() {
			cells.push_back(make_pair(5, 1));
			cells.push_back(make_pair(5, 0));
			cells.push_back(make_pair(6, 0));
			cells.push_back(make_pair(6, 1));
			rotatable = false;
		}
	};

	struct LeftL : Figure
	{
		LeftL() {
			cells.push_back(make_pair(5, 1));
			cells.push_back(make_pair(4, 1));
			cells.push_back(make_pair(4, 0));
			cells.push_back(make_pair(6, 1));
		}
	};

	struct RightL : Figure
	{
		RightL() {
			cells.push_back(make_pair(5, 1));
			cells.push_back(make_pair(4, 1));
			cells.push_back(make_pair(6, 1));
			cells.push_back(make_pair(6, 0));
		}
	};

	struct Line : Figure
	{
		Line() {
			cells.push_back(make_pair(5, 0));
			cells.push_back(make_pair(4, 0));
			cells.push_back(make_pair(6, 0));
			cells.push_back(make_pair(7, 0));
		}
	};

	struct LeftS : Figure
	{
		LeftS() {
			cells.push_back(make_pair(5, 1));
			cells.push_back(make_pair(4, 1));
			cells.push_back(make_pair(5, 0));
			cells.push_back(make_pair(6, 0));
		}
	};

	struct RightS : Figure
	{
		RightS() {
			cells.push_back(make_pair(5, 1));
			cells.push_back(make_pair(5, 0));
			cells.push_back(make_pair(4, 0));
			cells.push_back(make_pair(6, 1));
		}
	};

	void CheckNeighbours(bool& hasLeftNeighbour, bool& hasRightNeighbour)
	{
		for (auto cell : figure->cells) {
			if (game_map.map[cell.second][cell.first - 1] == block && CheckCell(figure->cells, cell.second, cell.first - 1)
				|| cell.first - 1 == -1)
				hasLeftNeighbour = true;
			if (game_map.map[cell.second][cell.first + 1] == block && CheckCell(figure->cells, cell.second, cell.first + 1)
				|| cell.first + 1 == game_map.width)
				hasRightNeighbour = true;
		}
	}

	void RemoveFilledColumns()
	{
		int count = 0;
		vector<int> columns;
		for (int i = 0; i < game_map.height; i++) {
			count = 0;
			for (int j = 0; j < game_map.width; j++) {
				if (game_map.map[i][j] == block)
					count++;
				if (count == game_map.width)
					columns.push_back(i);
			}
		}
		for (auto column : columns) {
			game_score += 1;
			for (int i = column; i > 0; i--) {
				for (int j = 0; j < game_map.width; j++)
					game_map.map[i][j] = game_map.map[i - 1][j];
			}
		}
	}

	void CheckGameState() {
		for (auto cell : figure->cells) {
			if (cell.second == 1)
				game_state = false;
		}
	}

	void SetNewPostion(vector<pair<int, int>>& temp_cells)
	{
		figure->cells = temp_cells;
		for (auto cell : figure->cells) {
			if (cell.second == game_map.height - 1
				|| (game_map.map[cell.second + 1][cell.first] == block
					&& CheckCell(figure->cells, cell.second + 1, cell.first))) {
				figure->mobile = false;
				next_figure = true;
			}
			game_map.map[cell.second][cell.first] = block;
		}
	}

	void GetNewPosition(bool hasLeftNeighbour, bool hasRightNeighbour, vector<pair<int, int>>& temp_cells, int dx, int dy)
	{
		for (auto cell : figure->cells) {
			game_map.map[cell.second][cell.first] = space;
			if (!hasLeftNeighbour && hasRightNeighbour && dx <= 0 || hasLeftNeighbour && !hasRightNeighbour && dx >= 0
				|| !hasLeftNeighbour && !hasRightNeighbour)
				temp_cells.push_back(make_pair(cell.first + dx, cell.second + dy));
			else
				temp_cells.push_back(make_pair(cell.first, cell.second + dy));
		}
	}

	void UpdateMap(int dx, int dy) {
		if (figure->mobile) {
			vector<pair<int, int>> temp_cells;
			bool hasLeftNeighbour = false;
			bool hasRightNeighbour = false;
			CheckNeighbours(hasLeftNeighbour, hasRightNeighbour);
			GetNewPosition(hasLeftNeighbour, hasRightNeighbour, temp_cells, dx, dy);
			SetNewPostion(temp_cells);
			if (!figure->mobile) {
				RemoveFilledColumns();
				CheckGameState();
			}
		}
	}

	void GenerateFigure() {
		random_device dev;
		mt19937 rng(dev());
		uniform_int_distribution<mt19937::result_type> range(0, 6);
		switch (range(rng))
		{
		case 0:
			figure = new TFigure();
			break;
		case 1:
			figure = new Square();
			break;
		case 2:
			figure = new LeftL();
			break;
		case 3:
			figure = new RightL();
			break;
		case 4:
			figure = new Line();
			break;
		case 5:
			figure = new LeftS();
			break;
		case 6:
			figure = new RightS();
			break;
		}
	}

	static bool CheckCell(vector<pair<int, int>> cells, int y, int x) {
		for (auto cell : cells) {
			if (cell.first == x && cell.second == y)
				return false;
		}
		return true;
	}

	void ProcessAction(int scale = 1)
	{
		PrintMap();
		Sleep(100 * scale);
		time += speed / 7;
	}

	void PrintMap()
	{
		DWORD dword;
		HANDLE handle = (HANDLE)GetStdHandle(STD_OUTPUT_HANDLE);
		string s;
		for (int i = 0; i < game_map.height; i++) {
			for (int j = 0; j < game_map.width; j++) {
				if (game_map.map[i][j] == space)
					s += ' ';
				if (game_map.map[i][j] == block)
					s += '#';
			}
		}
		for (int i = 0; i < game_map.width; i++)
			s += '-';
		string score = "Score " + to_string(game_score);
		s += score;
		wstring stemp = wstring(s.begin(), s.end());
		LPCWSTR sw = stemp.c_str();
		WriteConsoleOutputCharacter(handle, sw, (game_map.height + 1) * game_map.width + score.length(), COORD{0, 0}, &dword);
	}

	enum Cell { space, block };
	Map game_map;
	int time = 0;
	const int speed = 200000;
	int game_score = 0;
	bool game_state = true;
	bool next_figure = false;
	Figure* figure;
};

int main() {
	system("mode con cols=20 lines=22");
	Tetris game;
	game.StartGame();
}