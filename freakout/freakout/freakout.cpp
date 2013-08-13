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

#define WINDOW_WIDTH            640     // ���ڴ�С
#define WINDOW_HEIGHT           480

// states for game loop
#define GAME_STATE_INIT         0	//��Ϸ��ʼ��
#define GAME_STATE_START_LEVEL  1	//��Ϸ��ʼ�ȼ�
#define GAME_STATE_RUN          2	//��Ϸ����
#define GAME_STATE_SHUTDOWN     3	//��Ϸ�ر�
#define GAME_STATE_EXIT         4	//��Ϸ�˳�

// block defines
#define NUM_BLOCK_ROWS          6	//����������
#define NUM_BLOCK_COLUMNS       8	//����������

#define BLOCK_WIDTH             64	//���
#define BLOCK_HEIGHT            16	//���
#define BLOCK_ORIGIN_X          8	//���ʼXֵ
#define BLOCK_ORIGIN_Y          8	//���ʼYֵ
#define BLOCK_X_GAP             80	//���ż�϶
#define BLOCK_Y_GAP             32	//���ż�϶

// paddle defines
#define PADDLE_START_X          (SCREEN_WIDTH/2 - 16)	//������ʼʱX����
#define PADDLE_START_Y          (SCREEN_HEIGHT - 32);	//������ʼʱY����
#define PADDLE_WIDTH            32	//��������
#define PADDLE_HEIGHT           8	//�����߶�
#define PADDLE_COLOR            191	//������ɫ

// ball defines
#define BALL_START_Y            (SCREEN_HEIGHT/2)	//С��ʼʱY������
#define BALL_SIZE                4					//С���С

// PROTOTYPES /////////////////////////////////////////////////

// game console
int Game_Init(void *parms=NULL);
int Game_Shutdown(void *parms=NULL);
int Game_Main(void *parms=NULL);

// GLOBALS ////////////////////////////////////////////////////

HWND main_window_handle  = NULL; // save the window handle
HINSTANCE main_instance  = NULL; // save the instance
int game_state           = GAME_STATE_INIT; // starting state

int paddle_x = 0, paddle_y = 0; // �����켣λ��
int ball_x   = 0, ball_y   = 0; // С��켣λ��
int ball_dx  = 0, ball_dy  = 0; // С������
int score    = 0;               // �÷�
int level    = 1;               // ��ǰ�ȼ�
int blocks_hit = 0;             // �������

// �����������Ϸ��������

UCHAR blocks[NUM_BLOCK_ROWS][NUM_BLOCK_COLUMNS];     

// FUNCTIONS //////////////////////////////////////////////////

LRESULT CALLBACK WindowProc(HWND hwnd, 
						    UINT msg, 
                            WPARAM wparam, 
                            LPARAM lparam)
{
// ����ϵͳ��Ҫ��Ϣ���
PAINTSTRUCT	ps;		   // ��WM_PAINT��ʹ��
HDC			hdc;	   // �豸�����ľ��

// ��Ϣ����
switch(msg)
	{	
	case WM_CREATE: 
        {
		// �˴���һЩ��ʼ������
		return(0);
		} break;

    case WM_PAINT:
         {
         // ��ʼ��ͼ
         hdc = BeginPaint(hwnd,&ps);

         // ����������Ч

         // ������ͼ
         EndPaint(hwnd,&ps);
         return(0);
        } break;

	case WM_DESTROY: 
		{
		// ɱ������		
		PostQuitMessage(0);
		return(0);
		} break;

	default:break;

    } // end switch

// �������ⲻ��Ҫ��ע����Ϣ
return (DefWindowProc(hwnd, msg, wparam, lparam));

} // end WinProc

// WINMAIN ////////////////////////////////////////////////////

int WINAPI WinMain(	HINSTANCE hinstance,
					HINSTANCE hprevinstance,
					LPSTR lpcmdline,
					int ncmdshow)
{
// ����winmain����

WNDCLASS winclass;	// ��������Ǵ�������
HWND	 hwnd;		// ͨ�ô��ھ��
MSG		 msg;		// ͨ����Ϣ
HDC      hdc;       // ͨ��DC
PAINTSTRUCT ps;     // ͨ�û�ͼ�ṹ

// �������봰���������
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

// ע�ᴰ����
if (!RegisterClass(&winclass))
	return(0);

// �������ڣ�ע��ʹ����WS_POPUP
if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME,    // ����
             "WIN3D Game Console",	// ����
			 WS_POPUP | WS_VISIBLE,
			 0,0,	                // ��ʼ�� x,y
   		     GetSystemMetrics(SM_CXSCREEN),  // ��ʼ������
             GetSystemMetrics(SM_CYSCREEN),  // ��ʼ���߶�
			 NULL,	    // ���׾��
			 NULL,	    // �˵����
			 hinstance,// ʵ����winmain����
			 NULL)))	// ��������
return(0);

// �������
ShowCursor(FALSE);

// ���洰�ھ����ʵ����ȫ�ֱ�����save the window handle and instance in a global
main_window_handle = hwnd;
main_instance      = hinstance;

// ��������ָ������Ϸ���Ƶĳ�ʼ��perform all game console specific initialization
Game_Init();//?

// enter main event loop������Ҫ�¼�ѭ��
while(1)
	{
	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))//?
		{ 
		//�����ǲ����˳�
        if (msg.message == WM_QUIT)
           break;
	
		// ���������ݼ�translate any accelerator keys
		TranslateMessage(&msg);

		// ����Ϣ�͵�WINDOW proc
		DispatchMessage(&msg);
		} // end if
    
    // ��Ҫ��Ϸ���������ڴ�main game processing goes here
    Game_Main();//?

	} // end while

// �رճ����ͷ�������Դ
Game_Shutdown();//?

// ��ʾ���
ShowCursor(TRUE);

// ����Windows
return(msg.wParam);

} // WinMain����

// T3DX GAME PROGRAMMING CONSOLE FUNCTIONS 3D��Ϸ�������ƺ���////////////////////

int Game_Init(void *parms)
{
// ��������ǽ���������Ϸ��ʼ���ĵط�



// �ɹ�����
return(1);

} // Game_Init ����

///////////////////////////////////////////////////////////////

int Game_Shutdown(void *parms)
{
// �����������ر���Ϸ�����ͷ�����ռ�е���Դ�ĵط�


// �ɹ�����
return(1);

} // Game_Shutdown ����

///////////////////////////////////////////////////////////////

void Init_Blocks(void)
{
// ��ʼ������,��blocks[][]2ά���鸳ֵ
for (int row=0; row < NUM_BLOCK_ROWS; row++)
    for (int col=0; col < NUM_BLOCK_COLUMNS; col++)
         blocks[row][col] = row*16+col*3+16;

} // end Init_Blocks

///////////////////////////////////////////////////////////////

void Draw_Blocks(void)
{
// �����������Ҫ�����л������п�
int x1 = BLOCK_ORIGIN_X, // �������ﵱǰλ��
    y1 = BLOCK_ORIGIN_Y; 

// �������п�
for (int row=0; row < NUM_BLOCK_ROWS; row++)
    {    
    // ������λ��
    x1 = BLOCK_ORIGIN_X;

    // ���ƴ��еĿ�
    for (int col=0; col < NUM_BLOCK_COLUMNS; col++)
        {
        // ������һ���죨����еĻ���
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

char buffer[80]; // ������ӡ����

// ��Ϸ��״̬�ǣ�
if (game_state == GAME_STATE_INIT)//��Ϸ״̬�ǳ�ʼ��
    {
    // �����ʼ������ͼ��
    DD_Init(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP);

    // seed the random number generator
    // so game is different each play
    srand(Start_Clock());

    // set the paddle position here to the middle bottom
    paddle_x = PADDLE_START_X;
    paddle_y = PADDLE_START_Y;

    // set ball position and velocity
    ball_x = 8+rand()%(SCREEN_WIDTH-16);
    ball_y = BALL_START_Y;
    ball_dx = -4 + rand()%(8+1);
    ball_dy = 6 + rand()%2;

    // transition to start level state
    game_state = GAME_STATE_START_LEVEL;

    } // end if 
////////////////////////////////////////////////////////////////
else
if (game_state == GAME_STATE_START_LEVEL)//��Ϸ״̬�ǿ�ʼ�ȼ�
    {
    // get a new level ready to run

    // initialize the blocks
    Init_Blocks();

    // reset block counter
    blocks_hit = 0;

    // transition to run state
    game_state = GAME_STATE_RUN;

    } // end if
///////////////////////////////////////////////////////////////
else
if (game_state == GAME_STATE_RUN)//��Ϸ״̬����������
    {
    // start the timing clock
    Start_Clock();

    // clear drawing surface for the next frame of animation
    Draw_Rectangle(0,0,SCREEN_WIDTH-1, SCREEN_HEIGHT-1,200);

    // move the paddle
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

    // keep ball on screen, if the ball hits the edge of 
    // screen then bounce it by reflecting its velocity
    if (ball_x > (SCREEN_WIDTH - BALL_SIZE) || ball_x < 0) 
       {
       // reflect x-axis velocity
       ball_dx=-ball_dx;

       // update position 
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

    // test if ball hit any blocks or the paddle
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
if (game_state == GAME_STATE_SHUTDOWN)//��Ϸ״̬�ǹر�
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