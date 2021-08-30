////////////////////////////////////
//
//	server.h
//	服务器通讯相关
//
//	huidong 2021-8-29
//


#pragma once

#include <string>
using namespace std;

#include <windows.h>
#include <wininet.h>
#include <comutil.h>

#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "wininet.lib")

// 服务器域名
//wstring g_strServerName = L"http://localhost/onlinegobang_webserver/";
wstring g_strServerName = L"http://web.huidongweb.tk/OnlineGobang_WebServer/";

// 玩家颜色
COLORREF g_color;

// 棋子
struct CHESSMAN
{
	int x;
	int y;
	COLORREF color;
};

// 棋子数组
struct CHESSMEN
{
	CHESSMAN* chess;	// 棋子
	int num;	// 数量
};

string wtos(const wstring& ws)
{
	_bstr_t t = ws.c_str();
	char* pchar = (char*)t;
	string result = pchar;
	return result;
}

wstring stow(const string& s)
{
	_bstr_t t = s.c_str();
	wchar_t* pwchar = (wchar_t*)t;
	wstring result = pwchar;
	return result;
}


/**
 * @brief        UTF-8 编码字符串转 GBK 编码字符串
 * @param[in]    lpUTF8Str: 原 utf-8 字符串
 * @param[out]    lpGBKStr: 转码后的 gbk 字符串
 * @param[in]    nGBKStrLen: gbk 字符串的最大长度
 * @return        返回转换后字符串的长度
 * @note        代码来自 https://www.cnblogs.com/zhongbin/p/3160641.html
*/
int UTF8ToGBK(char* lpUTF8Str, char* lpGBKStr, int nGBKStrLen)
{
	wchar_t* lpUnicodeStr = NULL;
	int nRetLen = 0;
	if (!lpUTF8Str) return 0;
	nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, (char*)lpUTF8Str, -1, NULL, NULL);
	lpUnicodeStr = new WCHAR[nRetLen + 1];
	nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, (char*)lpUTF8Str, -1, lpUnicodeStr, nRetLen);
	if (!nRetLen) return 0;
	nRetLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, NULL, NULL, NULL, NULL);
	if (!lpGBKStr)
	{
		if (lpUnicodeStr) delete[] lpUnicodeStr;
		return nRetLen;
	}
	if (nGBKStrLen < nRetLen)
	{
		if (lpUnicodeStr) delete[] lpUnicodeStr;
		return 0;
	}
	nRetLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, (char*)lpGBKStr, nRetLen, NULL, NULL);
	if (lpUnicodeStr) delete[] lpUnicodeStr;
	return nRetLen;
}

/**
 * @brief    判断一个字符串是否为 UTF-8 编码
 * @note    来自 https://blog.csdn.net/jiankekejian/article/details/106720432 （有删改）
*/
bool isUTF8(const char* str)
{
	int length = strlen(str);
	int check_sub = 0;
	int i = 0;
	int j = 0;

	for (i = 0; i < length; i++)
	{
		if (check_sub == 0)
		{
			if ((str[i] >> 7) == 0)
			{
				continue;
			}
			struct
			{
				int cal;
				int cmp;
			} Utf8NumMap[] = { {0xE0,0xC0},{0xF0,0xE0},{0xF8,0xF0},{0xFC,0xF8},{0xFE,0xFC}, };
			for (j = 0; j < (sizeof(Utf8NumMap) / sizeof(Utf8NumMap[0])); j++)
			{
				if ((str[i] & Utf8NumMap[j].cal) == Utf8NumMap[j].cmp)
				{
					check_sub = j + 1;
					break;
				}
			}
			if (0 == check_sub)
			{
				return false;
			}
		}
		else
		{
			if ((str[i] & 0xC0) != 0x80)
			{
				return false;
			}
			check_sub--;
		}
	}
	return true;
}

/**
 * @brief        获取网页源码
 * @param[in]    Url 网页链接
 * @return        返回网页源码
 * @note    代码来自 https://www.cnblogs.com/croot/p/3391003.html （有删改）
*/
string GetWebSrcCode(LPCTSTR Url)
{
	string strHTML;
	HINTERNET hSession = InternetOpen(L"IE6.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hSession != NULL)
	{
		HINTERNET hURL = InternetOpenUrl(hSession, Url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
		if (hURL != NULL)
		{
			const int nBlockSize = 1024;
			char Temp[nBlockSize] = { 0 };
			ULONG Number = 1;

			while (Number > 0)
			{
				InternetReadFile(hURL, Temp, nBlockSize - 1, &Number);
				for (int i = 0; i < (int)Number; i++)
					strHTML += Temp[i];
			}

			InternetCloseHandle(hURL);
			hURL = NULL;
		}

		InternetCloseHandle(hSession);
		hSession = NULL;
	}

	if (isUTF8(strHTML.c_str()))
	{
		string strGBK;
		strGBK.resize(strHTML.size() * 2);
		UTF8ToGBK(&strHTML[0], &strGBK[0], strHTML.size() * 2);
		strGBK.resize(strlen(strGBK.c_str()));	// 删除多余 \0
		return strGBK;
	}
	else
	{
		return strHTML;
	}
}

// 得到一个字符串内容对应的布尔值
bool GetStringBool(string str)
{
	if (str == "true")
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 获取当前玩家数量
int server_getPlayerNum()
{
	string res = GetWebSrcCode((g_strServerName + L"?mode=getusernum").c_str());
	return atoi(res.c_str());
}

// 询问服务器是否已经满员
bool server_isFull()
{
	if (server_getPlayerNum() == 2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 加入服务器
bool server_join(wstring name)
{
	// 如果自己是第二个加入者，则执黑棋
	if (server_getPlayerNum())
	{
		g_color = BLACK;
	}
	// 否则执白棋
	else
	{
		g_color = WHITE;
	}

	string res = GetWebSrcCode((g_strServerName + L"?mode=join&name=" + name).c_str());
	return GetStringBool(res);
}

// 获取自己的颜色
COLORREF server_getColor()
{
	return g_color;
}

// 设置自己的颜色
// 此操作仅在观战模式下有效，否则会出现 bug
void server_setMyColor(COLORREF c)
{
	g_color = c;
}

// 开始游戏
bool server_start()
{
	string res = GetWebSrcCode((g_strServerName + L"?mode=start").c_str());
	return GetStringBool(res);
}

// 下棋
bool server_set(int x, int y)
{
	char strX[3] = { 0 }, strY[3] = { 0 };
	_itoa_s(x, strX, 3, 10);
	_itoa_s(y, strY, 3, 10);

	wstring flag = L"white";
	if (g_color == BLACK)
	{
		flag = L"black";
	}

	string res = GetWebSrcCode((g_strServerName + L"?mode=set&x=" + stow(strX) + L"&y=" + stow(strY) + L"&flag=" + flag).c_str());
	return GetStringBool(res);
}

// 当前是否为我的回合
bool server_isMyRound()
{
	string res = GetWebSrcCode((g_strServerName + L"?mode=getround").c_str());
	if ((res == "white" && g_color == WHITE) || (res == "black" && g_color == BLACK))
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 根据棋盘信息字符串获取棋子信息
CHESSMEN server_getPointsByString(string strChessMap)
{
	if (strChessMap.size() < 6)
	{
		return { NULL,0 };
	}

	// 当前棋子
	CHESSMEN chess_array = { new CHESSMAN[19 * 19],0 };

	// 当前读取的键的索引
	int info_index = 0;

	// 键值
	string strInfo[3];

	// 遍历棋盘
	for (int i = 0; i < (int)strChessMap.size(); i++)
	{
		// 信息分隔符
		if (strChessMap[i] == ' ')
		{
			// 切换到下一键的读取
			info_index++;

			// 已经读满三个键
			if (info_index >= 3)
			{
				// 将键值存储到棋子信息
				int x = atoi(strInfo[0].c_str());
				int y = atoi(strInfo[1].c_str());
				int flag = atoi(strInfo[2].c_str());
				COLORREF c = (flag == 0) ? BLACK : WHITE;

				chess_array.chess[chess_array.num] = { x,y,c };
				chess_array.num++;

				// 键索引归零
				info_index = 0;

				// 键归零
				for (int j = 0; j < 3; j++)
				{
					strInfo[j] = "";
				}
			}

			continue;
		}

		// 读取键
		strInfo[info_index] += strChessMap[i];
	}

	return chess_array;
}

// 获取当前完整棋盘
CHESSMEN server_getChessMap()
{
	string res = GetWebSrcCode((g_strServerName + L"?mode=getmap").c_str());
	return server_getPointsByString(res);
}

// 获取最新落子
// 如获取失败，则返回的棋子坐标为负数
CHESSMAN server_getLatestPoint()
{
	// 这一段代码专门用于获取最后一个棋子，已废弃
	/*string res = GetWebSrcCode((g_strServerName + L"?mode=getmap").c_str());
	
	if (res.size() <= 4)
	{
		return { -1,-1,0 };
	}

	// 从后往前找到第四个空格，标记位置为 index
	int index, mark = 0;
	for (index = res.size() - 1; index >= 0; index--)
	{
		if (res[index] == ' ' || index == 0)
		{
			mark++;
		}
		if (mark == 4)
		{
			if (index != 0)
			{
				index++;
			}
			break;
		}
	}

	// 从 index 往后找，以空格为界区分出三个字符串
	string str[3];
	int array_index = 0;
	for (int i = index; i < (int)res.size() - 1; i++)
	{
		if (res[i] == ' ')
		{
			array_index++;
			if (array_index >= 3)
			{
				break;
			}
			continue;
		}

		str[array_index] += res[i];
	}

	// 转整形
	int x = atoi(str[0].c_str());
	int y = atoi(str[1].c_str());
	int flag = atoi(str[2].c_str());

	COLORREF c = WHITE;
	if (flag == 0)
	{
		c = BLACK;
	}

	return { x,y,c };*/

	// 获取全棋盘
	CHESSMEN chess_map = server_getChessMap();

	if (chess_map.num <= 0)
	{
		return { -1,-1,0 };
	}

	// 获取最后一个落子
	CHESSMAN chess = chess_map.chess[chess_map.num - 1];
	delete[] chess_map.chess;
	
	return chess;
}

// 获取双方玩家名称
void server_getUserName(wstring& me, wstring& him)
{
	wstring res = stow(GetWebSrcCode((g_strServerName + L"?mode=getusername").c_str()));
	for (int i = 0; i < (int)res.size(); i++)
	{
		me += res[i];
		if (res[i] == '\n')
		{
			for (int j = i + 1; j < (int)res.size(); j++)
			{
				him += res[j];
			}
			break;
		}
	}

	// 删除最后的 '\n'
	me.pop_back();
	him.pop_back();
}

// 判断游戏是否进行中
bool server_isGaming()
{
	string res = GetWebSrcCode((g_strServerName + L"?mode=getstate").c_str());
	return res == "gaming";
}

// 判断是否胜利
// 0 表示还未出胜负
// 1 表示白胜
// 2 表示黑胜
int server_isWin()
{
	string res = GetWebSrcCode((g_strServerName + L"?mode=getstate").c_str());
	if (res == "white_win")
	{
		return 1;
	}
	else if (res == "black_win")
	{
		return 2;
	}
	else
	{
		return 0;
	}
}

// 结束游戏
bool server_end(wstring winner)
{
	string res = GetWebSrcCode((g_strServerName + L"?mode=over&winner=" + winner).c_str());
	return GetStringBool(res);
}

// 重置游戏状态
bool server_reset()
{
	string res = GetWebSrcCode((g_strServerName + L"?mode=reset").c_str());
	return GetStringBool(res);
}

// 设置服务器域名
void server_setServer(wstring strServer)
{
	g_strServerName = strServer;
}


