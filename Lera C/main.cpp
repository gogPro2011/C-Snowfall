#include <glfw3.h>
#include <stdlib.h>
#include <iostream>
#include <Windows.h>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>
#define HIGHT 30
#define WIDTH 10

using namespace std;

int xPlayer;
int yPlayer;
int gameSpeed;

typedef struct {
    clock_t time_exist;
    int x;
    int y;
} SnowBall;

typedef struct {
    bool snow_ball;
    bool is_player; 
} Pixel;

vector <SnowBall> balls;
vector <string> recs;
clock_t timer;
clock_t last_spawn_ball;

Pixel map[WIDTH][HIGHT];

void SpawnBall() {
    SnowBall now;
    now.x = rand() % WIDTH;
    now.y = 29;
    now.time_exist = clock();
    last_spawn_ball = now.time_exist;
    balls.push_back(now);
}

void StartNewGame() {
    timer = clock();
    gameSpeed = 4;
    balls.clear();
    SpawnBall();
    xPlayer = 0;
    yPlayer = 0;
    map[xPlayer][yPlayer].is_player = true;

    last_spawn_ball = timer;
}

void Gamelogic() {
    int k = 0;
    int i = clock();
    for (SnowBall snow : balls) {
        if (i - snow.time_exist > 1000 / gameSpeed && snow.y > 0) {
            map[snow.x][snow.y].snow_ball = false;
            snow.y--;
            map[snow.x][snow.y].snow_ball = true;
            balls[k].time_exist = i;
            balls[k].y = snow.y;
            
        }
        else if (snow.y == 0) {
            balls.erase(balls.begin() + k);
            k--;
            map[snow.x][snow.y].snow_ball = false;
        }
        k++;
    }
    if (i - last_spawn_ball > 2000 / gameSpeed) {
        SpawnBall();
    }
    if (i - timer > 10000 * gameSpeed) {
        gameSpeed++;
    }
}

void get_recs() {
    ifstream in("D:\\Projects\\C++\\Lera\\x64\\Debug\\rec.txt");
    string line;
    if (in.is_open())
    {
        for (int i = 0; i < 10; i++) {
            getline(in, line);
            recs.push_back(line);
        }
        in.close();
    }
}
bool IsRec() {
    for (int i = 0; i < 10; i++) {
        if (stoi(recs[i]) < timer) {
            recs[i] = to_string(timer);
            cout << "new rec" << endl;
            return true;
        }
    }
    return false;

}
void NewRecs() {
    ofstream out("D:\\Projects\\C++\\Lera\\x64\\Debug\\rec.txt");
    if (out.is_open())
    {
        for(int i = 0; i < 10; i++)
            out << recs[i] << std::endl;
        out.close();
    }
}
void print_rec() {
     for (int i = 0; i < 10; i++) {
         cout << (double)stoi(recs[i]) / 1000 << endl;
     }
    
}
void GameEnd(bool p) {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HIGHT; j++) {
            map[i][j].snow_ball = false;
            map[i][j].is_player = false;
        }
    }
    double t = (double)(clock() - timer) / CLOCKS_PER_SEC;
    timer = clock() - timer;
    if (p && IsRec()) NewRecs();
    cout << "time elapsed:" << t << "\n\n" << endl;
    StartNewGame();
}
bool IsEnd() {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HIGHT; j++) {
            if (map[i][j].is_player && map[i][j].snow_ball) return true;
        }
    }
    return false;
}

void ShowCell(int x, int y) {
    if (map[x][y].is_player) {//рисуем игрока
        glBegin(GL_QUADS);
        glColor3f(0.5, 0.1, 0.1);
        glVertex2f(0.0, 0.0);
        glVertex2f(1.0, 0.0);
        glVertex2f(1.0, 1.0);
        glVertex2f(0.0, 1.0);
        glEnd();
    }
    if (map[x][y].snow_ball) {//тут нарисуем снежный шар
        glBegin(GL_QUADS);
        glColor3f(1, 1, 1);
        glVertex2f(0.0, 0.0);
        glVertex2f(1.0, 0.0);
        glVertex2f(1.0, 1.0);
        glVertex2f(0.0, 1.0);
        glEnd();
    }
}

void Game() {
    glLoadIdentity();
    //glScalef(2.0 / HIGHT, 2.0 / WIDTH, 1.0);
    glScalef(2.0 / WIDTH, 2.0 / HIGHT, 1.0);
    //glTranslatef(-HIGHT * 0.5, - WIDTH * 0.5, 0.0);
    glTranslatef(-WIDTH * 0.5, -HIGHT * 0.5, 0.0);

    for (int i = 0; i < HIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            glPushMatrix();

            glTranslatef((float)j, (float)i, 0);
            
            ShowCell(j, i);
            Gamelogic();
            if (IsEnd()) {
                cout << "WIN" << endl;
                GameEnd(true);
            }
            glPopMatrix();
        }
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if ((key == GLFW_KEY_A || key == GLFW_KEY_LEFT) && action == GLFW_PRESS) {
        if (xPlayer != 0) {
            map[xPlayer][yPlayer].is_player = false;
            xPlayer--;
            map[xPlayer][yPlayer].is_player = true;
        }
    }
    else if ((key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) && action == GLFW_PRESS) {
        if (xPlayer != WIDTH - 1) {
            map[xPlayer][yPlayer].is_player = false;
            xPlayer++;
            map[xPlayer][yPlayer].is_player = true;
        }
    }
    else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        cout << "LOSS" << endl;
        GameEnd(false);
    }
}

int main(void) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    if (!glfwInit())
        return -1;
    GLFWwindow* window = glfwCreateWindow(200, 600, "123", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    get_recs();
    cout << "Текущие рекорды:" << endl;
    print_rec();
    glfwMakeContextCurrent(window);
    StartNewGame();
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    srand(time(NULL));
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        Game();

        glfwSetKeyCallback(window, key_callback);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }
    glfwTerminate();
}