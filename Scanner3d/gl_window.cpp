#include "stdafx.h"

#include "gl/gl.h"
#include "gl_window.h"
float theta = 0.0f;

void GL_Window::DrawGrid()
{
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT );
	//gluLookAt(
	glPushMatrix();
	glRotatef( theta, 0.0f, 0.0f, 1.0f );
	glBegin( GL_TRIANGLES );
	glColor3f( 1.0f, 0.0f, 0.0f ); glVertex2f( 0.0f, 1.0f );
	glColor3f( 0.0f, 1.0f, 0.0f ); glVertex2f( 0.87f, -0.5f );
	glColor3f( 0.0f, 0.0f, 1.0f ); glVertex2f( -0.87f, -0.5f );
	glEnd();
	glPopMatrix();
	CDC *dc = GetDC();
	SwapBuffers( dc->m_hDC);
	ReleaseDC(dc);
}

void GL_Window::TestFunc()
{
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	glPushMatrix();
	glRotatef( theta, 0.0f, 0.0f, 1.0f );
	glBegin( GL_TRIANGLES );
	glColor3f( 1.0f, 0.0f, 0.0f ); glVertex2f( 0.0f, 1.0f );
	glColor3f( 0.0f, 1.0f, 0.0f ); glVertex2f( 0.87f, -0.5f );
	glColor3f( 0.0f, 0.0f, 1.0f ); glVertex2f( -0.87f, -0.5f );
	glEnd();
	glPopMatrix();
	CDC *dc = GetDC();
	SwapBuffers( dc->m_hDC);
	ReleaseDC(dc);

	theta += 1.0f;
}
void GL_Window::EnableOpenGL()
{
	PIXELFORMATDESCRIPTOR pfd;
	int format;
	HWND hWnd;
	HDC hDC;
	HGLRC hRC;
	// get the device context (DC)
	hWnd = GetSafeHwnd();
	hDC = GetDC()->m_hDC;
	
	// set the pixel format for the DC
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	format = ChoosePixelFormat( hDC, &pfd );
	SetPixelFormat( hDC, format, &pfd );
	
	// create and enable the render context (RC)
	hRC = wglCreateContext( hDC );
	wglMakeCurrent( hDC, hRC );
	
}

// Disable OpenGL

void GL_Window::DisableOpenGL()//HWND hWnd, HDC hDC, HGLRC hRC)
{
	/*
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( hRC );

	*/
	//ReleaseDC( hWnd, hDC );
}
