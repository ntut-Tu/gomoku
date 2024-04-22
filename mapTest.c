
#include <stdio.h>
#define MAX 10
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
            if(nx>=0&&ny>=0&&ny<MAX&&nx<MAX&&board[nx][ny]==1){
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
            if(nx>=0&&ny>=0&&ny<MAX&&nx<MAX&&board[nx][ny]==1){
                count++;
                //printf("cord[%d][%d]=%d\n",nx,ny,board[nx][ny]);
            }
            if(nx>=0&&ny>=0&&ny<MAX&&nx<MAX&&board[nx][ny]==2){
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

int main(){
    int board[MAX][MAX]={
            {0,0,0,0,0,0,0,0,1,0},
            {0,0,1,0,0,0,0,1,0,1},
            {0,0,0,1,0,0,0,1,1,0},
            {0,0,0,0,1,1,1,0,0,0},
            {0,2,0,1,1,0,0,0,0,0},
            {0,0,0,0,0,1,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0}
    };
    printf("%d\n",checkLine(board,0,1));
    printf("%d\n",checkLine(board,4,5));
    printf("%d\n",checkLine(board,1,8));

    printf("checkLine\n");
    for(int i=0;i<MAX;i++){
        for(int j=0;j<MAX;j++){
            printf("%2d",checkLine(board,i,j));
            if(j!=MAX-1){
                printf(",");
            }
        }
        printf("\n");
    }
    printf("checkUnValid\n");
    for(int i=0;i<MAX;i++){
        for(int j=0;j<MAX;j++){
            printf("%2d",checkUnValid(board,i,j));
            if(j!=MAX-1){
                printf(",");
            }
        }
        printf("\n");
    }
    printf("Summory\n");
    int sum=0;
    for(int i=0;i<MAX;i++){
        for(int j=0;j<MAX;j++){
            if(checkUnValid(board,i,j)<=0){
                sum=0;
                if(board[i][j]!=0){
                    printf("%s"," j");
                }else{
                    printf("%2d",0);
                }
            }else{
                sum=1+checkLine(board,i,j);
                printf("%2d",sum);
            }
            if(j!=MAX-1){
                printf(",");
            }
        }
        printf("\n");
    }
    printf("Original board\n");
    for(int i=0;i<MAX;i++){
        for(int j=0;j<MAX;j++){
            printf("%2d",board[i][j]);
            if(j!=MAX-1){
                printf(",");
            }
        }
        printf("\n");
    }
}