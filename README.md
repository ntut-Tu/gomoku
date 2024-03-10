# gomoku
 # 注意事項&更新
* 編譯環境不要有中文
* github檔案上傳可以用拉的
* 03/10:

  - 完成playerA起始3回合
  - 為了做測試playerB.py已完成起手規則
  - 創建playerUrSelf用於手動測試玩家當白棋 (將rand()改回來就是playerB.c了)
    
 # 構想
 
* 也許能新增一個chessArray作來儲存禁點，當作已被下過，在pushBack playerB 時同時掃描並pushBack 禁點

* 應該新增尋找已下棋點是否連線的方法，用於確認禁點，並算出每個空位 進攻 防守 的優先值

* 防禦(最高優先級):

  - 當有能力且對手活三時應當防守

* 干擾模式:
  - 近距離:對白棋剛下的子周圍8格算優先度
  - 遠距離:下白棋剛下的子連線的另一端
 
* 進攻當不是死棋繼續往同方向延伸

  - 判斷眠三、活三 
  - 當死棋時改下遠處空曠點?

* 防禦(最弱優先級):

  - 適度阻擋並尋找新連線

 # 新增起手三回合規則
 
|01, 02, 03, 04, 05|

|11, 12, 13, 14, 15|

|21, 22, 23, 24, 25|

|31, 32, 33, 34, 35|

|41, 42, 43, 44, 45|

不確定白棋根黑棋換我方是否吃虧，所以以白棋不會想換的棋型為目標

現在的邏輯是辦法和白棋連成一條線ex:(23,13,03)

不確定是否真的佔優，需更改

 # 缺33/44/長聯規則
 

 # 新增Expand算法:

 起始3回合後以中心5x5為基底作rand()，希望縮小rand()範圍增加連成線的機率，若被setXY拒絕的次數>剩餘範圍內的空格數，寬+1，此算法沒有想到33/44/長聯規則下的效率，可能一下rand()範圍一下擴到很大，失去算法用意

 也許可以保留一部分但改成往某一方向延伸，作為進攻模式
 
 ![螢幕擷取畫面 2024-03-10 011154](https://github.com/ntut-Tu/gomoku/assets/160988691/043c8275-14e0-4cec-8523-8ead17e8f23a)

# 缺判斷何時防守
