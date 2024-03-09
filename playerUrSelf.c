//
// Created by 杜坤翰 on 2024/3/9.
//
#include <stdio.h>
#include <stdlib.h> /* 亂數相關函數 */
#include <unistd.h> //usleep()
#include <stdbool.h>


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

//原class Chess 底下函式實作------------START
//Chess.setXY() 實作 -----
bool setXY(ChessArray *chessBoard,int x,int y,int player){
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

//原 writeChessBoard() 實作-----
//註:使用 int* result=writeChessBoard(&chessBoard,player) 讀取回傳值，用完 result 記得 free(result)
int* writeChessBoard(ChessArray *chessBoard,int player){
    int x,y;
    int *coordinate = (int *) malloc(3*sizeof (int));      //建立動態內存//白話文:建立一個在函式裡建立並回傳的陣列(指標)//int*:指標變數，如果你不知道，C的陣列存的其實是陣列起始的地址
    while(true){                                                //malloc(成員數量*每個成員的記憶體空間):建立動態內存//(int *):轉換為int的指標，因為原本的 malloc 不知道為什麼類型是 (void *)
        printf("Enter x y :\n");
        scanf(" %d %d ",&x,&y);
        if(setXY(chessBoard,x,y,player)){
            coordinate[0]=x;
            coordinate[1]=y;
            return coordinate;
        }else{
            printf("Occupied!\n");
        }
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
GoResult go(char *fileName,ChessArray *chessBoard){
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
        setXY(chessBoard,x,y,co_player);
        int* result=writeChessBoard(chessBoard,player);
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

int main(){
    ChessArray chessBoard;
    init_ChessArray(&chessBoard);
    char fileName[6];
    printf("fileName A:");
    scanf(" %s",fileName);
    int count=0;
    GoResult goResult;
    while(true){
        goResult= go(fileName,&chessBoard);
        if(goResult.legal==true){
            count+=1;
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