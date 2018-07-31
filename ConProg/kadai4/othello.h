//========================ヘッダファイル========================
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
//========================大域マクロ=========================
#define SIZE 8
//========================大域定義=========================
//-- 駒型
typedef enum {WHT=-1, EMP=0, BLK=+1, OUT=+2}  Piece;

//-- 盤面型
typedef struct {
    Piece cell[SIZE][SIZE];   // 二次元配列で盤面を表す
} Board;

//-- 打ち手型
typedef struct {
    int x;           // 縦
    int y;           // 横
    Piece color;     // 駒の色
} Hand;

//-- 打ち手候補型
typedef struct {
    Hand targets[SIZE*SIZE];
    int size;
} THands;

//-- ゲームの状態型
typedef struct {
  Board brd;    // 盤面
  Piece turn;    // 手番
  Hand last;    // 1つ前の手
  bool isEnd;    // 終了フラグ
  bool isPass;    // パスフラグ
  bool isBegin;
} GameState;

//========================関数宣言=========================
void player(GameState *gs, THands t_hands);
GameState game_init(void);    // ゲーム初期状態の生成
Board brd_init(void);                       // 盤面の初期化
void brd_output(GameState gs, THands *t_hands);                 // 盤面の出力
void brd_output_simple(GameState gs);                 // 盤面の出力
void hand_input(Hand *hand, THands t_hands);                // 打ち手の入力
bool brd_turn(Board *brd, Hand hand);       // 盤面の更新
int brd_turn_dir(Board *brd, Hand hand, int dx, int dy);     // 指定方向への走査
bool end_check(Board brd);                  // 終了の判定
Piece cell_val(Board brd, int x, int y);    // 指定位置のマスの状態
int count_stone(Board brd, Piece wh);
void winner_check(Board brd);              // 勝敗の判定
bool pass_check(Board brd, Piece wh);       // パスの判定
void turn_change(GameState *gs);

void turn_change(GameState *gs) {
  gs->turn *= -1;
}
//========================プレイヤ==========================
void player(GameState *gs, THands t_hands) {
    //-- 宣言
    Hand hand;
    ( gs->turn == BLK ) ? printf("黒のターン\n") : printf("白のターン\n");
    //-- パス判定
    if ( pass_check(gs->brd, gs->turn) ) {
        printf("打てる場所がありません。パスします。\n");
        if (gs->isPass) {
          puts("連続パス");
          gs->isEnd = true;
        }
        gs->isPass = true;
        return;
    }

    while ( 1 ) {
        //-- 打ち手入力
        hand_input(&hand, t_hands);
        //-- 盤面更新
        if ( brd_turn(&(gs->brd), hand) ) {   // 盤面の更新と判定
            break;
        }
        printf("そこには打てません\n");
    }
    //printf("%d %d\n", hand.x, hand.y);
    gs->isPass = false;
    gs->isEnd = end_check(gs->brd);
    gs->last = hand;
}

//========================初期化処理========================
GameState game_init(void) {
  GameState newState;
  newState.brd = brd_init();
  newState.turn = BLK;
  newState.isEnd = false;
  newState.isPass = false;
  newState.isBegin = false;
  return newState;
}

Board brd_init(void) {
    //-- 宣言
    Board tmp;
    int i, j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            tmp.cell[i][j] = EMP;
            if (i == 3 || i == 4) {
                if (i == j) tmp.cell[i][j] = WHT;
                else if (i+j == 7) tmp.cell[i][j] = BLK;
            }
        }
    }
    return tmp;
}

//========================盤面出力=========================
void brd_output(GameState gs, THands *t_hands) {
    //-- 宣言
    int x, y;    // 反復変数
    Board tmp_b;
    Board brd = gs.brd;
    Piece c = gs.turn;
    if (gs.isBegin) {
      if (gs.turn == BLK) {
        if (gs.isPass) printf("白は パス しました\n");
        else printf("白が (%d, %d)に置きました\n", gs.last.x, gs.last.y);
      } else if (gs.turn == WHT) {
        if (gs.isPass) printf("黒は パス しました\n");
        else printf("黒が (%d, %d)に置きました\n", gs.last.x, gs.last.y);
      }
    } else {
      puts("Game Start");
    }

    //-- 出力処理
    t_hands->size = 1;
    for ( x = 0; x < SIZE; x++) {
        for ( y = 0; y < SIZE; y++ ) {
            switch ( brd.cell[x][y] ) {
                case WHT: printf("%2s","白");   break;
                case EMP:
                    tmp_b = brd;
                    Hand tmp_h = {x, y, c};
                    if (brd_turn(&tmp_b, tmp_h)) {
                        t_hands->targets[t_hands->size] = tmp_h;
                        printf("%02d",(t_hands->size)++);
                    } else printf("%2s","口");
                    break;
                case BLK: printf("%2s","黒");   break;
                default : printf("%2s","?");
            }
        }
        puts("");
    }
    printf("\n");
}

void brd_output_simple(GameState gs) {
    //-- 宣言
    int x, y;    // 反復変数
    Board brd = gs.brd;
    Piece c = gs.turn;

    //-- 出力処理
    for ( x = 0; x < SIZE; x++) {
        for ( y = 0; y < SIZE; y++ ) {
            switch ( brd.cell[x][y] ) {
                case WHT: printf("%2s","白");   break;
                case EMP: printf("%2s","口");   break;
                case BLK: printf("%2s","黒");   break;
                default : printf("%2s","不");
            }
        }
        puts("");
    }
    printf("\n");
}
//========================打手入力=========================
void hand_input(Hand *hand, THands t_hands) {
    //-- 宣言
    int x, y;
    char buff[256];

    //--入力処理
    while ( 1 ) {
        printf("Hand No ? > ");
        scanf("%s", buff);
        if ( sscanf(buff, "%d", &x) == 1 && x > 0 && x < t_hands.size ) { break; }    // 正しい入力なら反復を脱出
        printf("不正な入力です。  再入力してください。\n");
    }

    //-- 格納
    *hand = t_hands.targets[x];

}
//========================盤面更新=========================
bool brd_turn(Board *brd, Hand hand) {
    //-- 宣言
    int count = 0;    // ひっくり返した駒の数

    //-- 打つ位置がEMPでないなら失敗
    if ( cell_val(*brd, hand.x, hand.y) != EMP ) { return false; }

    //-- 打つ位置から上下左右斜め８方向への走査
    count += brd_turn_dir(brd, hand, 1, 0);      // 下
    count += brd_turn_dir(brd, hand, 1, 1);      // 右下
    count += brd_turn_dir(brd, hand, 1, -1);     // 左下
    count += brd_turn_dir(brd, hand, 0, 1);      // 右
    count += brd_turn_dir(brd, hand, 0, -1);     // 左
    count += brd_turn_dir(brd, hand, -1, 0);     // 上
    count += brd_turn_dir(brd, hand, -1, 1);     // 右上
    count += brd_turn_dir(brd, hand, -1, -1);    // 左上

    //-- 駒をひっくり返せなければ失敗
    if ( count == 0 ) { return false; }
    //printf("%d %d\n", hand.x, hand.y);

    //-- 返却
    return true;
}
//=====================指定方向への盤面走査=====================
int brd_turn_dir(Board *brd, Hand hand, int dx, int dy) {
    //-- 宣言
    int tx = hand.x;          // 現在位置
    int ty = hand.y;          // 現在位置
    Piece wh = hand.color;    // 自分の駒
    int len = 0;              // 相手の駒の連長

    //-- はさんでいるか判定
    while ( 1 ) {
        //- 現在位置を指定方向に更新
        tx += dx;
        ty += dy;
        //- 現在位置が相手の駒なら連長を増分
        if ( cell_val(*brd, tx, ty) == -wh ) { len++; }
        //- 連長が正で自分の駒なら打ち切り
        else if ( len > 0 && cell_val(*brd, tx, ty) == wh ) { break; }
        //- どちらでもない(盤外か空マス)なら0を返す
        else { return 0; }
    }

    //-- 駒をひっくり返す
    while ( 1 ) {
        //- 現在位置から指定方向の逆へ更新
        tx -= dx;
        ty -= dy;
        //- 駒が相手の駒でないなら打ち切り
        if ( cell_val(*brd, tx, ty) != -wh ) { break; }
        //- 自分の駒に更新
        brd->cell[tx][ty] = wh;
    }

    //-- 打つ位置に自分の駒を置く
    brd->cell[hand.x][hand.y] = wh;
    //-- 返却
    return len;
}
//=====================指定位置の駒の状態======================
Piece cell_val(Board brd, int x, int y) {
    //-- 盤外ならOUTを返却
    if ( x < 0 || x >= SIZE ) { return OUT; }
    if ( y < 0 || y >= SIZE ) { return OUT; }

    //-- 駒の状態を返却
    return brd.cell[x][y];
}
//========================勝敗判定=========================
void winner_check(Board brd) {
    //-- 宣言
    int black, white;  // 駒の数

    //-- カウント
    black = count_stone(brd, BLK);
    white = count_stone(brd, WHT);

    //-- 判定
    printf("黒 : 白\n");
    printf("%2d : %2d\n", black, white);
    if ( black > white ) { printf("黒の勝ち\n"); }
    else if ( black == white ) { printf("引き分け\n"); }
    else { printf("白の勝ち\n"); }
}
//========================石カウント=========================
int count_stone(Board brd, Piece wh) {
    int x, y;
    int count = 0;

    for ( x = 0; x < SIZE; x++ ) {
        for ( y = 0; y < SIZE; y++ ) {
            if ( brd.cell[x][y] == wh ) { count++; }
        }
    }

    return count;
}
//========================パス判定=========================
bool pass_check(Board brd, Piece wh) {
    int x, y;    // 反復変数
    Hand tmp;
    tmp.color = wh;

    //-- おける場所があるか走査
    for ( x = 0; x < SIZE; x++ ) {
        tmp.x = x;
        for ( y = 0; y < SIZE; y++ ) {
            tmp.y = y;
            if ( brd_turn(&brd, tmp) ) { return false; }
        }
    }

    //-- 返却
    return true;

}
//========================終了判定=========================
bool end_check(Board brd) {
    int x, y;
    for ( x = 0; x < SIZE; x++ ) {
        for ( y = 0; y < SIZE; y++ ) {
            if ( brd.cell[x][y] == EMP ) {   // brd.cell[x][y]がEMPか調べる
                return false;                // EMPならfalseを返す
            }
        }
    }
    return true;    // EMPがなければ終了
}
