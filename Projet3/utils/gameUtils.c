#include <curses.h>
#include "gameUtils.h"
#include "windowDrawer.h"

void initializeGame(int mapWidth, int mapHeight, char *map, WINDOW **borderInformationWindow,
                    WINDOW **informationWindow, WINDOW **borderGameWindow, WINDOW **gameWindow,
                    WINDOW **borderStateWindow, WINDOW **stateWindow) {
    *borderInformationWindow = createBorderInformationWindow(mapWidth);
    *informationWindow = createInformationWindow(*borderInformationWindow, mapWidth);
    *borderStateWindow = createBorderStateWindow(mapWidth);
    *stateWindow = createStateWindow(*borderStateWindow, mapWidth);
    *borderGameWindow = createBorderGameWindow(mapWidth, mapHeight);
    *gameWindow = createGameWindow(*borderGameWindow, mapWidth, mapHeight);

    drawMap(*gameWindow, mapWidth, mapHeight, map);
}

void placeLemming(WINDOW *window, int lemmingNumber, int posX, int posY) {
    drawSquare(window, LEMMING, posX, posY, TRUE);
}

int getTool(int mouseX, int mouseY) {
    int tool = 0;

    if (mouseX >= COMMAND_TOOLS_POS_X) {
        switch (mouseY) {
            case COMMAND_TOOLS_ADD_POS_Y:
                tool = TOOL_ADD;
                break;

            case COMMAND_TOOLS_REMOVE_POS_Y:
                tool = TOOL_REMOVE;
                break;

            case COMMAND_TOOLS_EXPLODE_POS_Y:
                tool = TOOL_EXPLODE;
                break;

            case COMMAND_TOOLS_FREEZE_POS_Y:
                tool = TOOL_FREEZE;
                break;

            case COMMAND_TOOLS_PAUSE_RESUME_POS_Y:
                tool = TOOL_PAUSE_RESUME;
                break;

            default:
                tool = 0;
        }
    }

    return tool;
}

int makeMultipleOf(int v, int m) {
    return v - (v % m);
}