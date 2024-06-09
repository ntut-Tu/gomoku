//
// Created by 杜坤翰 on 2024/3/9.
// Updated by 鄭錦鑫 on 2024/6/8

#include <stdio.h>
#include <stdlib.h> /* 亂數相關函數 */
#include <time.h>   /* 時間相關函數 */
#include <unistd.h> //usleep()
#include <stdbool.h>
#include <limits.h>

#define MIDPOINT_X 11
#define MIDPOINT_Y 11
#define MAX 21
//全域變數roundCounter
int roundCounter=0;

//替代class Chess 自訂義Struct : ChessArray({int x,int y,int player},int size)-----
typedef struct{             //typedef:讓你能像在用其他語言時一樣用: Struct名稱 物件名稱 ，而不是: Struct Struct名稱 物件名稱
    int (*cord)[3];
    int size;
}ChessArray;
//自訂義Struct : ChessArray 預設建構子
void init_ChessArray(ChessArray *array) {
    array->cord =NULL;
    array->size = 0;
}
//自訂義Struct : ChessArray 解構子，不想要電腦藍屏記得放
void destructor_ChessArray(ChessArray *array){
    free(array->cord);
    array->cord = NULL;
    array->size = 0;
}

//ChessArray 基本新增與移除函式-----START
void pushback_ChessArray(ChessArray *array, int x, int y, int player) {
    array->cord = realloc(array->cord, (array->size + 1) * sizeof(int[3]));     //realloc(array,newSize of array) 重新分配array大小，使用後需要free()
    array->cord[array->size][0] = x;
    array->cord[array->size][1] = y;
    array->cord[array->size][2] = player;
    array->size++;
}
void pop_ChessArray(ChessArray *array){
    if (array->size > 0) {
        array->cord = realloc(array->cord, (array->size - 1) * sizeof(int[3]));
        array->size--;
    }
}
//ChessArray 基本新增與移除函式-----END

/*檢查指定位置落子後是否形成指定有效連線數的數量
參數：
- board: 棋盤的狀態，二維整數陣列表示
- x: 要檢查的位置的 x 座標
- y: 要檢查的位置的 y 座標
- player: 當前玩家的標識（1 或 2）
返回值：
- 返回連線的數量，如果落子後形成有效連線，則返回大於1的正整數，否則返回 <=1*/
int checkLine(int board[MAX][MAX], int x, int y, int player, int num) {
    int total = 0;  // 有 num 连线的数量
    int dx[] = {1, 1, 0, -1}; // 水平、垂直、主对角线、副对角线的移动方向
    int dy[] = {0, 1, 1, 1};
    
    if (board[y][x] != 0) { // 如果指定位置已经有棋子，则返回错误
        return -1;
    }

    // 检查四个方向
    for (int i = 0; i < 4; i++) {
        int count = 1; // 包含假设落子的这一个
        int openEnds = 0; // 记录连线的两端是否开放

        for (int round = 0; round < 2; round++) {
            for (int j = 1; j < MAX; j++) {
                int nx = x + j * dx[i]; // 计算相邻位置的 x 坐标
                int ny = y + j * dy[i]; // 计算相邻位置的 y 坐标
                if (round == 1) { // 反方向
                    nx = x - j * dx[i];
                    ny = y - j * dy[i];
                }
                if (nx >= 0 && nx < MAX && ny >= 0 && ny < MAX) {
                    if (board[ny][nx] == player) {
                        count++;
                    } else if (board[ny][nx] == 0) {
                        openEnds++;
                        break; // 遇到空位停止计算
                    } else {
                        break; // 遇到对手棋子停止计算
                    }
                } else {
                    break; // 超出边界停止计算
                }
            }
        }
        // 冲四處理
        if (num == 6 && count == 4 && openEnds >= 1) {
            total++;
        } else if (count == num) {
            total++;
        }
    }
    return total;
}

/* 檢查指定位置落子後是否形成無效連線（禁手）
參數：
- board: 棋盤的狀態，二維整數陣列表示
- x: 要檢查的位置的 x 座標
- y: 要檢查的位置的 y 座標
- player: 當前玩家的標識（1 或 2）
返回值：
- 返回1如果落子後形成有效連線，否則返回禁手代碼（-3：三三禁手，-4：四四禁手，-5：長連禁手）*/
int checkUnValid(int board[MAX][MAX], int x, int y, int player) {
    int dx[] = {1, 1, 0, -1}; // 水平、垂直、主對角線、副對角線的移動方向
    int dy[] = {0, 1, 1, 1};
    int threeCount = 0, fourCount = 0; // 禁手規則計數器

    if (board[y][x] != 0) { // 如果指定位置已經有棋子，則無效
        return 0;
    }

    // 檢查指定位置周圍的相鄰位置
    for (int i = 0; i < 4; i++) {
        int count = 1; // 包含假設落子的這一個

        for (int round = 0; round < 2; round++) {
            for (int j = 1; j < MAX; j++) {
                int nx = x + j * dx[i]; // 計算相鄰位置的 x 座標
                int ny = y + j * dy[i]; // 計算相鄰位置的 y 座標
                if (round == 1) { // 反方向
                    nx = x - j * dx[i];
                    ny = y - j * dy[i];
                }
                if (nx < 0 || ny < 0 || nx >= MAX || ny >= MAX || board[ny][nx] == 3 - player) {
                    break; // 遇到邊界或對手棋子停止計算
                }

                if (board[ny][nx] == player) {
                    count++;
                }
            }
        }

        // 檢查三三禁點
        if (count == 3) {
            threeCount++;
        }
        // 檢查四四禁點
        if (count == 4) {
            fourCount++;
        }
        // 檢查長連禁點
        if (count > 5) {
            return -1; // 長連禁點
        }
    }

    if (threeCount >= 2 || fourCount >= 2) {
        return -1; // 三三禁點 / 四四禁點
    }
    return 1; // 有效
}

// 加權函數
int evaluatePosition(int board[MAX][MAX], int x, int y, int player) {
    // 根据进攻和防守策略评估位置的函数
    int total_score = 0, attack = 0, defence = 0;
    // [0, 0, 活二，活三，活四， 活五， 冲四]
    int my_line[7] = {0,0,0,0,0,0,0}, op_line[7] = {0,0,0,0,0,0,0}; // 该位置落子后，自己和对手的连线数

    // 更新
    for (int i = 2; i < 7; i++) {
        my_line[i] = checkLine(board, x, y, player, i); 
        op_line[i] = checkLine(board, x, y, 3 - player, i); 
    }

    // 调试输出，确认 checkLine 函数返回的数据
    printf("my: ");
    for (int i = 2; i < 7; i++) {
        printf("%d ", my_line[i]);
    }
    printf("\n op: ");
    for (int i = 2; i < 7; i++) {
        printf("%d ", op_line[i]);
    }
    printf("\n");

    // 进攻策略
    attack += 1000 * my_line[5] + 95 * my_line[4] + 90 * my_line[6];
    attack += 70 * my_line[3] + 55 * my_line[2];

    // VCF 基本评估
    if (my_line[6] > 0 && op_line[5] == 0) {
        attack += 80; // 优先考虑自己的冲四
    } else if (my_line[3] > 0 && my_line[6] > 0) { 
        attack += 80; // 四三解禁
    }

    // 防守策略
    defence += 1000 * op_line[5] + 80 * op_line[4] + 50 * op_line[3];
    
    total_score += attack + defence;
    return total_score;
}

// 找最佳落子
void findBestMove(int board[MAX][MAX], int *bestX, int *bestY, int player) {
    int maxScore = -1; // 初始化最大分數
    int x, y;
    bool found = false; // 判斷是否找到合適位置

    // 遍歷棋盤上的每個位置
    for (x = 1; x < MAX; x++) {
        for (y = 1; y < MAX; y++) {
            if (checkUnValid(board, x, y, player)) { // 使用禁手規則檢查
                int score = evaluatePosition(board, x, y, player); // 計算當前位置的分數
                // 若當前位置的權重值大於當前最大值，更新最大值及對應座標
                if (score > maxScore) {
                    maxScore = score;
                    *bestX = x;
                    *bestY = y;
                    found = true;
                }
            }
        }
    }
    // 如果沒有找到合適的位置，則隨機選擇一個空位置（備用方案）
    if (!found) {
        do {
            x = rand() % MAX;
            y = rand() % MAX;
        } while (board[y][x] != 0);
        *bestX = x;
        *bestY = y;
    }
}
//原class Chess 底下函式實作------------START
//Chess.setXY() 實作 -----
bool setXY(ChessArray *chessBoard, int x, int y, int player, int board[MAX][MAX]) {
    int data[chessBoard->size][2];
    bool checkXY = true;
    if (x == 0 || y == 0) {
        return false;
    }
    for (int i = 0; i < chessBoard->size; i++) {
        data[i][0] = chessBoard->cord[i][0];
        data[i][1] = chessBoard->cord[i][1];
        if (x == data[i][0] && y == data[i][1]) {
            checkXY = false;
        }
    }
    if (checkXY) {
        pushback_ChessArray(chessBoard, x, y, player);
        board[y][x] = player; // 更新棋盘数组
        return true;
    }
    return false;
}
//Chess.printChess() 實作 -----
void printChess(ChessArray *chessBoard){
    for(int i=0;i<chessBoard->size;i++){
        printf("(%d, %d, %d) ",chessBoard->cord[i][0],chessBoard->cord[i][1],chessBoard->cord[i][2]);
    }
    printf("size=%d\n",chessBoard->size);
}
//原class Chess 底下函式實作------------END

//原 writeBackSever() 實作-----
void writeBackSever(char *fileName,int x,int y){
    FILE *file = fopen(fileName, "w");   //w for Write , w+ for R/W ,r for Read
    fprintf(file,"R\n");
    fprintf(file,"%d %d",x,y);
    fclose(file); //close
}


void initBoard(int board[MAX][MAX]){
    for(int i=0;i<MAX;i++){
        for(int j=0;j<MAX;j++){
            board[i][j]=0;
        }
    }
}

//原 writeChessBoard() 實作-----
//註:使用 int* result=writeChessBoard(&chessBoard,player) 讀取回傳值，用完 result 記得 free(result)
int* writeChessBoard(ChessArray *chessBoard, int player, int board[MAX][MAX]) {
    int denyCount = 0;
    int width = 2;
    int score[MAX][MAX];
    initBoard(score);

    int x, y;
    int *coordinate = (int *) malloc(3 * sizeof(int));
    while (true) {
        if (roundCounter > 1) {
            int bestX, bestY;
            findBestMove(board, &bestX, &bestY, player); // 找到最佳位置
            x = bestX;
            y = bestY;
        } else if (roundCounter == 0) { 
            x = MIDPOINT_X;
            y = MIDPOINT_Y;
        } else { 
            if (chessBoard->cord[1][0] > MIDPOINT_X) {
                if (chessBoard->cord[1][1] > MIDPOINT_Y) {
                    x = MIDPOINT_X + 2;
                    y = MIDPOINT_Y + 2;
                } else if (chessBoard->cord[1][1] < MIDPOINT_Y) {
                    x = MIDPOINT_X + 2;
                    y = MIDPOINT_Y - 2;
                } else {
                    x = MIDPOINT_X + 2;
                    y = MIDPOINT_Y;
                }
            } else if (chessBoard->cord[1][0] < MIDPOINT_X) {
                if (chessBoard->cord[1][1] > MIDPOINT_Y) {
                    x = MIDPOINT_X - 2;
                    y = MIDPOINT_Y + 2;
                } else if (chessBoard->cord[1][1] < MIDPOINT_Y) {
                    x = MIDPOINT_X - 2;
                    y = MIDPOINT_Y - 2;
                } else {
                    x = MIDPOINT_X - 2;
                    y = MIDPOINT_Y;
                }
            } else {
                if (chessBoard->cord[1][1] > MIDPOINT_Y) {
                    x = MIDPOINT_X;
                    y = MIDPOINT_Y + 2;
                } else {
                    x = MIDPOINT_X;
                    y = MIDPOINT_Y - 2;
                }
            }
        }
        //printf("Trying to set piece at (%d, %d)\n", x, y);

        if (setXY(chessBoard, x, y, player, board)) {
            coordinate[0] = x;
            coordinate[1] = y;
            return coordinate;
        } else {
            //printf("Position (%d, %d) is already occupied or invalid\n", x, y);
            denyCount++;
            if (denyCount > 10) {
                //printf("Too many invalid positions, breaking out of loop.\n");
                break;
            }
        }
    }
    // 放棄治療-->自殺
    coordinate[0] = -1;
    coordinate[1] = -1;
    return coordinate;
}

//原 go() 實作----------START
//go的自訂義回傳struct:GoResult-----
typedef struct{
    bool legal;
    int x;
    int y;
}GoResult;
//檢查字串是否相等函式-----
int checkString(char *fileName,char *targetName){
    while (*fileName != '\0' && *targetName != '\0' && *fileName == *targetName) {
        fileName++;
        targetName++;
    }
    return (*fileName - *targetName);
}
//原 go() 實作-----
GoResult go(char *fileName,ChessArray *chessBoard, char playerRole,int board[MAX][MAX]){
    GoResult goResult;
    FILE *file = fopen(fileName,"r");
    int player = 2,co_player = 1;
    if (playerRole == 'A') {
        player = 1;
        co_player = 2;
    }
    char data0;
    fscanf(file," %c ",&data0);     //原本的程式是讀取每一行，取出第一行做判斷，但其實檔案只有兩行所以怎麼讀取沒差
    if(data0=='W'){
        int coor[2];
        fscanf(file,"%d %d",&coor[0],&coor[1]);     //讀取第二行
        int x=coor[0];
        int y=coor[1];
        printf("2:%d %d\n",x,y);
        setXY(chessBoard,x,y,co_player,board);
        int* result=writeChessBoard(chessBoard,player,board);
        x=result[0];
        y=result[1];
        free(result);   // Must Have
        printf("1:%d %d\n",x,y);
        writeBackSever(fileName,x,y);
        fclose(file);
        goResult.legal=true;
        goResult.x=x;
        goResult.y=y;
        return goResult;
    }
    fclose(file);
    goResult.legal=false;
    goResult.x=0;
    goResult.y=0;
    return goResult;
}
//原 go() 實作----------END

void printBoard(int board[MAX][MAX]) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            printf("%d ", board[i][j]);
        }
        printf("\n");
    }
}


int main(){
    ChessArray chessBoard;
    init_ChessArray(&chessBoard);

    char fileName[10];
    printf("fileName A:");
    scanf("%s",fileName);

    char playerRole;
    printf("Player A/B: ");
    scanf(" %c", &playerRole);
    srand((unsigned int)time(NULL));

    int count=0;
    int board[MAX][MAX];
    initBoard(board);
    GoResult goResult;
    while(true){
        goResult= go(fileName, &chessBoard, playerRole,board);
        if(goResult.legal==true){
            count+=1;
            roundCounter=count;
            printf("%d\n",count);
            printChess(&chessBoard);
            printBoard(board); // 打印棋盘状态
            if(count>50){
                break;
            }
        }
        sleep(3); // 3 seconds
    }
    destructor_ChessArray(&chessBoard); // Must Have
}
