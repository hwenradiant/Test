// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>

// define the screen resolution and keyboard macros
#define SCREEN_WIDTH  500
#define SCREEN_HEIGHT 800
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
#define PI 3.14159265358979323846264338327950288419716939937f
#define DEGTORAD PI/180
#define ENEMY_NUM 1 
#define HowHeroCenter 16
#define HeroToLaunchet 70
#define FirstBulletSpeed 10.0f
#define FirstBulletLife 5
#define n_sprite 4
#define SpriteLength 422.75


// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
LPD3DXSPRITE d3dspt;    // the pointer to our Direct3D Sprite interface



LPDIRECT3DTEXTURE9 sprite;				// the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_hero;			// the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_enemy;		// the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_bullet;		// the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_l_bullet;		// the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_TScore;		// the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_OScore;		// the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_effect;
LPDIRECT3DTEXTURE9 sprite_life;
LPDIRECT3DTEXTURE9 sprite_ScoreUI;



									 // function prototypes
void initD3D(HWND hWnd);    // sets up and initializes Direct3D
void render_frame(void);    // renders a single frame
void cleanD3D(void);		// closes Direct3D and releases memory


float fWorldTick = 0;
void init_game(void);
void do_game_logic(void);
bool sphere_collision_check(float x0, float y0, float size0, float x1, float y1, float size1);
int Score = 0;
int T_Score = 0;
int O_Score = 0;
int t_xPos = 1;
int o_xPos = 1;

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

using namespace std;

float alpha = 170;
float n_alpha;

enum { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };


//기본 클래스 
class entity {

public:
	float x_pos;
	float y_pos;
	int status;
	int HP;
};


bool sphere_collision_check(float x0, float y0, float size0, float x1, float y1, float size1)
{

	if ((x0 - x1)*(x0 - x1) + (y0 - y1)*(y0 - y1) < (size0 + size1) * (size0 + size1))
		return true;
	else
		return false;

}



//주인공 클래스 
class Hero :public entity {

public:
	void move(int i);
	void init(float x, float y);

	float l_x_pos;
	float l_y_pos;

	float left_Bullet;

	bool End_Game;
};

void Hero::init(float x, float y)
{
	x_pos = x;
	y_pos = y;

	l_x_pos = x_pos + (sin(alpha*DEGTORAD) * HeroToLaunchet);
	l_y_pos = y_pos + (cos(alpha*DEGTORAD) * HeroToLaunchet);

	left_Bullet = 5;

	End_Game = false;
}

void Hero::move(int i)
{
	switch (i)
	{
	case MOVE_UP:
		y_pos -= 3;
		break;

	case MOVE_DOWN:
		y_pos += 3;
		break;


	case MOVE_LEFT:
		x_pos -= 3;
		break;


	case MOVE_RIGHT:
		x_pos += 3;
		break;
	}
}


// 적 클래스 
class Enemy :public entity {

public:
	void init(float x, float y);
	void move();
	void hide();
	void active();

	bool eshow;

};

void Enemy::hide()
{
	eshow = false;
}

void Enemy::active()
{
	eshow = true;
}

void Enemy::init(float x, float y)
{
	eshow = false;

	x_pos = x;
	y_pos = y;
}


void Enemy::move()
{
	y_pos += 2;
}

Enemy enemy;
Hero hero;

// 총알 클래스 
class Bullet :public entity {

public:
	bool bShow;
	bool eShow;

	void init(float x, float y);
	void move();
	bool show();
	void hide();
	void active();
	void e_hide();
	void e_active();
	bool check_collision(float x, float y);

	void s_pos();

	float n_xpos;
	float n_ypos;

	float B_Speed;

	int B_Life = FirstBulletLife;
};


bool Bullet::check_collision(float x, float y)
{
	//충돌 처리 시 
	if (sphere_collision_check(x_pos, y_pos, 8, x, y, 32) == true)
	{
		enemy.hide();
		if (hero.left_Bullet == 0)
			hero.End_Game = true;
		bShow = false;
		return true;
	}
	else 
	{
		return false;
	}
}

void Bullet::s_pos()
{
	n_xpos = x_pos;
	n_ypos = y_pos;
}




void Bullet::init(float x, float y)
{
	x_pos = x;
	y_pos = y;

	B_Speed = 10.0f;

	B_Life = FirstBulletLife;
}	



bool Bullet::show()
{
	return bShow;
}


void Bullet::active()
{
	bShow = true;
}

void Bullet::hide()
{
	bShow = false;
}

void Bullet::e_active()
{
	eShow = true;
}

void Bullet::e_hide()
{
	eShow = false;
}




void Bullet::move()
{
 	x_pos = x_pos + sin(n_alpha*DEGTORAD)*B_Speed*fWorldTick*30;
	y_pos = y_pos + cos(n_alpha*DEGTORAD)*B_Speed*fWorldTick*30;
}


//객체 생성 
Bullet bullet;

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"WindowClass";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL, L"WindowClass", L"Our Direct3D Program",
		WS_EX_TOPMOST | WS_POPUP, 50, 50, SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	// set up and initialize Direct3D
	initD3D(hWnd);


	//게임 오브젝트 초기화 
	init_game();

	// enter the main loop:

	MSG msg;

	while (TRUE)
	{
		DWORD starting_point = GetTickCount();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		do_game_logic();

		render_frame();

		// check the 'escape' key
		if (KEY_DOWN(VK_ESCAPE))
			PostMessage(hWnd, WM_DESTROY, 0, 0);
		fWorldTick = (GetTickCount() - starting_point) / 1000.f;
	}

	// clean up DirectX and COM
	cleanD3D();

	return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} 
	break;

	case WM_LBUTTONDOWN:
	{
		enemy.x_pos = LOWORD(lParam);
		enemy.y_pos = HIWORD(lParam);
		enemy.active();
	}
	break;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;


	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	D3DXCreateSprite(d3ddev, &d3dspt);    // create the Direct3D Sprite object

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"background2.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 255, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite);    // load to sprite


	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"unity.png",    // the file name
		760,    // default width
		88,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_hero);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Smoke.png",    // the file name
		384,    // default width
		320,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_enemy);    // load to sprite


	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Bullet.png",    // the file name
		50.0f,    // default width
		63.0f,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_bullet);    // load to sprite sprite_l_bullet

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Feather.png",    // the file name
		19.0f,    // default width
		51.0f,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 255, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_l_bullet);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"number-liquid-effect.png",    // the file name
		720.0f,    // default width
		77.0f,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 255, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_TScore);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"number-liquid-effect.png",    // the file name
		720.0f,    // default width
		77.0f,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 255, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_OScore);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"effect.png",    // the file name
		900.0f,    // default width
		327.0f,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 255, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_effect);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Egg_1.png",    // the file name
		35.0f,    // default width
		42.0f,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 255, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_life);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"ScoreUI.png",    // the file name
		510.0f,    // default width
		810.0f,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 255, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_ScoreUI);    // load to sprite

	return;
}


void init_game(void)
{
	//객체 초기화 
	hero.init(200, 600);

	//적들 초기화 
	enemy.init(0, 0);

	//총알 초기화 
	bullet.init(hero.x_pos + (sin(alpha*DEGTORAD) * 50), hero.y_pos+ (cos(alpha*DEGTORAD) * 50));
}


void do_game_logic(void)
{

	//주인공 처리 
	if (KEY_DOWN(0x57))
		hero.move(MOVE_UP);

	if (KEY_DOWN(0x53))
		hero.move(MOVE_DOWN);

	if (KEY_DOWN(0x41))
		hero.move(MOVE_LEFT);

	if (KEY_DOWN(0x44))
		hero.move(MOVE_RIGHT);


	//총알 처리 
	if (bullet.show() == false)
	{
		if (KEY_DOWN(VK_SPACE))
		{
			hero.left_Bullet--;
 			n_alpha = alpha;
			bullet.active();
			bullet.init(hero.x_pos + (sin(alpha*DEGTORAD) * 50), hero.y_pos + (cos(alpha*DEGTORAD) * 50));
		}
	}


	if (bullet.show() == true)
	{
		bullet.move();

		if (bullet.y_pos <= 0)
		{
			bullet.y_pos = 0;
			n_alpha = n_alpha - 90;
			bullet.move();
			bullet.B_Speed = bullet.B_Speed*1.2;
			bullet.B_Life--;
			bullet.e_active();
			bullet.s_pos();
		}
		else if (bullet.y_pos >= SCREEN_HEIGHT-63)
		{
			bullet.y_pos = SCREEN_HEIGHT-63;
			n_alpha = n_alpha - 90;
			bullet.move();
			bullet.B_Speed = bullet.B_Speed*1.2;
			bullet.B_Life--;
			bullet.e_active();
			bullet.s_pos();
		}
  		else if (bullet.x_pos >= SCREEN_WIDTH-50)
		{
			bullet.x_pos = SCREEN_WIDTH-50;
			n_alpha = n_alpha - 90;
			bullet.move();
			bullet.B_Speed = bullet.B_Speed*1.2;
			bullet.B_Life--;
			bullet.e_active();
			bullet.s_pos();
		}
		else if (bullet.x_pos <= 0)
		{
			bullet.x_pos = 0;
			n_alpha = n_alpha - 90;
			bullet.move();
			bullet.B_Speed = bullet.B_Speed*1.2;
			bullet.B_Life--;
			bullet.e_active();
			bullet.s_pos();
		}

		if (bullet.B_Life == 0)
			bullet.bShow = false;

		//충돌 처리 
		if (enemy.eshow == true)
		{
			if (bullet.check_collision(enemy.x_pos, enemy.y_pos) == true)
			{
				Score = Score + (10-bullet.B_Life);
			}
		}

		if (hero.left_Bullet == 0 && bullet.B_Life == 0)
		{
			hero.End_Game = true;
		}
		
	}
}

float fAnitick = 0;
float fEntick = 0;
float fEtick = 0;
int x_Pos_ = 0;
int e_xPos = 0;

// this is the function used to render a single frame
void render_frame(void)
{
	if (alpha >= 360)
	{
		alpha = 0;
	}

	alpha = alpha + 300 * fWorldTick;

	// clear the window to a deep blue
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();    // begins the 3D scene

	d3dspt->Begin(D3DXSPRITE_ALPHABLEND);    // // begin sprite drawing with transparency

	{
		static float frame = 4;    // start the program on the final frame


		if (frame < 2.1)
		{
			frame = frame + 0.01;
		}

		else
		{
			frame = -0.3;
		}

		int Length = frame * SpriteLength*-1 + 890;

		RECT part4;
		SetRect(&part4, 0, Length, SCREEN_WIDTH, Length + SCREEN_HEIGHT);
		D3DXVECTOR3 center4(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position4(0.0f, 0.0f, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite, &part4, &center4, &position4, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	//주인공 
	static int frame_ = 0;

	fAnitick += fWorldTick;
	if (fAnitick >= 0.1)
	{
		fAnitick = 0;
		if (frame_ < 7)
		{
			frame_ = frame_ + 1;
		}
		else
			frame_ = 0;
	}
	int x_Pos = frame_ * 95;

	RECT part;

	SetRect(&part, x_Pos, 0, x_Pos + 95, 88);
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position(hero.x_pos, hero.y_pos, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(sprite_hero, &part, NULL, &position, D3DCOLOR_ARGB(255, 255, 255, 255));


	////총알 
	if (bullet.bShow == true)
	{
		RECT part1;
		SetRect(&part1, 0, 0, 50, 63);
		D3DXVECTOR3 center1(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position1(bullet.x_pos, bullet.y_pos, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_bullet, &part1, &center1, &position1, D3DCOLOR_ARGB(255, 255, 255, 255));
	}


	////구름 
	static int frame_2 = 0;

	if (enemy.eshow == true)
	{
		RECT part2;
		fEntick += fWorldTick;
		if (fEntick >= 0.1)
		{
			fEntick = 0;
			if (frame_2 < 30)
			{
				frame_2 = frame_2 + 1;
			}
			else
				frame_2 = 0;
		}

		if(0 <= frame_2 && frame_2 < 6)
			SetRect(&part2, (frame_2)*64, 0, (frame_2+1)*64, 64);
		else if (6 <= frame_2 && frame_2 < 12)
			SetRect(&part2, (frame_2%6) * 64, 64, (frame_2%6 + 1) * 64, 64*2);
		else if (12 <= frame_2 && frame_2 < 18)
			SetRect(&part2, (frame_2 % 6) * 64, 64*2, (frame_2 % 6 + 1) * 64, 64*3);
		else if (18 <= frame_2 && frame_2 < 24)
			SetRect(&part2, (frame_2 % 6) * 64, 64*3, (frame_2 % 6 + 1) * 64, 64*4);
		else if (24 <= frame_2 && frame_2 < 30)
			SetRect(&part2, (frame_2 % 6) * 64, 64*4, (frame_2 % 6 + 1) * 64, 64*5);

		D3DXVECTOR3 center2(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position2(enemy.x_pos - 32, enemy.y_pos - 32, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_enemy, &part2, &center2, &position2, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	//런처
	RECT part3;
	SetRect(&part3, 0, 0, 19, 51);
	D3DXVECTOR3 center3(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position3(hero.x_pos + HowHeroCenter + (sin(alpha*DEGTORAD) * HeroToLaunchet), hero.y_pos + HowHeroCenter + (cos(alpha*DEGTORAD) * HeroToLaunchet), 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(sprite_l_bullet, &part3, &center3, &position3, D3DCOLOR_ARGB(255, 255, 255, 255));

	//점수 UI
	T_Score = Score / 10;
	O_Score = Score % 10;

	//십의자리
	RECT part4;
	t_xPos = t_xPos * T_Score;
	SetRect(&part4, (T_Score)*72, 0, ((T_Score+1)*72), 77);
	D3DXVECTOR3 center4(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position4(360.0f, 0.0f, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(sprite_TScore, &part4, &center4, &position4, D3DCOLOR_ARGB(255, 255, 255, 255));

	//일의자리
	RECT part5;
	o_xPos = o_xPos * O_Score;
	if (O_Score == 0)
		SetRect(&part5, (O_Score) * 72, 0, (O_Score+1 * 72), 77);
	else
		SetRect(&part5, (O_Score - 1) * 72, 0, (O_Score * 72), 77);
	D3DXVECTOR3 center5(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position5(432.0f, 0.0f, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(sprite_OScore, &part5, &center5, &position5, D3DCOLOR_ARGB(255, 255, 255, 255));

	if (bullet.eShow == true)
	{
		RECT part6;

		static int frame_1 = 0;

		fEtick += fWorldTick;
		if (fEtick >= 0.01)
		{
			fEtick = 0;

			if (0 <= frame_1 && frame_1 < 6)
			{
				frame_1 = frame_1 + 1;
				SetRect(&part6, x_Pos_, 0, x_Pos_ + 150, 109);
				x_Pos_ = frame_ * 150;
			}
			else if (6 <= frame_1 && frame_1 < 12)
			{
				frame_1 = frame_1 + 1;
				SetRect(&part6, x_Pos_, 109, x_Pos_ + 150, 109 * 2);
				x_Pos_ = (frame_%6) * 150;
			}
			else if (12 <= frame_1 && frame_1 < 18)
			{
				frame_1 = frame_1 + 1;
				SetRect(&part6, x_Pos_, 109 * 2, x_Pos_ + 150, 109 * 3);
				x_Pos_ = (frame_ % 6) * 150;
			}
			else
			{
				frame_1 = 0;
				bullet.e_hide();
			}
		}
		D3DXVECTOR3 center6(50.0, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position6(bullet.n_xpos, bullet.n_ypos, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_effect, &part6, &center6, &position6, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	RECT part7;
	SetRect(&part7, 0, 0, 35, 42);
	D3DXVECTOR3 center7(0.0f, 0.0f, 0.0f);    // center at the upper-left corner

	if (hero.left_Bullet >= 1)
	{
		D3DXVECTOR3 position7(40.0f * 0, 0.0f, 0.0f);
		d3dspt->Draw(sprite_life, &part7, &center7, &position7, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	if (hero.left_Bullet >= 2)
	{
		D3DXVECTOR3 position8(40.0f * 1, 0.0f, 0.0f);
		d3dspt->Draw(sprite_life, &part7, &center7, &position8, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	if (hero.left_Bullet >= 3)
	{
		D3DXVECTOR3 position9(40.0f * 2, 0.0f, 0.0f);
		d3dspt->Draw(sprite_life, &part7, &center7, &position9, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	if (hero.left_Bullet >= 4)
	{
		D3DXVECTOR3 position10(40.0f * 3, 0.0f, 0.0f);
		d3dspt->Draw(sprite_life, &part7, &center7, &position10, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	if (hero.left_Bullet >= 5)
	{
		D3DXVECTOR3 position11(40.0f * 4, 0.0f, 0.0f);
		d3dspt->Draw(sprite_life, &part7, &center7, &position11, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	if(hero.End_Game == true)
	{
		RECT part12;
		SetRect(&part12, 5.0f, 5.0f, 510.0f, 810.0f);
		D3DXVECTOR3 center12(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position12(0.0f, 0.0f, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_ScoreUI, &part12, &center12, &position12, D3DCOLOR_ARGB(255, 255, 255, 255));

		RECT part4;
		t_xPos = t_xPos * T_Score;
		SetRect(&part4, (T_Score) * 72, 0, ((T_Score + 1) * 72), 77);
		D3DXVECTOR3 center4(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position4(178.0f, 300.0f, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_TScore, &part4, &center4, &position4, D3DCOLOR_ARGB(255, 255, 255, 255));

		//일의자리
		RECT part5;
		o_xPos = o_xPos * O_Score;
		if (O_Score == 0)
			SetRect(&part5, (O_Score) * 72, 0, (O_Score + 1 * 72), 77);
		else
			SetRect(&part5, (O_Score - 1) * 72, 0, (O_Score * 72), 77);
		D3DXVECTOR3 center5(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position5(250.0f, 300.0f, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_OScore, &part5, &center5, &position5, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	d3dspt->End();    // end sprite drawing
	d3ddev->EndScene();    // ends the 3D scene
	d3ddev->Present(NULL, NULL, NULL, NULL);

	return;

}


// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	sprite->Release();
	d3ddev->Release();
	d3d->Release();

	//객체 해제 
	sprite_hero->Release();
	sprite_enemy->Release();
	sprite_bullet->Release();
	sprite_l_bullet->Release();
	sprite_TScore->Release();
	sprite_OScore->Release();
	sprite_effect->Release();

	return;
}
