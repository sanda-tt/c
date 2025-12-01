#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- 定义与枚举 (保留你的基础定义) ---

typedef enum
{
    红桃,
    黑桃,
    梅花,
    方块,
} Suit;

typedef enum
{
    A,  // 0
    二, // 1
    三,
    四,
    五,
    六,
    七,
    八,
    九,
    十,
    J,
    Q,
    K,
    零 // 用于标记无效或结束
} Rank;

// 增加牌型枚举
typedef enum {
    散牌 = 0,
    一对,
    两对,
    三炸,
    顺子,
    同花,
    三拖二, // 葫芦
    四炸,
    同花顺
} HandType;

const char* suitNames[] = { "红桃", "黑桃", "梅花", "方块" };
const char* rankNames[] = { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
const char* typeNames[] = { "散牌", "一对", "两对", "三炸", "顺子", "同花", "三拖二", "四炸", "同花顺" };

typedef struct
{
    Suit suit;
    Rank rank;
} Card;

// 扩展 Hand 结构体以包含玩家ID和分析结果
typedef struct
{
    int id;             // 玩家ID
    Card cards[5];      // 5张牌
    HandType type;      // 牌型
    int primary_rank;   // 主比较值 (如对子的大小)
    int secondary_rank; // 副比较值 (如两对中的小对子)
} Hand;

// --- 菜单函数 ---

void menu_1()
{
    printf("------------------------\n");
    printf("*      1.开始游戏      *\n");
    printf("*      2.退出游戏      *\n");
    printf("------------------------\n");
}

void menu_2()
{
    printf("------------------------\n");
    printf("*      1.多人对局      *\n"); // 稍微改了下名字以符合逻辑
    printf("*      2.返回上一步    *\n");
    printf("------------------------\n");
}

// --- 核心逻辑函数 ---

// 初始化牌堆
void Init_puke(Card deck[52])
{
    int idx = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 13; j++)
        {
            deck[idx].suit = (Suit)i;
            deck[idx].rank = (Rank)j;
            idx++;
        }
    }
}

// 洗牌算法
void fisher_yates_shuffle(Card deck[52])
{
    int idx = 0;
    srand((unsigned int)time(NULL));
    for (idx = 51; idx > 0; idx--)
    {
        int i = rand() % (idx + 1); // 修正: rand % (idx+1) 才能取到 idx
        Card temp;
        temp = deck[idx];
        deck[idx] = deck[i];
        deck[i] = temp;
    }
}

// 辅助：将枚举Rank转换为逻辑大小 (2=2...K=13, A=14)
int get_logic_val(Rank r) {
    if (r == A) return 14;
    return (int)r + 1; // 枚举中 二是1，所以+1等于逻辑值2
}

// 冒泡排序手牌：按逻辑点数从小到大
void sort_cards(Hand* h) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 - i; j++) {
            int val1 = get_logic_val(h->cards[j].rank);
            int val2 = get_logic_val(h->cards[j + 1].rank);
            if (val1 > val2) {
                Card temp = h->cards[j];
                h->cards[j] = h->cards[j + 1];
                h->cards[j + 1] = temp;
            }
        }
    }
}

// 核心算法：分析牌型
void analyze_hand(Hand* h) {
    sort_cards(h); // 先按点数排序

    int flush = 1;
    int straight = 1;
    int counts[15] = { 0 }; // 记录点数出现次数 (下标2-14)

    // 1. 检查同花
    for (int i = 0; i < 4; i++) {
        if (h->cards[i].suit != h->cards[i + 1].suit) {
            flush = 0;
            break;
        }
    }

    // 2. 检查顺子 (逻辑值连续)
    for (int i = 0; i < 4; i++) {
        if (get_logic_val(h->cards[i + 1].rank) != get_logic_val(h->cards[i].rank) + 1) {
            straight = 0;
            break;
        }
    }
    // 特殊顺子处理: A, 2, 3, 4, 5
    // 在 get_logic_val 体系下，它们是 14, 2, 3, 4, 5。排序后是 2, 3, 4, 5, 14
    if (!straight && h->cards[0].rank == 二 && h->cards[1].rank == 三 &&
        h->cards[2].rank == 四 && h->cards[3].rank == 五 && h->cards[4].rank == A) {
        straight = 1;
        // 这种顺子比较特殊，通常认为5最大，但在逻辑值里为了方便比较，稍后处理
    }

    // 3. 统计点数频率
    for (int i = 0; i < 5; i++) {
        counts[get_logic_val(h->cards[i].rank)]++;
    }

    int pairs = 0, threes = 0, fours = 0;
    for (int i = 2; i <= 14; i++) {
        if (counts[i] == 2) pairs++;
        if (counts[i] == 3) threes++;
        if (counts[i] == 4) fours++;
    }

    // 初始化
    h->primary_rank = 0;
    h->secondary_rank = 0;

    // 4. 判定牌型
    if (flush && straight) {
        h->type = 同花顺;
        // 如果是 A,2,3,4,5 (排序后 2,3,4,5,A)，最大值是 5
        if (h->cards[4].rank == A && h->cards[0].rank == 二) h->primary_rank = 5;
        else h->primary_rank = get_logic_val(h->cards[4].rank);
    }
    else if (fours == 1) {
        h->type = 四炸;
        for (int i = 2; i <= 14; i++) if (counts[i] == 4) h->primary_rank = i;
    }
    else if (threes == 1 && pairs == 1) {
        h->type = 三拖二;
        for (int i = 2; i <= 14; i++) {
            if (counts[i] == 3) h->primary_rank = i;
            if (counts[i] == 2) h->secondary_rank = i;
        }
    }
    else if (flush) {
        h->type = 同花;
        h->primary_rank = get_logic_val(h->cards[4].rank); // 只记录最大张
    }
    else if (straight) {
        h->type = 顺子;
        if (h->cards[4].rank == A && h->cards[0].rank == 二) h->primary_rank = 5;
        else h->primary_rank = get_logic_val(h->cards[4].rank);
    }
    else if (threes == 1) {
        h->type = 三炸;
        for (int i = 2; i <= 14; i++) if (counts[i] == 3) h->primary_rank = i;
    }
    else if (pairs == 2) {
        h->type = 两对;
        int p1 = 0, p2 = 0;
        for (int i = 2; i <= 14; i++) {
            if (counts[i] == 2) {
                if (p1 == 0) p1 = i; else p2 = i;
            }
        }
        // 确保 primary 是大对子
        if (p1 > p2) { h->primary_rank = p1; h->secondary_rank = p2; }
        else { h->primary_rank = p2; h->secondary_rank = p1; }
    }
    else if (pairs == 1) {
        h->type = 一对;
        for (int i = 2; i <= 14; i++) if (counts[i] == 2) h->primary_rank = i;
    }
    else {
        h->type = 散牌;
        h->primary_rank = get_logic_val(h->cards[4].rank);
    }
}

// 比较两个玩家手牌大小 (用于 qsort)
// 返回负数: A赢 (排前面), 正数: B赢
int compare_hands(const void* a, const void* b) {
    Hand* h1 = (Hand*)a;
    Hand* h2 = (Hand*)b;

    // 1. 比牌型
    if (h2->type != h1->type) return h2->type - h1->type;

    // 2. 比主数值 (如三条的大小)
    if (h2->primary_rank != h1->primary_rank) return h2->primary_rank - h1->primary_rank;

    // 3. 比副数值 (如葫芦里的对子)
    if (h2->secondary_rank != h1->secondary_rank) return h2->secondary_rank - h1->secondary_rank;

    // 4. 比单张 (踢脚牌)，从大到小逐个比
    // 注意：cards数组已经是按逻辑值从小到大排好序的
    for (int i = 4; i >= 0; i--) {
        int val1 = get_logic_val(h1->cards[i].rank);
        int val2 = get_logic_val(h2->cards[i].rank);
        if (val1 != val2) return val2 - val1;
    }

    return 0; // 平局
}

// 发牌并显示
void deal_cards(Card deck[52], Hand players[], int player_num, int* deck_idx) {
    for (int i = 0; i < player_num; i++) {
        players[i].id = i + 1;
        printf("玩家 %d: ", players[i].id);

        // 发5张
        for (int k = 0; k < 5; k++) {
            players[i].cards[k] = deck[*deck_idx];
            (*deck_idx)++;

            // 打印
            printf("[%s%s] ", suitNames[players[i].cards[k].suit], rankNames[players[i].cards[k].rank]);
        }

        // 分析
        analyze_hand(&players[i]);
        printf(" -> %s\n", typeNames[players[i].type]);
    }
}

void game(Card deck[52])
{
    Init_puke(deck);
    fisher_yates_shuffle(deck);

    int num_players = 0;
    printf("请选择玩家数(2~5): ");
    if (scanf_s("%d", &num_players) != 1) {
        while (getchar() != '\n'); return;
    }
    if (num_players < 2) num_players = 2;
    if (num_players > 5) num_players = 5;

    Hand players[5];
    int deck_idx = 0;

    printf("\n--- 正在发牌 ---\n");
    deal_cards(deck, players, num_players, &deck_idx);

    printf("\n--- 对局结果 ---\n");
    // 排序玩家，决定胜负
    qsort(players, num_players, sizeof(Hand), compare_hands);

    printf(" 获胜者: 玩家 %d (牌型: %s)\n", players[0].id, typeNames[players[0].type]);

    // 检查平局
    int i = 1;
    while (i < num_players && compare_hands(&players[0], &players[i]) == 0) {
        printf(" 平局:   玩家 %d (牌型: %s)\n", players[i].id, typeNames[players[i].type]);
        i++;
    }
    printf("\n");
}

// --- 主函数 ---

int main()
{
    // 必须有随机种子
    srand((unsigned int)time(NULL));

    while (1)
    {
        menu_1();
        int Flag_Num = -1;
        if (scanf_s("%d", &Flag_Num) != 1) {
            while (getchar() != '\n'); continue;
        }

        if (Flag_Num != 1 && Flag_Num != 2)
        {
            printf("输入错误，请重新输入\n");
        }
        else
        {
            if (Flag_Num == 1)
            {
                menu_2();
                while (1)
                {
                    int Flag_Num2 = -1;
                    scanf_s("%d", &Flag_Num2);

                    if (Flag_Num2 == 1) // 多人对局
                    {
                        Card deck[52];
                        game(deck);
                        break; // 打完一局跳出内层循环，回主菜单方便重开
                    }
                    else if (Flag_Num2 == 2) // 返回
                    {
                        break;
                    }
                    else
                    {
                        printf("输入错误，请重新输入\n");
                    }
                }
            }
            if (Flag_Num == 2)
            {
                printf("游戏结束。\n");
                break;
            }
        }
    }
    return 0;
}
