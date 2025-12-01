#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

typedef enum
{
	红桃,
	黑桃,
	梅花,
	方块,
}Suit;

typedef enum
{
    A,
    二,
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
    零
} Rank;
const char* suitNames[] = { "红桃", "黑桃", "梅花", "方块" };
const char* rankNames[] = { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
typedef struct
{
    Suit suit;
    Rank rank;
}Card;
typedef struct
{
	Card cards[5];
	int type;
}Hand;
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
    printf("*      1.人机对局      *\n");
    printf("*      2.玩家对局      *\n");
    printf("*      3.返回上一步    *\n");
    printf("------------------------\n");
}
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
        int i = rand() % (idx);
        Card temp;
        temp = deck[idx];
        deck[idx] = deck[i];
        deck[i] = temp;
    }
}
void Init_player(Card deck[52],Hand player[5], int player_num)
{
    int idx = 0;
    for (int i = 0; i < 5; i++)
    {
        if (deck[idx].rank == 零)
        {
            continue;
        }
        else
        {
            player[0].cards[i] = deck[idx];
            deck[idx].rank = 零;
            printf("%s%s ", suitNames[player[0].cards[i].suit], rankNames[player[0].cards[i].rank]);
        }
        idx++;
    }
    printf("\n");
}
void game(Card deck[52])
{
    Init_puke(deck);
    fisher_yates_shuffle(deck);
    printf("请选择玩家数(2~5)\n");
    int flag=0;
    scanf_s("%d", &flag);
    Hand player[5];
    Init_player(deck, player, flag);
}
int main()
{
    while (1)
    {
        menu_1();
        int Flag_Num = -1;
        scanf_s("%d", &Flag_Num);
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
                    if (Flag_Num2 != 1 && Flag_Num2 != 2 && Flag_Num2 != 3)
                    {
                        printf("输入错误，请重新输入\n");
                    }
                    else
                    {
                        if (Flag_Num2 == 1)
                        {
                            Card deck[52];
                            game(deck);
                        }
                        if (Flag_Num2 == 3)
                        {
                            break;
                        }
                    }
                }
            }
            if (Flag_Num == 2)
            {
                break;
            }
        }
    }
    return 0;
}