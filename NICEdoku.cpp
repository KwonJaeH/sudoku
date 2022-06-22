#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <locale.h>
#include <stdlib.h>
#include <process.h>

#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77
#define ENTER 13

typedef struct stack
{
	int num;
	int row, col;
	int si, sj;

	struct stack* next;

}STACK;


// 현재 커서의 위치와 입력을 위한 변수
int x, y, ch;

// 보드판의 크기를 위한 변수
int Size = 9;
int dx = 3;
int dy = 3;

// 힌트 개수
int hint_count = 10;

// 난이도
int level = 0;

// 잘못된 입력 개수
int wrongnum = 0;

// 타이머를 위한 변수
int timestop = 0;
int second = 0;
int minute = 0;

// 스도쿠 문제 및 정답 생성 배열
int sudoku[9][9] = { 0 };
int prob[9][9] = { 0 };

// 보드판 생성 배열
wchar_t chart[37][19] = { 0 };

// 빈칸과 스도쿠를 비교해 값을 넣어주도록 하기 위한 배열
int checkblank_i[50] = { 0 }; // 보드판에서 빈 공간의 x좌표 저장
int checkblank_j[50] = { 0 }; // 보드판에서 빈 공간의 y좌표 저장
int sudokublank_i[50] = { 0 }; // 문제에서 0의 x좌표 저장
int sudokublank_j[50] = { 0 }; // 문제에서 0의 y좌표 저장

						// 빈칸 개수
int blank = 0;
int leftblank;

// 재실행 및 되돌리기 기능을 위한 구조체
STACK* BACK = NULL;

STACK* RE_TOP = NULL;
STACK* RE_BACK = NULL;

STACK* S_NODE = NULL;
STACK* TOTAL_TOP = NULL;
STACK* WRONGPOP = NULL;


void gotoxy(int x, int y)//커서를 x, y좌표로 옮겨주는 함수.
{
	COORD XY = { (short)x, (short)y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), XY);
}

void SetColor(int color)// 콘솔 색깔을 지정하는 함수
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

enum {
	BLACK,
	DARK_BLUE,
	DARK_GREEN,
	DARK_SKY_BLUE,
	DARK_RED,
	DARK_VIOLET,
	DARK_YELLOW,
	GRAY,
	DARK_GRAY,
	BLUE,
	GREEN,
	SKY_BLUE,
	RED,
	VIOLET,
	YELLOW,
	WHITE,
};



unsigned __stdcall Threadtimer(void* arg)
{
	while (1)
	{
		SetColor(WHITE);
		gotoxy(50, 20);
		printf("시간  %2d:  %2d", minute, second);
		gotoxy(x * 4 + 2, y * 2 + 1);
		Sleep(1000);
		second++;
		if (second == 60)
		{
			second = 0;
			minute++;
		}
		if (timestop > 0)
		{
			timestop = 0;
			break;
		}
	}
	return 0;
}





void total(int row, int col, int si, int sj, int num)
{
	STACK* T = (STACK*)malloc(sizeof(STACK));


	T->row = row;
	T->col = col;
	T->si = si;
	T->sj = sj;
	T->num = num;
	T->next = NULL;

	if (TOTAL_TOP == NULL)
	{
		TOTAL_TOP = T;
	}
	else
	{
		T->next = TOTAL_TOP;
		TOTAL_TOP = T;
	}
}

void restore(int row, int col, int si, int sj, int num)
{
	STACK* R = (STACK*)malloc(sizeof(STACK));


	R->row = row;
	R->col = col;
	R->si = si;
	R->sj = sj;
	R->num = num;
	R->next = NULL;

	if (RE_TOP == NULL)
	{
		RE_TOP = R;
	}
	else
	{
		R->next = RE_TOP;
		RE_TOP = R;
	}
}

void Redo()
{
	if (RE_TOP == NULL)
	{
		RE_BACK = NULL;

	}
	else
	{
		RE_BACK = RE_TOP;

		if (prob[RE_TOP->si][RE_TOP->sj] == 0)
		{
			prob[RE_TOP->si][RE_TOP->sj] = RE_TOP->num;
			gotoxy(RE_TOP->row, RE_TOP->col);
			SetColor(GREEN);
			printf("%d", prob[RE_TOP->si][RE_TOP->sj]);
			total(RE_TOP->row, RE_TOP->col, RE_TOP->si, RE_TOP->sj, prob[RE_TOP->si][RE_TOP->sj]);
			RE_TOP = RE_TOP->next;

			leftblank--;
		}
		else
		{

			prob[RE_TOP->si][RE_TOP->sj] = 0;
			gotoxy(RE_TOP->row, RE_TOP->col);
			printf(" ");
			total(RE_TOP->row, RE_TOP->col, RE_TOP->si, RE_TOP->sj, RE_TOP->num);
			RE_TOP = RE_TOP->next;

			leftblank++;
		}


	}
}



void wrongpop()
{
	WRONGPOP = TOTAL_TOP;
	if (WRONGPOP == NULL)
	{

	}
	else
	{
		while (1)
		{
			if (prob[WRONGPOP->si][WRONGPOP->sj] != sudoku[WRONGPOP->si][WRONGPOP->sj])
			{

				gotoxy(WRONGPOP->row, WRONGPOP->col);

				if (prob[WRONGPOP->si][WRONGPOP->sj] != 0)
				{
					SetColor(RED);
					printf("%d", prob[WRONGPOP->si][WRONGPOP->sj]);
					wrongnum++;
				}

				WRONGPOP = WRONGPOP->next;

			}
			else
			{
				WRONGPOP = WRONGPOP->next;
			}
			if (WRONGPOP == NULL)
			{
				break;
			}

		}
	}

}


void Undo()
{

	if (TOTAL_TOP == NULL)
	{
		BACK = NULL;

	}
	else
	{
		BACK = TOTAL_TOP;

		if (prob[TOTAL_TOP->si][TOTAL_TOP->sj] == 0)
		{
			prob[TOTAL_TOP->si][TOTAL_TOP->sj] = TOTAL_TOP->num;
			gotoxy(TOTAL_TOP->row, TOTAL_TOP->col);
			SetColor(GREEN);
			printf("%d", prob[TOTAL_TOP->si][TOTAL_TOP->sj]);
			restore(TOTAL_TOP->row, TOTAL_TOP->col, TOTAL_TOP->si, TOTAL_TOP->sj, prob[TOTAL_TOP->si][TOTAL_TOP->sj]);

			TOTAL_TOP = TOTAL_TOP->next;
			leftblank--;

		}
		else
		{

			prob[TOTAL_TOP->si][TOTAL_TOP->sj] = 0;
			gotoxy(TOTAL_TOP->row, TOTAL_TOP->col);
			printf(" ");
			restore(TOTAL_TOP->row, TOTAL_TOP->col, TOTAL_TOP->si, TOTAL_TOP->sj, TOTAL_TOP->num);

			TOTAL_TOP = TOTAL_TOP->next;
			leftblank++;

		}


	}
}


void erase(int row, int col)
{
	S_NODE = TOTAL_TOP;
	if (S_NODE == NULL)
	{
		return;
	}
	else
	{
		while (1)
		{

			if (S_NODE->row == row && S_NODE->col == col)
			{
				gotoxy(row, col);
				printf(" ");

				total(S_NODE->row, S_NODE->col, S_NODE->si, S_NODE->sj, prob[S_NODE->si][S_NODE->sj]);
				prob[S_NODE->si][S_NODE->sj] = 0;
				leftblank++;
				break;
			}
			else
			{
				S_NODE = S_NODE->next;
			}
			if (S_NODE == NULL)
			{
				break;
			}
		}
	}
}


bool check_(int x, int y, int n)
{
	for (int i = 0; i < 9; i++)
	{
		if (sudoku[i][y] == n)
			return false;
	}
	for (int j = 0; j < 9; j++)
	{
		if (sudoku[x][j] == n)
			return false;
	}
	int a = (x / 3) * 3;
	int b = (y / 3) * 3;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (sudoku[a + i][b + j] == n)
				return false;
		}
	}
	return true;
}

void DrawBox()
{

	setlocale(LC_ALL, "");

	chart[0][0] = L'┏';
	chart[36][0] = L'┓';
	chart[0][18] = L'┗';
	chart[36][18] = L'┛';
	// 외곽 위쪽

	for (int i = 1; i <= 35; i++)
	{

		if (i % 4 != 0)
			chart[i][0] = L'─';
		else
			chart[i][0] = L'┬';
	}

	// 외곽 왼쪽
	for (int i = 1; i < 18; i++)
	{

		if (i % 2 == 0)
			chart[0][i] = L'├';
		else
			chart[0][i] = L'┃';
	}

	// 외곽 오른쪽
	for (int i = 1; i < 18; i++)
	{

		if (i % 2 == 0)
			chart[36][i] = L'┤';
		else
			chart[36][i] = L'┃';
	}

	// 내부 십자
	for (int i = 1; i < 18; i += 2)
	{
		for (int j = 1; j <= 35; j++)
		{

			if (j % 4 != 0)
				chart[j][i + 1] = L'─';
			else
				chart[j][i + 1] = L'┼';
		}
	}
	// 내부 일자
	for (int i = 1; i < 18; i += 2)
	{
		for (int j = 1; j <= 35; j++)
		{

			if (j % 4 != 0)
				chart[j][i] = L' ';
			else
				chart[j][i] = L'┃';
		}
	}

	// 외곽 아래쪽
	for (int i = 1; i <= 35; i++)
	{

		if (i % 4 != 0)
			chart[i][18] = L'─';
		else
			chart[i][18] = L'┴';
	}

	int check = 0;
	int mcheck = 5;
	int pi = 0, pj = 0;


	for (int j = 0; j < 19; j++)
	{
		for (int i = 0; i < 37; i++)
		{
			if (chart[i][j] == ' ')
			{
				check++;
				if (check == 2)
				{
					if (prob[pi][pj] == 0)
					{
						printf(" ");
						checkblank_i[blank] = i;
						checkblank_j[blank] = j;
						sudokublank_i[blank] = pi;
						sudokublank_j[blank] = pj;
						blank++;
					}
					else
					{
						SetColor(WHITE);
						printf("%d", prob[pi][pj]);

					}
					pj++;
				}
				else if (check == mcheck)
				{
					if (prob[pi][pj] == 0)
					{
						printf(" ");
						checkblank_i[blank] = i;
						checkblank_j[blank] = j;
						sudokublank_i[blank] = pi;
						sudokublank_j[blank] = pj;
						blank++;
					}
					else
					{
						SetColor(WHITE);
						printf("%d", prob[pi][pj]);

					}
					pj++;
					if (pj == 9)
					{
						pi++;
						pj = 0;
					}
					mcheck += 4;
					check++;
					continue;
				}
				else

				{
					SetColor(DARK_GRAY);
					printf("%lc", chart[i][j]);
				}
			}
			else
			{
				if (i % 12 == 0 || j % 6 == 0) SetColor(SKY_BLUE);
				else SetColor(WHITE);
				printf("%lc", chart[i][j]);
			}
		}
		printf("\n");
	}
}

int getXY()
{
	if (x < 0) x = 0;
	else if (x >= Size) x = Size - 1;
	if (y < 0) y = 0;
	else if (y >= Size) y = Size - 1;
	gotoxy(x * 4 + 2, y * 2 + 1);
	return x, y;
}

int GetDigit(int n)
{
	int ch = n;

	if (ch >= '0' && ch <= '9') ch = ch - '0';
	else ch = -1;

	return ch;
}

void InputNum()
{
	int num;
	int str[] = { 0,1,2,3,4,5,6,7,8,9 };

	num = GetDigit(ch);

	if (num < 0 || num > Size) return;


	if (prob[y][x] == 0)
	{
		prob[y][x] = str[num];
		if (str[num] == 0)
			return;
		SetColor(GREEN);
		printf("%d", str[num]);
		leftblank--;
		gotoxy(40, 15);
		SetColor(WHITE);
		printf("남은 빈칸 : %2d", leftblank);

		total(x * 4 + 2, y * 2 + 1, y, x, str[num]);

	}
}

void KeyControl()
{

	while (1)
	{
		int check = 0;
		if (!_kbhit())
		{
			Sleep(10);
			continue;
		}

		ch = _getch();//키입력을 받아서 ch변수에 저장

		if (ch == 0xE0)//특수키이면
		{
			ch = _getch();
			switch (ch)
			{
			case LEFT:x--;   break;
			case RIGHT: x++; break;
			case UP: y--; break;
			case DOWN: y++; break;

			}
			getXY();
		}
		else
		{
			switch (ch)
			{
			case 'a':  //정답 확인
			case 'A':
				for (int i = 0; i < 9; i++)
				{
					for (int j = 0; j < 9; j++)
					{
						if (sudoku[i][j] == prob[i][j])
							check++;
						else
						{
							wrongpop(); // 틀린 숫자의 색을 변경하는 함수
						}
					}
				}
				if (check == 81){
				
					SetColor(WHITE);
					gotoxy(50, 10);
					printf("정답입니다!!      \n");
					gotoxy(50, 11);
					printf("걸린 시간 : %2d:%2d", minute, second);
					timestop++;
				}
				else if (wrongnum > 0)
				{
					SetColor(WHITE);
					gotoxy(50, 10);
					printf("다시 작성하십시오.\n");
					check = 0;
					wrongnum = 0;
				}
				else
				{
					SetColor(WHITE);
					gotoxy(50, 10);
					printf("계속 작성하십시오.\n");
					check = 0;
					wrongnum = 0;
				}
				break;
			case'h':
			case 'H':
				while (1)
				{
					if (hint_count != 0)
					{
						int n = rand() % blank;
						int blcheck = 0;
						if (prob[sudokublank_i[n]][sudokublank_j[n]] == 0)
						{
							prob[sudokublank_i[n]][sudokublank_j[n]] = sudoku[sudokublank_i[n]][sudokublank_j[n]];
							SetColor(BLUE);
							gotoxy(checkblank_i[n], checkblank_j[n]);
							printf("%d", prob[sudokublank_i[n]][sudokublank_j[n]]);
							blcheck++;
							hint_count--;

							SetColor(WHITE);
							gotoxy(50, 5);
							printf("남은 힌트 : %d", hint_count);
							leftblank--;
							gotoxy(40, 15);
							SetColor(WHITE);
							printf("남은 빈칸 : %2d", leftblank);
						}
						if (blcheck == 1)
						{
							blcheck = 0;
							break;
						}
					}
					else
					{
						break;
					}


				}
				break;
			case 'b':
			case 'B':
				Undo();
				gotoxy(40, 15);
				SetColor(WHITE);
				printf("남은 빈칸 : %2d", leftblank);
				break;
			case 'e':
			case 'E':
				erase(x * 4 + 2, y * 2 + 1);

				gotoxy(40, 15);
				SetColor(WHITE);
				printf("남은 빈칸 : %2d", leftblank);
				break;
			case 'r':
			case 'R':
				Redo();
				gotoxy(40, 15);
				SetColor(WHITE);
				printf("남은 빈칸 : %2d", leftblank);
				break;
			case 'f':
			case 'F':
				timestop++;
				return;
			default:
				InputNum();
				break;
			}
		}

		gotoxy(x * 4 + 2, y * 2 + 1);//숫자가 입력된후 커서위치를 원위치로
	}
}

void printMenu(int n, int& y)
{
	int i, x;
	const char* MainMenu[] =
	{
	   "     ########                     ####             ###   ###                 ",
	   "    ##########                   ####             ###  ###                   ",
	   "   ####            ##   ##   #######   ######    ######       ##   ##        ",
	   "       ####       ##   ##   ##   ##   ##  ##    ### ###      ##   ##         ",
	   "         ####    ##   ##   ##   ##   ##  ##    ##    ##     ##   ##          ",
	   "   #########    ##   ##   ###  ##   ##  ##    ##     ##    ##   ##           ",
	   "    #######    #######    ######   ######    ##      ##   #######            ",
	   "                                                                             ",
	   "                       ---    초          급    ---                          ",
	   "                                                                             ",
	   "                       ---    중          급    ---                          ",
	   "                                                                             ",
	   "                       ---    고          급    ---                          ",
	   "                                                                             ",
	   "                       ---    종          료    ---                          ",
	   "                                                                             " };


	x = 39 - lstrlen(MainMenu[0]) / 2;
	y = 10 - sizeof(MainMenu) / sizeof(MainMenu[0]) / 2 - 1;
	for (i = 0; i < sizeof(MainMenu) / sizeof(MainMenu[0]); i++)
	{
		gotoxy(x, y + i);
		if (i == n * 2) SetColor(GREEN);
		printf("%s", MainMenu[i]);
		if (i == n * 2) SetColor(WHITE);
	}
	gotoxy(x, y + n * 2);
}


int  Menu()
{
	int y, yy, ch = 1;
	int num = 4;


	system("mode con: lines=30 cols=77");
	system("cls");
	printMenu(num, y);

	yy = y;
	y = yy + 2;
	while (1)
	{
		ch = _getch();
		if (ch == 0xE0)
		{
			ch = _getch();
			switch (ch)
			{
			case UP: y -= 2; break;
			case DOWN: y += 2; break;
			}
			if (y > 9 + yy) y = yy + 2;
			else if (y < yy + 2) y = yy + 9;
			num = (y - yy) / 2 + 3;
			printMenu(num, yy);
		}
		if (ch == ENTER)
		{
			if (y == 3)
			{
				level = 1;
				return num;
			}
			else if (y == 5)
			{
				level = 2;
				return num;
			}
			else if (y == 7)
			{
				level = 3;
				return num;
			}
			else if (y == 9)
			{
				level = 4;
				return num;
			}
		}
	}
	return ch - '0';
}


int main()
{
	HANDLE hThread;

	unsigned threadID;

	while (1)
	{
		SetColor(WHITE);
		Menu();
		timestop = 0;
		system("cls");

		if (level == 4)
			return 0;

		int n = 0;
		int count = 0;
		srand(time(NULL));


		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				n = rand() % 9 + 1;

				if (check_(i, j, n)) sudoku[i][j] = n;
				else
				{
					count++;
					if (count == 1000)
					{
						i = -1;
						count = 0;
						for (int a = 0; a < 9; a++)
						{
							for (int b = 0; b < 9; b++)
							{
								sudoku[a][b] = 0;
							}
						}
						break;
					}
					j--;
					continue;
				}
			}
		}



		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				prob[i][j] = sudoku[i][j];
			}
		}

		int ri, rj;


		int check = 0;

		int easy = rand() % 10 + 20;
		int normal = rand() % 10 + 30;
		int hard = rand() % 10 + 40;

		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				prob[i][j] = sudoku[i][j];
			}
		}

		if (level == 1)
		{
			for (check = 0; check < easy;)
			{
				ri = rand() % 9;
				rj = rand() % 9;

				if (prob[ri][rj] == 0)
					continue;
				else
					prob[ri][rj] = 0;
				check++;

			}
			leftblank = easy;
		}
		else if (level == 2)
		{
			for (check = 0; check < normal;)
			{
				ri = rand() % 9;
				rj = rand() % 9;

				if (prob[ri][rj] == 0)
					continue;
				else
					prob[ri][rj] = 0;
				check++;

			}
			leftblank = normal;
		}
		else if (level == 3)
		{
			for (check = 0; check < hard;)
			{
				ri = rand() % 9;
				rj = rand() % 9;

				if (prob[ri][rj] == 0)
					continue;
				else
					prob[ri][rj] = 0;
				check++;

			}
			leftblank = hard;
		}
		hint_count = 10;

		DrawBox();

		second = 0;
		minute = 0;


		if (level == 1)
		{
			SetColor(DARK_SKY_BLUE);
			printf("초급");
		}
		else if (level == 2)
		{
			SetColor(DARK_SKY_BLUE);
			printf("중급");
		}
		else if (level == 3)
		{
			SetColor(DARK_SKY_BLUE);
			printf("고급");
		}

		SetColor(YELLOW);
		printf("\n");
		printf("\n힌트(BLUE) : H");
		printf("\n정답 확인(RED) : A");
		printf("\n되돌리기 : B, 재실행 : R");
		printf("\n지우기 : E");
		SetColor(VIOLET);
		printf("\n\n종료(FINISH) : F");



		hThread = (HANDLE)_beginthreadex(NULL, 0, Threadtimer, 0, 0, &threadID);

		if (0 == hThread)
		{

			puts("_beginthreadex() error");


			return -1;

		}



		gotoxy(2, 1);

		KeyControl();


	}
}