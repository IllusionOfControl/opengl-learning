#include <windows.h>
#include <gl/gl.h>
#include <math.h>

#pragma comment (lib, "opengl32.lib")

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

    float theta = 0.0f;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
        "GLSample",
        "OpenGL Sample",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        512,
        512,
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

            //Background
            glClearColor(0.3f * light, 0.6f * light, 0.1f * light, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glPushMatrix(); /// ???

            glPushMatrix();
            glLoadIdentity();
            glBegin(GL_TRIANGLE_STRIP);
                glColor3f(0.0f, 0.0f, 0.3f * light);
                glVertex2f(-1.0f, 1.0f);
                glVertex2f(-1.0f, 0.3f);
                glVertex2f(-0.7f, 1.0f);
                glVertex2f(-0.4f, 0.2f);
                glVertex2f(-0.15f, 1.0f);
                glVertex2f(0.1f, 0.36f);
                glVertex2f(0.3f, 1.0f);
                glVertex2f(0.53f, 0.32f);
                glVertex2f(0.7f, 1.0f);
                glVertex2f(0.9f, 0.28f);
                glVertex2f(1.0f, 1.0f);
                glVertex2f(1.0f, 0.24f);
            glEnd();
            glPopMatrix();

            auto drawHome = [](float x, float y, float scale) {
                glPushMatrix();
                glLoadIdentity();

                glTranslatef(x, y, 0);
                glScalef(scale, scale, 1);

                // walls
                glBegin(GL_TRIANGLE_STRIP);
                glColor3f(0.2f * light, 0.0f, 0.3f * light);
                glVertex2f(-0.2f, 0.1f);
                glVertex2f(-0.2f, -0.1f);
                glVertex2f(0.2f, 0.1f);
                glVertex2f(0.2f, -0.1f);
                glEnd();

                // window
                glBegin(GL_TRIANGLE_STRIP);
                glColor3f(0.7f * light, 0.7f * light, 0.7f * light);
                glVertex2f(-0.05f, 0.05f);
                glVertex2f(-0.05f, -0.05f);
                glVertex2f(0.05f, 0.05f);
                glVertex2f(0.05f, -0.05f);
                glEnd();

                //cells
                glBegin(GL_LINES);
                glColor3f(0.0f, 0.0f, 0.0f);
                glVertex2f(0.0f, 0.05f);
                glVertex2f(0.0f, -0.05f);
                glVertex2f(0.05f, 0.0f);
                glVertex2f(-0.05f, 0.0f);
                glEnd();

                // pipe
                glBegin(GL_TRIANGLE_STRIP);
                glColor3f(0.7f * light, 0.7f * light, 0.7f * light);
                glVertex2f(0.14f, 0.28f);
                glVertex2f(0.14f, 0.1f);
                glVertex2f(0.08f, 0.28f);
                glVertex2f(0.08f, 0.1f);
                glEnd();

                //roof
                glBegin(GL_TRIANGLES);
                glColor3f(0.6f * light, 0.6f * light, 0.9f * light);
                glVertex2f(-0.25f, 0.1f);
                glVertex2f(0.0f, 0.3f);
                glVertex2f(0.25f, 0.1f);
                glEnd();

                glPopMatrix(); 
            };

            auto drawQuad = [](float x, float y, float dx, float dy, float alpha) {
                glPushMatrix();
                glLoadIdentity();
                glTranslatef(0.0f, -1.0f, 0.0f);
                glRotatef(alpha, 0, 0, 1);
                glTranslatef(1.7f, 0.0f, 0.0f);
                glColor3f(1.0f, 1.0f, 0.3f);
                glBegin(GL_TRIANGLE_FAN);
                glVertex2f(x, y);
                glVertex2f(x + dx, y);
                glVertex2f(x + dx, y + dy);
                glVertex2f(x, y + dy);
                glEnd();
                glPopMatrix();
            };

            drawHome(-0.7f, -0.6f, 1.2f);
            drawHome(-0.1f, -0.3f, 0.9f);
            drawHome(0.7f, -0.48f, 1.0f);
            drawHome(0.37f, -0.08f, 0.7);

            // sun
            static float alpha = 0.0f;
            alpha -= 2;
            drawQuad(0.0f, 0.0f, 0.2f, 0.2f, alpha);

            light = sin(alpha / 180 * 3.1415926) * 0.40 + 0.5;

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