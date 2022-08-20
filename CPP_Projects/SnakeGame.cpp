using namespace std;
#include <string>
#include <map>
#include <tuple>
#include <random>
#include <windows.h>
#include <deque>

class SnakeGame {
public:
	SnakeGame() : player(5, 5, 5, 6), game_map(20, 40, space, wall) {
		direction_dict["up"] = make_tuple(0, -1);
		direction_dict["down"] = make_tuple(0, 1);
		direction_dict["left"] = make_tuple(-1, 0);
		direction_dict["right"] = make_tuple(1, 0);
		game_map.map[old_y_pos][old_x_pos] = food;
	}

	void StartGame() {
		while (game_state) {
			k++;
			if (GetAsyncKeyState(VK_UP) & 0x8000 && !down && !up) {
				direction = "up";
				up = true, down = false, left = false, right = false;
			}
			if (GetAsyncKeyState(VK_DOWN) & 0x8000 && !up && !down) {
				direction = "down";
				up = false, down = true, left = false, right = false;
			}
			if (GetAsyncKeyState(VK_LEFT) & 0x8000 && !right && !left) {
				direction = "left";
				up = false, down = false, left = true, right = false;
			}
			if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && !left && !right) {
				direction = "right";
				up = false, down = false, left = false, right = true;
			}
			if (k == speed) {
				k = 0;
				MakeStep();
				PrintMap();
			}
		}
	}
private:
	struct Snake {
		Snake(int hx, int hy, int tx, int ty) {
			body.push_back(make_tuple(tx, ty));
			body.push_back(make_tuple(tx, ty));
		}
		deque<tuple<int, int>> body;
	};

	struct Map {
		Map(int height, int width, int space, int wall) {
			this->height = height;
			this->width = width;
			map = new int* [height];
			for (int i = 0; i < height; i++) {
				map[i] = new int[width];
			}
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					if (i == 0 || i == height - 1 || j == 0 || j == width - 1)
						map[i][j] = wall;
					else
						map[i][j] = space;
				}
			}
		}
		int height;
		int width;
		int** map;
	};

	void MakeStep()
	{
		int dx, dy;
		tie(dx, dy) = direction_dict[direction];
		int hx, hy;
		tie(hx, hy) = player.body.front();
		tuple<int, int> new_head = make_tuple(hx + dx, hy + dy);
		if (!(std::find(player.body.begin(), player.body.end(), new_head) == player.body.end())
			|| (dx + hx == 0) || (dy + hy == 0) || (dx + hx == game_map.width - 1) || (dy + hy == game_map.height - 1))
			game_state = false;
		player.body.push_front(new_head);
		if (game_map.map[hy + dy][hx + dx] != food)
			player.body.pop_back();
		else
		{
			game_score += 1;
			random_device dev;
			mt19937 rng(dev());
			uniform_int_distribution<mt19937::result_type> xdist(5, 15);
			uniform_int_distribution<mt19937::result_type> ydist(5, 15);
			int xrnd = xdist(rng), yrnd = ydist(rng);
			game_map.map[yrnd][xrnd] = food;
			game_map.map[old_y_pos][old_x_pos] = space;
			old_x_pos = xrnd;
			old_y_pos = yrnd;
		}
	}

	void PrintMap()
	{
		DWORD dword;
		HANDLE handle = (HANDLE)GetStdHandle(STD_OUTPUT_HANDLE);
		string s;
		for (int i = 0; i < game_map.height; i++) {
			for (int j = 0; j < game_map.width; j++) {
				if (!(std::find(player.body.begin(), player.body.end(), make_tuple(j, i)) == player.body.end()))
					s += 'P';
				else
				{
					if (game_map.map[i][j] == space)
						s += ' ';
					if (game_map.map[i][j] == wall)
						s += '#';
					if (game_map.map[i][j] == food)
						s += '*';
				}
			}
		}
		string score = "Score " + to_string(game_score);
		s += score;
		wstring stemp = wstring(s.begin(), s.end());
		LPCWSTR sw = stemp.c_str();
		WriteConsoleOutputCharacter(handle, sw, game_map.height * game_map.width + score.length(), COORD{0, 0}, &dword);
	}

	int space = 0;
	int wall = 1;
	int food = 2;
	int k = 0;
	int speed = 200000;
	int game_score = 0;
	bool up = false, down = true, left = false, right = false;
	bool game_state = true;
	string direction = "down";
	map<string, tuple<int, int>> direction_dict;
	Snake player;
	Map game_map;
	int old_x_pos = 20, old_y_pos = 10;
};

int main() {
	system("mode con cols=40 lines=21");
	SnakeGame game;
	game.StartGame();
}
