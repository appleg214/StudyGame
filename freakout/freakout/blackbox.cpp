// BLACKBOX.CPP - Game Engine 
 
// INCLUDES ///////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN  // make sure all macros are included


#include <windows.h>         // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>

#include <iostream>        // include important C/C++ stuff
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

#include <ddraw.h>           // directX includes
#include "blackbox.h" 
using namespace std;

// game library includes
                                   
// DEFINES ////////////////////////////////////////////////////

// TYPES //////////////////////////////////////////////////////

// PROTOTYPES /////////////////////////////////////////////////

// EXTERNALS //////////////////////////////////////////////////

extern HWND main_window_handle; // extern可以置于变量或者函数前，以表示变量或者函数的定义在别的文件中，save the window handle
extern HINSTANCE main_instance; // 提示编译器遇到此变量和函数时在其他模块中寻找其定义。save the instance

// GLOBALS ////////////////////////////////////////////////////

LPDIRECTDRAW7         lpdd         = NULL;   // dd object
LPDIRECTDRAWSURFACE7  lpddsprimary = NULL;   // dd primary surface
LPDIRECTDRAWSURFACE7  lpddsback    = NULL;   // dd back surface
LPDIRECTDRAWPALETTE   lpddpal      = NULL;   // a pointer to the created dd palette
LPDIRECTDRAWCLIPPER   lpddclipper  = NULL;   // dd clipper
PALETTEENTRY          palette[256];          // color palette
PALETTEENTRY          save_palette[256];     // used to save palettes
DDSURFACEDESC2        ddsd;                  // a direct draw surface description struct
DDBLTFX               ddbltfx;               // used to fill
DDSCAPS2              ddscaps;               // a direct draw surface capabilities struct
HRESULT               ddrval;                // result back from dd calls
DWORD                 start_clock_count = 0; // used for timing

// these defined the general clipping rectangle
int min_clip_x = 0,                          // clipping rectangle
    max_clip_x = SCREEN_WIDTH-1,			 //	定义修建长方形 最小0 最大屏幕长宽
    min_clip_y = 0,
    max_clip_y = SCREEN_HEIGHT-1;

// these are overwritten globally by DD_Init()
int screen_width  = SCREEN_WIDTH,            // 屏幕宽度
    screen_height = SCREEN_HEIGHT,           // 屏幕高度
    screen_bpp    = SCREEN_BPP;              // 像素位宽

// FUNCTIONS //////////////////////////////////////////////////

int DD_Init(int width, int height, int bpp)
{
// 这个函数初始化
int index; // 循环变量looping variable

// 创建对象和测试错误(创建一个IDirectDraw7的对象，如果失败则返回0
if (DirectDrawCreateEx(NULL, (void **)&lpdd, IID_IDirectDraw7, NULL)!=DD_OK)
   return(0);

// 设置协作模式到常用窗口main_window_handle是当前句柄
//DDSCL_ALLOWMODEX允许使用Mode X显示模式。只有当DDSCL_EXCLUSIVE 和DDSCL_FULLSCREEN标志存在的时候才能使用 
//DDSCL_EXCLUSIVE 表示独占级别
//DDSCL_FULLSCREEN 表示需要全屏模式。其他程序中的GDI将不允许在屏幕上画图。这个标志必须和DDSCL_EXCLUSIVE 一起使用
//DDSCL_ALLOWREBOOT 当处于独占（全屏）模式时，允许Ctrl+Alt+Del被检测到
//if (lpdd->SetCooperativeLevel(main_window_handle,
//           DDSCL_ALLOWMODEX | DDSCL_FULLSCREEN | 
//           DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT)!=DD_OK)
if (lpdd->SetCooperativeLevel(main_window_handle,DDSCL_NORMAL)!=DD_OK)
    return(0);

// 设置显示模式
if (lpdd->SetDisplayMode(width,height,bpp,0,0)!=DD_OK)
   return(0);

// 设置全局变量set globals
screen_height = height;
screen_width  = width;
screen_bpp    = bpp;

// 建立主要界面Create the primary surface
memset(&ddsd,0,sizeof(ddsd));
ddsd.dwSize = sizeof(ddsd);
ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;

// 我们必须让DD知道我们需要一个复杂的we need to let dd know that we want a complex 
// 屏幕可翻转的表面结构，设置标志位flippable surface structure, set flags for that
//DDSCAPS_PRIMARYSURFACE：主表面 
//DDSCAPS_FLIP指出这个表面是表面切换结构的一部分。前缓冲区紧跟着一个或多个建立好的后缓冲区
//DDSCAPS_COMPLEX 是一个复杂表面，由主表面，一个或多个粘贴表面组成，通常是为了页面切换
//向DirectDraw说明我要创建一个支持翻页的主页面。说明后，就可以创建主页面了CreateSurface
ddsd.ddsCaps.dwCaps = 
  DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

// DWORD dwBackBufferCount：后缓冲区的数目设置为1 set the backbuffer count to 1
ddsd.dwBackBufferCount = 1;

// 创建主界面create the primary surface
lpdd->CreateSurface(&ddsd,&lpddsprimary,NULL);

// 查询后缓冲区，比如说第二个表面query for the backbuffer i.e the secondary surface
//
ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
//创建后备表面的函数、由主表面指针调用、参数二为接收后备表面指针、意为把后备表面加载至主表面
lpddsprimary->GetAttachedSurface(&ddscaps,&lpddsback);

// create and attach palette

// create palette data
// clear all entries defensive programming
memset(palette,0,256*sizeof(PALETTEENTRY));

// create a R,G,B,GR gradient palette
for (index=0; index < 256; index++)
    {
    // set each entry
    if (index < 64) 
        palette[index].peRed = index*4; 
    else           // shades of green
    if (index >= 64 && index < 128) 
        palette[index].peGreen = (index-64)*4;
    else           // shades of blue
    if (index >= 128 && index < 192) 
       palette[index].peBlue = (index-128)*4;
    else           // shades of grey
    if (index >= 192 && index < 256) 
        palette[index].peRed = palette[index].peGreen = 
        palette[index].peBlue = (index-192)*4;
    
    // set flags
    palette[index].peFlags = PC_NOCOLLAPSE;
    } // end for index

// now create the palette object
if (lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256,
                         palette,&lpddpal,NULL)!=DD_OK)
   return(0);

// attach the palette to the primary
if (lpddsprimary->SetPalette(lpddpal)!=DD_OK)
   return(0);

// clear out both primary and secondary surfaces
DD_Fill_Surface(lpddsprimary,0);
DD_Fill_Surface(lpddsback,0);

// attach a clipper to the screen
RECT screen_rect = {0,0,screen_width,screen_height};
lpddclipper = DD_Attach_Clipper(lpddsback,1,&screen_rect);

// return success
return(1);
} // end DD_Init

///////////////////////////////////////////////////////////////

int DD_Shutdown(void)
{
// this function release all the resources directdraw
// allocated, mainly to com objects

// release the clipper first
if (lpddclipper)
    lpddclipper->Release();

// release the palette
if (lpddpal)
   lpddpal->Release();

// release the secondary surface
if (lpddsback)
    lpddsback->Release();

// release the primary surface
if (lpddsprimary)
   lpddsprimary->Release();

// finally, the main dd object
if (lpdd)
    lpdd->Release();

// return success
return(1);
} // end DD_Shutdown

///////////////////////////////////////////////////////////////   

LPDIRECTDRAWCLIPPER DD_Attach_Clipper(LPDIRECTDRAWSURFACE7 lpdds,
                                      int num_rects,
                                      LPRECT clip_list)

{
// this function creates a clipper from the sent clip list and attaches
// it to the sent surface

int index;                         // looping var
LPDIRECTDRAWCLIPPER lpddclipper;   // pointer to the newly created dd clipper
LPRGNDATA region_data;             // pointer to the region data that contains
                                   // the header and clip list

// first create the direct draw clipper
if ((lpdd->CreateClipper(0,&lpddclipper,NULL))!=DD_OK)
   return(NULL);

// now create the clip list from the sent data

// first allocate memory for region data
region_data = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER)+num_rects*sizeof(RECT));

// now copy the rects into region data
memcpy(region_data->Buffer, clip_list, sizeof(RECT)*num_rects);

// set up fields of header
region_data->rdh.dwSize          = sizeof(RGNDATAHEADER);
region_data->rdh.iType           = RDH_RECTANGLES;
region_data->rdh.nCount          = num_rects;
region_data->rdh.nRgnSize        = num_rects*sizeof(RECT);

region_data->rdh.rcBound.left    =  64000;
region_data->rdh.rcBound.top     =  64000;
region_data->rdh.rcBound.right   = -64000;
region_data->rdh.rcBound.bottom  = -64000;

// find bounds of all clipping regions
for (index=0; index<num_rects; index++)
    {
    // test if the next rectangle unioned with the current bound is larger
    if (clip_list[index].left < region_data->rdh.rcBound.left)
       region_data->rdh.rcBound.left = clip_list[index].left;

    if (clip_list[index].right > region_data->rdh.rcBound.right)
       region_data->rdh.rcBound.right = clip_list[index].right;

    if (clip_list[index].top < region_data->rdh.rcBound.top)
       region_data->rdh.rcBound.top = clip_list[index].top;

    if (clip_list[index].bottom > region_data->rdh.rcBound.bottom)
       region_data->rdh.rcBound.bottom = clip_list[index].bottom;

    } // end for index

// now we have computed the bounding rectangle region and set up the data
// now let's set the clipping list

if ((lpddclipper->SetClipList(region_data, 0))!=DD_OK)
   {
   // release memory and return error
   free(region_data);
   return(NULL);
   } // end if

// now attach the clipper to the surface
if ((lpdds->SetClipper(lpddclipper))!=DD_OK)
   {
   // release memory and return error
   free(region_data);
   return(NULL);
   } // end if

// all is well, so release memory and send back the pointer to the new clipper
free(region_data);
return(lpddclipper);

} // end DD_Attach_Clipper

///////////////////////////////////////////////////////////////
   
int DD_Flip(void)
{
// this function flip the primary surface with the secondary surface

// flip pages
while(lpddsprimary->Flip(NULL, DDFLIP_WAIT)!=DD_OK);

// flip the surface

// return success
return(1);

} // end DD_Flip

///////////////////////////////////////////////////////////////

DWORD Start_Clock(void)
{
// this function starts the clock, that is, saves the current
// count, use in conjunction with Wait_Clock()

return(start_clock_count = Get_Clock());

} // end Start_Clock

///////////////////////////////////////////////////////////////

DWORD Get_Clock(void)
{
// GetTickCount返回（retrieve）从操作系统启动到现在所经过（elapsed）的毫秒数，它的返回值是DWORD
//this function returns the current tick count

// return time
return(GetTickCount());

} // end Get_Clock

///////////////////////////////////////////////////////////////

DWORD Wait_Clock(DWORD count)
{
// this function is used to wait for a specific number of clicks
// since the call to Start_Clock

while((Get_Clock() - start_clock_count) < count);
return(Get_Clock());

} // end Wait_Clock

///////////////////////////////////////////////////////////////

int DD_Fill_Surface(LPDIRECTDRAWSURFACE7 lpdds,int color)
{
DDBLTFX ddbltfx; // this contains the DDBLTFX structure

// clear out the structure and set the size field 
DD_INIT_STRUCT(ddbltfx);

// set the dwfillcolor field to the desired color
ddbltfx.dwFillColor = color; 

// ready to blt to surface
lpdds->Blt(NULL,       // ptr to dest rectangle
           NULL,       // ptr to source surface, NA            
           NULL,       // ptr to source rectangle, NA
           DDBLT_COLORFILL | DDBLT_WAIT | DDBLT_ASYNC,   // fill and wait                   
           &ddbltfx);  // ptr to DDBLTFX structure

// return success
return(1);
} // end DD_Fill_Surface

///////////////////////////////////////////////////////////////   

int Draw_Rectangle(int x1, int y1, int x2, int y2, int color,
                   LPDIRECTDRAWSURFACE7 lpdds)
{
// this function uses directdraw to draw a filled rectangle
//这个函数用来绘制一个封闭的长方形
DDBLTFX ddbltfx; // 包含了一个DDBLTFX结构
RECT fill_area;  // 包含了一个目标矩形

// 清除结构并且设置大小域clear out the structure and set the size field 
DD_INIT_STRUCT(ddbltfx);

// set the dwfillcolor field to the desired color
ddbltfx.dwFillColor = color; 

// fill in the destination rectangle data (your data)
fill_area.top    = y1;
fill_area.left   = x1;
fill_area.bottom = y2+1;
fill_area.right  = x2+1;

// ready to blt to surface, in this case blt to primary
lpdds->Blt(&fill_area, // ptr to dest rectangle
           NULL,       // ptr to source surface, NA            
           NULL,       // ptr to source rectangle, NA
           DDBLT_COLORFILL | DDBLT_WAIT | DDBLT_ASYNC,   // fill and wait                   
           &ddbltfx);  // ptr to DDBLTFX structure

// return success
return(1);

} // end Draw_Rectangle

///////////////////////////////////////////////////////////////

int Draw_Text_GDI(char *text, int x,int y,int color, LPDIRECTDRAWSURFACE7 lpdds)
{
// this function draws the sent text on the sent surface 
// using color index as the color in the palette

HDC xdc; // the working dc

// get the dc from surface
if (lpdds->GetDC(&xdc)!=DD_OK)
   return(0);

// set the colors for the text up
SetTextColor(xdc,RGB(palette[color].peRed,palette[color].peGreen,palette[color].peBlue) );

// set background mode to transparent so black isn't copied
SetBkMode(xdc, TRANSPARENT);

// draw the text a
TextOut(xdc,x,y,text,strlen(text));

// release the dc
lpdds->ReleaseDC(xdc);

// return success
return(1);
} // end Draw_Text_GDI

///////////////////////////////////////////////////////////////

