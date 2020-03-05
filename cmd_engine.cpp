#include <iostream>
#include <vector>
#include <windows.h>
#include <string>
#include <cstring>
#include <random>
#include <w32api/wincon.h>
#include "cmd_engine.h"
#include <cmath>

namespace cmd {
    CmdGraphicEngine::CmdGraphicEngine(int xSize, int ySize, std::string gameName) {
        std::cout << "Console based game engine" << std::endl;
        cmdSize.X = xSize;
        cmdSize.Y = ySize;
        bufCmd = new CHAR_INFO[cmdSize.X * cmdSize.Y];
        memset(bufCmd, 0, sizeof(CHAR_INFO) * cmdSize.X * cmdSize.Y);
        hNewScreenBuffer = CreateConsoleScreenBuffer(
                GENERIC_READ | // read/write access
                GENERIC_WRITE,
                FILE_SHARE_READ |
                FILE_SHARE_WRITE, // shared
                NULL, // default security attributes
                CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE
                NULL); // reserved; must be NULL
        consoleRect.Left = 0;
        consoleRect.Top = 0;
        consoleRect.Right = cmdSize.X;
        consoleRect.Bottom = cmdSize.Y;
        cellCount = getCmdBufferSize();
        this->gameName = gameName;
//            SetConsoleTitle(this->gameName); // a cast from string to const char needs to be implemented
        this->hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        this->hIn = GetStdHandle(STD_INPUT_HANDLE);
        if (hStdOut == INVALID_HANDLE_VALUE) {
            return;
        }

        CONSOLE_FONT_INFOEX info = {0};
        info.cbSize       = sizeof(info);
        info.dwFontSize.Y = 5; // leave X as zero
        info.dwFontSize.X = 5;
        info.FontWeight   = FW_NORMAL;
        wcscpy(info.FaceName, L"Lucida Console");
        SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &info);
//        SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE),CONSOLE_FULLSCREEN_MODE,0);
        ShowWindow(GetConsoleWindow(),SW_MAXIMIZE);
    }

    CmdGraphicEngine::~CmdGraphicEngine() {
        std::cout << "CmdGraphEngine destructor" << std::endl;
    }

    void CmdGraphicEngine::randomise(void) {
        for (int j = 0; j < cmdSize.Y; j++) {
            for (int i = 0; i < cmdSize.X; i++) {
                bufCmd[i + j * cmdSize.X].Char.AsciiChar = (char) rand() % 256;
                bufCmd[i + j * cmdSize.X].Attributes = FOREGROUND_INTENSITY | FG_WHITE;
            }
        }
    }

    void CmdGraphicEngine::cmdOut(std::string s) {
        for (int i = 0; i < s.length(); i++) {
            bufCmd[i].Char.AsciiChar = s[i];
            bufCmd[i].Attributes = FOREGROUND_INTENSITY | FG_WHITE;
        }

        WriteConsoleOutputA(hStdOut, bufCmd, cmdSize, homeCoords, &consoleRect);
    }

    void CmdGraphicEngine::printScreen() {
        //Write the character
        WriteConsoleOutputA(hStdOut, bufCmd, cmdSize, homeCoords, &consoleRect);
    }

    void CmdGraphicEngine::resetCoursorPtrPosiotion(void) {
        /* Move the cursor home */
        SetConsoleCursorPosition(hStdOut, homeCoords);
    }

    DWORD CmdGraphicEngine::getCmdBufferSize() {
        /* Get the number of cells in the current buffer */
        if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
            return 0;
        }
        return (csbi.dwSize.X * csbi.dwSize.Y);
    }

    void CmdGraphicEngine::clearScreen() {
        memset(bufCmd, 0, sizeof(CHAR_INFO) * cmdSize.X * cmdSize.Y);
    }

    void CmdGraphicEngine::fillConsole(TCHAR fill) {
        /* Fill the entire buffer with spaces */
        if (!FillConsoleOutputCharacter(hStdOut, fill, cellCount, homeCoords, &count)) {
            return;
        }
    }

    INPUT_RECORD CmdGraphicEngine::getConsoleInput() {
        ReadConsoleInput(hIn, &InRec, 1, &NumRead);
        return InRec;
    }

    void CmdGraphicEngine::drawLine(COORD st, COORD end)
    {
        // y = ax + b
        // y1 = a x1 + b => b = y1 - a x1
        // y2 = a x2 + b
        // y2 = a x2 + y1 - a x1
        // y2 - y1 = a(x2 - x1)
        // ## a = (y2 - y1) / (x2 - x1) ##
        // b = y1 - a x1
        // ## b = y1 - x1 ((y2 - y1) / (x2 - x1)) ##
        float a = (float)(end.Y - st.Y) / (float)(end.X - st.X);
        float b = st.Y - a * st.X;

        // lambda:  <- use it
        int newVal = 0;
        for(int i = st.X; i <= end.X; i++)
        {
            newVal = int((i*1.0) * a + b);
            this->bufCmd[i + newVal * cmdSize.X].Char.AsciiChar = '#';
            this->bufCmd[i + newVal * cmdSize.X].Attributes = FG_WHITE;
        }
    }

    void CmdGraphicEngine::drawCircle(COORD center, int radius)
    {
        // 2 pi r
        const float pi = 3.14;
        int circumference = (float)(2 * pi * radius);
        float angleUpdate = 2.0 * pi / circumference;
        int posX = 0;
        int posY = 0;
        for(int i = 0; i < circumference; i++)
        {
            posX = center.X + (int)(radius * sin(angleUpdate * (float)i));
            posY = center.Y + (int)(radius * cos(angleUpdate * (float)i));
            this->bufCmd[posX + posY * cmdSize.X].Char.AsciiChar = '#';
            this->bufCmd[posX + posY * cmdSize.X].Attributes = FG_WHITE;
        }

    }

    char CmdGraphicEngine::getLastKeyboardAction(void)
    {
        return this->lastlyPressedButton;
    }

    void CmdGraphicEngine::setBufferElement(int posX, int posY, char shape)
    {
        this->bufCmd[posX + posY * cmdSize.X].Char.AsciiChar = shape;
        this->bufCmd[posX + posY * cmdSize.X].Attributes = FG_WHITE;
    }

    void CmdGraphicEngine::handleKeyboardAction(void) {
        unsigned int InRecSize;
        GetNumberOfConsoleInputEvents(this->hIn, &InRecSize);
//            this->cmdOut(std::to_string(InRecSize));
        INPUT_RECORD eventBuffer[InRecSize];
        ReadConsoleInput(this->hIn, eventBuffer, InRecSize, &this->NumRead);

        this->lastlyPressedButton = ' ';
        for (int i = 0; i < InRecSize; i++) {
            if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar && eventBuffer[i].Event.KeyEvent.bKeyDown) {
                this->clearScreen();
                switch (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar) {
                    case 'w':
                        this->lastlyPressedButton = 'w';
                        break;
                    case 's':
                        this->lastlyPressedButton = 's';
                        break;
                    case 'a':
                        this->lastlyPressedButton = 'a';
                        break;
                    case 'd':
                        this->lastlyPressedButton = 'd';
                        break;
                    case 'q':
                        this->lastlyPressedButton = 'q';
                        break;
                }
            }
#if 1 // not working for a press
            else if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode) {
                this->clearScreen();
                switch (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode) {
                    case VK_UP:
                        this->cmdOut("U");
                        break;
                    case VK_DOWN:
                        this->cmdOut("D");
                        break;
                    case VK_LEFT:
                        this->cmdOut("L");
                        break;
                    case VK_RIGHT:
                        this->cmdOut("R");
                        break;
                    case VK_ESCAPE:
                        this->cmdOut("Escape");
                        break;
                    default:
                        this->cmdOut("E");
                        break;
                }
#endif
                FlushConsoleInputBuffer(this->hIn);
            }
        }
    }
}