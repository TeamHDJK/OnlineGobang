////////////////////////////////////
//
//	server.h
//	������ͨѶ���
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

// ����������
//wstring g_strServerName = L"http://localhost/onlinegobang_webserver/";
wstring g_strServerName = L"http://web.huidongweb.tk/OnlineGobang_WebServer/";

// �����ɫ
COLORREF g_color;

// ����
struct CHESSMAN
{
	int x;
	int y;
	COLORREF color;
};

// ��������
struct CHESSMEN
{
	CHESSMAN* chess;	// ����
	int num;	// ����
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
 * @brief        UTF-8 �����ַ���ת GBK �����ַ���
 * @param[in]    lpUTF8Str: ԭ utf-8 �ַ���
 * @param[out]    lpGBKStr: ת���� gbk �ַ���
 * @param[in]    nGBKStrLen: gbk �ַ�������󳤶�
 * @return        ����ת�����ַ����ĳ���
 * @note        �������� https://www.cnblogs.com/zhongbin/p/3160641.html
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
 * @brief    �ж�һ���ַ����Ƿ�Ϊ UTF-8 ����
 * @note    ���� https://blog.csdn.net/jiankekejian/article/details/106720432 ����ɾ�ģ�
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
 * @brief        ��ȡ��ҳԴ��
 * @param[in]    Url ��ҳ����
 * @return        ������ҳԴ��
 * @note    �������� https://www.cnblogs.com/croot/p/3391003.html ����ɾ�ģ�
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
		strGBK.resize(strlen(strGBK.c_str()));	// ɾ������ \0
		return strGBK;
	}
	else
	{
		return strHTML;
	}
}

// �õ�һ���ַ������ݶ�Ӧ�Ĳ���ֵ
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

// ��ȡ��ǰ�������
int server_getPlayerNum()
{
	string res = GetWebSrcCode((g_strServerName + L"?mode=getusernum").c_str());
	return atoi(res.c_str());
}

// ѯ�ʷ������Ƿ��Ѿ���Ա
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

// ���������
bool server_join(wstring name)
{
	// ����Լ��ǵڶ��������ߣ���ִ����
	if (server_getPlayerNum())
	{
		g_color = BLACK;
	}
	// ����ִ����
	else
	{
		g_color = WHITE;
	}

	string res = GetWebSrcCode((g_strServerName + L"?mode=join&name=" + name).c_str());
	return GetStringBool(res);
}

// ��ȡ�Լ�����ɫ
COLORREF server_getColor()
{
	return g_color;
}

// �����Լ�����ɫ
// �˲������ڹ�սģʽ����Ч���������� bug
void server_setMyColor(COLORREF c)
{
	g_color = c;
}

// ��ʼ��Ϸ
bool server_start()
{
	string res = GetWebSrcCode((g_strServerName + L"?mode=start").c_str());
	return GetStringBool(res);
}

// ����
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

// ��ǰ�Ƿ�Ϊ�ҵĻغ�
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

// ����������Ϣ�ַ�����ȡ������Ϣ
CHESSMEN server_getPointsByString(string strChessMap)
{
	if (strChessMap.size() < 6)
	{
		return { NULL,0 };
	}

	// ��ǰ����
	CHESSMEN chess_array = { new CHESSMAN[19 * 19],0 };

	// ��ǰ��ȡ�ļ�������
	int info_index = 0;

	// ��ֵ
	string strInfo[3];

	// ��������
	for (int i = 0; i < (int)strChessMap.size(); i++)
	{
		// ��Ϣ�ָ���
		if (strChessMap[i] == ' ')
		{
			// �л�����һ���Ķ�ȡ
			info_index++;

			// �Ѿ�����������
			if (info_index >= 3)
			{
				// ����ֵ�洢��������Ϣ
				int x = atoi(strInfo[0].c_str());
				int y = atoi(strInfo[1].c_str());
				int flag = atoi(strInfo[2].c_str());
				COLORREF c = (flag == 0) ? BLACK : WHITE;

				chess_array.chess[chess_array.num] = { x,y,c };
				chess_array.num++;

				// ����������
				info_index = 0;

				// ������
				for (int j = 0; j < 3; j++)
				{
					strInfo[j] = "";
				}
			}

			continue;
		}

		// ��ȡ��
		strInfo[info_index] += strChessMap[i];
	}

	return chess_array;
}

// ��ȡ��ǰ��������
CHESSMEN server_getChessMap()
{
	string res = GetWebSrcCode((g_strServerName + L"?mode=getmap").c_str());
	return server_getPointsByString(res);
}

// ��ȡ��������
// ���ȡʧ�ܣ��򷵻ص���������Ϊ����
CHESSMAN server_getLatestPoint()
{
	// ��һ�δ���ר�����ڻ�ȡ���һ�����ӣ��ѷ���
	/*string res = GetWebSrcCode((g_strServerName + L"?mode=getmap").c_str());
	
	if (res.size() <= 4)
	{
		return { -1,-1,0 };
	}

	// �Ӻ���ǰ�ҵ����ĸ��ո񣬱��λ��Ϊ index
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

	// �� index �����ң��Կո�Ϊ�����ֳ������ַ���
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

	// ת����
	int x = atoi(str[0].c_str());
	int y = atoi(str[1].c_str());
	int flag = atoi(str[2].c_str());

	COLORREF c = WHITE;
	if (flag == 0)
	{
		c = BLACK;
	}

	return { x,y,c };*/

	// ��ȡȫ����
	CHESSMEN chess_map = server_getChessMap();

	if (chess_map.num <= 0)
	{
		return { -1,-1,0 };
	}

	// ��ȡ���һ������
	CHESSMAN chess = chess_map.chess[chess_map.num - 1];
	delete[] chess_map.chess;
	
	return chess;
}

// ��ȡ˫���������
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

	// ɾ������ '\n'
	me.pop_back();
	him.pop_back();
}

// �ж���Ϸ�Ƿ������
bool server_isGaming()
{
	string res = GetWebSrcCode((g_strServerName + L"?mode=getstate").c_str());
	return res == "gaming";
}

// �ж��Ƿ�ʤ��
// 0 ��ʾ��δ��ʤ��
// 1 ��ʾ��ʤ
// 2 ��ʾ��ʤ
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

// ������Ϸ
bool server_end(wstring winner)
{
	string res = GetWebSrcCode((g_strServerName + L"?mode=over&winner=" + winner).c_str());
	return GetStringBool(res);
}

// ������Ϸ״̬
bool server_reset()
{
	string res = GetWebSrcCode((g_strServerName + L"?mode=reset").c_str());
	return GetStringBool(res);
}

// ���÷���������
void server_setServer(wstring strServer)
{
	g_strServerName = strServer;
}


