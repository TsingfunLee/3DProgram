/*
 *		This Code Was Created By Jeff Molofee 2000
 *		A HUGE Thanks To Fredric Echols For Cleaning Up
 *		And Optimizing The Base Code, Making It More Flexible!
 *		If You've Found This Code Useful, Please Let Me Know.
 *		Visit My Site At nehe.gamedev.net
 */

#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glut.h>
#include <gl\glaux.h>		// Header File For The Glaux Library

#pragma comment( lib, "opengl32.lib")    // OpenGL32连接库
#pragma comment( lib, "glu32.lib")        // GLu32连接库
#pragma comment( lib, "glaux.lib")        // GLaux连接库
#pragma comment( lib, "glut.lib")        // Glut链接库

#include "CBMPLoader.h"
#include "Camera.h"
#include "Terrain.h"
#include "SkyBox.h"
#include "3DSLoader.h"
#include "MD2Loader.h"
//#include "Snow.h"
#include "Font.h"



HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default

//GLfloat	rtri;				// Angle For The Triangle ( NEW )
//GLfloat	rquad;				// Angle For The Quad ( NEW )

CTerrain    g_Terrain;                     /**< 地形类 */
CSkyBox     g_SkyBox;                      /**< 天空类 */
Camera      g_Camera;
C3DSLoader  g_3DS;
C3DSLoader  g_3DS2;
C3DSLoader  g_3DShehualou;
//C3DSLoader  g_3DSlangren;
//C3DSLoader  g_3DStrees;
CMD2Loader  g_MD2;
//CMD2Loader  g_MD2sodf;
CMD2Loader  g_MD2knight;
//CSnow       g_Snow;
GLFont      g_Font;

bool       g_RenderMode=TRUE;
bool       sp=FALSE;
bool       mp=FALSE;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,4000.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

}


int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glDepthFunc(GL_LEQUAL);	                            // The Type Of Depth Testing To Do
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	//glEnable(GL_CULL_FACE);							
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glEnable(GL_TEXTURE_2D);             /**< 开启纹理映射 */

	/** 设置混合因子获得半透明效果 */
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_BLEND);				     /**< 启用混和 */

	/** 初始化雪花实例 */
	/*if (!g_Snow.Init(800))
	{
		MessageBox(NULL, "雪花系统初始化失败!", "错误", MB_OK);
		exit(-1);
	}*/

	/** 初始化3DS文件 */
	g_3DS.Init("Rabbit.3DS");
	g_3DS2.Init("model.3ds");
	g_3DShehualou.Init("hehualou.3DS");
	//g_3DSlangren.Init("langren.3DS");

	//g_3DStrees.Init("trees.3DS");

	//glDisable(GL_BLEND);

	/** 初始化地形 */
	if (!g_Terrain.init())
	{
		MessageBox(NULL, "初始化地形失败!", "错误", MB_OK);
		exit(0);
	}

	/** 初始化天空 */
	if (!g_SkyBox.init())
	{
		MessageBox(NULL, "初始化天空失败!", "错误", MB_OK);
		exit(0);
	}

	/** 初始化MD2文件 */
	g_MD2.Init("hobgoblin.md2", "hobgoblin.bmp");
	//g_MD2sodf.Init("sodf.md2", "sodf.bmp");
	g_MD2knight.Init("knight.md2", "knight.bmp");

	//glDisable(GL_CULL_FACE);
	
	//glDisable(GL_TEXTURE_2D);

	/** 初始化字体 */
	if (!g_Font.InitFont())
		MessageBox(NULL, "初始化字体失败!", "错误", MB_OK);

	g_Camera.setCamera(385, 32, 674, 374.5, 35, 669, 0, 1, 0);

	return TRUE;										// Initialization Went OK
}

void UpdateCamera()
{
	g_Camera.setViewByMouse();

    /** 键盘按键响应 */
	if(keys[VK_SHIFT])                        /**< 按下SHIFT键时加速 */
	{
		g_Camera.setSpeed(0.5f);
	}
	if(!keys[VK_SHIFT])
	{
		g_Camera.setSpeed(0.2f);
	}

	if (keys[VK_UP])   /**< 向上方向键或'W'键按下 */
		g_Camera.moveCamera(g_Camera.getSpeed());          /**< 移动摄像机 */

	if (keys[VK_DOWN]) /**< 向下方向键或'S'键按下 */
		g_Camera.moveCamera(-g_Camera.getSpeed());         /**< 移动摄像机 */

	if (keys[VK_LEFT]) /**< 向左方向键或'A'键按下 */
		g_Camera.yawCamera(-g_Camera.getSpeed());          /**< 移动摄像机 */

	if (keys[VK_RIGHT]) /**< 向右方向键或'D'键按下 */
		g_Camera.yawCamera(g_Camera.getSpeed());            /**< 移动摄像机 */

	/** 根据地形高度更新摄像机 */
	Vector3 vPos = g_Camera.getPosition();                  /**< 得到当前摄像机位置 */
    Vector3 vNewPos = vPos; 
	
	/** 设置摄像机高度为 地形高度 + 10 */
	vNewPos.y = (float)g_Terrain.getAveHeight(vPos.x,vPos.z ) + 10;

		/** 得到高度差值 */
		float temp = vNewPos.y - vPos.y;

		/** 更新摄像机方向 */
		Vector3 vView = g_Camera.getView();
		vView.y += temp;

		/** 设置摄像机 */
		g_Camera.setCamera(vNewPos.x,  vNewPos.y,  vNewPos.z,
						   vView.x,	   vView.y,	   vView.z,	 
						   0, 1, 0);	
}

void Update(/*DWORD milliseconds*/)
{
	/** 更新摄像机 */
	UpdateCamera();

	/** 'M'键播放下一个动作 */
	if (keys['M'] && !mp)
	{
		mp = true;

		/** 设置当前动作为下一个动作 */
		g_MD2.GetModel().currentAnim = (g_MD2.GetModel().currentAnim + 1) % (g_MD2.GetModel().numOfAnimations);
		//g_MD2sodf.GetModel().currentAnim = (g_MD2sodf.GetModel().currentAnim + 1) % (g_MD2sodf.GetModel().numOfAnimations);

		/** 设置当前帧为下一个动作的开始帧 */
		g_MD2.GetModel().currentFrame = (g_MD2.GetModel()).pAnimations[g_MD2.GetModel().currentAnim].startFrame;
		//g_MD2sodf.GetModel().currentFrame = (g_MD2sodf.GetModel()).pAnimations[g_MD2sodf.GetModel().currentAnim].startFrame;
	}
	if (!keys['M'])
	{
		mp = false;
	}

	/** 'M'键播放下一个动作 */
	if (keys['L'] && !mp)
	{
		mp = true;

		/** 设置当前动作为下一个动作 */
		//g_MD2.GetModel().currentAnim = (g_MD2.GetModel().currentAnim + 1) % (g_MD2.GetModel().numOfAnimations);
		g_MD2knight.GetModel().currentAnim = (g_MD2knight.GetModel().currentAnim + 1) % (g_MD2knight.GetModel().numOfAnimations);

		/** 设置当前帧为下一个动作的开始帧 */
		//g_MD2.GetModel().currentFrame = (g_MD2.GetModel()).pAnimations[g_MD2.GetModel().currentAnim].startFrame;
		g_MD2knight.GetModel().currentFrame = (g_MD2knight.GetModel()).pAnimations[g_MD2knight.GetModel().currentAnim].startFrame;
	}
	if (!keys['L'])
	{
		mp = false;
	}

}

void Show3DS(float x, float z, float scale)
{
	glPushMatrix();
	float y = g_Terrain.getAveHeight(x, z);  /**< 获得此处地形高度 */
	glTranslatef(x, y, z);
	glScalef(scale, scale, scale);
	g_3DS.Draw();                            /**< 显示3DS模型 */
	glPopMatrix();
}

void Show3DS2(float x, float z, float scale)
{
	glPushMatrix();
	float y = g_Terrain.getAveHeight(x, z);  /**< 获得此处地形高度 */
	glTranslatef(x, y, z);
	glScalef(scale, scale, scale);
	g_3DS2.Draw();                            /**< 显示3DS模型 */
	glPopMatrix();
}

void Show3DShehualou(float x, float z, float scale)
{
	glPushMatrix();
	float y = g_Terrain.getAveHeight(x, z);  /**< 获得此处地形高度 */
	glTranslatef(x, y, z);
	glScalef(scale, scale, scale);
	g_3DShehualou.Draw();                            /**< 显示3DS模型 */
	glPopMatrix();
}

//void Show3DSlangren(float x, float z, float scale)
//{
//	glPushMatrix();
//	float y = g_Terrain.getAveHeight(x, z);  /**< 获得此处地形高度 */
//	glTranslatef(x, y, z);
//	glScalef(scale, scale, scale);
//	g_3DSlangren.Draw();                            /**< 显示3DS模型 */
//	glPopMatrix();
//}
//void Show3DStrees(float x, float z, float scale)
//{
//	glPushMatrix();
//	float y = g_Terrain.getAveHeight(x, z);  /**< 获得此处地形高度 */
//	glTranslatef(x, y, z);
//	glScalef(scale, scale, scale);
//	g_3DStrees.Draw();                            /**< 显示3DS模型 */
//	glPopMatrix();
//}


void Animate(float x, float z,float h, float scale)
{
	glPushAttrib(GL_CURRENT_BIT); /**< 保存现有颜色属实性 */
	float y = g_Terrain.getAveHeight(x, z)+h;
	glPushMatrix();
	glTranslatef(x, y, z);
	glScalef(scale, scale, scale);
	g_MD2.AnimateMD2Model();
	glPopMatrix();
	glPopAttrib();   /**< 恢复前一属性 */
}

//void Animatesodf(float x, float z, float h, float scale)
//{
//	glPushAttrib(GL_CURRENT_BIT); /**< 保存现有颜色属实性 */
//	float y = g_Terrain.getAveHeight(x, z) + h;
//	glPushMatrix();
//	glTranslatef(x, y, z);
//	glScalef(scale, scale, scale);
//	g_MD2sodf.AnimateMD2Model();
//	glPopMatrix();
//	glPopAttrib();   /**< 恢复前一属性 */
//}

void Animateknight(float x, float z, float h, float scale)
{
	glPushAttrib(GL_CURRENT_BIT); /**< 保存现有颜色属实性 */
	float y = g_Terrain.getAveHeight(x, z) + h;
	glPushMatrix();
	glTranslatef(x, y, z);
	glScalef(scale, scale, scale);
	g_MD2knight.AnimateMD2Model();
	glPopMatrix();
	glPopAttrib();   /**< 恢复前一属性 */
}

void PrintText()
{
	char string[128];                               /**< 用于保存输出字符串 */
	glPushAttrib(GL_CURRENT_BIT);                   /**< 保存现有颜色属性信息 */
	glColor3f(0.0f, 1.0f, 1.0f);                      /**< 设置文字颜色 */

	/** 输出帧速 */
	/*CaculateFrameRate(); */                              /**< 计算帧速 */
	sprintf(string, "移动鼠标上下左右旋转视角"/*(int)m_Fps*/);               /**< 字符串赋值 */
	g_Font.PrintText(string, -5.0f, 3.5f);              /**< 输出字符串 */
	sprintf(string, "方向键移动视线，按下Shift键加速"/*(int)m_Fps*/);               /**< 字符串赋值 */
	g_Font.PrintText(string, -5.0f, 3.0f);              /**< 输出字符串 */
	sprintf(string, "M键切换怪兽动作"/*(int)m_Fps*/);               /**< 字符串赋值 */
	g_Font.PrintText(string, -5.0f, 2.5f);              /**< 输出字符串 */
	sprintf(string, "L键切换士兵动作"/*(int)m_Fps*/);               /**< 字符串赋值 */
	g_Font.PrintText(string, -5.0f, 2.0f);              /**< 输出字符串 */
	glPopAttrib();

}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -6.0f);
   /* glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
	/** 渲染雪花 */
	//g_Snow.Render();
	/*glDisable(GL_BLEND);*/

	Update();

	/** 放置摄像机 */
	g_Camera.setLook();

	/** 绘制天空 */
	g_SkyBox.render();
	
	/** 渲染地形 */
	g_Terrain.render();

	/** 显示3DS模型 */
	Show3DS(300, 600, 4);
	Show3DS2(290, 583, 10);
	Show3DShehualou(290, 600, 0.3);
	//Show3DSlangren(325, 595, 2);
	//Show3DStrees(240, 550, 1);

	Animate(320, 600,13,0.5);
	//Animatesodf(325, 595, 13, 0.5);
	Animateknight(330, 585,13,0.5);
	
	/** 输出屏幕信息 */
	PrintText();

	glFlush();	                 /**< 强制执行所有的OpenGL命令 */

	_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);

	return TRUE;										// Keep Going
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}

}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=TRUE;						// Program Is Active
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}

		case WM_SIZE:								// Resize The OpenGL Window
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	BOOL	done=FALSE;								// Bool Variable To Exit Loop

	// Ask The User Which Screen Mode They Prefer
	if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;							// Windowed Mode
	}

	// Create Our OpenGL Window
	if (!CreateGLWindow("NeHe's Rotation Tutorial",640,480,16,fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}

	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if ((active && !DrawGLScene()) || keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
			{
				done=TRUE;							// ESC or DrawGLScene Signalled A Quit
			}
			else									// Not Time To Quit, Update Screen
			{
				SwapBuffers(hDC);					// Swap Buffers (Double Buffering)
			}

			if (keys[VK_F1])						// Is F1 Being Pressed?
			{
				keys[VK_F1]=FALSE;					// If So Make Key FALSE
				KillGLWindow();						// Kill Our Current Window
				fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode
				// Recreate Our OpenGL Window
				if (!CreateGLWindow("NeHe's Rotation Tutorial",640,480,16,fullscreen))
				{
					return 0;						// Quit If Window Was Not Created
				}
			}
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
}
