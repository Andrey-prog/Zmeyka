#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

const int tile_size = 12;
const int hight = 384;
const int wight = 384;

using namespace cv;
using namespace std;
typedef struct Tile {
    int x;
    int y;
}Tile;


void paint(Mat& m, Tile tile, int u) {
    vector<Point> points;
    points.resize(4);
    int offsetx = tile_size * tile.x;
    int offsety = tile_size * tile.y;

    points[0] = Point(0, tile_size / 2);
    points[1] = Point(tile_size / 2, tile_size);
    points[2] = Point(tile_size, tile_size / 2);
    points[3] = Point(tile_size / 2, 0);

    for (auto& point : points) {
        point.x += offsetx;
        point.y += offsety;
    }

    vector<Point>p;
    p.resize(4);

    p[0] = Point(10 * tile_size, 12 * tile_size);
    p[1] = Point(10 * tile_size, 18 * tile_size);
    p[2] = Point(11 * tile_size, 18 * tile_size);
    p[3] = Point(11 * tile_size, 12 * tile_size);

    vector<Point>k;
    k.resize(4);

    k[0] = Point(20 * tile_size, 12 * tile_size);
    k[1] = Point(20 * tile_size, 18 * tile_size);
    k[2] = Point(21 * tile_size, 18 * tile_size);
    k[3] = Point(21 * tile_size, 12 * tile_size);

    if (u == 1) {
        fillConvexPoly(m, points.data(), points.size(), Scalar(255, 200, 0));
    }
    else {
        fillConvexPoly(m, p.data(), p.size(), Scalar(70, 140, 170));
        fillConvexPoly(m, k.data(), k.size(), Scalar(70, 140, 170));
        fillConvexPoly(m, points.data(), points.size(), Scalar(0, 0, 255));
    }
};


class Food {
    Tile tile;
public:
    Food(Tile tile) : tile(tile) {
    };

    void draw(Mat& m) {
        paint(m, tile, 0);
    }

    int getX() {
        return tile.x;
    }

    int getY() {
        return tile.y;
    }

    void moove(Mat& m, vector<Tile> snakeTiles) {

        int a = rand() % 32;
        int b = rand() % 32;

        int k = snakeTiles.size();
        for (int i = 0; i < k; i++) {
            if (snakeTiles[i].x == a && snakeTiles[i].y == b || a == 20 && b <= 12 && b >= 17 || a == 10 && b <= 12 && b >= 17) {
                if (k <= 1011) {
                    while (a == snakeTiles[i].x && b == snakeTiles[i].y || a == 20 && b <= 12 && b >= 17 || a == 10 && b <= 12 && b >= 17) {
                        a = rand() % 32;
                        b = rand() % 32;
                    }
                }
                else {
                    break;
                }
            }
        }
        tile.x = a;
        tile.y = b;
        draw(m);
    }
};


class Snake {
public:
    vector<Tile> snakeTiles;
    int dx;
    int dy;
    Snake() {
        dx = 1;
        dy = 0;
    }

    void draw(Mat& m) {
        for (auto& tile : snakeTiles) {
            paint(m, tile, 1);
        }
    }

    void add(Tile t) {
        snakeTiles.push_back(t);
    }

    void changeDirection(int dx, int dy) {
        if (snakeTiles.size() > 1 && snakeTiles[1].x == snakeTiles[0].x + dx
            && snakeTiles[1].y == snakeTiles[0].y + dy) return;
        this->dx = dx; this->dy = dy;
    }

    bool moove(Food& food, Mat& m) {

        bool youFail = false;
        snakeTiles.insert(snakeTiles.begin(), { snakeTiles[0].x + dx, snakeTiles[0].y + dy });

        if (snakeTiles[0].x == -1 || snakeTiles[0].y == -1 || snakeTiles[0].x == 32 || snakeTiles[0].y == 32) {
            youFail = true;
        }

        if (snakeTiles[0].x == 20 && (snakeTiles[0].y >= 12 && snakeTiles[0].y <= 17)) {
            youFail = true;
        }

        if (snakeTiles[0].x == 10 && (snakeTiles[0].y >= 12 && snakeTiles[0].y <= 17)) {
            youFail = true;
        }

        for (int i = 1; i < snakeTiles.size(); i++) {
            if (snakeTiles[0].x == snakeTiles[i].x && snakeTiles[0].y == snakeTiles[i].y) {
                youFail = true;
                break;
            }
        }
        if (snakeTiles[0].x == food.getX() && snakeTiles[0].y == food.getY()) {
            food.moove(m, snakeTiles);
        }
        else {
            snakeTiles.erase(snakeTiles.begin() + snakeTiles.size() - 1);
        }
        return youFail;
    }
};

class GameController {
    Snake snake;
    Food food;
    Mat m;

public:
    GameController() : food({ 0,0 }) {
        resetState();
        m = Mat(wight, hight, CV_8UC3);
    }

    void resetState() {
        snake = Snake();
        snake.add({ 2,3 });
        snake.add({ 2,2 });
        food = Food({ 5,5 });
    }

    void gameLoop() {
        resetState();
        int key;
        while (true) {
            m = Scalar(0, 0, 0);
            food.draw(m);
            snake.draw(m);
            imshow("Snake", m);
            key = waitKey(100);

            switch (key) {
            case 'w': snake.changeDirection(0, -1); break;
            case 's': snake.changeDirection(0, 1); break;
            case 'a': snake.changeDirection(-1, 0); break;
            case 'd': snake.changeDirection(1, 0); break;
            case 'p': exit(0);
            }
            bool youFail = snake.moove(food, m);
            if (youFail) {

                char a[] = "You are failed..";
                int font = FONT_HERSHEY_SIMPLEX;
                putText(m, a, { 40,40 }, font, 1, Scalar(0, 255, 255), 2, 8);
                imshow("Snake", m);
                while (-1 == waitKey(25));
                imwrite("Snake.png", m);
                Mat b = imread("Snake.png");
                break;
            }
        }
    }
};

int main() {

    GameController g;
    g.gameLoop();
    return 0;

}