//
// Created by 杜坤翰 on 2024/3/9.
//
#include <stdio.h>
#include <stdlib.h> /* 亂數相關函數 */
#include <time.h>   /* 時間相關函數 */
#include <unistd.h> //usleep()
#include <stdbool.h>

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

int checkLine(int board[MAX][MAX],int x,int y){
    int dx[]={1,1,0,-1};
    int dy[]={0,1,1,1};
    int lines=0,count=0;
    if(board[x][y]!=0){
        return -1;
    }
    for(int i=0;i<4;i++){
        for(int j=-2;j<=2;j++){
            int nx=x+j*dx[i];
            int ny=y+j*dy[i];
            if(j==0){
                continue;
            }
            if(nx>0&&ny>0&&ny<MAX&&nx<MAX&&board[nx][ny]==1){
                count++;
                //printf("cord[%d][%d]=%d\n",nx,ny,board[nx][ny]);
            }
        }
        if(count>=2){
            lines++;
            count=0;
        }
    }
    return lines;
}
int checkUnValid(int board[MAX][MAX],int x,int y){
    int dx[]={1,1,0,-1};
    int dy[]={0,1,1,1};
    int lines=0,count=0,countOP=0;
    if(board[x][y]!=0){
        return 0;
    }
    if(checkLine(board,x,y)<=1){
        return 1;
    }
    for(int i=0;i<4;i++){
        for(int j=-2;j<=2;j++){
            int nx=x+j*dx[i];
            int ny=y+j*dy[i];
            if(j==0){
                continue;
            }
            if(nx>0&&ny>0&&ny<MAX&&nx<MAX&&board[nx][ny]==1){
                count++;
                //printf("cord[%d][%d]=%d\n",nx,ny,board[nx][ny]);
            }
            if(nx>0&&ny>0&&ny<MAX&&nx<MAX&&board[nx][ny]==2){
                countOP++;
                //printf("cord[%d][%d]=%d\n",nx,ny,board[nx][ny]);
            }
        }
        if(count>=2&&countOP==0){
            lines--;
            count=0;
            countOP=0;
        }
    }
    return lines;
}

//原class Chess 底下函式實作------------START
//Chess.setXY() 實作 -----
bool setXY(ChessArray *chessBoard,int x,int y,int player,int board[MAX][MAX]){
    int data[chessBoard->size][2];
    bool checkXY=true;
    if(x==0 ||y==0){
        return false;
    }
    for(int i=0;i<chessBoard->size;i++){
        data[i][0]=chessBoard->cord[i][0];
        data[i][1]=chessBoard->cord[i][1];
        if(x==data[i][0]&&y==data[i][1]){
            checkXY=false;
        }
    }
    if(checkXY){
        pushback_ChessArray(chessBoard,x,y,player);
        board[x][y]=player;
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

//新增的expand計算區域--------------------------------
int expand(int width,int denyCount,int size){
    int trueWidth=2*width+1;
    if(trueWidth>MAX){
        return width;
    }
    if(denyCount>(trueWidth*trueWidth-size)){
        return width+1;
    }
    return width;
}
//新增的approach計算區域--------------------------------

void initBoard(int board[MAX][MAX]){
    for(int i=0;i<MAX;i++){
        for(int j=0;j<MAX;j++){
            board[i][j]=0;
        }
    }
}

//原 writeChessBoard() 實作-----
//註:使用 int* result=writeChessBoard(&chessBoard,player) 讀取回傳值，用完 result 記得 free(result)
int* writeChessBoard(ChessArray *chessBoard,int player,int board[MAX][MAX]){
    //新增的expand算法計數
    int denyCount=0;
    int width=2;
    int score[MAX][MAX];
    initBoard(score);

    //
    int x,y;
    int *coordinate = (int *) malloc(3*sizeof (int));      //建立動態內存//白話文:建立一個在函式裡建立並回傳的陣列(指標)//int*:指標變數，如果你不知道，C的陣列存的其實是陣列起始的地址
    while(true){                                                //malloc(成員數量*每個成員的記憶體空間):建立動態內存//(int *):轉換為int的指標，因為原本的 malloc 不知道為什麼類型是 (void *)
        if(roundCounter>1){
            /*
            //random 1~20 , from stdlib.h , 此部分應該要用決策樹取代，新增讀取已落子區域
            x=rand() % (MAX+1) + 1;
            y=rand() % (MAX+1) + 1;
             */
            width=expand(width,denyCount,chessBoard->size);
            printf("width=%d\n",width);
            int maxX=rand() % (width) + (MIDPOINT_X- width);
            int maxY=rand() % (width) + (MIDPOINT_Y- width);
            int sum=0;
            for(int i=0;i<MAX;i++){
                for(int j=0;j<MAX;j++){
                    if(checkUnValid(board,i,j)<=0){
                        sum=0;
                        if(board[i][j]!=0){
                            score[i][j]=0;
                            printf("%s"," j");
                        }else{
                            score[i][j]=checkUnValid(board,i,j);
                            printf("%2d",score[i][j]);
                        }
                    }else{
                        sum=1+checkLine(board,i,j);
                        score[i][j]=sum;
                        if(score[maxX][maxY]<score[i][j]){
                            maxX=i;
                            maxY=j;
                        }
                        printf("%2d",sum);
                    }
                    if(j!=MAX-1){
                        printf(",");
                    }
                }
                printf("\n");
            }
            x=maxX;
            y=maxY;

//新增的起手規則-------------------------------------------------START
        }else if(roundCounter==0){
            x=MIDPOINT_X;
            y=MIDPOINT_Y;
        }else{
            if(chessBoard->cord[1][0]>MIDPOINT_X){//右
                if(chessBoard->cord[1][1]>MIDPOINT_Y){ //右上
                    x=MIDPOINT_X+2;
                    y=MIDPOINT_Y+2;
                }else{
                    if(chessBoard->cord[1][1]<MIDPOINT_Y){  //右下
                        x=MIDPOINT_X+2;
                        y=MIDPOINT_Y-2;
                    }else{  //正右
                        x=MIDPOINT_X+2;
                        y=MIDPOINT_Y;
                    }
                }
            }else if(chessBoard->cord[1][0]<MIDPOINT_X){    //左
                if(chessBoard->cord[1][1]>MIDPOINT_Y){  //左上
                    x=MIDPOINT_X-2;
                    y=MIDPOINT_Y+2;
                }else if(chessBoard->cord[1][1]<MIDPOINT_Y){  //左下
                    x=MIDPOINT_X-2;
                    y=MIDPOINT_Y-2;
                }else{  //正左
                    x=MIDPOINT_X-2;
                    y=MIDPOINT_Y;
                }
                }else{
                    if(chessBoard->cord[1][1]>MIDPOINT_Y){  //正上
                        x=MIDPOINT_X;
                        y=MIDPOINT_Y+2;
                    }else{  //正下
                        x=MIDPOINT_X;
                        y=MIDPOINT_Y-2;
                    }
            }
//新增的起手規則-------------------------------------------------END
        }
        if(setXY(chessBoard,x,y,player,board)){
            coordinate[0]=x;
            coordinate[1]=y;
            return coordinate;
        }
        //新增的expand算法計數
        else{
            denyCount++;
        }
        //
    }
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
GoResult go(char *fileName,ChessArray *chessBoard,int board[MAX][MAX]){
    GoResult goResult;
    FILE *file = fopen(fileName,"r");
    char data0;
    fscanf(file," %c ",&data0);     //原本的程式是讀取每一行，取出第一行做判斷，但其實檔案只有兩行所以怎麼讀取沒差
    int player,co_player;
    if(checkString(fileName,"a.txt")==0){
        player=0;
        co_player=1;
    }
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

//黑白棋交換實作-----
void switchSide(ChessArray *chessBoard){
    for(int i =0;i<chessBoard->size;i++){
        if(chessBoard->cord[i][2]==1){
            chessBoard->cord[i][2]==0;
        }else{
            chessBoard->cord[i][2]==1;
        }
    }
}


int main(){
    ChessArray chessBoard;
    init_ChessArray(&chessBoard);
    char fileName[6];
    printf("fileName A:");
    scanf(" %s",fileName);
    srand((unsigned int)time(NULL));
    int count=0;
    int board[MAX][MAX];
    initBoard(board);
    GoResult goResult;
    while(true){
        goResult= go(fileName,&chessBoard,board);
        if(goResult.legal==true){
            count+=1;
            roundCounter=count;
            printf("%d\n",count);
            printChess(&chessBoard);
            if(count>50){
                break;
            }
        }
        usleep(300000); //sleep(0.3)
    }
    destructor_ChessArray(&chessBoard); // Must Have
}