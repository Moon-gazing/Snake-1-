#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>

#define U 1
#define D 2
#define L 3 
#define R 4       //蛇的状态，U：上 ；D：下；L:左 R：右

typedef struct SNAKE //蛇身的一个节点
{
    int x;
    int y;
    struct SNAKE* next;
} snake;

typedef struct USER {
    char username[50];
    char password[50];
    struct USER* next;
} User;

typedef struct GAMELOG {
    char username[50];
    time_t start_time;
    time_t end_time;
    int score;
    struct GAMELOG* next;
} GameLog;

//全局变量//
int score = 0, add = 10;//总得分与每次吃食物得分。
int status, sleeptime = 200;//每次运行的时间间隔
snake* head, * food;//蛇头指针，食物指针
snake* q;//遍历蛇的时候用到的指针
int endgamestatus = 0; //游戏结束的情况，1：撞到墙；2：咬到自己；3：主动退出游戏。
User* currentUser = NULL; // 当前登录用户
GameLog* gameLogs = NULL; // 游戏日志
char currentUsername[50] = ""; // 当前用户名

//函数声明//
void Pos(int x, int y);
void creatMap();
void initsnake();
int biteself();
void createfood();
void cantcrosswall();
void snakemove();
void pause();
void gamecircle();
void welcometogame();
void endgame();
void gamestart();
void registerUser();
int login();
void saveUsers();
void loadUsers();
void saveGameLogs();
void loadGameLogs();
void showGameLogs();
void addGameLog(time_t start, time_t end, int score);
void freeUsers();
void freeGameLogs();

void Pos(int x, int y)//设置光标位置
{
    COORD pos;
    HANDLE hOutput;
    pos.X = x;
    pos.Y = y;
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOutput, pos);
}

void creatMap()//创建地图
{
    int i;
    for (i = 0; i < 58; i += 2)//打印上下边框
    {
        Pos(i, 0);
        printf("■");
        Pos(i, 26);
        printf("■");
    }
    for (i = 1; i < 26; i++)//打印左右边框
    {
        Pos(0, i);
        printf("■");
        Pos(56, i);
        printf("■");
    }
}

void initsnake()//初始化蛇身
{
    snake* tail;
    int i;
    tail = (snake*)malloc(sizeof(snake));//从蛇尾开始，头插法，以x,y设定开始的位置//
    tail->x = 24;
    tail->y = 5;
    tail->next = NULL;
    for (i = 1; i <= 4; i++)
    {
        head = (snake*)malloc(sizeof(snake));
        head->next = tail;
        head->x = 24 + 2 * i;
        head->y = 5;
        tail = head;
    }
    while (tail != NULL)//从头到为，输出蛇身
    {
        Pos(tail->x, tail->y);
        printf("■");
        tail = tail->next;
    }
}

int biteself()//判断是否咬到了自己
{
    snake* self;
    self = head->next;
    while (self != NULL)
    {
        if (self->x == head->x && self->y == head->y)
        {
            return 1;
        }
        self = self->next;
    }
    return 0;
}

void createfood()//随机出现食物
{
    snake* food_1;
    srand((unsigned)time(NULL));
    food_1 = (snake*)malloc(sizeof(snake));
    while ((food_1->x % 2) != 0)    //保证其为偶数，使得食物能与蛇头对其
    {
        food_1->x = rand() % 52 + 2;
    }
    food_1->y = rand() % 24 + 1;
    q = head;
    while (q->next != NULL)
    {
        if (q->x == food_1->x && q->y == food_1->y) //判断蛇身是否与食物重合
        {
            free(food_1);
            createfood();
            return;
        }
        q = q->next;
    }
    Pos(food_1->x, food_1->y);
    food = food_1;
    printf("■");
}

void cantcrosswall()//不能穿墙
{
    if (head->x == 0 || head->x == 56 || head->y == 0 || head->y == 26)
    {
        endgamestatus = 1;
        endgame();
    }
}

void snakemove()//蛇前进,上U,下D,左L,右R
{
    snake* nexthead;
    cantcrosswall();

    nexthead = (snake*)malloc(sizeof(snake));
    if (status == U)
    {
        nexthead->x = head->x;
        nexthead->y = head->y - 1;
        if (nexthead->x == food->x && nexthead->y == food->y)//如果下一个有食物//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                               //如果没有食物//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == D)
    {
        nexthead->x = head->x;
        nexthead->y = head->y + 1;
        if (nexthead->x == food->x && nexthead->y == food->y)  //有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                               //没有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == L)
    {
        nexthead->x = head->x - 2;
        nexthead->y = head->y;
        if (nexthead->x == food->x && nexthead->y == food->y)//有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                //没有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == R)
    {
        nexthead->x = head->x + 2;
        nexthead->y = head->y;
        if (nexthead->x == food->x && nexthead->y == food->y)//有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                         //没有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (biteself() == 1)       //判断是否会咬到自己
    {
        endgamestatus = 2;
        endgame();
    }
}

void pause()//暂停
{
    Pos(60, 22);
    printf("游戏已暂停，按空格键继续...");
    while (1)
    {
        Sleep(300);
        if (GetAsyncKeyState(VK_SPACE))
        {
            Pos(64, 22);
            printf("                          ");
            break;
        }
    }
}


void gamecircle()//控制游戏        
{
    system("cls");   // 清屏后再进入游戏
    time_t start_time = time(NULL);
    // 添加游戏日志记录
    if (strlen(currentUsername) > 0) {
        addGameLog(start_time, 0, 0);  // 先记录开始时间，结束时间和分数后面更新
    }
    // 绘制游戏界面       
    creatMap();
    initsnake();
    createfood();

    // 显示游戏信息
    Pos(60, 8);
    printf("当前用户: %s", currentUsername);
    Pos(60, 10);
    printf("得分：%d  ", score);
    Pos(60, 11);
    printf("每个食物得分：%d分", add);
    Pos(60, 15);
    printf("游戏控制:");
    Pos(60, 16);
    printf("↑.↓.←.→ - 控制方向");
    Pos(60, 17);
    printf("F1 - 加速, F2 - 减速");
    Pos(60, 18);
    printf("ESC - 退出游戏");
    Pos(60, 19);
    printf("空格 - 暂停游戏");
    Pos(60, 20);
    printf("F5 - 查看游戏日志");

    status = R;
    while (1)
    {
        // 更新分数显示
        Pos(64, 10);
        printf("得分：%d  ", score);
        Pos(64, 11);
        printf("每个食物得分：%d分", add);

        // 处理按键输入
        if (GetAsyncKeyState(VK_UP) && status != D)
        {
            status = U;
        }
        else if (GetAsyncKeyState(VK_DOWN) && status != U)
        {
            status = D;
        }
        else if (GetAsyncKeyState(VK_LEFT) && status != R)
        {
            status = L;
        }
        else if (GetAsyncKeyState(VK_RIGHT) && status != L)
        {
            status = R;
        }
        else if (GetAsyncKeyState(VK_SPACE))
        {
            pause();
        }
        else if (GetAsyncKeyState(VK_ESCAPE))
        {
            endgamestatus = 3;
            break;
        }
        else if (GetAsyncKeyState(VK_F1))
        {
            if (sleeptime >= 50)
            {
                sleeptime = sleeptime - 30;
                add = add + 2;
                if (sleeptime == 320)
                {
                    add = 2;
                }
            }
        }
        else if (GetAsyncKeyState(VK_F2))
        {
            if (sleeptime < 350)
            {
                sleeptime = sleeptime + 30;
                add = add - 2;
                if (sleeptime == 350)
                {
                    add = 1;
                }
            }
        }
        else if (GetAsyncKeyState(VK_F5))
        {
            showGameLogs();

        }

        Sleep(sleeptime);
        snakemove();
    }

    time_t end_time = time(NULL);
    addGameLog(start_time, end_time, score);
}
void showGameLogs() {
    // 保存当前光标位置
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    COORD savedPos = csbi.dwCursorPosition;

    // 设置日志显示起始位置（游戏界面右侧）
    int startX = 88;  // 从第60列开始显示
    int startY = 2;   // 从第2行开始显示

    // 显示日志标题
    Pos(startX, startY);
    printf("========== 游戏日志 ==========");

    // 显示日志内容
    GameLog* log = gameLogs;
    int line = startY + 2;  // 标题下空一行开始显示内容

    while (log != NULL && line < 26) {  // 限制显示行数，避免超出屏幕
        Pos(startX, line++);
        printf("用户: %s", log->username);

        Pos(startX, line++);
        printf("开始: %s", ctime(&log->start_time));

        Pos(startX, line++);
        printf("时长: %d秒", (int)(log->end_time - log->start_time));

        Pos(startX, line++);
        printf("得分: %d", log->score);

        Pos(startX, line++);
        printf("-----------------------");
        line++;  // 空一行

        log = log->next;
    }

    // 恢复光标位置
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), savedPos);
}
void welcometogame()//开始界面
{
    system("cls");
    Pos(40, 10);
    printf("欢迎来到贪食蛇游戏！");
    Pos(40, 12);
    printf("当前用户: %s", currentUsername);
    Pos(40, 14);
    printf("按任意键开始游戏...");
    _getch();
}

void endgame()//结束游戏
{
    time_t end_time = time(NULL);  // 获取当前时间作为游戏结束时间

    system("cls");
    Pos(24, 10);
    printf("游戏结束!");
    Pos(24, 12);

    switch (endgamestatus) {
    case 1:
        printf("原因: 撞到墙了");
        break;
    case 2:
        printf("原因: 咬到自己了");
        break;
    case 3:
        printf("原因: 主动退出游戏");
        break;
    default:
        printf("未知原因");
    }

    Pos(24, 14);
    printf("您的得分是: %d", score);

    // 确保添加游戏日志
    if (strlen(currentUsername) > 0) {
        GameLog* log = gameLogs;
        while (log != NULL && log->next != NULL) {
            log = log->next;
        }
        if (log != NULL && log->end_time == 0) {
            log->end_time = end_time;
            log->score = score;
        }
        else {
            addGameLog(log->start_time, end_time, score);
        }
    }

    saveGameLogs();  // 保存日志

    Pos(24, 16);
    printf("按任意键退出...");
    _getch();

    freeUsers();
    freeGameLogs();
    exit(0);
}

void registerUser() {
    system("cls");
    char username[50];
    char password[50];
    char ch;

    Pos(35, 8);
    printf("========== 用户注册 ==========");

    Pos(35, 10);
    printf("用户名 (不超过50字符): ");
    scanf("%s", username);

    // 检查用户名是否已存在
    User* temp = currentUser;
    while (temp != NULL) {
        if (strcmp(temp->username, username) == 0) {
            Pos(35, 15);
            printf("用户名已存在!");
            Sleep(1000);
            return;
        }
        temp = temp->next;
    }

    Pos(35, 12);
    printf("密码 (不超过50字符): ");

    // 密码输入处理
    int i = 0;
    while (1) {
        ch = _getch();
        if (ch == '\r') break;
        if (ch == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        }
        else if (i < 49) {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';

    // 创建新用户
    User* newUser = (User*)malloc(sizeof(User));
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    newUser->next = NULL;

    // 添加到用户列表
    if (currentUser == NULL) {
        currentUser = newUser;
    }
    else {
        temp = currentUser;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newUser;
    }

    saveUsers();

    Pos(35, 15);
    printf("注册成功!");
    Sleep(1000);
}

int login() {
    system("cls");
    char username[50];
    char password[50];
    char ch;

    Pos(35, 8);
    printf("========== 用户登录 ==========");

    Pos(35, 10);
    printf("用户名: ");
    scanf("%s", username);

    Pos(35, 12);
    printf("密码: ");

    // 密码输入处理
    int i = 0;
    while (1) {
        ch = _getch();
        if (ch == '\r') break;
        if (ch == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        }
        else if (i < 49) {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';

    // 验证用户信息
    User* temp = currentUser;
    while (temp != NULL) {
        if (strcmp(temp->username, username) == 0 && strcmp(temp->password, password) == 0) {
            strcpy(currentUsername, username);
            return 1;
        }
        temp = temp->next;
    }

    Pos(35, 15);
    printf("用户名或密码错误!");
    Sleep(1000);
    return 0;
}

void saveUsers() {
    FILE* fp = fopen("users.dat", "wb");
    if (fp == NULL) {
        Pos(35, 15);
        printf("无法保存用户数据!");
        Sleep(1000);
        return;
    }

    User* temp = currentUser;
    while (temp != NULL) {
        fwrite(temp, sizeof(User), 1, fp);
        temp = temp->next;
    }

    fclose(fp);
}

void loadUsers() {
    FILE* fp = fopen("users.dat", "rb");
    if (fp == NULL) return;

    User user;
    while (fread(&user, sizeof(User), 1, fp)) {
        User* newUser = (User*)malloc(sizeof(User));
        strcpy(newUser->username, user.username);
        strcpy(newUser->password, user.password);
        newUser->next = NULL;

        if (currentUser == NULL) {
            currentUser = newUser;
        }
        else {
            User* temp = currentUser;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = newUser;
        }
    }

    fclose(fp);
}

void saveGameLogs() {
    FILE* fp = fopen("gamelogs.dat", "wb");
    if (fp == NULL) return;

    GameLog* temp = gameLogs;
    while (temp != NULL) {
        fwrite(temp, sizeof(GameLog), 1, fp);
        temp = temp->next;
    }

    fclose(fp);
}

void loadGameLogs() {
    FILE* fp = fopen("gamelogs.dat", "rb");
    if (fp == NULL) return;

    GameLog log;
    while (fread(&log, sizeof(GameLog), 1, fp)) {
        GameLog* newLog = (GameLog*)malloc(sizeof(GameLog));
        strcpy(newLog->username, log.username);
        newLog->start_time = log.start_time;
        newLog->end_time = log.end_time;
        newLog->score = log.score;
        newLog->next = NULL;

        if (gameLogs == NULL) {
            gameLogs = newLog;
        }
        else {
            GameLog* temp = gameLogs;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = newLog;
        }
    }

    fclose(fp);
}

void addGameLog(time_t start, time_t end, int score) {
    GameLog* newLog = (GameLog*)malloc(sizeof(GameLog));
    strcpy(newLog->username, currentUsername);
    newLog->start_time = start;
    newLog->end_time = end;
    newLog->score = score;
    newLog->next = NULL;

    if (gameLogs == NULL) {
        gameLogs = newLog;
    }
    else {
        GameLog* temp = gameLogs;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newLog;
    }
}

void freeUsers() {
    User* temp;
    while (currentUser != NULL) {
        temp = currentUser;
        currentUser = currentUser->next;
        free(temp);
    }
}

void freeGameLogs() {
    GameLog* temp;
    while (gameLogs != NULL) {
        temp = gameLogs;
        gameLogs = gameLogs->next;
        free(temp);
    }
}

void gamestart()//游戏初始化
{
    system("mode con cols=100 lines=30");
    system("title 贪吃蛇游戏");

    // 加载用户数据和游戏日志
    loadUsers();
    loadGameLogs();

    // 用户登录或注册
    int choice;
    do {
        system("cls");
        Pos(40, 8);
        printf("========== 贪吃蛇游戏 ==========");
        Pos(40, 10);
        printf("1. 登录");
        Pos(40, 12);
        printf("2. 注册");
        Pos(40, 14);
        printf("3. 退出");
        Pos(40, 16);
        printf("请选择: ");

        // 防止输入非数字导致无限循环
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // 清除输入缓冲区
            choice = 0;
        }

        switch (choice) {
        case 1:
            if (login()) {
                welcometogame();
                return;
            }
            break;
        case 2:
            registerUser();
            break;
        case 3:
            freeUsers();
            freeGameLogs();
            exit(0);
        default:
            Pos(40, 18);
            printf("无效选择!");
            Sleep(1000);
        }
    } while (1);
}

int main()
{
    gamestart();
    gamecircle();
    endgame();
    return 0;
}
