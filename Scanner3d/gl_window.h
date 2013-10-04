#pragma once
#include <scanner3dlib.h>
class GL_Window :public CFrameWnd
{
    public:

    GL_Window()
    {
        Create(NULL,0);
    }
	void EnableOpenGL();
	void DisableOpenGL();
	void TestFunc();
	void DrawGrid();
};