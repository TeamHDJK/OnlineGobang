/////////////////////////////////////////////////////////
//
//	联机五子棋
//	main.cpp
// 
//	编译环境：Visual C++ 2019 | EasyX 20210730
//	原 作 者：陈可佳 <emil09_chen@126.com>
//	联机模组：huidong <huidong_mail@163.com>
// 
//	原版发布时间：2020-4-28
//	联机版发布时间：2021-8-30
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

// 服务器通讯相关
#include "server.h"

// 类定义

// 保存位置的类
class seat
{
public:
	int i = 0;      // y 坐标
	int j = 0;      // x 坐标
	int number = 0; // 分数
};

// 保存棋盘的类
class box
{
public:
	void draw();            // 绘制
public:
	int x = 0;              // x 坐标
	int y = 0;              // y 坐标
	int value = -1;         // 值（黑棋：1，白棋：0，空位：-1）
	int modle = 0;          // 模式
	bool isnew = false;     // 是否有选择框
	COLORREF color = WHITE; // 棋盘背景色
};


// 全局变量
box BOX[19][19];      // 棋盘
int win = -1;         // 谁赢了（0：白棋，1：黑棋，2：平局）
int whoplay = 0;      // 轮到谁下棋了
int playercolor = 0;  // 玩家颜色
int dx[4]{ 1,0,1,1 }; // - | \ / 四个方向
int dy[4]{ 0,1,1,-1 };
int Score[3][5] = //评分表
{
	{ 0, 80, 250, 500, 500 }, // 防守0子
	{ 0, 0,  80,  250, 500 }, // 防守1子
	{ 0, 0,  0,   80,  500 }  // 防守2子
};
int MAXxs[361];   //最优x坐标
int MAXys[361];   //最优y坐标
int mylength = 0; //最优解数
bool vis[19][19][4]; // 这个位置是否访问过

//// 联机相关

bool bOnline;	// 模式是否为联机
bool bWatch;	// 是否为旁观模式
wstring strPlayerName;	// 当前玩家名称
wstring strHisName;		// 另一玩家名称


// 函数声明
void draw();                  // 绘制
void init();                  // 初始化
seat findbestseat(int color, int c); // 寻找最佳位置
void isWIN();                 // 判断输赢
void game();                  // 游戏主函数


// 服务器设置对话框
void ServerSettingDialog()
{
	wchar_t strServer[1024] = { 0 };
	if (InputBox(strServer, 1024,
		L"自定义联机服务器域名\r\n（例如 \"http://www.game.com/onlinegobang/\"）\r\n注意：域名末尾需要带'/'",
		(LPCTSTR)0, (LPCTSTR)0, 480, 0, false))
	{
		server_setServer(strServer);
		MessageBox(GetHWnd(), L"设置成功。\r\n请确保您的服务器地址可用，接下来的联机数据都会发送到您设置的服务器。", L"", MB_OK | MB_ICONINFORMATION);
	}
}

// 游戏菜单
void Menu()
{
	settextcolor(BLACK);

	settextstyle(50,0,L"Consolas");
	outtextxy(20,20,L"Online Gobang");
	settextstyle(24, 0, L"system");
	outtextxy(100, 200, L"Press [A] 加入联机对战");
	outtextxy(100, 240, L"Press [B] 加入离线游戏");
	outtextxy(100, 280, L"Press [T] 进入联机设置");

	outtextxy(340, 48, L"lovely_ckj & huidong 五子棋联机对战版 (beta)");
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

// 获取当前在线游戏的棋盘，自动存储到当前棋盘中
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

// 在线匹配
bool OnlineMatch()
{
	// 满人
	if (server_isFull())
	{
		server_getUserName(strPlayerName, strHisName);
		server_setMyColor(WHITE);
		
		MessageBox(GetHWnd(), L"已经有玩家开始下棋了，您即将进入观战模式", L"", MB_OK | MB_ICONINFORMATION);
		
		// 加载他们的棋盘
		GetGamingMap();
		
		bWatch = true;
	}
	else
	{
		wchar_t name[128] = { 0 };
		InputBox(name, 128, L"输入您的昵称");
		strPlayerName = name;

		int player_num = server_getPlayerNum();

		// 加入游戏
		if (server_join(strPlayerName))
		{
			// 匹配
			cleardevice();
			outtextxy(50, 50, L"匹配其他玩家中……");

			int t = clock();
			while (true)
			{
				// 够人
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

					wstring str = L"匹配到在线玩家 " + strHisName + L"，按确定后将开始游戏";

					MessageBox(GetHWnd(), str.c_str(), L"", MB_OK | MB_ICONINFORMATION);
					
					if (!server_start())
					{
						MessageBox(GetHWnd(), L"开始游戏失败，请检查网络", L"", MB_OK | MB_ICONERROR);
					}
					
					bWatch = false;
					playercolor = 1 - (bool)server_getColor();

					break;
				}

				int wait = (clock() - t) / CLOCKS_PER_SEC;
				wchar_t buf[24] = { 0 };
				wsprintf(buf, L"已等待：%d s", wait);
				
				if (wait > 120)
				{
					MessageBox(GetHWnd(), L"匹配超时，将返回主界面。\r\n去呼叫你的伙伴一起来玩吧~", L"", MB_OK | MB_ICONINFORMATION);
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
			MessageBox(GetHWnd(), L"加入游戏失败，请重启游戏然后重试", L"", MB_OK);
			return false;
		}
	}

	return true;
}


// main 函数
int main()
{
	initgraph(700, 750); // 初始化绘图环境
	setbkcolor(WHITE);
	setbkmode(TRANSPARENT); // 设置透明文字输出背景

	while (1)
	{
		cleardevice();

		// 初始化
		init();

		// 游戏菜单
		Menu();

		// 匹配
		if (bOnline)
		{
			if (!OnlineMatch())
			{
				continue;
			}
		}

		// 完成一局游戏，然后重新开始
		game();
	}
}


// 类函数定义

// 绘制函数
void box::draw()
{
	COLORREF thefillcolor = getfillcolor(); // 备份填充颜色
	setlinestyle(PS_SOLID, 2);              // 线样式设置
	setfillcolor(color);                    // 填充颜色设置
	solidrectangle(x, y, x + 30, y + 30);   // 绘制无边框的正方形
	if (isnew)
	{
		// 如果是新下的
		// 绘制边框线
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
		// 以下是不同位置棋盘的样式
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
	case 0: // 白棋
		setfillcolor(WHITE);
		setlinestyle(PS_SOLID, 1);
		fillcircle(x + 15, y + 15, 13);
		break;
	case 1: // 黑棋
		setfillcolor(BLACK);
		setlinestyle(PS_SOLID, 1);
		fillcircle(x + 15, y + 15, 13);
		break;
	}
	setfillcolor(thefillcolor); // 还原填充色
}



// 其他函数定义

// 绘制棋盘
void draw()
{
	int number = 0; // 坐标输出的位置
	// 坐标（数值）
	TCHAR strnum[19][3] = { _T("1"),_T("2") ,_T("3") ,_T("4"),_T("5") ,_T("6") ,_T("7"),_T("8"),_T("9"),_T("10"), _T("11"),_T("12") ,_T("13") ,_T("14"),_T("15") ,_T("16") ,_T("17"),_T("18"),_T("19") };
	// 坐标（字母）
	TCHAR strabc[19][3] = { _T("A"),_T("B") ,_T("C") ,_T("D"),_T("E") ,_T("F") ,_T("G"),_T("H"),_T("I"),_T("J"), _T("K"),_T("L") ,_T("M") ,_T("N"),_T("O") ,_T("P") ,_T("Q"),_T("R"),_T("S") };
	LOGFONT nowstyle;
	gettextstyle(&nowstyle);
	settextstyle(0, 0, NULL);
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			BOX[i][j].draw(); // 绘制
			if (BOX[i][j].isnew == true)
			{
				BOX[i][j].isnew = false; // 把上一个下棋位置的黑框清除
			}
		}
	}
	// 画坐标
	for (int i = 0; i < 19; i++)
	{
		outtextxy(75 + number, 35, strnum[i]);
		outtextxy(53, 55 + number, strabc[i]);
		number += 30;
	}
	settextstyle(&nowstyle);
}

// 对局初始化
void init()
{
	win = -1;// 谁赢了
	for (int i = 0, k = 0; i < 570; i += 30)
	{
		for (int j = 0, g = 0; j < 570; j += 30)
		{
			int modle = 0;  // 棋盘样式
			BOX[k][g].value = -1;
			BOX[k][g].color = RGB(255, 205, 150);// 棋盘底色
			// x、y 坐标
			BOX[k][g].x = 65 + j;
			BOX[k][g].y = 50 + i;
			// 棋盘样式的判断
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



// 核心函数

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

// 寻找最佳位置
/*
seat findbestseat(int color, int c)
{
	if (c == 0)
	{
		//如果是第一层
		//清空数组
		mylength = 0;
	}
	int MAXnumber = -1;    //最佳分数
	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			if (BOX[i][j].value == -1) {
				//遍历每一个空位置
				int length;        //当前方向长度
				int emeny;         //当前方向敌子
				int nowi = 0;      //现在遍历到的y坐标
				int nowj = 0;      //现在遍历到的x坐标
				int thescore = 0;  //这个位置的初始分数
				//判断周边有没有棋子
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
					//如果周围没有棋子，就不用递归了
					continue;
				}
				//自己
				BOX[i][j].value = color;//尝试下在这里
				for (int k = 0; k < 4; k++)
				{
					//检测四个方向
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
					length -= 2;//判断长度
					if (length > 4)
					{
						length = 4;
					}
					if (Score[emeny][length] == 500)
					{
						//己方胜利，结束递归
						BOX[i][j].value = -1;
						return{ i,j,Score[emeny][length] };
					}
					thescore += Score[emeny][length];
					length = 0;
					emeny = 0;
				}
				//敌人（原理同上）
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
				//如果已经比最高分数小，就没必要递归了
				if (thescore >= MAXnumber)
				{
					if (c < 3)
					{
						//只能找4层，否则时间太长
						BOX[i][j].value = color;
						//递归寻找对方分数
						int nowScore = thescore - findbestseat(!color, c + 1).number;//递归求出这个位置的分值
						BOX[i][j].value = -1;
						if (nowScore > MAXnumber)
						{
							//比最高分值大
							MAXnumber = nowScore;
							if (c == 0)
							{
								//第一层
								mylength = 0;//清空数组
							}
						}
						if (c == 0)
						{
							//第一层
							if (nowScore >= MAXnumber)
							{
								//把当前位置加入数组
								MAXxs[mylength] = i;
								MAXys[mylength] = j;
								mylength++;
							}
						}
					}
					else {
						//如果递归到了最后一层
						if (thescore > MAXnumber)
						{
							//直接更新
							MAXnumber = thescore;
						}
					}
				}
			}
		}
	}
	if (c == 0)
	{
		//第一层
		//随机化落子位置
		int mynum = rand() % 19;
		if (mylength)
		{
			mynum = rand() % mylength;
		}
		return { MAXxs[mynum],MAXys[mynum],MAXnumber };
	}
	//其他层
	return { 0,0,MAXnumber };
}
*/

seat findbestseat(int color, int c)
{
	std::vector<seat> res;   // 最佳位置
	int MAXnumber = -999;    // 最佳分数
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			if (BOX[i][j].value == -1)
			{
				// 尝试下在这里
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
					// 把当前位置加入最佳位置
					res.clear();
					res.push_back({ i,j,thescore });
				}
				else if (thescore == MAXnumber)
				{
					// 把当前位置加入最佳位置
					res.push_back({ i,j,thescore });
				}
			}
		}
	}
	return res[0];
}

// 判断输赢
void isWIN()
{
	bool isfull = true; // 棋盘是否满了
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			if (BOX[i][j].value != -1)
			{
				// 遍历每个可能的位置
				int nowcolor = BOX[i][j].value; // 现在遍历到的颜色
				int length[4] = { 0,0,0,0 };    // 四个方向的长度
				for (int k = 0; k < 4; k++)
				{
					// 原理同寻找最佳位置
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
						// 如果满五子
						if (nowcolor == playercolor)
						{
							win = playercolor; // 玩家胜
						}
						if (nowcolor == 1 - playercolor)
						{
							win = 1 - playercolor; // 电脑胜
						}
					}
				}
			}
			else
			{
				//如果为空
				isfull = false;//棋盘没满
			}
		}
	}
	if (isfull)
	{
		// 如果棋盘满了
		win = 2; // 平局
	}
}

// 游戏主函数
void game()
{
	bool isinit;

	// 上一个鼠标停的坐标
	int oldi = 0;
	int oldj = 0;

	if (!bOnline)
	{
		// 随机化玩家颜色
		srand((UINT)time(NULL));
		playercolor = rand() % 2;
	}

	// 绘制背景
	setfillcolor(RGB(255, 205, 150));
	solidrectangle(40, 25, 645, 630);

	// 设置字体样式
	settextstyle(30, 15, 0, 0, 0, 1000, false, false, false);
	settextcolor(BLACK);

	// 输出标示语

	// 离线
	if (!bOnline)
	{
		if (playercolor == 0)
		{
			isinit = 1;
			outtextxy(150, 650, _T("玩家执白后行，电脑执黑先行"));
			whoplay = 1;
		}
		else
		{
			isinit = 0;
			outtextxy(150, 650, _T("玩家执黑先行，电脑执白后行"));
			whoplay = 0;
		}
	}

	// 在线
	else
	{
		// 观战
		if (bWatch)
		{
			outtextxy(150, 650, L"您正在观战");
		}
		
		// 对抗玩家名称绘制

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

	draw(); // 绘制

	// 非观战模式
	if (!bWatch)
	{
		while (1)
		{
		NEXTPLAYER:
			
			// 在线
			if (bOnline)
			{
				wstring text;

				// 我的回合
				if (server_isMyRound())
				{
					text = L"，请您落子";
				}
				else
				{
					text = L"，等对方落子";
				}

				// 擦除上次的文字
				setlinecolor(WHITE);
				setfillcolor(WHITE);
				fillrectangle(150, 650, 480, 700);

				if (playercolor == 0)
				{
					isinit = 1;
					outtextxy(150, 650, (L"您执白棋" + text).c_str());
					whoplay = 1;
				}
				else
				{
					isinit = 0;
					outtextxy(150, 650, (L"您执黑棋" + text).c_str());
					whoplay = 0;
				}
			}
			
			// 我的回合
			if ((!bOnline && whoplay == 0) || (bOnline && server_isMyRound()))
			{
				// 循环直到玩家落下一子，减少资源占用
				while (true)
				{
					// 玩家下棋
					ExMessage mouse = getmessage(EM_MOUSE); // 获取鼠标信息
					for (int i = 0; i < 19; i++)
					{
						for (int j = 0; j < 19; j++)
						{
							if (mouse.x > BOX[i][j].x && mouse.x<BOX[i][j].x + 30//判断x坐标
								&& mouse.y>BOX[i][j].y && mouse.y < BOX[i][j].y + 30//判断y坐标
								&& BOX[i][j].value == -1)//判断是否是空位置
							{
								// 如果停在某一个空位置上面
								if (mouse.lbutton)
								{
									// 在线模式
									if (bOnline)
									{
										// 下棋失败
										if (!server_set(j, i))
										{
											// 重试
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

											// 仍然下棋失败，需要重试
											if (!ok)
											{
												MessageBox(GetHWnd(), L"下棋失败，请检查网络", L"", MB_OK | MB_ICONERROR);
												continue;
											}
										}
									}

									// 如果按下了
									BOX[i][j].value = playercolor; // 下棋
									BOX[i][j].isnew = true;        // 新位置更新
									oldi = -1;
									oldj = -1;

									// 下一个玩家
									whoplay = 1;
									goto DRAW;
								}

								// 更新选择框
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
				// 在线模式，等待对方下棋
				if (bOnline)
				{
					while (true)
					{
						// 检查是否轮到自己
						// 若为真，则说明对方已下子
						if (server_isMyRound())
						{
							// 多次尝试获取最后一个落子
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

							// 判断棋子是否异常
							if (!ok)
							{
								MessageBox(GetHWnd(), L"服务器异常，棋子信息有误。", L"", MB_OK | MB_ICONERROR);
								//exit(-1);
							}

							BOX[chess.y][chess.x].value = 1 - (bool)chess.color;
							BOX[chess.y][chess.x].isnew = true;
							BOX[chess.y][chess.x].draw();

							flushmessage();

							whoplay = 0;
							goto DRAW; // 轮到下一个

							break;
						}

						Sleep(300);
					}
				}
				else
				{
					// 电脑下棋
					if (isinit)
					{
						// 开局情况
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
						best = findbestseat(1 - playercolor, 0); // 寻找最佳位置
						BOX[best.i][best.j].value = 1 - playercolor;//下在最佳位置
						BOX[best.i][best.j].isnew = true;
					}
					whoplay = 0;
					goto DRAW; // 轮到下一个
				}

			}
		}
	}

	// 观战模式
	else
	{
		// 不断获取新棋况
		int last_x = -1, last_y = -1;

		// 游戏中
		while (server_isGaming())
		{
			// 获取新落子
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

		// 游戏结束

		// 白胜
		if (server_isWin() == 1)
		{
			win = 0;
		}
		// 黑胜
		else
		{
			win = 1;
		}

		goto DRAW;
	}

DRAW: // 绘制

	if (!bWatch)
	{
		isWIN(); // 检测输赢
	}

	draw();

	oldi = 0;
	oldj = 0;

	if (win == -1)
	{
		// 如果没有人胜利
		Sleep(500);
		goto NEXTPLAYER; // 前往下一个玩家
	}

	// 胜利处理
	settextcolor(GREEN);
	setbkcolor(WHITE);
	setbkmode(OPAQUE);

	Sleep(500);

	switch (win)
	{
	case 0:
		outtextxy(320, 320, _T("白胜"));

		if (bOnline && !bWatch)
		{
			server_end(L"white");
		}
		
		break;
	case 1:
		outtextxy(320, 320, _T("黑胜"));

		if (bOnline && !bWatch)
		{
			server_end(L"black");
		}

		break;
	case 2:
		outtextxy(320, 320, _T("平局"));
		
		// 平局默认白胜，因为服务器没有平局选项
		if (bOnline && !bWatch)
		{
			server_end(L"white");
		}

		break;
	}

	Sleep(500);

	settextcolor(BLACK);
	outtextxy(210, 400, L"按任意键返回主界面");
	setbkmode(TRANSPARENT);

	Sleep(1000);
	flushmessage();
	getmessage(EM_KEY);

	return;
}
