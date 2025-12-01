#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h> // 引入Windows API用于控制颜色

// --- 定义与枚举 ---

typedef enum
{
    红桃, // 0
    黑桃, // 1
    梅花, // 2
    方块, // 3
} Suit;

typedef enum
{
    A, 二, 三, 四, 五, 六, 七, 八, 九, 十, J, Q, K, 零
} Rank;

// 牌型枚举
typedef enum {
    散牌 = 0, 一对, 两对, 三炸, 顺子, 同花, 三拖二, 四炸, 同花顺
} HandType;

// 修改点：这里改为汉字，确保在所有电脑上都能显示
const char* suitNames[] = { "红桃", "黑桃", "梅花", "方块" };
const char* rankNames[] = { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
const char* typeNames[] = { "散牌", "一对", "两对", "三炸", "顺子", "同花", "三拖二", "四炸", "同花顺" };

typedef struct
{
    Suit suit;
    Rank rank;
} Card;

typedef struct
{
    int id;
    Card cards[5];
    HandType type;
    int primary_rank;
    int secondary_rank;
} Hand;

// --- 颜色控制函数 ---
void set_color(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void reset_color() {
    set_color(7); // 恢复默认白色
}

// --- 菜单函数 ---

void menu_1()
{
    set_color(3000); // 青色
    printf("╔════════════════════════╗\n");
    printf("║      扑克游戏 v3.0     ║\n");
    printf("╠════════════════════════╣\n");
    printf("║      1. 开始游戏       ║\n");
    printf("║      2. 退出游戏       ║\n");
    printf("╚════════════════════════╝\n");
    reset_color();
}

void menu_2()
{
    set_color(14); // 黄色
    printf("\n--- 模式选择 ---\n");
    printf("1. 多人对局 (图形演示)\n");
    printf("2. 返回上一步\n");
    printf("----------------\n");
    reset_color();
}

// --- 核心逻辑 (保持不变) ---

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

void fisher_yates_shuffle(Card deck[52])
{
    int idx = 0;
    srand((unsigned int)time(NULL));
    for (idx = 51; idx > 0; idx--)
    {
        int i = rand() % (idx + 1);
        Card temp = deck[idx];
        deck[idx] = deck[i];
        deck[i] = temp;
    }
}

int get_logic_val(Rank r) {
    if (r == A) return 14;
    return (int)r + 1;
}

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

void analyze_hand(Hand* h) {
    sort_cards(h);

    int flush = 1;
    int straight = 1;
    int counts[15] = { 0 };

    for (int i = 0; i < 4; i++) {
        if (h->cards[i].suit != h->cards[i + 1].suit) { flush = 0; break; }
    }
    for (int i = 0; i < 4; i++) {
        if (get_logic_val(h->cards[i + 1].rank) != get_logic_val(h->cards[i].rank) + 1) { straight = 0; break; }
    }
    if (!straight && h->cards[0].rank == 二 && h->cards[4].rank == A && h->cards[3].rank == 五) {
        straight = 1;
    }

    for (int i = 0; i < 5; i++) counts[get_logic_val(h->cards[i].rank)]++;

    int pairs = 0, threes = 0, fours = 0;
    for (int i = 2; i <= 14; i++) {
        if (counts[i] == 2) pairs++;
        if (counts[i] == 3) threes++;
        if (counts[i] == 4) fours++;
    }

    h->primary_rank = 0;
    h->secondary_rank = 0;

    if (flush && straight) {
        h->type = 同花顺;
        h->primary_rank = (h->cards[4].rank == A && h->cards[0].rank == 二) ? 5 : get_logic_val(h->cards[4].rank);
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
        h->primary_rank = get_logic_val(h->cards[4].rank);
    }
    else if (straight) {
        h->type = 顺子;
        h->primary_rank = (h->cards[4].rank == A && h->cards[0].rank == 二) ? 5 : get_logic_val(h->cards[4].rank);
    }
    else if (threes == 1) {
        h->type = 三炸;
        for (int i = 2; i <= 14; i++) if (counts[i] == 3) h->primary_rank = i;
    }
    else if (pairs == 2) {
        h->type = 两对;
        int p1 = 0, p2 = 0;
        for (int i = 2; i <= 14; i++) { if (counts[i] == 2) { if (p1 == 0) p1 = i; else p2 = i; } }
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

int compare_hands(const void* a, const void* b) {
    Hand* h1 = (Hand*)a;
    Hand* h2 = (Hand*)b;
    if (h2->type != h1->type) return h2->type - h1->type;
    if (h2->primary_rank != h1->primary_rank) return h2->primary_rank - h1->primary_rank;
    if (h2->secondary_rank != h1->secondary_rank) return h2->secondary_rank - h1->secondary_rank;
    for (int i = 4; i >= 0; i--) {
        int val1 = get_logic_val(h1->cards[i].rank);
        int val2 = get_logic_val(h2->cards[i].rank);
        if (val1 != val2) return val2 - val1;
    }
    return 0;
}

// --- 图形化绘制函数 (已针对汉字优化) ---

void draw_hand_graphically(Hand* h) {
    // 边框颜色：灰色
    set_color(8);
    // 行1: 上边框 (宽度为7个字符位)
    for (int i = 0; i < 5; i++) printf("┌─────┐ ");
    printf("\n");

    // 行2: 点数 (左上)
    for (int i = 0; i < 5; i++) {
        printf("│");
        // 颜色设置
        int color = (h->cards[i].suit == 红桃 || h->cards[i].suit == 方块) ? 12 : 11; // 红或青
        set_color(color);
        printf("%-2s   ", rankNames[h->cards[i].rank]);
        set_color(8);
        printf("│ ");
    }
    printf("\n");

    // 行3: 花色 (中间) - 关键修改
    for (int i = 0; i < 5; i++) {
        printf("│");
        int color = (h->cards[i].suit == 红桃 || h->cards[i].suit == 方块) ? 12 : 11;
        set_color(color);

        // 汉字占4个字节(或2个全角宽度)，卡片内宽5格
        // "红桃" 占4个显存位，加上1个空格 = 5个位，正好填满
        printf("%s ", suitNames[h->cards[i].suit]);

        set_color(8);
        printf("│ ");
    }
    printf("\n");

    // 行4: 点数 (右下)
    for (int i = 0; i < 5; i++) {
        printf("│   ");
        int color = (h->cards[i].suit == 红桃 || h->cards[i].suit == 方块) ? 12 : 11;
        set_color(color);
        printf("%2s", rankNames[h->cards[i].rank]);
        set_color(8);
        printf("│ ");
    }
    printf("\n");

    // 行5: 下边框
    for (int i = 0; i < 5; i++) printf("└─────┘ ");
    reset_color();
    printf("\n");
}

void game(Card deck[52])
{
    Init_puke(deck);
    fisher_yates_shuffle(deck);

    int num_players = 0;
    printf("请选择玩家数(2-5): ");
    if (scanf_s("%d", &num_players) != 1) { while (getchar() != '\n'); return; }
    if (num_players < 2) num_players = 2;
    if (num_players > 5) num_players = 5;

    Hand players[5];
    int deck_idx = 0;

    // 发牌与分析
    for (int i = 0; i < num_players; i++) {
        players[i].id = i + 1;
        for (int k = 0; k < 5; k++) {
            players[i].cards[k] = deck[deck_idx++];
        }
        analyze_hand(&players[i]);
    }

    // 图形化显示每个玩家
    for (int i = 0; i < num_players; i++) {
        printf("\n");
        set_color(15); // 亮白
        printf("玩家 %d 的手牌: ", players[i].id);

        // 高亮特殊牌型
        if (players[i].type >= 三拖二) set_color(14); // 黄色
        else if (players[i].type >= 三炸) set_color(10); // 绿色
        else set_color(7); // 默认

        printf("【 %s 】\n", typeNames[players[i].type]);
        reset_color();

        draw_hand_graphically(&players[i]);
    }

    // 排序定胜负
    qsort(players, num_players, sizeof(Hand), compare_hands);

    printf("\n══════════════════════════════\n");
    set_color(14);
    printf("  获胜者: 玩家 %d \n", players[0].id);
    printf("  牌型: %s \n", typeNames[players[0].type]);
    reset_color();
    printf("══════════════════════════════\n\n");

    int i = 1;
    while (i < num_players && compare_hands(&players[0], &players[i]) == 0) {
        printf(" 平局: 玩家 %d (%s)\n", players[i].id, typeNames[players[i].type]);
        i++;
    }
}

int main()
{
    srand((unsigned int)time(NULL));

    while (1)
    {
        menu_1();
        int Flag_Num = -1;
        if (scanf_s("%d", &Flag_Num) != 1) { while (getchar() != '\n'); continue; }

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
                    printf("输入选项: ");
                    scanf_s("%d", &Flag_Num2);

                    if (Flag_Num2 == 1)
                    {
                        Card deck[52];
                        game(deck);
                        system("pause");
                        system("cls");
                        break;
                    }
                    else if (Flag_Num2 == 2)
                    {
                        break;
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
