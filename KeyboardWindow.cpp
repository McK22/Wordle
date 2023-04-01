#include "KeyboardWindow.h"

void KeyboardWindow::paintGameWindow(HWND gameHwnd)
{
    for (int i = 0; i < MAX_GAME_WINDOW_COUNT; i++)
    {
        if (gameWindows[i]->getHwnd() == gameHwnd)
        {
            gameWindows[i]->paint();
            return;
        }
    }
}

void KeyboardWindow::reset()
{
    currentWord = "";
    currentRow = 0;
    for (int i = 0; i < BOARD_HEIGHT_HARD; i++)
        animationTime[i] = -1;
    destroyGameWindows();
}

BOOL KeyboardWindow::create()
{
    hWnd = CreateWindowExW(WS_EX_LAYERED, KEYBOARD_CLASS, TITLE, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInst, nullptr);

    if (!hWnd)
        return false;
    
    SetLayeredWindowAttributes(hWnd, 0, KEYBOARD_ALPHA, LWA_ALPHA);
    resize(width, height);
    moveCenter(screenWidth / 2, screenHeight - BOTTOM_MARGIN - height / 2);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    createBitmap();

    return true;
}

int KeyboardWindow::getBoardHeight()
{
    if (difficulty == IDM_EASY)
        return BOARD_HEIGHT_EASY;
    else if (difficulty == IDM_MEDIUM)
        return BOARD_HEIGHT_MEDIUM;
    else
        return BOARD_HEIGHT_HARD;
}

void KeyboardWindow::createGameWindows()
{
    if (difficulty == IDM_EASY)
    {
        gameWindows[0]->create();
        gameWindows[0]->setWordToGuess(getRandomWord());
        gameWindows[0]->moveCenter(screenWidth / 2, screenHeight / 2);
    }
    else if (difficulty == IDM_MEDIUM)
    {
        gameWindows[0]->create();
        gameWindows[0]->setWordToGuess(getRandomWord());
        gameWindows[0]->moveCenter(screenWidth / 4, screenHeight / 2);
        gameWindows[1]->create();
        gameWindows[1]->setWordToGuess(getRandomWord());
        gameWindows[1]->moveCenter(screenWidth * 3 / 4, screenHeight / 2);
    }
    else
    {
        int centerX = screenWidth / 2;
        int centerY = screenHeight / 2;
        for (int i = 0; i < MAX_GAME_WINDOW_COUNT; i++)
        {
            gameWindows[i]->create();
            gameWindows[i]->setWordToGuess(getRandomWord());
            gameWindows[i]->moveCenter(centerX + screenWidth / 4 * (i % 2 == 0 ? -1 : 1),
                centerY + screenHeight / 4 * (i < 2 ? -1 : 1));
        }
    }
    SetFocus(hWnd);
}

void KeyboardWindow::updateGameWindows()
{
    for (int i = 0; i < MAX_GAME_WINDOW_COUNT; i++)
        if (gameWindows[i]->getHwnd() != NULL)
            gameWindows[i]->update();
}

void KeyboardWindow::finish()
{
    for (int i = 0; i < MAX_GAME_WINDOW_COUNT; i++)
        gameWindows[i]->finish();
    finished = true;
    updateGameWindows();
}

void KeyboardWindow::validateWord()
{
    if (currentWord.size() != BOARD_WIDTH)
        return;

    if (isWordValid(currentWord))
    {
        startAnimation(currentRow);
        currentRow++;
        for (int i = 0; i < MAX_GAME_WINDOW_COUNT; i++)
            if (gameWindows[i]->getHwnd() != NULL && !gameWindows[i]->isFinished())
                gameWindows[i]->confirmWord(currentRow - 1);

        update();
        
        if ((difficulty == IDM_EASY && currentRow == BOARD_HEIGHT_EASY)
                || (difficulty == IDM_MEDIUM && currentRow == BOARD_HEIGHT_MEDIUM)
                || (difficulty == IDM_HARD && currentRow == BOARD_HEIGHT_HARD))
            finish();
    }
    else
        for (int i = 0; i < BOARD_WIDTH; i++)
            setLetter(currentRow, i, '\0');

    currentWord = "";
}

void KeyboardWindow::setLetter(int x, int y, char c)
{
    for (int i = 0; i < MAX_GAME_WINDOW_COUNT; i++)
    {
        if (gameWindows[i]->getHwnd() != NULL && !gameWindows[i]->isFinished())
        {
            gameWindows[i]->setLetter(x, y, c);
            gameWindows[i]->update();
        }
    }
}

void KeyboardWindow::type(char c)
{
    c = toupper(c);
    if (currentWord.size() < BOARD_WIDTH)
    {
        setLetter(currentRow, currentWord.size(), c);
        currentWord += c;
    }
}

void KeyboardWindow::erase()
{
    if (currentWord.size() > 0)
    {
        currentWord = currentWord.substr(0, currentWord.size() - 1);
        setLetter(currentRow, currentWord.size(), '\0');
    }
}

void KeyboardWindow::destroyGameWindows()
{
    for (int i = 0; i < MAX_GAME_WINDOW_COUNT; i++)
        gameWindows[i]->destroy();
}

void KeyboardWindow::startGame()
{
    finished = false;
    reset();
    createGameWindows();
    update();
}

void KeyboardWindow::drawTileBackground(int x, int y, char c)
{
    if (gameWindows[0]->getLetterColor(c) == COLOR_DEFAULT)
    {
        drawRect(x, y, TILE_SIZE, TILE_SIZE, COLOR_DEFAULT);
        return;
    }

    HPEN pen = CreatePen(PS_NULL, 0, 0);
    HPEN oldPen = (HPEN)SelectObject(offDC, pen);

    if (difficulty == IDM_EASY)
        drawRect(x, y, TILE_SIZE, TILE_SIZE, gameWindows[0]->getLetterColor(c));
    else if (difficulty == IDM_MEDIUM)
    {
        drawRect(x, y, TILE_SIZE / 2 + 3, TILE_SIZE, gameWindows[0]->getLetterColor(c));
        drawRect(x + TILE_SIZE / 2, y, TILE_SIZE / 2, TILE_SIZE, gameWindows[1]->getLetterColor(c));
    }
    else
    {
        drawRect(x                , y                , TILE_SIZE / 2 + 3, TILE_SIZE / 2 + 3, gameWindows[0]->getLetterColor(c));
        drawRect(x + TILE_SIZE / 2, y                , TILE_SIZE / 2    , TILE_SIZE / 2 + 3, gameWindows[1]->getLetterColor(c));
        drawRect(x                , y + TILE_SIZE / 2, TILE_SIZE / 2 + 3, TILE_SIZE / 2    , gameWindows[2]->getLetterColor(c));
        drawRect(x + TILE_SIZE / 2, y + TILE_SIZE / 2, TILE_SIZE / 2    , TILE_SIZE / 2    , gameWindows[3]->getLetterColor(c));
    }
    SelectObject(offDC, oldPen);
    DeleteObject(pen);
}

void KeyboardWindow::paint()
{
    fillBitmapBackground();

    int y = MARGIN;
    for (int i = 0; i < KEYBOARD_HEIGHT; i++)
    {
        int lettersInRow = keyboardWidth[i];
        int x = (width - lettersInRow * (TILE_SIZE + MARGIN) + MARGIN) / 2;
        for (int j = 0; j < lettersInRow; j++)
        {
            drawTileBackground(x, y, letters[i][j]);
            drawLetter(x, y, letters[i][j]);
            x += TILE_SIZE + MARGIN;
        }
        y += TILE_SIZE + MARGIN;
    }

    drawBitmapContent();
}

void KeyboardWindow::setDifficulty(int difficulty)
{

    CheckMenuItem(GetMenu(hWnd), IDM_EASY, MF_UNCHECKED);
    CheckMenuItem(GetMenu(hWnd), IDM_MEDIUM, MF_UNCHECKED);
    CheckMenuItem(GetMenu(hWnd), IDM_HARD, MF_UNCHECKED);
    CheckMenuItem(GetMenu(hWnd), difficulty, MF_CHECKED);
    this->difficulty = difficulty;
}

ATOM KeyboardWindow::registerClass(HINSTANCE hInst)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = KeyboardWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInst;
    wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_WORDLE));
    wcex.hCursor = nullptr;
    wcex.hbrBackground = CreateSolidBrush(COLOR_BACKGROUND);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GAME);
    wcex.lpszClassName = KEYBOARD_CLASS;
    wcex.hIconSm = nullptr;
    return RegisterClassExW(&wcex);
}

void KeyboardWindow::startAnimation(int row)
{
    animationTime[row] = 0;
    lastTimeMillis = getTime();
    SetTimer(hWnd, IDT_TIMER, ANIMATION_STEP, NULL);
}

void KeyboardWindow::updateAnimation()
{
    long long currentTime = getTime();
    bool animationInProgress = false;
    for (int i = 0; i < BOARD_HEIGHT_HARD; i++)
    {
        if (animationTime[i] != NO_ANIMATION)
        {
            animationTime[i] += currentTime - lastTimeMillis;
            if (animationTime[i] > TILE_ANIMATION_DURATION + 4 * TILE_ANIMATION_DURATION / 2)
                animationTime[i] = NO_ANIMATION;
            else
                animationInProgress = true;
        }
    }
    lastTimeMillis = currentTime;
    updateGameWindows();
    if (!animationInProgress)
        KillTimer(hWnd, IDT_TIMER);
}

bool KeyboardWindow::isRowInAnimation(int row)
{
    return animationTime[row] != NO_ANIMATION;
}

int KeyboardWindow::getAnimationTime(int row)
{
    return animationTime[row];
}
