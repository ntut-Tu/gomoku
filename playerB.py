import random
import time

class Chess:
    def __init__(self):
        self.cord = []        
        self.size = 0
    def setXY(self,x: int, y: int, player: int)->bool:
        if x==0 or y==0: return False
        data = [(d[0], d[1]) for d in self.cord]
        if (x, y) not in data:
            self.cord.append((x, y, player))
            self.size += 1
            return True
        return False
    def printChess(self):
        for d in self.cord:
            print(d, end=' ')
        print('size=', self.size)   
        
def writeBackServer(fileName: str, x:int, y: int):
    f = open(fileName,'w')
    f.write('R\n')
    f.write(str(x)+' '+str(y))
    f.close()

def writeChessBoard(chess: Chess, player: int):
    while True:
        x, y = random.randint(1, 20), random.randint(1, 20)
        if chess.setXY(x, y, player) == True:
            return x, y

def go(fileName: str, chess: Chess):
    f = open(fileName)
    data = f.readlines()
    player, co_player = 1, 0
    if fileName=='a.txt':                       # 'a.txt' player 是 0
        player = 0
        co_player = 1
    if data[0][0] =='W':
        coor = data[1].split()        
        x, y = int(coor[0]), int(coor[1])        
        if x==0 and y==0:                       #後下，要有對手先下的資料
            f.close()
            return False, 0, 0
        print('2:', x, y)
        chess.setXY(x, y, co_player)            # 將Server傳來 另一位玩家的x, y 寫入棋盤資料
        x, y = writeChessBoard(chess, player)   # 本身產生 x, y，並寫入棋盤資料
        print('1:', x, y)
        writeBackServer(fileName, x, y)         # 本身產生 x, y，傳給 server
        f.close()
        return True, x, y
    
    f.close()
    return False, 0, 0

def main():
    chess = Chess()
    fileName = input('fileName B: ')
    random.seed()
    count = 0
    while True:
        flag, x, y = go(fileName, chess)
        if flag==True:
           count += 1    
           print(count)
           chess.printChess()           
           if count>50: break
        time.sleep(0.3)
    
main()    