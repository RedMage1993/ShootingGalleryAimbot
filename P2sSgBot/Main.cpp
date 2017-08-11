#define ColorNoAlpha(p) ((p) & 0x00FFFFFF) // Only RGB
#define HIBIT(b) ((0x80 << ((sizeof(b) - 1) * 0x08)) & b)

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <Windows.h>

using namespace std;

typedef struct _BITMAPCAPTURE {
    HBITMAP hbm;
    LPDWORD pixels;
    INT     width;
    INT     height;
} BITMAPCAPTURE;

BOOL CaptureScreen(BITMAPCAPTURE *bmpCapture);
int RandomNum(int min, int max);
void MoveCursor(int x, int y);

int main()
{
	POINT pt;
	BITMAPCAPTURE grab;
	bool bLeftFound = false;
	bool bTopFound = false;
	int nWhiteCount = 0;
	bool bDone = false;
	POINT ptHor;
	POINT ptVer;

	SetConsoleTitle(TEXT("P2S Shooting Gallery Bot by RedMage1993"));

	srand(static_cast<UINT> (time(0)));

	do
	{
		cout << "Enter randomization horizontally (0 14): ";
		cin >> ptHor.x >> ptHor.y;
		cout << endl;

		cout << "Enter randomization vertically (20 50): ";
		cin >> ptVer.x >> ptVer.y;
		cout << endl;

		do
		{
			cout << "Place cursor over top, left-most, red color. Press Ctrl when ready, Escape to\nstop the bot, Escape + Alt to close.";
			while (!HIBIT(GetAsyncKeyState(VK_CONTROL)))
				Sleep(30);

			GetCursorPos(&pt);
		} while (pt.x == 0 || pt.y == 0);

		cout << endl;

		cout << "\nScanning for FFFFFFh...";

		if (!CaptureScreen(&grab))
			return 0;

		do
		{
			if (ColorNoAlpha(grab.pixels[grab.width * pt.y + pt.x]) != 0xFFFFFF)
				pt.x--;
			else
			{
				bLeftFound = true;
				continue;
			}

			if (pt.x == -1)
			{
				cout << "Could not continue x scan!";
				cin.get();

				return 0;
			}
		} while (!bLeftFound);
		bLeftFound = false;

		pt.x++; // Stay on red

		cout << "\n\nLeft found!";

		do
		{
			if (ColorNoAlpha(grab.pixels[grab.width * pt.y + pt.x]) != 0xFFFFFF)
				pt.y--;
			else
			{
				bTopFound = true;
				continue;
			}

			if (pt.y == -1)
			{
				cout << "Could not continue y scan!";
				cin.get();

				return 0;
			}
		} while (!bTopFound);
		bTopFound = false;

		cout << "\nTop Found!";

		pt.y++; // Stay on red

		MoveCursor(pt.x + 320, pt.y + 180);
		mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

		DeleteObject(grab.hbm);

		do
		{
			if (!CaptureScreen(&grab))
				continue;

			// Check if game is still there before doing anything.
			if (ColorNoAlpha(grab.pixels[grab.width * pt.y + pt.x]) != 0x9D0000)
			{
				bDone = true;
				break;
			}

			// Check for reload.
			if (ColorNoAlpha(grab.pixels[grab.width * (pt.y + 35) + pt.x + 45]) == 0xFFFFFF)
			{
				MoveCursor(pt.x + 320, pt.y + 285);
				mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			}

			for (int i = 0; i < 640; i++)
			{
				if (ColorNoAlpha(grab.pixels[grab.width * (pt.y + 129) + pt.x + i]) == 0xFFFFFF)
				{
					nWhiteCount++;
					if (nWhiteCount == 14)
					{
						MoveCursor(pt.x + i - 14 + RandomNum(ptHor.x, ptHor.y), pt.y + 129 + RandomNum(ptVer.x, ptVer.y));
						mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

						nWhiteCount = 0;
					}
				}
				else
					nWhiteCount = 0;
			}

			DeleteObject(grab.hbm);

			if (bDone)
				break;

			Sleep(33);
		} while (!HIBIT(GetAsyncKeyState(VK_ESCAPE)));

		bDone = false;
		cout << endl << endl;
	} while (!HIBIT(GetAsyncKeyState(VK_ESCAPE))  && !HIBIT(GetAsyncKeyState(VK_MENU)));

	return 0;
}

BOOL CaptureScreen(BITMAPCAPTURE *bmpCapture) // by Napalm
{
    BOOL bResult = FALSE;
    if(!bmpCapture)
        return bResult;
     
    ZeroMemory(bmpCapture, sizeof(BITMAPCAPTURE));
     
    HDC hdcScreen  = GetDC(NULL);
    HDC hdcCapture = CreateCompatibleDC(NULL);
    int nWidth     = GetSystemMetrics(SM_CXVIRTUALSCREEN),
        nHeight    = GetSystemMetrics(SM_CYVIRTUALSCREEN);
     
    // Bitmap is stored top down as BGRA,BGRA,BGRA when used as
    // DWORDs endianess would change it to ARGB.. windows COLORREF is ABGR
    LPBYTE lpCapture;
    BITMAPINFO bmiCapture = { {
        sizeof(BITMAPINFOHEADER), nWidth, -nHeight, 1, 32, BI_RGB, 0, 0, 0, 0, 0,
    } };
     
    bmpCapture->hbm = CreateDIBSection(hdcScreen, &bmiCapture,
        DIB_RGB_COLORS, (LPVOID *)&lpCapture, NULL, 0);
    if(bmpCapture->hbm){
        HBITMAP hbmOld = (HBITMAP)SelectObject(hdcCapture, bmpCapture->hbm);
        BitBlt(hdcCapture, 0, 0, nWidth, nHeight, hdcScreen, 0, 0, SRCCOPY);
        SelectObject(hdcCapture, hbmOld);
        bmpCapture->pixels = (LPDWORD)lpCapture;
        bmpCapture->width  = nWidth;
      bmpCapture->height = nHeight;
        bResult = TRUE;
    
	}
     
    DeleteDC(hdcCapture);
    DeleteDC(hdcScreen);

    return bResult;
}

int RandomNum(int min, int max)
{
	return min + (rand() % (max - min + 1));
}

void MoveCursor(int x, int y)
{
	POINT ptCursor;
	bool bSetX = false;
	bool bSetY = false;

	do
	{
		GetCursorPos(&ptCursor);

		if (ptCursor.x > x)
			ptCursor.x--;
		else if (ptCursor.x < x)
			ptCursor.x++;
		else
			bSetX = true;

		if (ptCursor.y > y)
			ptCursor.y--;
		else if (ptCursor.y < y)
			ptCursor.y++;
		else
			bSetY = true;

		SetCursorPos(ptCursor.x, ptCursor.y);

	} while (!(bSetX && bSetY));
}