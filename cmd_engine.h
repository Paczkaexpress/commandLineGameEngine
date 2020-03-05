#ifndef CMDGRAPHICTEST_CMD_ENGINE_H
#define CMDGRAPHICTEST_CMD_ENGINE_H

#include <string>
#include <windows.h>

namespace cmd
{
    class CmdGraphicEngine
    {

        typedef enum {
            FG_BLACK		= 0x0000,
            FG_DARK_BLUE    = 0x0001,
            FG_DARK_GREEN   = 0x0002,
            FG_DARK_CYAN    = 0x0003,
            FG_DARK_RED     = 0x0004,
            FG_DARK_MAGENTA = 0x0005,
            FG_DARK_YELLOW  = 0x0006,
            FG_GREY			= 0x0007, // Thanks MS :-/
            FG_DARK_GREY    = 0x0008,
            FG_BLUE			= 0x0009,
            FG_GREEN		= 0x000A,
            FG_CYAN			= 0x000B,
            FG_RED			= 0x000C,
            FG_MAGENTA		= 0x000D,
            FG_YELLOW		= 0x000E,
            FG_WHITE		= 0x000F,
            BG_BLACK		= 0x0000,
            BG_DARK_BLUE	= 0x0010,
            BG_DARK_GREEN	= 0x0020,
            BG_DARK_CYAN	= 0x0030,
            BG_DARK_RED		= 0x0040,
            BG_DARK_MAGENTA = 0x0050,
            BG_DARK_YELLOW	= 0x0060,
            BG_GREY			= 0x0070,
            BG_DARK_GREY	= 0x0080,
            BG_BLUE			= 0x0090,
            BG_GREEN		= 0x00A0,
            BG_CYAN			= 0x00B0,
            BG_RED			= 0x00C0,
            BG_MAGENTA		= 0x00D0,
            BG_YELLOW		= 0x00E0,
            BG_WHITE		= 0x00F0,
        }color_t;

    public:
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        HANDLE hStdOut;
        HANDLE hNewScreenBuffer;
        HANDLE hIn;
        COORD homeCoords = { 0, 0 };
        COORD cmdSize = { 0, 0 };
        DWORD count;
        DWORD cellCount;
        SMALL_RECT consoleRect;
        DWORD NumRead;
        INPUT_RECORD InRec;
        std::string gameName;

    private:
        char lastlyPressedButton = ' ';
    protected:
        CHAR_INFO *bufCmd;

    public:

        CmdGraphicEngine(int xSize, int ySize, std::string gameName);

        ~CmdGraphicEngine();

        void randomise(void);

        void cmdOut(std::string s);

        void printScreen();

        void resetCoursorPtrPosiotion(void);

        DWORD getCmdBufferSize();

        void clearScreen();

        void fillConsole(TCHAR fill);

        INPUT_RECORD getConsoleInput();

        void handleKeyboardAction(void);

        char getLastKeyboardAction(void);

        void setBufferElement(int posX, int posY, char shape);

        void drawLine(COORD st, COORD end);

        void drawCircle(COORD center, int radius);
    };
}

#endif //CMDGRAPHICTEST_CMD_ENGINE_H
