#include <windows.h>
#include <windowsx.h>
#include <ctime>
#include <string>
#include <fstream>
#include <ctime>
#include "Lab5.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
int** solutionFV;
int** combinations;
int combinationsAmount;
int* cubesP;
int* cubesB;
int* usedSpecialCombP;
int* usedSpecialCombB;
int* usedCombP;
int* usedCombB;
int turnB;
int turnP;
int scoreB;
int scoreP;
bool isPlayerTurn;
bool isAnimationStarted;
int reRollNum;
int m;
bool isStarted;
int* fixedCubesG;
int* fixedCubesFieldG;
void (*drawFunction[6])(HDC, int, int);

void fillArray(int*, int, int);
bool cmpArrays(int*, int*);
void draw1(HDC, int, int);
void draw2(HDC, int, int);
void draw3(HDC, int, int);
void draw4(HDC, int, int);
void draw5(HDC, int, int);
void draw6(HDC, int, int);
void clearField();
int notifyPlayerWon(HWND);
int notifyBotWon(HWND);
int notifyTie(HWND);


ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    // TODO: Разместите код здесь.

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB5, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow)) {
        return FALSE;
    }
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB5));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB5));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB5);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      100, 100, 700, 700, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd) {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void fillArray(int* a, int n, int num) {
    for (int i = 0; i < n; i++)
        a[i] = num;
}

bool cmpArrays(int* a, int* b) {
    for (int i = 0; i < 6; i++)
        if (a[i] != b[i])
            return false;
    return true;
}

void cpyArray(int* a, int* b, int n) {
    for (int i = 0; i < n; i++)
        a[i] = b[i];
}

int sumCombinations(int* comb) {
    int sum = 0;
    for (int i = 0; i < combinationsAmount; i++)
        if (comb[i] == 1)
            sum += combinations[i][6];
    return sum;
}

int getCombinationIndex(int* cubes, int* comb) {
    for (int i = 0; i < combinationsAmount; i++) {
        if (comb[i] == 0) {
            bool f = true;
            for (int j = 0; j < 6; j++)
                f = f & (cubes[j] - combinations[i][j] >= 0);
            if (f) {
                return i;
            }
        }
    }
    return -1;
}

void fixCubes(int* cubes, int* fixedCubes, int* comb) {
    int imin = -1;
    int min = 999;
    for (int i = 0; i < combinationsAmount; i++) {
        if (comb[i] == 0) {
            int k = 0;
            for (int j = 0; j < 6; j++)
                //if (cubes[j] - combinations[i][j] < 0 && (cubes[j] != 0 || combinations[i][j] != 0))
                    k += abs(combinations[i][j] - cubes[j]);
            if (k < min) {
                min = k;
                imin = i;
            }
        }
    }
    if (imin != -1) {
        for (int j = 0; j < 6; j++)
            if (cubes[j] != 0 && combinations[imin][j] != 0) {
                if (cubes[j] - combinations[imin][j] >= 0)
                    fixedCubes[j] = combinations[imin][j];
                else
                    fixedCubes[j] = cubes[j];
            }
    }
}

int applyMask(int* a, int* mask) {
    for (int i = 0; i < 6; i++)
        if (mask[i] != 0)
            a[i] = mask[i];
    int sum = 0;
    for (int i = 0; i < 6; i++)
            sum += a[i];
    return sum;
}

bool cmpArrays(int* a, int* b, int n) {
    for (int i = 0; i < n; i++)
        if (a[i] != b[i])
            return false;
    return true;
}

void markCombinations(int* usedComb, int ind) {
    int* usedCombination = new int[6];
    fillArray(usedCombination, 6, 0);
    for (int i = 0; i < 6; i++) {
        usedCombination[combinations[ind][i]]++;
    }
    int* currentCombination = new int[6];
    for (int i = 0; i < combinationsAmount; i++) {
        fillArray(currentCombination, 6, 0);
        if (usedComb[i] == 0) {
            for (int j = 0; j < 6; j++) {
                currentCombination[combinations[i][j]]++;
            }
            if (cmpArrays(currentCombination, usedCombination, 6))
                usedComb[i] = -1;
        }
    }
}

double findSolution(int* cubes, int* fixedCubes, int* comb, int turn, bool isMax, bool newTurn, int reRollNum, int* bestFixation, bool& tryToReroll) {
    if (newTurn) {
        int k = getCombinationIndex(cubes, comb);
        if (k != -1) {
            markCombinations(comb, k);
            comb[k] = 1;
            int* tCubes = new int[6];
            for (int i = 0; i < 6; i++)
                tCubes[i] = cubes[i] - combinations[k][i];
            k = getCombinationIndex(cubes, comb);
            if (k != -1) {
                markCombinations(comb, k);
                comb[k] = 1;
            }
            delete[] tCubes;
        }
    }
    if (turn == 1) { 
        return sumCombinations(comb);
    }
    if (!isMax) {
        double mathexpactation = 0;
        int kk = 0;
        int* tCubes = new int[6];
        cpyArray(tCubes, cubes, 6);
        int divider = 0;
        for (int i = 0; i < m; i++) {
            cpyArray(cubes, solutionFV[i], 6);
            int t1 = applyMask(cubes, fixedCubes);
            if (solutionFV[i][6] >= 1 && t1 == 10) {
                divider += solutionFV[i][6];
            }
        }
        for (int i = 0; i < m && divider != 0; i++) {
            cpyArray(cubes, solutionFV[i], 6);
            int t1 = applyMask(cubes, fixedCubes);
            if (solutionFV[i][6] >= 1 && t1 == 10) {
                double t = findSolution(cubes, fixedCubes, comb, turn, true, false, reRollNum, bestFixation, tryToReroll);
                //if (solutionFV[i][6] / (double)divider * t > max)
                mathexpactation += solutionFV[i][6] / (double)divider * t;
            }
        }
        cpyArray(cubes, tCubes, 6);
        delete[] tCubes;
        return mathexpactation;
    }
    if (isMax) {
        if (reRollNum < 2) {
            int* tComb = new int[combinationsAmount];
            int* tCubes = new int[6];
            int* t1Cubes = new int[6];
            cpyArray(tComb, comb, combinationsAmount);
            double c1 = findSolution(cubes, fixedCubes, comb, turn + 1, false, true, 0, bestFixation, tryToReroll);
            cpyArray(comb, tComb, combinationsAmount);
            cpyArray(t1Cubes, fixedCubes, 6);
            fixCubes(cubes, fixedCubes, comb);
            cpyArray(tComb, comb, combinationsAmount);
            double c2 = findSolution(cubes, fixedCubes, comb, turn, false, false, reRollNum + 1, bestFixation, tryToReroll);
            if (turn == 0) {
                if (c1 > c2)
                    tryToReroll = false;
                else {
                    tryToReroll = true;
                    cpyArray(bestFixation, fixedCubes, 6);
                }
            }
            cpyArray(comb, tComb, combinationsAmount);
            cpyArray(fixedCubes, t1Cubes, 6);
            delete[] tComb;
            delete[] tCubes;
            delete[] t1Cubes;
            return max(c1, c2);
        }
        return findSolution(cubes, fixedCubes, comb, turn + 1, false, true, 0, bestFixation, tryToReroll);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            srand(time(NULL));
            drawFunction[0] = &draw1;
            drawFunction[1] = &draw2;
            drawFunction[2] = &draw3;
            drawFunction[3] = &draw4;
            drawFunction[4] = &draw5;
            drawFunction[5] = &draw6;
            std::ifstream in("comb", std::ios::in);
            combinationsAmount = 41;
            combinations = new int*[combinationsAmount];
            for (int i = 0; i < combinationsAmount; i++)
                combinations[i] = new int[7];
            for (int i = 0; i < combinationsAmount; i++) {
                for (int j = 0; j < 7; j++)
                    in >> combinations[i][j];
            }
            in.close();
            in.open("uniquecomb10", std::ios::in);
            m = 3003;
            solutionFV = new int*[m];
            for (int i = 0; i < m; i++) {
                solutionFV[i] = new int[7];
                for (int j = 0; j < 7; j++)
                    in >> solutionFV[i][j];
            }
            cubesP = new int[10];
            cubesB = new int[10];
            usedCombP = new int[combinationsAmount];
            usedCombB = new int[combinationsAmount];
            fixedCubesG = new int[6];
            fixedCubesFieldG = new int[10];
            usedSpecialCombB = new int[6];
            usedSpecialCombP = new int[6];
            clearField();
        }
        break;
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            switch (wmId) {
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
        case WM_KEYUP: {
            if (wParam == VK_SPACE) {
                int tv = 0;
                for (int i = 0; i < 10; i++)
                    if (fixedCubesFieldG[i] == 1)
                        tv++;
                if (isPlayerTurn && tv == 5) {
                    int* tcbs = new int[6];
                    fillArray(tcbs, 6, 0);
                    for (int i = 0; i < 10; i++)
                        if (fixedCubesFieldG[i] == 1)
                            tcbs[cubesP[i]]++;
                    int t = getCombinationIndex(tcbs, usedCombP);
                    if (t != -1) {
                        markCombinations(usedCombP, t);
                        scoreP += combinations[t][6];
                    } else {
                        int kmax = -1;
                        int max = -999;
                        for (int i = 0; i < 6; i++)
                            if (tcbs[i] > max && usedSpecialCombP[i] == 0) {
                                max = tcbs[i];
                                kmax = i;
                            }
                        scoreP += (kmax + 1) * max * 10;
                        usedSpecialCombP[kmax] = (kmax + 1) * max * 10;
                    }
                    fillArray(tcbs, 6, 0);
                    for (int i = 0; i < 10; i++)
                        if (fixedCubesFieldG[i] == 0)
                            tcbs[cubesP[i]]++;
                    t = getCombinationIndex(tcbs, usedCombP);
                    if (t != -1) {
                        markCombinations(usedCombP, t);
                        scoreP += combinations[t][6];
                    }
                    else {
                        int kmax = -1;
                        int max = -999;
                        for (int i = 0; i < 6; i++)
                            if (tcbs[i] > max && usedSpecialCombP[i] == 0) {
                                max = tcbs[i];
                                kmax = i;
                            }
                        scoreP += (kmax + 1) * max * 10;
                        usedSpecialCombP[kmax] = (kmax + 1) * max * 10;
                    }
                    reRollNum = 0;
                    isPlayerTurn = false;
                    turnP++;
                    fillArray(fixedCubesFieldG, 10, 0);
                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                    if (turnP == 5) {
                        if (scoreB < scoreP)
                            notifyPlayerWon(hWnd);
                        else
                            if (scoreB > scoreP)
                                notifyBotWon(hWnd);
                            else
                                if (scoreB == scoreP)
                                    notifyTie(hWnd);
                    }
                }
            }
        }
        break;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            HGDIOBJ original = SelectObject(hdc, GetStockObject(DC_PEN));
            SelectObject(hdc, GetStockObject(DC_PEN));
            SelectObject(hdc, GetStockObject(DC_BRUSH));
            SetDCBrushColor(hdc, RGB(255, 255, 255));
            if (turnP < 5) {
                if (!isPlayerTurn)
                    for (int i = 0; i < 10 && isStarted; i++) {
                        if (fixedCubesFieldG[i] == 1) {
                            SetDCBrushColor(hdc, RGB(255, 0, 0));
                            Rectangle(hdc, 15 + i * 60 + 40, 15 + 530, 65 + i * 60 + 40, 65 + 530);
                            SetDCBrushColor(hdc, RGB(255, 255, 255));
                        }
                        drawFunction[cubesB[i]](hdc, 60 + i * 60, 550);
                    }
                else
                    for (int i = 0; i < 10; i++) {
                        if (fixedCubesFieldG[i] == 1) {
                            SetDCBrushColor(hdc, RGB(0, 0, 255));
                            Rectangle(hdc, 15 + i * 60 + 40, 15 + 530, 65 + i * 60 + 40, 65 + 530);
                            SetDCBrushColor(hdc, RGB(255, 255, 255));
                        }
                        drawFunction[cubesP[i]](hdc, 60 + i * 60, 550);
                    }
            }
            /////
            SetTextColor(hdc, RGB(0, 0, 255));
            drawFunction[0](hdc, 20, 160);
            TextOutA(hdc, 90, 170, std::to_string(usedSpecialCombP[0]).c_str(), strlen(std::to_string(usedSpecialCombP[0]).c_str()));
            TextOutA(hdc, 90, 230, std::to_string(usedSpecialCombP[1]).c_str(), strlen(std::to_string(usedSpecialCombP[1]).c_str()));
            TextOutA(hdc, 90, 290, std::to_string(usedSpecialCombP[2]).c_str(), strlen(std::to_string(usedSpecialCombP[2]).c_str()));
            TextOutA(hdc, 90, 350, std::to_string(usedSpecialCombP[3]).c_str(), strlen(std::to_string(usedSpecialCombP[3]).c_str()));
            TextOutA(hdc, 90, 410, std::to_string(usedSpecialCombP[4]).c_str(), strlen(std::to_string(usedSpecialCombP[4]).c_str()));
            TextOutA(hdc, 90, 470, std::to_string(usedSpecialCombP[5]).c_str(), strlen(std::to_string(usedSpecialCombP[5]).c_str()));
            SetTextColor(hdc, RGB(255, 0, 0));
            TextOutA(hdc, 130, 170, std::to_string(usedSpecialCombB[0]).c_str(), strlen(std::to_string(usedSpecialCombB[0]).c_str()));
            drawFunction[1](hdc, 20, 220);
            TextOutA(hdc, 130, 230, std::to_string(usedSpecialCombB[1]).c_str(), strlen(std::to_string(usedSpecialCombB[1]).c_str()));
            drawFunction[2](hdc, 20, 280); 
            TextOutA(hdc, 130, 290, std::to_string(usedSpecialCombB[2]).c_str(), strlen(std::to_string(usedSpecialCombB[2]).c_str()));
            drawFunction[3](hdc, 20, 340);
            TextOutA(hdc, 130, 350, std::to_string(usedSpecialCombB[3]).c_str(), strlen(std::to_string(usedSpecialCombB[3]).c_str()));
            drawFunction[4](hdc, 20, 400);
            TextOutA(hdc, 130, 410, std::to_string(usedSpecialCombB[4]).c_str(), strlen(std::to_string(usedSpecialCombB[4]).c_str()));
            drawFunction[5](hdc, 20, 460);
            TextOutA(hdc, 130, 470, std::to_string(usedSpecialCombB[5]).c_str(), strlen(std::to_string(usedSpecialCombB[5]).c_str()));
            /////
            
            drawFunction[0](hdc, 320, 160);
            drawFunction[0](hdc, 370, 160);
            drawFunction[0](hdc, 420, 160);
            drawFunction[0](hdc, 470, 160);
            drawFunction[0](hdc, 520, 160);
            int multiplierP = abs(usedCombP[0]);
            int multiplierB = abs(usedCombB[0]);
            SetTextColor(hdc, RGB(0, 0, 255));
            TextOutA(hdc, 590, 170, std::to_string(multiplierP * 500).c_str(), strlen(std::to_string(multiplierP * 500).c_str()));
            SetTextColor(hdc, RGB(255, 0, 0));
            TextOutA(hdc, 630, 170, std::to_string(multiplierB * 500).c_str(), strlen(std::to_string(multiplierB * 500).c_str()));
            drawFunction[0](hdc, 320, 220);
            drawFunction[1](hdc, 370, 220);
            drawFunction[2](hdc, 420, 220);
            drawFunction[3](hdc, 470, 220);
            drawFunction[4](hdc, 520, 220);
            multiplierP = abs(usedCombP[6]);
            multiplierB = abs(usedCombB[6]);
            SetTextColor(hdc, RGB(0, 0, 255));
            TextOutA(hdc, 590, 230, std::to_string(multiplierP * 400).c_str(), strlen(std::to_string(multiplierP * 400).c_str()));
            SetTextColor(hdc, RGB(255, 0, 0));
            TextOutA(hdc, 630, 230, std::to_string(multiplierB * 400).c_str(), strlen(std::to_string(multiplierB * 400).c_str()));
            drawFunction[0](hdc, 320, 280);
            drawFunction[1](hdc, 370, 280);
            drawFunction[2](hdc, 420, 280);
            drawFunction[3](hdc, 470, 280);
            multiplierP = abs(usedCombP[8]);
            multiplierB = abs(usedCombB[8]);
            SetTextColor(hdc, RGB(0, 0, 255));
            TextOutA(hdc, 590, 290, std::to_string(multiplierP * 300).c_str(), strlen(std::to_string(multiplierP * 300).c_str()));
            SetTextColor(hdc, RGB(255, 0, 0));
            TextOutA(hdc, 630, 290, std::to_string(multiplierB * 300).c_str(), strlen(std::to_string(multiplierB * 300).c_str()));
            drawFunction[0](hdc, 320, 340);
            drawFunction[0](hdc, 370, 340);
            drawFunction[0](hdc, 420, 340);
            drawFunction[1](hdc, 470, 340);
            drawFunction[1](hdc, 520, 340);
            multiplierP = abs(usedCombP[11]);
            multiplierB = abs(usedCombB[11]);
            SetTextColor(hdc, RGB(0, 0, 255));
            TextOutA(hdc, 590, 350, std::to_string(multiplierP * 250).c_str(), strlen(std::to_string(multiplierP * 250).c_str()));
            SetTextColor(hdc, RGB(255, 0, 0));
            TextOutA(hdc, 630, 350, std::to_string(multiplierB * 250).c_str(), strlen(std::to_string(multiplierB * 250).c_str()));
            /////
            SetTextColor(hdc, RGB(0, 0, 255));
            TextOutA(hdc, 150, 20, "Player score: ", strlen("Player score: "));
            TextOutA(hdc, 250, 20, std::to_string(scoreP).c_str(), strlen(std::to_string(scoreP).c_str()));
            SetTextColor(hdc, RGB(255, 0, 0));
            TextOutA(hdc, 370, 20, "Bot score: ", strlen("Bot score: "));
            TextOutA(hdc, 450, 20, std::to_string(scoreB).c_str(), strlen(std::to_string(scoreB).c_str()));
            SelectObject(hdc, original);
            EndPaint(hWnd, &ps);
        }
        break;
        case WM_RBUTTONUP: {
            if (isPlayerTurn && reRollNum != 0 && !isAnimationStarted) {
                int mouseX = GET_X_LPARAM(lParam);
                int mouseY = GET_Y_LPARAM(lParam);
                int cn = (mouseX - 60) / 60;
                if (cn >= 0 && cn < 10) {
                    fixedCubesFieldG[cn]++;
                    fixedCubesFieldG[cn] %= 2;
                }
                InvalidateRect(hWnd, NULL, TRUE);
                UpdateWindow(hWnd);
            }
        }
        break;
        case WM_LBUTTONUP: {
            if (isPlayerTurn && turnP < 5 && !isAnimationStarted) {
                if (reRollNum < 3) {
                    if (reRollNum != 0) {
                        for (int j = 0; j < 20; j++) {
                            for (int i = 0; i < 10; i++) {
                                if (fixedCubesFieldG[i] == 0)
                                    cubesP[i] = rand() % 6;
                            }
                            InvalidateRect(hWnd, NULL, TRUE);
                            UpdateWindow(hWnd);
                            Sleep(50);
                        }
                    } else {
                        for (int j = 0; j < 20; j++) {
                            for (int i = 0; i < 10; i++) {
                                cubesP[i] = rand() % 6;
                            }
                            InvalidateRect(hWnd, NULL, TRUE);
                            UpdateWindow(hWnd);
                            Sleep(50);
                        }
                    }
                    int* tcbs = new int[6];
                    fillArray(tcbs, 6, 0);
                    fillArray(fixedCubesFieldG, 10, 0);
                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                    for (int i = 0; i < 10; i++)
                        tcbs[cubesP[i]]++;
                    delete[] tcbs;
                    reRollNum++;
                }
            } else
                if (!isPlayerTurn && turnB < 5 && !isAnimationStarted) {
                    isAnimationStarted = true;
                    int tTurn = turnB;
                    while (tTurn == turnB) {
                        int* tcbs = new int[6];
                        fillArray(tcbs, 6, 0);
                        for (int i = 0; i < 10; i++)
                            tcbs[cubesB[i]]++;
                        if (reRollNum == 0) {
                            isStarted = true;
                            for (int k = 0; k < 20; k++) {
                                for (int i = 0; i < 10; i++)
                                    cubesB[i] = rand() % 6;
                                InvalidateRect(hWnd, NULL, TRUE);
                                UpdateWindow(hWnd);
                                Sleep(50);
                            }
                            fillArray(tcbs, 6, 0);
                            for (int i = 0; i < 10; i++) {
                                //cubesB[i] = rand() % 6;
                                tcbs[cubesB[i]]++;
                            }
                            InvalidateRect(hWnd, NULL, TRUE);
                            UpdateWindow(hWnd);
                            int* bestSolution = NULL;
                            fillArray(fixedCubesG, 6, 0);
                            bool f = false;
                            int* comb = new int[combinationsAmount];
                            fillArray(comb, combinationsAmount, 0);
                            for (int i = 0; i < combinationsAmount; i++)
                                if (usedCombB[i] != 0)
                                    comb[i] = -1;
                            int* tFixed = new int[6];
                            fillArray(tFixed, 6, 0);
                            double k = findSolution(tcbs, tFixed, comb, 0, true, false, reRollNum, fixedCubesG, f);
                            OutputDebugStringA(std::to_string(k).c_str());
                            OutputDebugStringA("\n");
                            OutputDebugStringA(std::to_string(f).c_str());
                            OutputDebugStringA("\n");
                            
                            //InvalidateRect(hWnd, NULL, TRUE);
                            //UpdateWindow(hWnd);
                            if (!f) {
                                int t = getCombinationIndex(tcbs, usedCombB);
                                if (t != -1) {
                                    markCombinations(usedCombB, t);
                                    f = false;
                                    scoreB += combinations[t][6];
                                    for (int i = 0; i < 6; i++)
                                        tcbs[i] = tcbs[i] - combinations[t][i];
                                    t = getCombinationIndex(tcbs, usedCombB);
                                    if (t != -1) {
                                        markCombinations(usedCombB, t);
                                        scoreB += combinations[t][6];
                                    }
                                    else {
                                        int kmax = -1;
                                        int max = -999;
                                        for (int i = 0; i < 6; i++)
                                            if (tcbs[i] > max && usedSpecialCombB[i] == 0) {
                                                max = tcbs[i];
                                                kmax = i;
                                            }
                                        if (max > 5)
                                            max = 5;
                                        scoreB += (kmax + 1) * max * 10;
                                        usedSpecialCombB[kmax] = (kmax + 1) * max * 10;
                                    }
                                }
                                else {
                                    int kmax = -1;
                                    int max = -999;
                                    for (int i = 0; i < 6; i++)
                                        if (tcbs[i] > max && usedSpecialCombB[i] == 0) {
                                            max = tcbs[i];
                                            kmax = i;
                                        }
                                    if (max > 5)
                                        max = 5;
                                    scoreB += (kmax + 1) * max * 10;
                                    usedSpecialCombB[kmax] = (kmax + 1) * max * 10;
                                    kmax = -1;
                                    max = -999;
                                    for (int i = 0; i < 6; i++)
                                        if (tcbs[i] > max && usedSpecialCombB[i] == 0) {
                                            max = tcbs[i];
                                            kmax = i;
                                        }
                                    if (max > 5)
                                        max = 5;
                                    scoreB += (kmax + 1) * max * 10;
                                    usedSpecialCombB[kmax] = (kmax + 1) * max * 10;
                                }
                                turnB++;
                            } else {
                                reRollNum++;
                                if (k < 0.00000001) {
                                    fillArray(tcbs, 6, 0);
                                    for (int i = 0; i < 10; i++)
                                        tcbs[cubesB[i]]++;
                                    int kmax = -1;
                                    int max = -999;
                                    for (int i = 0; i < 6; i++)
                                        if (tcbs[i] > max && usedSpecialCombB[i] == 0) {
                                            max = tcbs[i];
                                            kmax = i;
                                        }
                                    if (k != -1) {
                                        fixedCubesG[kmax] = max;
                                    }
                                }
                                //fixCubes(tcbs, fixedCubesG, usedCombB);
                                OutputDebugStringA(std::to_string(fixedCubesG[0]).c_str());
                                OutputDebugStringA(std::to_string(fixedCubesG[1]).c_str());
                                OutputDebugStringA(std::to_string(fixedCubesG[2]).c_str());
                                OutputDebugStringA(std::to_string(fixedCubesG[3]).c_str());
                                OutputDebugStringA(std::to_string(fixedCubesG[4]).c_str());
                                OutputDebugStringA(std::to_string(fixedCubesG[5]).c_str());
                                OutputDebugStringA("\n");
                            }
                            delete[] tFixed;
                        } else {
                            int* transformedFixation = new int[10];
                            int* tFixedCubes = new int[6];
                            fillArray(fixedCubesFieldG, 10, 0);
                            cpyArray(tFixedCubes, fixedCubesG, 6);
                            fillArray(transformedFixation, 10, 0);
                            for (int i = 0; i < 10; i++)
                                if (tFixedCubes[cubesB[i]] > 0) {
                                    tFixedCubes[cubesB[i]]--;
                                    fixedCubesFieldG[i] = 1;
                                }
                            //Переделать систему фиксаций - заменить tFixedCubes на fixedCubesFieldG
                            for (int k = 0; k < 20; k++) {
                                for (int i = 0; i < 10; i++)
                                    if (fixedCubesFieldG[i] == 0)
                                        cubesB[i] = rand() % 6;
                                InvalidateRect(hWnd, NULL, TRUE);
                                UpdateWindow(hWnd);
                                Sleep(50);
                            }
                            fillArray(tcbs, 6, 0);
                            for (int i = 0; i < 10; i++)
                                tcbs[cubesB[i]]++;
                            int* bestSolution = NULL;
                            bool f = false;
                            int* comb = new int[combinationsAmount];
                            fillArray(comb, combinationsAmount, 0);
                            for (int i = 0; i < combinationsAmount; i++)
                                if (usedCombB[i] != 0)
                                    comb[i] = -1;
                            int* tFixed = new int[6];
                            fillArray(tFixed, 6, 0);
                            double k = findSolution(tcbs, tFixed, comb, 0, true, false, reRollNum, fixedCubesG, f);
                            OutputDebugStringA(std::to_string(k).c_str());
                            OutputDebugStringA("\n");
                            OutputDebugStringA(std::to_string(f).c_str());
                            OutputDebugStringA("\n");
                            
                            //InvalidateRect(hWnd, NULL, TRUE);
                            //UpdateWindow(hWnd);
                            if (!f) {
                                int t = getCombinationIndex(tcbs, usedCombB);
                                if (t != -1) {
                                    markCombinations(usedCombB, t);
                                    f = false;
                                    scoreB += combinations[t][6];
                                    for (int i = 0; i < 6; i++) {

                                        tcbs[i] = tcbs[i] - combinations[t][i];
                                    }
                                    t = getCombinationIndex(tcbs, usedCombB);
                                    if (t != -1) {
                                        markCombinations(usedCombB, t);
                                        scoreB += combinations[t][6];
                                    }
                                    else {
                                        int kmax = -1;
                                        int max = -999;
                                        for (int i = 0; i < 6; i++)
                                            if (tcbs[i] > max && usedSpecialCombB[i] == 0) {
                                                max = tcbs[i];
                                                kmax = i;
                                            }
                                        if (max > 5)
                                            max = 5;
                                        scoreB += (kmax + 1) * max * 10;
                                        usedSpecialCombB[kmax] = (kmax + 1) * max * 10;
                                    }
                                }
                                else {
                                    int kmax = -1;
                                    int max = -999;
                                    for (int i = 0; i < 6; i++)
                                        if (tcbs[i] > max && usedSpecialCombB[i] == 0) {
                                            max = tcbs[i];
                                            kmax = i;
                                        }
                                    if (max > 5)
                                        max = 5;
                                    scoreB += (kmax + 1) * max * 10;
                                    usedSpecialCombB[kmax] = (kmax + 1) * max * 10;
                                    kmax = -1;
                                    max = -999;
                                    for (int i = 0; i < 6; i++)
                                        if (tcbs[i] > max && usedSpecialCombB[i] == 0) {
                                            max = tcbs[i];
                                            kmax = i;
                                        }
                                    if (max > 5)
                                        max = 5;
                                    scoreB += (kmax + 1) * max * 10;
                                    usedSpecialCombB[kmax] = (kmax + 1) * max * 10;
                                }
                                turnB++;
                                reRollNum = 0;
                            } else {
                                if (k < 0.00000001) {
                                    fillArray(tcbs, 6, 0);
                                    for (int i = 0; i < 10; i++)
                                        tcbs[cubesB[i]]++;
                                    int kmax = -1;
                                    int max = -999;
                                    for (int i = 0; i < 6; i++)
                                        if (tcbs[i] > max && usedSpecialCombB[i] == 0) {
                                            max = tcbs[i];
                                            kmax = i;
                                        }
                                    if (k != -1) {
                                        fixedCubesG[kmax] = max;
                                    }
                                }
                                reRollNum++;
                                OutputDebugStringA(std::to_string(fixedCubesG[0]).c_str());
                                OutputDebugStringA(std::to_string(fixedCubesG[1]).c_str());
                                OutputDebugStringA(std::to_string(fixedCubesG[2]).c_str());
                                OutputDebugStringA(std::to_string(fixedCubesG[3]).c_str());
                                OutputDebugStringA(std::to_string(fixedCubesG[4]).c_str());
                                OutputDebugStringA(std::to_string(fixedCubesG[5]).c_str());
                                OutputDebugStringA("\n");
                                if (reRollNum >= 3) {
                                    reRollNum = 0;
                                    turnB++;
                                }
                            }
                            delete[] tFixed;
                        }
                        Sleep(2000);
                        isAnimationStarted = false;
                        delete[] tcbs;
                    }
                    isStarted = false;
                    //for (int i = 0; i < 10; i++)
                        //cubesB[i] = rand() % 6;
                    fillArray(fixedCubesFieldG, 10, 0);
                    isPlayerTurn = true;
                    reRollNum = 0;
                }
        }
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void draw1(HDC hdc, int x, int y) {
    Rectangle(hdc, x, y, x + 40, y + 40);
    SetDCBrushColor(hdc, RGB(0, 0, 0));
    Ellipse(hdc, x + 15, y + 15, x + 25, y + 25);
    SetDCBrushColor(hdc, RGB(255, 255, 255));
}

void draw2(HDC hdc, int x, int y) {
    Rectangle(hdc, x, y, x + 40, y + 40);
    SetDCBrushColor(hdc, RGB(0, 0, 0));
    Ellipse(hdc, x + 5, y + 5, x + 15, y + 15);
    Ellipse(hdc, x + 25, y + 25, x + 35, y + 35);
    SetDCBrushColor(hdc, RGB(255, 255, 255));
}

void draw3(HDC hdc, int x, int y) {
    Rectangle(hdc, x, y, x + 40, y + 40);
    SetDCBrushColor(hdc, RGB(0, 0, 0));
    Ellipse(hdc, x + 5, y + 5, x + 15, y + 15);
    Ellipse(hdc, x + 25, y + 25, x + 35, y + 35);
    Ellipse(hdc, x + 15, y + 15, x + 25, y + 25);
    SetDCBrushColor(hdc, RGB(255, 255, 255));
}

void draw4(HDC hdc, int x, int y) {
    Rectangle(hdc, x, y, x + 40, y + 40);
    SetDCBrushColor(hdc, RGB(0, 0, 0));
    Ellipse(hdc, x + 5, y + 5, x + 15, y + 15);
    Ellipse(hdc, x + 25, y + 25, x + 35, y + 35);
    Ellipse(hdc, x + 5, y + 25, x + 15, y + 35);
    Ellipse(hdc, x + 25, y + 5, x + 35, y + 15);
    SetDCBrushColor(hdc, RGB(255, 255, 255));
}

void draw5(HDC hdc, int x, int y) {
    Rectangle(hdc, x, y, x + 40, y + 40);
    SetDCBrushColor(hdc, RGB(0, 0, 0));
    Ellipse(hdc, x + 5, y + 5, x + 15, y + 15);
    Ellipse(hdc, x + 25, y + 25, x + 35, y + 35);
    Ellipse(hdc, x + 5, y + 25, x + 15, y + 35);
    Ellipse(hdc, x + 25, y + 5, x + 35, y + 15);
    Ellipse(hdc, x + 15, y + 15, x + 25, y + 25);
    SetDCBrushColor(hdc, RGB(255, 255, 255));
}

void draw6(HDC hdc, int x, int y) {
    Rectangle(hdc, x, y, x + 40, y + 40);
    SetDCBrushColor(hdc, RGB(0, 0, 0));
    Ellipse(hdc, x + 5, y + 3, x + 15, y + 13);
    Ellipse(hdc, x + 25, y + 3, x + 35, y + 13);
    Ellipse(hdc, x + 25, y + 27, x + 35, y + 37);
    Ellipse(hdc, x + 5, y + 27, x + 15, y + 37);
    Ellipse(hdc, x + 5, y + 15, x + 15, y + 25);
    Ellipse(hdc, x + 25, y + 15, x + 35, y + 25);
    SetDCBrushColor(hdc, RGB(255, 255, 255));
}

void clearField() {
    fillArray(cubesP, 10, 0);
    fillArray(cubesB, 10, 0);
    fillArray(usedCombP, combinationsAmount, 0);
    fillArray(usedCombB, combinationsAmount, 0);
    fillArray(fixedCubesFieldG, 10, 0);
    fillArray(usedSpecialCombB, 6, 0);
    fillArray(usedSpecialCombP, 6, 0);
    turnB = 0;
    turnP = 0;
    scoreB = 0;
    scoreP = 0;
    isAnimationStarted = false;
    isPlayerTurn = false;
    isStarted = false;
    reRollNum = 0;
    for (int i = 0; i < 10; i++)
        cubesB[i] = rand() % 6;
}

int notifyPlayerWon(HWND hWnd) {
    int msgboxID = MessageBox(NULL, L"Player has won!", L"", MB_ICONINFORMATION | MB_OK);
    clearField();
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
    return msgboxID;
}

int notifyBotWon(HWND hWnd) {
    int msgboxID = MessageBox(NULL, L"Bot has won!", L"", MB_ICONINFORMATION | MB_OK);
    clearField();
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
    return msgboxID;
}

int notifyTie(HWND hWnd) {
    int msgboxID = MessageBox(NULL, L"Tie!", L"", MB_ICONINFORMATION | MB_OK);
    clearField();
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
    return msgboxID;
}