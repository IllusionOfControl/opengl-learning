#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <deque>

#pragma comment (lib, "opengl32.lib")

std::deque<POINTFLOAT> points;
int points_count = 100;
const float start = 0, end = 20;
float scaleY = 1.0f, curX = 0, dx = 0;

void FillPoints(float start, float finish, float count);
void AddNextPoint(float x, float y);
float CalculateFunction(float x);
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


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

    curX = end;
    dx = (end - start) / (points_count - 1);
    FillPoints(0, curX, points_count);
    
    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
        "GLSample",
        "OpenGL Sample",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1200,
        700,
        NULL,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

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
            static float light = 0;

            auto draw_axes = [](float alpha) {
                static float d = 0.05;

                glPushMatrix();
                glRotatef(alpha, 0, 0, 1);
                glBegin(GL_LINES);
                    glVertex2f(-1, 0);
                    glVertex2f(1, 0);
                    glVertex2f(1 - d, 0 + d);
                    glVertex2f(1, 0);
                    glVertex2f(1 - d, 0 - d);
                    glVertex2f(1, 0);
                glEnd();
                glPopMatrix();
            };

            auto draw_graph = []() {
                POINTFLOAT point_first = points.front();
                POINTFLOAT point_last = points.back();
                float sx = 2.0f / (point_last.x - point_first.x);
                float dx = (point_last.x + point_first.x) * 0.5;

                glColor3f(0.6f, 0.6f, 0.8f);
                glPushMatrix();
                glScalef(sx, scaleY, 1.0f);
                glTranslatef(-dx, 0.0f, 0.0f);

                glBegin(GL_LINE_STRIP);
                for (auto p : points) {
                    glVertex2f(p.x, p.y);
                }
                glEnd();

                glPopMatrix();
            };

            //Background
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glColor3f(0.4f, 0.4f, 0.4f);
            draw_axes(0.0f);
            draw_axes(90.0f);

            curX += dx;
            AddNextPoint(curX, CalculateFunction(curX));

            glColor3f(0.0f, 1.0f, 1.0f);
            draw_graph();
           

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


void FillPoints(float start, float finish, float count) {
    for (int i = 0; i < count; i++) {
        points.push_back(POINTFLOAT{ start, CalculateFunction(start) });
        start += dx;
    }
}

void AddNextPoint(float x, float y) {
    points.pop_front();
    points.push_back(POINTFLOAT{ x, y });
}

float CalculateFunction(float x) {
    float y = sinf(x);
    return y;
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

    case WM_MOUSEWHEEL:
        if ((int) wParam > 0.0f) scaleY *= 1.5f;
        else scaleY *= 0.7f;
        if (scaleY < 0.02f) scaleY = 0.02f;
        break;

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