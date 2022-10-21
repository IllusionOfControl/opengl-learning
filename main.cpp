#include <windows.h>
#include <gl/gl.h>
#define _USE_MATH_DEFINES
#include <math.h>
#pragma comment (lib, "opengl32.lib")

const int w_height = 720;
const int w_width = 1280;
const float ratio = w_width / (float) w_height;
const float gravity = 0.002;
const float gravity_p = 0.004;
const float netHeight = -0.2;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


void DrawCircle(int edges) {
    float x, y;
    float alpha = M_PI * 2.0f / edges;

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0);
    for (int i = 0; i <= edges; i++) {
        x = sinf(alpha * i);
        y = cosf(alpha * i);
        glVertex2f(x, y);
    }
    glEnd();
}

void DrawQuad(float x, float y, float dx, float dy) {
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        glVertex2f(x+dx, y);
        glVertex2f(x+dx, y+dy);
        glVertex2f(x, y+dy);
    glEnd();
}

typedef struct {
    float x, y;
    float dx, dy;
} GameObject;


typedef struct {
    float x, y;
    float dx, dy;
    float r;
} TBall;


TBall ball;
TBall players[2];


BOOL IsCross(float x1, float y1, float r, float x2, float y2) {
    return pow(x1 - x2, 2) + pow(y1 - y2, 2) < r * r;
}

void TBall_Mirror(TBall* obj, float x, float y, float speed) {
    float objVec = atan2(obj->dx, obj->dy);
    float crossVec = atan2(obj->x, obj->y - y);

    float resVec = speed == 0 ? M_PI - objVec + crossVec * 2 : crossVec;
    speed = speed == 0 ? sqrt(pow(obj->dx, 2) + pow(obj->dy, 2)) : speed;

    obj->dx = sin(resVec) * speed;
    obj->dy = cos(resVec) * speed;
}

void TBall_Init(TBall* obj, float x, float y, float dx, float dy, float r) {
    obj->x = x;
    obj->y = y;
    obj->dx = dx;
    obj->dy = dy;
    obj->r = r;
}


void TBall_Reflect(float* speed, float* a, BOOL cond, float wall) {
    if (!cond) return;
    *speed *= -0.85;
    *a = wall;
}


void TBall_Move(TBall* obj) {
    obj->x += obj->dx;
    obj->y += obj->dy;

    TBall_Reflect(&obj->dy, &obj->y, (obj->y < obj->r - 1), obj->r - 1);
    TBall_Reflect(&obj->dy, &obj->y, (obj->y > 1 - obj->r), 1 - obj->r);

    obj->dy -= gravity;

    TBall_Reflect(&obj->dx, &obj->x, (obj->x < obj->r - ratio), obj->r - ratio);
    TBall_Reflect(&obj->dx, &obj->x, (obj->x > ratio - obj->r), ratio - obj->r);

    if (obj->y < netHeight) {
        if(obj->x > 0)
            TBall_Reflect(&obj->dx, &obj->x, (obj->x < obj->r), obj->r);
        else
            TBall_Reflect(&obj->dx, &obj->x, (obj->x > -obj->r), -obj->r);
    }
    else {
        if (IsCross(obj->x, obj->y, obj->r, 0, netHeight))
            TBall_Mirror(obj, 0, netHeight, 0);
    }
}

void TBall_PlayerMove(TBall* obj, char left, char right, char jump, float wl1, float wl2) {
    static float speed = 0.05;

    if (GetKeyState(left) < 0) obj->x -= speed;
    else if (GetKeyState(right) < 0) obj->x += speed;

    if (obj->x - obj->r < wl1) obj->x = wl1 + obj->r;
    if (obj->x + obj->r > wl2) obj->x = wl2 - obj->r;

    if ((GetKeyState(jump) < 0) && (obj->y < -0.99 + obj->r)) {
        obj->dy = speed * 1.4;
    }

    obj->y += obj->dy;
    obj->dy -= gravity;

    if (obj->y - obj->r < -1) {
        obj->y = -1 + obj->r;
        obj->dy = 0;
    }

    if (IsCross(obj->x, obj->y, obj->r, ball.x, ball.y)) {
        TBall_Mirror(&ball, obj->x, obj->y, 0.2);
        ball.dy += 0.01;
    }
}


void TBall_Show(TBall obj) {
    glPushMatrix();
    glTranslatef(obj.x, obj.y, 0);
    glScalef(obj.r, obj.r, 1);
    DrawCircle(32);
    glPopMatrix();
}


void Game_Init() {
    TBall_Init(&ball, 0, 0, 0, 0, 0.2);
    TBall_Init(&players[0], 1, 0, 0, 0, 0.2);
    TBall_Init(&players[1], -1, 0, 0, 0, 0.2);
}

void Game_Show() {
    glColor3f(0.83, 0.81, 0.67);
    DrawQuad(-ratio, -1, ratio * 2, 1);
    
    glColor3f(0.21, 0.67, 0.88);
    DrawQuad(-ratio, 0, ratio * 2, 1);

    glColor3f(0.66, 0.85, 1);
    DrawQuad(-ratio, 0.2, ratio * 2, 1);

    glColor3f(0.66, 0.0, 0);
    TBall_Show(players[0]);

    glColor3f(0.0, 0.85, 0.0);
    TBall_Show(players[1]);

    glColor3f(0.5, 0.3, 0.0);
    TBall_Show(ball);

    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(8);
        glBegin(GL_LINES);
        glVertex2f(0, netHeight);
        glVertex2f(0, -1);
    glEnd();

    glColor3f(0.23, 0.29, 0.79);
 
}

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;


    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
        "GLSample",
        "OpenGL Sample",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        w_width,
        w_height,
        NULL,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    glScalef(1 / ratio, 1, 1);
    Game_Init();

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            // RGRBA
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            TBall_Move(&ball);
            TBall_PlayerMove(&players[0], 'A', 'D', 'W', -ratio, 0);
            TBall_PlayerMove(&players[1], VK_LEFT, VK_RIGHT, VK_UP, 0, ratio);

            Game_Show();

            SwapBuffers(hDC);

            Sleep(1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_DESTROY:
        return 0;

    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;
        }
    }
    break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
        PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}