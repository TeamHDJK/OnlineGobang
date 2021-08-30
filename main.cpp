/////////////////////////////////////////////////////////
//
//	����������
//	main.cpp
// 
//	���뻷����Visual C++ 2019 | EasyX 20210730
//	ԭ �� �ߣ��¿ɼ� <emil09_chen@126.com>
//	����ģ�飺huidong <huidong_mail@163.com>
// 
//	ԭ�淢��ʱ�䣺2020-4-28
//	�����淢��ʱ�䣺2021-8-30
//

#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <easyx.h>
#include <conio.h>

#include <windows.h>
#include <wininet.h>
#include <string>
#include <vector>
using namespace std;

// ������ͨѶ���
#include "server.h"

// �ඨ��

// ����λ�õ���
class seat
{
public:
	int i = 0;      // y ����
	int j = 0;      // x ����
	int number = 0; // ����
};

// �������̵���
class box
{
public:
	void draw();            // ����
public:
	int x = 0;              // x ����
	int y = 0;              // y ����
	int value = -1;         // ֵ�����壺1�����壺0����λ��-1��
	int modle = 0;          // ģʽ
	bool isnew = false;     // �Ƿ���ѡ���
	COLORREF color = WHITE; // ���̱���ɫ
};


// ȫ�ֱ���
box BOX[19][19];      // ����
int win = -1;         // ˭Ӯ�ˣ�0�����壬1�����壬2��ƽ�֣�
int whoplay = 0;      // �ֵ�˭������
int playercolor = 0;  // �����ɫ
int dx[4]{ 1,0,1,1 }; // - | \ / �ĸ�����
int dy[4]{ 0,1,1,-1 };
int Score[3][5] = //���ֱ�
{
	{ 0, 80, 250, 500, 500 }, // ����0��
	{ 0, 0,  80,  250, 500 }, // ����1��
	{ 0, 0,  0,   80,  500 }  // ����2��
};
int MAXxs[361];   //����x����
int MAXys[361];   //����y����
int mylength = 0; //���Ž���
bool vis[19][19][4]; // ���λ���Ƿ���ʹ�

//// �������

bool bOnline;	// ģʽ�Ƿ�Ϊ����
bool bWatch;	// �Ƿ�Ϊ�Թ�ģʽ
wstring strPlayerName;	// ��ǰ�������
wstring strHisName;		// ��һ�������


// ��������
void draw();                  // ����
void init();                  // ��ʼ��
seat findbestseat(int color, int c); // Ѱ�����λ��
void isWIN();                 // �ж���Ӯ
void game();                  // ��Ϸ������


// ���������öԻ���
void ServerSettingDialog()
{
	wchar_t strServer[1024] = { 0 };
	if (InputBox(strServer, 1024,
		L"�Զ�����������������\r\n������ \"http://www.game.com/onlinegobang/\"��\r\nע�⣺����ĩβ��Ҫ��'/'",
		(LPCTSTR)0, (LPCTSTR)0, 480, 0, false))
	{
		server_setServer(strServer);
		MessageBox(GetHWnd(), L"���óɹ���\r\n��ȷ�����ķ�������ַ���ã����������������ݶ��ᷢ�͵������õķ�������", L"", MB_OK | MB_ICONINFORMATION);
	}
}

// ��Ϸ�˵�
void Menu()
{
	settextcolor(BLACK);

	settextstyle(50,0,L"Consolas");
	outtextxy(20,20,L"Online Gobang");
	settextstyle(24, 0, L"system");
	outtextxy(100, 200, L"Press [A] ����������ս");
	outtextxy(100, 240, L"Press [B] ����������Ϸ");
	outtextxy(100, 280, L"Press [T] ������������");

	outtextxy(340, 48, L"lovely_ckj & huidong ������������ս�� (beta)");
	outtextxy(400, 88, L"2021-8-30");
	
	ExMessage msg;
	while (true)
	{
		if (peekmessage(&msg, EM_KEY))
		{
			switch (msg.vkcode)
			{
			case 'A':
				bOnline = true;
				break;
			case 'B':
				bOnline = false;
				break;
			case 'T':
				ServerSettingDialog();
				flushmessage();
				continue;
			default:
				continue;
			}

			break;
		}	
	}

	cleardevice();
	flushmessage();
}

// ��ȡ��ǰ������Ϸ�����̣��Զ��洢����ǰ������
void GetGamingMap()
{
	CHESSMEN chess_map = server_getChessMap();
	for (int i = 0; i < chess_map.num; i++)
	{
		CHESSMAN chess = chess_map.chess[i];
		BOX[chess.y][chess.x].value = 1 - (bool)chess.color;
	}
	delete[] chess_map.chess;
}

// ����ƥ��
bool OnlineMatch()
{
	// ����
	if (server_isFull())
	{
		server_getUserName(strPlayerName, strHisName);
		server_setMyColor(WHITE);
		
		MessageBox(GetHWnd(), L"�Ѿ�����ҿ�ʼ�����ˣ������������սģʽ", L"", MB_OK | MB_ICONINFORMATION);
		
		// �������ǵ�����
		GetGamingMap();
		
		bWatch = true;
	}
	else
	{
		wchar_t name[128] = { 0 };
		InputBox(name, 128, L"���������ǳ�");
		strPlayerName = name;

		int player_num = server_getPlayerNum();

		// ������Ϸ
		if (server_join(strPlayerName))
		{
			// ƥ��
			cleardevice();
			outtextxy(50, 50, L"ƥ����������С���");

			int t = clock();
			while (true)
			{
				// ����
				if (server_isFull())
				{
					wstring strName[2];
					server_getUserName(strName[0], strName[1]);
					
					if (player_num)
					{
						strHisName = strName[0];
					}
					else
					{
						strHisName = strName[1];
					}

					wstring str = L"ƥ�䵽������� " + strHisName + L"����ȷ���󽫿�ʼ��Ϸ";

					MessageBox(GetHWnd(), str.c_str(), L"", MB_OK | MB_ICONINFORMATION);
					
					if (!server_start())
					{
						MessageBox(GetHWnd(), L"��ʼ��Ϸʧ�ܣ���������", L"", MB_OK | MB_ICONERROR);
					}
					
					bWatch = false;
					playercolor = 1 - (bool)server_getColor();

					break;
				}

				int wait = (clock() - t) / CLOCKS_PER_SEC;
				wchar_t buf[24] = { 0 };
				wsprintf(buf, L"�ѵȴ���%d s", wait);
				
				if (wait > 120)
				{
					MessageBox(GetHWnd(), L"ƥ�䳬ʱ�������������档\r\nȥ������Ļ��һ�������~", L"", MB_OK | MB_ICONINFORMATION);
					server_reset();
					return false;
				}

				int x = 100,y = 100;

				setlinecolor(WHITE);
				setfillcolor(WHITE);
				fillrectangle(x, y, x + 200, y + 100);
				outtextxy(x, y, buf);

				Sleep(500);
			}
		}
		else
		{
			MessageBox(GetHWnd(), L"������Ϸʧ�ܣ���������ϷȻ������", L"", MB_OK);
			return false;
		}
	}

	return true;
}


// main ����
int main()
{
	initgraph(700, 750); // ��ʼ����ͼ����
	setbkcolor(WHITE);
	setbkmode(TRANSPARENT); // ����͸�������������

	while (1)
	{
		cleardevice();

		// ��ʼ��
		init();

		// ��Ϸ�˵�
		Menu();

		// ƥ��
		if (bOnline)
		{
			if (!OnlineMatch())
			{
				continue;
			}
		}

		// ���һ����Ϸ��Ȼ�����¿�ʼ
		game();
	}
}


// �ຯ������

// ���ƺ���
void box::draw()
{
	COLORREF thefillcolor = getfillcolor(); // ���������ɫ
	setlinestyle(PS_SOLID, 2);              // ����ʽ����
	setfillcolor(color);                    // �����ɫ����
	solidrectangle(x, y, x + 30, y + 30);   // �����ޱ߿��������
	if (isnew)
	{
		// ��������µ�
		// ���Ʊ߿���
		setlinecolor(LIGHTGRAY);
		line(x + 1, y + 2, x + 8, y + 2);
		line(x + 2, y + 1, x + 2, y + 8);
		line(x + 29, y + 2, x + 22, y + 2);
		line(x + 29, y + 1, x + 29, y + 8);
		line(x + 2, y + 29, x + 8, y + 29);
		line(x + 2, y + 22, x + 2, y + 29);
		line(x + 29, y + 29, x + 22, y + 29);
		line(x + 29, y + 22, x + 29, y + 29);
	}
	setlinecolor(BLACK);
	switch (modle)
	{
		// �����ǲ�ͬλ�����̵���ʽ
	case 0:
		line(x + 15, y, x + 15, y + 30);
		line(x - 1, y + 15, x + 30, y + 15);
		break;
		//  *
		// ***
		//  *
	case 1:
		line(x + 14, y + 15, x + 30, y + 15);
		setlinestyle(PS_SOLID, 3);
		line(x + 15, y, x + 15, y + 30);
		setlinestyle(PS_SOLID, 2);
		break;
		// *
		// ***
		// *
	case 2:
		line(x - 1, y + 15, x + 15, y + 15);
		setlinestyle(PS_SOLID, 3);
		line(x + 15, y, x + 15, y + 30);
		setlinestyle(PS_SOLID, 2);
		break;
		//   *
		// ***
		//   *
	case 3:
		line(x + 15, y + 15, x + 15, y + 30);
		setlinestyle(PS_SOLID, 3);
		line(x - 1, y + 15, x + 30, y + 15);
		setlinestyle(PS_SOLID, 2);
		break;
		// ***
		//  *
		//  *
	case 4:
		line(x + 15, y, x + 15, y + 15);
		setlinestyle(PS_SOLID, 3);
		line(x - 1, y + 15, x + 30, y + 15);
		setlinestyle(PS_SOLID, 2);
		break;
		//  *
		//  *
		// ***
	case 5:
		setlinestyle(PS_SOLID, 3);
		line(x + 15, y, x + 15, y + 15);
		line(x + 15, y + 15, x + 30, y + 15);
		setlinestyle(PS_SOLID, 2);
		break;
		// *
		// *
		// ***
	case 6:
		setlinestyle(PS_SOLID, 3);
		line(x + 15, y, x + 15, y + 15);
		line(x - 1, y + 15, x + 15, y + 15);
		setlinestyle(PS_SOLID, 2);
		break;
		//   *
		//   *
		// ***
	case 7:
		setlinestyle(PS_SOLID, 3);
		line(x - 1, y + 15, x + 15, y + 15);
		line(x + 15, y + 15, x + 15, y + 30);
		setlinestyle(PS_SOLID, 2);
		break;
		// ***
		//   *
		//   *
	case 8:
		setlinestyle(PS_SOLID, 3);
		line(x + 15, y + 15, x + 30, y + 15);
		line(x + 15, y + 15, x + 15, y + 30);
		setlinestyle(PS_SOLID, 2);
		break;
		// ***
		// *
		// *
	case 9:
		line(x + 15, y, x + 15, y + 30);
		line(x - 1, y + 15, x + 30, y + 15);
		setfillcolor(BLACK);
		setlinestyle(PS_SOLID, 1);
		fillcircle(x + 15, y + 15, 4);
		break;
		//  *
		// *O*
		//  *
	}
	switch (value)
	{
	case 0: // ����
		setfillcolor(WHITE);
		setlinestyle(PS_SOLID, 1);
		fillcircle(x + 15, y + 15, 13);
		break;
	case 1: // ����
		setfillcolor(BLACK);
		setlinestyle(PS_SOLID, 1);
		fillcircle(x + 15, y + 15, 13);
		break;
	}
	setfillcolor(thefillcolor); // ��ԭ���ɫ
}



// ������������

// ��������
void draw()
{
	int number = 0; // ���������λ��
	// ���꣨��ֵ��
	TCHAR strnum[19][3] = { _T("1"),_T("2") ,_T("3") ,_T("4"),_T("5") ,_T("6") ,_T("7"),_T("8"),_T("9"),_T("10"), _T("11"),_T("12") ,_T("13") ,_T("14"),_T("15") ,_T("16") ,_T("17"),_T("18"),_T("19") };
	// ���꣨��ĸ��
	TCHAR strabc[19][3] = { _T("A"),_T("B") ,_T("C") ,_T("D"),_T("E") ,_T("F") ,_T("G"),_T("H"),_T("I"),_T("J"), _T("K"),_T("L") ,_T("M") ,_T("N"),_T("O") ,_T("P") ,_T("Q"),_T("R"),_T("S") };
	LOGFONT nowstyle;
	gettextstyle(&nowstyle);
	settextstyle(0, 0, NULL);
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			BOX[i][j].draw(); // ����
			if (BOX[i][j].isnew == true)
			{
				BOX[i][j].isnew = false; // ����һ������λ�õĺڿ����
			}
		}
	}
	// ������
	for (int i = 0; i < 19; i++)
	{
		outtextxy(75 + number, 35, strnum[i]);
		outtextxy(53, 55 + number, strabc[i]);
		number += 30;
	}
	settextstyle(&nowstyle);
}

// �Ծֳ�ʼ��
void init()
{
	win = -1;// ˭Ӯ��
	for (int i = 0, k = 0; i < 570; i += 30)
	{
		for (int j = 0, g = 0; j < 570; j += 30)
		{
			int modle = 0;  // ������ʽ
			BOX[k][g].value = -1;
			BOX[k][g].color = RGB(255, 205, 150);// ���̵�ɫ
			// x��y ����
			BOX[k][g].x = 65 + j;
			BOX[k][g].y = 50 + i;
			// ������ʽ���ж�
			if (k == 0 && g == 0)
			{
				modle = 8;
			}
			else if (k == 0 && g == 18)
			{
				modle = 7;
			}
			else if (k == 18 && g == 18)
			{
				modle = 6;
			}
			else if (k == 18 && g == 0)
			{
				modle = 5;
			}
			else if (k == 0)
			{
				modle = 3;
			}
			else if (k == 18)
			{
				modle = 4;
			}
			else if (g == 0)
			{
				modle = 1;
			}
			else if (g == 18)
			{
				modle = 2;
			}
			else  if ((k == 3 && g == 3) || (k == 3 && g == 15) || (k == 15 && g == 3) || (k == 15 && g == 15) || (k == 3 && g == 9) || (k == 9 && g == 3) || (k == 15 && g == 9) || (k == 9 && g == 15) || (k == 9 && g == 9))
			{
				modle = 9;
			}
			else
			{
				modle = 0;
			}
			BOX[k][g].modle = modle;
			g++;
		}
		k++;
	}
}



// ���ĺ���

int getscore(int color)
{
	int five = 0;
	int life1 = 0, life2 = 0, life3 = 0, life4 = 0;
	int die1 = 0, die2 = 0, die3 = 0, die4 = 0;
	memset(vis, 0, sizeof(vis));
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			if (BOX[i][j].value != color)
			{
				continue;
			}
			for (int k = 0; k < 4; k++)
			{
				if (vis[i][j][k])
				{
					continue;
				}
				vis[i][j][k] = true;
				int length = 1;
				int emeny = 0;
				int nx = i + dx[k], ny = j + dy[k];
				while (nx <= 18 && ny <= 18 && nx >= 0 && ny >= 0 && BOX[nx][ny].value == color)
				{
					length++;
					vis[nx][ny][k] = true;
					nx += dx[k];
					ny += dy[k];
				}
				if (nx < 0 || ny < 0 || nx > 18 || ny > 18 || BOX[nx][ny].value == !color)
				{
					emeny++;
				}
				nx = i - dx[k], ny = j - dy[k];
				while (nx <= 18 && ny <= 18 && nx >= 0 && ny >= 0 && BOX[nx][ny].value == color)
				{
					length++;
					vis[nx][ny][k] = true;
					nx -= dx[k];
					ny -= dy[k];
				}
				if (nx < 0 || ny < 0 || nx > 18 || ny > 18 || BOX[nx][ny].value == !color)
				{
					emeny++;
				}
				if (length >= 5)
				{
					five++;
				}
				else if (length == 4)
				{
					if (emeny == 0) life4++;
					else if (emeny == 1) die4++;
				}
				else if (length == 3)
				{
					if (emeny == 0) life3++;
					else if (emeny == 1) die3++;
				}
				else if (length == 2)
				{
					if (emeny == 0) life2++;
					else if (emeny == 1) die2++;
				}
				else if (length == 1)
				{
					if (emeny == 0) life1++;
					else if (emeny == 1) die1++;
				}
			}
		}
	}
	if (five >= 1)
	{
		return 100000000;
	}
	if (life4 >= 1 || life3 >= 2 || die4 >= 2)
	{
		return 10000000;
	}
	return life1 * 2 + die1 * 1 + life2 * 8 + die2 * 3 + life3 * 50 + die3 * 25 + die4 * 50;
}

// Ѱ�����λ��
/*
seat findbestseat(int color, int c)
{
	if (c == 0)
	{
		//����ǵ�һ��
		//�������
		mylength = 0;
	}
	int MAXnumber = -1;    //��ѷ���
	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			if (BOX[i][j].value == -1) {
				//����ÿһ����λ��
				int length;        //��ǰ���򳤶�
				int emeny;         //��ǰ�������
				int nowi = 0;      //���ڱ�������y����
				int nowj = 0;      //���ڱ�������x����
				int thescore = 0;  //���λ�õĳ�ʼ����
				//�ж��ܱ���û������
				int is = 0;
				for (int k = 0; k < 4; k++)
				{
					nowi = i;
					nowj = j;
					nowi += dx[k];
					nowj += dy[k];
					if (nowi >= 0 && nowj >= 0
						&& nowi <= 18 && nowj <= 18
						&& BOX[nowi][nowj].value != -1)
					{
						is = 1;
						break;
					}
					nowi = i;
					nowj = j;
					nowi += dx[k];
					nowj += dy[k];
					if (nowi >= 0 && nowj >= 0
						&& nowi <= 18 && nowj <= 18
						&& BOX[nowi][nowj].value != -1)
					{
						is = 1;
						break;
					}
					nowi = i;
					nowj = j;
					nowi -= dx[k];
					nowj -= dy[k];
					if (nowi >= 0 && nowj >= 0
						&& nowi <= 18 && nowj <= 18
						&& BOX[nowi][nowj].value != -1)
					{
						is = 1;
						break;
					}
					nowi = i;
					nowj = j;
					nowi -= dx[k];
					nowj -= dy[k];
					if (nowi >= 0 && nowj >= 0
						&& nowi <= 18 && nowj <= 18
						&& BOX[nowi][nowj].value != -1)
					{
						is = 1;
						break;
					}
				}
				if (!is)
				{
					//�����Χû�����ӣ��Ͳ��õݹ���
					continue;
				}
				//�Լ�
				BOX[i][j].value = color;//������������
				for (int k = 0; k < 4; k++)
				{
					//����ĸ�����
					length = 0;
					emeny = 0;
					nowi = i;
					nowj = j;
					while (nowi <= 18 && nowj <= 18 && nowi >= 0 && nowj >= 0 && BOX[nowi][nowj].value == color)
					{
						length++;
						nowj += dy[k];
						nowi += dx[k];
					}
					if (nowi < 0 || nowj < 0 || nowi > 18 || nowj > 18 || BOX[nowi][nowj].value == !color)
					{
						emeny++;
					}
					nowi = i;
					nowj = j;
					while (nowi <= 18 && nowj <= 18 && nowi >= 0 && nowj >= 0 && BOX[nowi][nowj].value == color)
					{
						length++;
						nowj -= dy[k];
						nowi -= dx[k];
					}
					if (nowi < 0 || nowj < 0 || nowi > 18 || nowj > 18 || BOX[nowi][nowj].value == !color)
					{
						emeny++;
					}
					length -= 2;//�жϳ���
					if (length > 4)
					{
						length = 4;
					}
					if (Score[emeny][length] == 500)
					{
						//����ʤ���������ݹ�
						BOX[i][j].value = -1;
						return{ i,j,Score[emeny][length] };
					}
					thescore += Score[emeny][length];
					length = 0;
					emeny = 0;
				}
				//���ˣ�ԭ��ͬ�ϣ�
				BOX[i][j].value = !color;
				for (int k = 0; k < 4; k++)
				{
					length = 0;
					emeny = 0;
					nowi = i;
					nowj = j;
					while (nowi <= 18 && nowj <= 18 && nowi >= 0 && nowj >= 0 && BOX[nowi][nowj].value == !color)
					{
						length++;
						nowj += dy[k];
						nowi += dx[k];
					}
					if (nowi < 0 || nowj < 0 || nowi > 18 || nowj > 18 || BOX[nowi][nowj].value == color)
					{
						emeny++;
					}
					nowi = i;
					nowj = j;
					while (nowi <= 18 && nowj <= 18 && nowi >= 0 && nowj >= 0 && BOX[nowi][nowj].value == !color)
					{
						length++;
						nowj -= dy[k];
						nowi -= dx[k];
					}
					if (nowi < 0 || nowj < 0 || nowi > 18 || nowj > 18 || BOX[nowi][nowj].value == color)
					{
						emeny++;
					}
					length -= 2;
					if (length > 4)
					{
						length = 4;
					}
					if (Score[emeny][length] == 500)
					{
						BOX[i][j].value = -1;
						return{ i,j,Score[emeny][length] };
					}
					thescore += Score[emeny][length];
					length = 0;
					emeny = 0;
				}
				BOX[i][j].value = -1;
				//����Ѿ�����߷���С����û��Ҫ�ݹ���
				if (thescore >= MAXnumber)
				{
					if (c < 3)
					{
						//ֻ����4�㣬����ʱ��̫��
						BOX[i][j].value = color;
						//�ݹ�Ѱ�ҶԷ�����
						int nowScore = thescore - findbestseat(!color, c + 1).number;//�ݹ�������λ�õķ�ֵ
						BOX[i][j].value = -1;
						if (nowScore > MAXnumber)
						{
							//����߷�ֵ��
							MAXnumber = nowScore;
							if (c == 0)
							{
								//��һ��
								mylength = 0;//�������
							}
						}
						if (c == 0)
						{
							//��һ��
							if (nowScore >= MAXnumber)
							{
								//�ѵ�ǰλ�ü�������
								MAXxs[mylength] = i;
								MAXys[mylength] = j;
								mylength++;
							}
						}
					}
					else {
						//����ݹ鵽�����һ��
						if (thescore > MAXnumber)
						{
							//ֱ�Ӹ���
							MAXnumber = thescore;
						}
					}
				}
			}
		}
	}
	if (c == 0)
	{
		//��һ��
		//���������λ��
		int mynum = rand() % 19;
		if (mylength)
		{
			mynum = rand() % mylength;
		}
		return { MAXxs[mynum],MAXys[mynum],MAXnumber };
	}
	//������
	return { 0,0,MAXnumber };
}
*/

seat findbestseat(int color, int c)
{
	std::vector<seat> res;   // ���λ��
	int MAXnumber = -999;    // ��ѷ���
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			if (BOX[i][j].value == -1)
			{
				// ������������
				BOX[i][j].value = color;
				int thescore = (int)(getscore(color) * 1.2);
				BOX[i][j].value = !color;
				thescore += getscore(!color);
				BOX[i][j].value = -1;
				if (thescore < MAXnumber)
				{
					continue;
				}
				if (thescore > MAXnumber)
				{
					MAXnumber = thescore;
					// �ѵ�ǰλ�ü������λ��
					res.clear();
					res.push_back({ i,j,thescore });
				}
				else if (thescore == MAXnumber)
				{
					// �ѵ�ǰλ�ü������λ��
					res.push_back({ i,j,thescore });
				}
			}
		}
	}
	return res[0];
}

// �ж���Ӯ
void isWIN()
{
	bool isfull = true; // �����Ƿ�����
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			if (BOX[i][j].value != -1)
			{
				// ����ÿ�����ܵ�λ��
				int nowcolor = BOX[i][j].value; // ���ڱ���������ɫ
				int length[4] = { 0,0,0,0 };    // �ĸ�����ĳ���
				for (int k = 0; k < 4; k++)
				{
					// ԭ��ͬѰ�����λ��
					int nowi = i;
					int nowj = j;
					while (nowi <= 18 && nowj <= 18 && nowi >= 0 && nowj >= 0 && BOX[nowi][nowj].value == nowcolor)
					{
						length[k]++;
						nowj += dx[k];
						nowi += dy[k];
					}
					nowi = i;
					nowj = j;
					while (nowi <= 18 && nowj <= 18 && nowi >= 0 && nowj >= 0 && BOX[nowi][nowj].value == 1 - nowcolor)
					{
						length[k]++;
						nowj -= dx[k];
						nowi -= dy[k];
					}
				}
				for (int k = 0; k < 4; k++)
				{
					if (length[k] >= 5) {
						// ���������
						if (nowcolor == playercolor)
						{
							win = playercolor; // ���ʤ
						}
						if (nowcolor == 1 - playercolor)
						{
							win = 1 - playercolor; // ����ʤ
						}
					}
				}
			}
			else
			{
				//���Ϊ��
				isfull = false;//����û��
			}
		}
	}
	if (isfull)
	{
		// �����������
		win = 2; // ƽ��
	}
}

// ��Ϸ������
void game()
{
	bool isinit;

	// ��һ�����ͣ������
	int oldi = 0;
	int oldj = 0;

	if (!bOnline)
	{
		// ����������ɫ
		srand((UINT)time(NULL));
		playercolor = rand() % 2;
	}

	// ���Ʊ���
	setfillcolor(RGB(255, 205, 150));
	solidrectangle(40, 25, 645, 630);

	// ����������ʽ
	settextstyle(30, 15, 0, 0, 0, 1000, false, false, false);
	settextcolor(BLACK);

	// �����ʾ��

	// ����
	if (!bOnline)
	{
		if (playercolor == 0)
		{
			isinit = 1;
			outtextxy(150, 650, _T("���ִ�׺��У�����ִ������"));
			whoplay = 1;
		}
		else
		{
			isinit = 0;
			outtextxy(150, 650, _T("���ִ�����У�����ִ�׺���"));
			whoplay = 0;
		}
	}

	// ����
	else
	{
		// ��ս
		if (bWatch)
		{
			outtextxy(150, 650, L"�����ڹ�ս");
		}
		
		// �Կ�������ƻ���

		int out_x = 80, out_y = 710;
		int myname_w = textwidth(strPlayerName.c_str());
		int hisname_w = textwidth(strHisName.c_str());
		int radius = 15;

		outtextxy(out_x, out_y, strPlayerName.c_str());

		settextcolor(RED);
		outtextxy(out_x + myname_w + 2 * radius + 10, out_y, L" VS ");

		settextcolor(BLACK);
		outtextxy(out_x + myname_w + 2 * radius + 60, out_y, strHisName.c_str());

		setlinecolor(BLACK);
		setfillcolor(BLACK);
		if (server_getColor() == WHITE)
		{
			circle(out_x + myname_w + radius + 5, out_y + radius, radius);
			fillcircle(out_x + myname_w + 3 * radius + 60 + hisname_w + 5, out_y + radius, radius);
		}
		else
		{
			fillcircle(out_x + myname_w + radius + 5, out_y + radius, radius);
			circle(out_x + myname_w + 3 * radius + 60 + hisname_w + 5, out_y + radius, radius);
		}
	}

	draw(); // ����

	// �ǹ�սģʽ
	if (!bWatch)
	{
		while (1)
		{
		NEXTPLAYER:
			
			// ����
			if (bOnline)
			{
				wstring text;

				// �ҵĻغ�
				if (server_isMyRound())
				{
					text = L"����������";
				}
				else
				{
					text = L"���ȶԷ�����";
				}

				// �����ϴε�����
				setlinecolor(WHITE);
				setfillcolor(WHITE);
				fillrectangle(150, 650, 480, 700);

				if (playercolor == 0)
				{
					isinit = 1;
					outtextxy(150, 650, (L"��ִ����" + text).c_str());
					whoplay = 1;
				}
				else
				{
					isinit = 0;
					outtextxy(150, 650, (L"��ִ����" + text).c_str());
					whoplay = 0;
				}
			}
			
			// �ҵĻغ�
			if ((!bOnline && whoplay == 0) || (bOnline && server_isMyRound()))
			{
				// ѭ��ֱ���������һ�ӣ�������Դռ��
				while (true)
				{
					// �������
					ExMessage mouse = getmessage(EM_MOUSE); // ��ȡ�����Ϣ
					for (int i = 0; i < 19; i++)
					{
						for (int j = 0; j < 19; j++)
						{
							if (mouse.x > BOX[i][j].x && mouse.x<BOX[i][j].x + 30//�ж�x����
								&& mouse.y>BOX[i][j].y && mouse.y < BOX[i][j].y + 30//�ж�y����
								&& BOX[i][j].value == -1)//�ж��Ƿ��ǿ�λ��
							{
								// ���ͣ��ĳһ����λ������
								if (mouse.lbutton)
								{
									// ����ģʽ
									if (bOnline)
									{
										// ����ʧ��
										if (!server_set(j, i))
										{
											// ����
											bool ok = false;
											for (int k = 0; k < 3; k++)
											{
												Sleep(200);
												if (server_set(j, i))
												{
													ok = true;
													break;
												}
											}

											// ��Ȼ����ʧ�ܣ���Ҫ����
											if (!ok)
											{
												MessageBox(GetHWnd(), L"����ʧ�ܣ���������", L"", MB_OK | MB_ICONERROR);
												continue;
											}
										}
									}

									// ���������
									BOX[i][j].value = playercolor; // ����
									BOX[i][j].isnew = true;        // ��λ�ø���
									oldi = -1;
									oldj = -1;

									// ��һ�����
									whoplay = 1;
									goto DRAW;
								}

								// ����ѡ���
								BOX[oldi][oldj].isnew = false;
								BOX[oldi][oldj].draw();
								BOX[i][j].isnew = true;
								BOX[i][j].draw();
								oldi = i;
								oldj = j;
							}
						}
					}
				}
			}
			else
			{
				// ����ģʽ���ȴ��Է�����
				if (bOnline)
				{
					while (true)
					{
						// ����Ƿ��ֵ��Լ�
						// ��Ϊ�棬��˵���Է�������
						if (server_isMyRound())
						{
							// ��γ��Ի�ȡ���һ������
							CHESSMAN chess;
							bool ok = false;
							for (int i = 0; i < 3; i++)
							{
								chess = server_getLatestPoint();
								if (!(chess.x < 0 || chess.y < 0))
								{
									ok = true;
									break;
								}
								Sleep(200);
							}

							// �ж������Ƿ��쳣
							if (!ok)
							{
								MessageBox(GetHWnd(), L"�������쳣��������Ϣ����", L"", MB_OK | MB_ICONERROR);
								//exit(-1);
							}

							BOX[chess.y][chess.x].value = 1 - (bool)chess.color;
							BOX[chess.y][chess.x].isnew = true;
							BOX[chess.y][chess.x].draw();

							flushmessage();

							whoplay = 0;
							goto DRAW; // �ֵ���һ��

							break;
						}

						Sleep(300);
					}
				}
				else
				{
					// ��������
					if (isinit)
					{
						// �������
						isinit = 0;
						int drawi = 9;
						int drawj = 9;
						while (BOX[drawi][drawj].value != -1)
						{
							drawi--;
							drawj++;
						}
						BOX[drawi][drawj].value = 1 - playercolor;
						BOX[drawi][drawj].isnew = true;
					}
					else
					{
						seat best;
						best = findbestseat(1 - playercolor, 0); // Ѱ�����λ��
						BOX[best.i][best.j].value = 1 - playercolor;//�������λ��
						BOX[best.i][best.j].isnew = true;
					}
					whoplay = 0;
					goto DRAW; // �ֵ���һ��
				}

			}
		}
	}

	// ��սģʽ
	else
	{
		// ���ϻ�ȡ�����
		int last_x = -1, last_y = -1;

		// ��Ϸ��
		while (server_isGaming())
		{
			// ��ȡ������
			CHESSMAN chess = server_getLatestPoint();
			if (chess.x != last_x && chess.y != last_y)
			{
				BOX[chess.y][chess.x].value = 1 - (bool)chess.color;
				BOX[chess.y][chess.x].isnew = true;
				BOX[chess.y][chess.x].draw();

				if (last_x != -1)
				{
					BOX[last_y][last_x].isnew = false;
					BOX[last_y][last_x].draw();
				}

				last_x = chess.x;
				last_y = chess.y;
			}

			Sleep(500);
		}

		// ��Ϸ����

		// ��ʤ
		if (server_isWin() == 1)
		{
			win = 0;
		}
		// ��ʤ
		else
		{
			win = 1;
		}

		goto DRAW;
	}

DRAW: // ����

	if (!bWatch)
	{
		isWIN(); // �����Ӯ
	}

	draw();

	oldi = 0;
	oldj = 0;

	if (win == -1)
	{
		// ���û����ʤ��
		Sleep(500);
		goto NEXTPLAYER; // ǰ����һ�����
	}

	// ʤ������
	settextcolor(GREEN);
	setbkcolor(WHITE);
	setbkmode(OPAQUE);

	Sleep(500);

	switch (win)
	{
	case 0:
		outtextxy(320, 320, _T("��ʤ"));

		if (bOnline && !bWatch)
		{
			server_end(L"white");
		}
		
		break;
	case 1:
		outtextxy(320, 320, _T("��ʤ"));

		if (bOnline && !bWatch)
		{
			server_end(L"black");
		}

		break;
	case 2:
		outtextxy(320, 320, _T("ƽ��"));
		
		// ƽ��Ĭ�ϰ�ʤ����Ϊ������û��ƽ��ѡ��
		if (bOnline && !bWatch)
		{
			server_end(L"white");
		}

		break;
	}

	Sleep(500);

	settextcolor(BLACK);
	outtextxy(210, 400, L"�����������������");
	setbkmode(TRANSPARENT);

	Sleep(1000);
	flushmessage();
	getmessage(EM_KEY);

	return;
}
