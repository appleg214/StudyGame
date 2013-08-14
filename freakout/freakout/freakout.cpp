// FREAKOUT.CPP - break game demo

// INCLUDES ///////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN // include all macros
#define INITGUID            // include all GUIDs 

#include <windows.h>        // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>

#include <iostream>       // include important C/C++ stuff
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>

#include <ddraw.h>          // directX includes
#include "blackbox.h"       // game library includes
using namespace std;

// DEFINES ////////////////////////////////////////////////////

// defines for windows 
#define WINDOW_CLASS_NAME "WIN3DCLASS"  // class name

#define WINDOW_WIDTH            640     // 窗口大小
#define WINDOW_HEIGHT           480

// states for game loop
#define GAME_STATE_INIT         0	//游戏初始化
#define GAME_STATE_START_LEVEL  1	//游戏开始等级
#define GAME_STATE_RUN          2	//游戏运行
#define GAME_STATE_SHUTDOWN     3	//游戏关闭
#define GAME_STATE_EXIT         4	//游戏退出

// block defines
#define NUM_BLOCK_ROWS          6	//数量块行数
#define NUM_BLOCK_COLUMNS       8	//数量快列数

#define BLOCK_WIDTH             64	//块宽
#define BLOCK_HEIGHT            16	//块高
#define BLOCK_ORIGIN_X          8	//块初始X值
#define BLOCK_ORIGIN_Y          8	//块初始Y值
#define BLOCK_X_GAP             80	//横排间隙
#define BLOCK_Y_GAP             32	//竖排间隙

// paddle defines
#define PADDLE_START_X          (SCREEN_WIDTH/2 - 16)	//划桨开始时X坐标
#define PADDLE_START_Y          (SCREEN_HEIGHT - 32);	//划桨开始时Y坐标
#define PADDLE_WIDTH            32	//划桨宽度
#define PADDLE_HEIGHT           8	//划桨高度
#define PADDLE_COLOR            191	//划桨颜色

// ball defines
#define BALL_START_Y            (SCREEN_HEIGHT/2)	//小球开始时Y的坐标
#define BALL_SIZE                4					//小球大小

// PROTOTYPES /////////////////////////////////////////////////

// game console
int Game_Init(void *parms=NULL);
int Game_Shutdown(void *parms=NULL);
int Game_Main(void *parms=NULL);

// GLOBALS ////////////////////////////////////////////////////

HWND main_window_handle  = NULL; // save the window handle
HINSTANCE main_instance  = NULL; // save the instance
int game_state           = GAME_STATE_INIT; // starting state

int paddle_x = 0, paddle_y = 0; // 划桨轨迹位置
int ball_x   = 0, ball_y   = 0; // 小球轨迹位置
int ball_dx  = 0, ball_dy  = 0; // 小球速率
int score    = 0;               // 得分
int level    = 1;               // 当前等级
int blocks_hit = 0;             // 块击中数

// 这个包含了游戏表格数据

UCHAR blocks[NUM_BLOCK_ROWS][NUM_BLOCK_COLUMNS];     

// FUNCTIONS //////////////////////////////////////////////////

LRESULT CALLBACK WindowProc(HWND hwnd, 
						    UINT msg, 
                            WPARAM wparam, 
                            LPARAM lparam)
{
// 这是系统主要消息句柄
PAINTSTRUCT	ps;		   // 在WM_PAINT中使用
HDC			hdc;	   // 设备上下文句柄

// 消息内容
switch(msg)
	{	
	case WM_CREATE: 
        {
		// 此处做一些初始化工作
		return(0);
		} break;

    case WM_PAINT:
         {
         // 开始绘图
         hdc = BeginPaint(hwnd,&ps);

         // 窗口现在生效

         // 结束绘图
         EndPaint(hwnd,&ps);
         return(0);
        } break;

	case WM_DESTROY: 
		{
		// 杀死进程		
		PostQuitMessage(0);
		return(0);
		} break;

	default:break;

    } // end switch

// 处理任意不需要关注的消息
return (DefWindowProc(hwnd, msg, wparam, lparam));

} // end WinProc

// WINMAIN ////////////////////////////////////////////////////

int WINAPI WinMain(	HINSTANCE hinstance,
					HINSTANCE hprevinstance,
					LPSTR lpcmdline,
					int ncmdshow)
{
// 这是winmain函数

WNDCLASS winclass;	// 这个是我们创建的类
HWND	 hwnd;		// 通用窗口句柄
MSG		 msg;		// 通用消息
HDC      hdc;       // 通用DC
PAINTSTRUCT ps;     // 通用绘图结构

// 首先填入窗口类的特性
winclass.style			= CS_DBLCLKS | CS_OWNDC | 
                          CS_HREDRAW | CS_VREDRAW;
winclass.lpfnWndProc	= WindowProc;
winclass.cbClsExtra		= 0;
winclass.cbWndExtra		= 0;
winclass.hInstance		= hinstance;
winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
winclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
winclass.lpszMenuName	= NULL; 
winclass.lpszClassName	= WINDOW_CLASS_NAME;

// 注册窗口类
if (!RegisterClass(&winclass))
	return(0);

// 创建窗口，注意使用了WS_POPUP
if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME,    // 类名
             "WIN3D Game Console",	// 标题
			 WS_POPUP | WS_VISIBLE,
			 0,0,	                // 初始化 x,y
   		     GetSystemMetrics(SM_CXSCREEN),  // 初始化宽度
             GetSystemMetrics(SM_CYSCREEN),  // 初始化高度
			 NULL,	    // 父亲句柄
			 NULL,	    // 菜单句柄
			 hinstance,// 实例由winmain创建
			 NULL)))	// 创建参数
return(0);

// 隐藏鼠标
ShowCursor(FALSE);

// 保存窗口句柄和实例到全局变量中save the window handle and instance in a global
main_window_handle = hwnd;
main_instance      = hinstance;

// 运行所有指定的游戏控制的初始化perform all game console specific initialization
Game_Init();//?

// enter main event loop输入主要事件循环
while(1)
	{
	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))//?
		{ 
		//测试是不是退出
        if (msg.message == WM_QUIT)
           break;
	
		// 翻译任意快捷键translate any accelerator keys
		TranslateMessage(&msg);

		// 将消息送到WINDOW proc
		DispatchMessage(&msg);
		} // end if
    
    // 主要游戏处理进程在此main game processing goes here
    Game_Main();//?

	} // end while

// 关闭程序并释放所有资源
Game_Shutdown();//?

// 显示鼠标
ShowCursor(TRUE);

// 返回Windows
return(msg.wParam);

} // WinMain结束

// T3DX GAME PROGRAMMING CONSOLE FUNCTIONS 3D游戏处理控制函数////////////////////

int Game_Init(void *parms)
{
// 这个函数是进行所有游戏初始化的地方



// 成功返回
return(1);

} // Game_Init 结束

///////////////////////////////////////////////////////////////

int Game_Shutdown(void *parms)
{
// 这个函数是你关闭游戏并且释放所有占有的资源的地方


// 成功返回
return(1);

} // Game_Shutdown 结束

///////////////////////////////////////////////////////////////

void Init_Blocks(void)
{
// 初始化块域,将blocks[][]2维数组赋值
for (int row=0; row < NUM_BLOCK_ROWS; row++)
    for (int col=0; col < NUM_BLOCK_COLUMNS; col++)
         blocks[row][col] = row*16+col*3+16;

} // end Init_Blocks

///////////////////////////////////////////////////////////////

void Draw_Blocks(void)
{
// 这个函数在主要表格中绘制所有块
int x1 = BLOCK_ORIGIN_X, // 用来到达当前位置
    y1 = BLOCK_ORIGIN_Y; 

// 绘制所有块
for (int row=0; row < NUM_BLOCK_ROWS; row++)
    {    
    // 重置列位置
    x1 = BLOCK_ORIGIN_X;

    // 绘制此行的块
    for (int col=0; col < NUM_BLOCK_COLUMNS; col++)
        {
        // 绘制下一个快（如果有的话）
        if (blocks[row][col]!=0)
            {
            // draw block     
            Draw_Rectangle(x1-4,y1+4,x1+BLOCK_WIDTH-4,y1+BLOCK_HEIGHT+4,0);

            Draw_Rectangle(x1,y1,x1+BLOCK_WIDTH,y1+BLOCK_HEIGHT,blocks[row][col]);
            } // end if

        // advance column position
        x1+=BLOCK_X_GAP;
        } // end for col

    // advance to next row position
    y1+=BLOCK_Y_GAP;

    } // end for row

} // end Draw_Blocks

///////////////////////////////////////////////////////////////

void Process_Ball(void)
{
// this function tests if the ball has hit a block or the paddle
// if so, the ball is bounced and the block is removed from 
// the playfield note: very cheesy collision algorithm :)

// first test for ball block collisions

// the algorithm basically tests the ball against each 
// block's bounding box this is inefficient, but easy to 
// implement, later we'll see a better way

int x1 = BLOCK_ORIGIN_X, // current rendering position
    y1 = BLOCK_ORIGIN_Y; 

int ball_cx = ball_x+(BALL_SIZE/2),  // computer center of ball
    ball_cy = ball_y+(BALL_SIZE/2);

// test of the ball has hit the paddle
if (ball_y > (SCREEN_HEIGHT/2) && ball_dy > 0)
   {
   // extract leading edge of ball
   int x = ball_x+(BALL_SIZE/2);
   int y = ball_y+(BALL_SIZE/2);

   // test for collision with paddle
   if ((x >= paddle_x && x <= paddle_x+PADDLE_WIDTH) &&
       (y >= paddle_y && y <= paddle_y+PADDLE_HEIGHT))
       {
       // reflect ball
       ball_dy=-ball_dy;

       // push ball out of paddle since it made contact
       ball_y+=ball_dy;

       // add a little english to ball based on motion of paddle
       if (KEY_DOWN(VK_RIGHT))
          ball_dx-=(rand()%3);
       else
       if (KEY_DOWN(VK_LEFT))
          ball_dx+=(rand()%3);
       else
          ball_dx+=(-1+rand()%3);
       
       // test if there are no blocks, if so send a message
       // to game loop to start another level
       if (blocks_hit >= (NUM_BLOCK_ROWS*NUM_BLOCK_COLUMNS))
          {
          game_state = GAME_STATE_START_LEVEL;
          level++;
          } // end if

       // make a little noise
       MessageBeep(MB_OK);

       // return
       return; 

       } // end if

   } // end if

// now scan thru all the blocks and see of ball hit blocks
for (int row=0; row < NUM_BLOCK_ROWS; row++)
    {    
    // reset column position
    x1 = BLOCK_ORIGIN_X;

    // scan this row of blocks
    for (int col=0; col < NUM_BLOCK_COLUMNS; col++)
        {
        // if there is a block here then test it against ball
        if (blocks[row][col]!=0)
           {
           // test ball against bounding box of block
           if ((ball_cx > x1) && (ball_cx < x1+BLOCK_WIDTH) &&     
               (ball_cy > y1) && (ball_cy < y1+BLOCK_HEIGHT))
               {
               // remove the block
               blocks[row][col] = 0; 

               // increment global block counter, so we know 
               // when to start another level up
               blocks_hit++;

               // bounce the ball
               ball_dy=-ball_dy;

               // add a little english
               ball_dx+=(-1+rand()%3);

               // make a little noise
               MessageBeep(MB_OK);

               // add some points
               score+=5*(level+(abs(ball_dx)));

               // that's it -- no more block
               return;

               } // end if  

           } // end if

        // advance column position
        x1+=BLOCK_X_GAP;
        } // end for col

    // advance to next row position
    y1+=BLOCK_Y_GAP;

    } // end for row

} // end Process_Ball

///////////////////////////////////////////////////////////////

int Game_Main(void *parms)
{
// this is the workhorse of your game it will be called
// continuously in real-time this is like main() in C
// all the calls for you game go here!

char buffer[80]; // 用来打印文字

// 游戏的状态是？
if (game_state == GAME_STATE_INIT)//游戏状态是初始化
    {
    // 这里初始化所有图形
    DD_Init(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP);

    // 产生种子到随机数字生成器seed the random number generator
    // 所以每次玩的时候都不同so game is different each play
    srand(Start_Clock());//用操作系统启动到现在所经过的毫秒数做种子

    // 设置船桨位置到中下方set the paddle position here to the middle bottom
    paddle_x = PADDLE_START_X;
    paddle_y = PADDLE_START_Y;

    // 设置小球位置和速率set ball position and velocity
    ball_x = 8+rand()%(SCREEN_WIDTH-16);
    ball_y = BALL_START_Y;
    ball_dx = -4 + rand()%(8+1);
    ball_dy = 6 + rand()%2;

    // transition to start level state
    game_state = GAME_STATE_START_LEVEL;

    } // end if 
////////////////////////////////////////////////////////////////
else
if (game_state == GAME_STATE_START_LEVEL)//游戏状态是开始等级
    {
    // get a new level ready to run

    // initialize the blocks
    Init_Blocks();//初始化blocks[][]2维数组

    // reset block counter
    blocks_hit = 0;//击中数=0

    // 进入运行状态transition to run state
    game_state = GAME_STATE_RUN;

    } // end if
///////////////////////////////////////////////////////////////
else
if (game_state == GAME_STATE_RUN)//游戏状态是正在运行
    {
    // 开始游戏时钟start the timing clock
    Start_Clock();

    // clear drawing surface for the next frame of animation
	//为下一个画面的动画清除画图表面
    Draw_Rectangle(0,0,SCREEN_WIDTH-1, SCREEN_HEIGHT-1,200);

    // 移动船桨
    if (KEY_DOWN(VK_RIGHT))
       {
       // move paddle to right
       paddle_x+=8;
 
       // make sure paddle doesn't go off screen
       if (paddle_x > (SCREEN_WIDTH-PADDLE_WIDTH))
          paddle_x = SCREEN_WIDTH-PADDLE_WIDTH;

       } // end if
    else
    if (KEY_DOWN(VK_LEFT))
       {
       // move paddle to right
       paddle_x-=8;
 
       // make sure paddle doesn't go off screen
       if (paddle_x < 0)
          paddle_x = 0;

       } // end if

    // draw blocks
    Draw_Blocks();

    // move the ball
    ball_x+=ball_dx;
    ball_y+=ball_dy;

    // 让小球在屏幕上，如果小球撞到屏幕边缘keep ball on screen, if the ball hits the edge of 
    // 反弹它，通过取反速率screen then bounce it by reflecting its velocity
    if (ball_x > (SCREEN_WIDTH - BALL_SIZE) || ball_x < 0) 
       {
       // 反向速率reflect x-axis velocity
       ball_dx=-ball_dx;

       // 更新位置update position 
       ball_x+=ball_dx;
       } // end if

    // now y-axis
    if (ball_y < 0) 
       {
       // reflect y-axis velocity
       ball_dy=-ball_dy;

       // update position 
       ball_y+=ball_dy;
       } // end if
   else 
   // penalize player for missing the ball
   if (ball_y > (SCREEN_HEIGHT - BALL_SIZE))
       {
       // reflect y-axis velocity
       ball_dy=-ball_dy;

       // update position 
       ball_y+=ball_dy;

       // minus the score
       score-=100;

       } // end if

    // next watch out for ball velocity getting out of hand
    if (ball_dx > 8) ball_dx = 8;
    else
    if (ball_dx < -8) ball_dx = -8;    

    // 测试小球是否击中任意块或者船桨test if ball hit any blocks or the paddle
    Process_Ball();

    // draw the paddle and shadow
    Draw_Rectangle(paddle_x-8, paddle_y+8, 
                   paddle_x+PADDLE_WIDTH-8, 
                   paddle_y+PADDLE_HEIGHT+8,0);

    Draw_Rectangle(paddle_x, paddle_y, 
                   paddle_x+PADDLE_WIDTH, 
                   paddle_y+PADDLE_HEIGHT,PADDLE_COLOR);

    // draw the ball
    Draw_Rectangle(ball_x-4, ball_y+4, ball_x+BALL_SIZE-4, 
                   ball_y+BALL_SIZE+4, 0);
    Draw_Rectangle(ball_x, ball_y, ball_x+BALL_SIZE, 
                   ball_y+BALL_SIZE, 255);

    // draw the info
    sprintf(buffer,"F R E A K O U T           Score %d             Level %d",score,level);
    Draw_Text_GDI(buffer, 8,SCREEN_HEIGHT-16, 127);
    
    // flip the surfaces
    DD_Flip();

    // sync to 33ish fps
    Wait_Clock(30);

    // check of user is trying to exit
    if (KEY_DOWN(VK_ESCAPE))
       {
       // send message to windows to exit
       PostMessage(main_window_handle, WM_DESTROY,0,0);

       // set exit state
       game_state = GAME_STATE_SHUTDOWN;

       } // end if

    } // end if
///////////////////////////////////////////////////////////////
else
if (game_state == GAME_STATE_SHUTDOWN)//游戏状态是关闭
   {
   // in this state shut everything down and release resources
   DD_Shutdown();

   // switch to exit state
   game_state = GAME_STATE_EXIT;

   } // end if

// return success
return(1);

} // end Game_Main

///////////////////////////////////////////////////////////////