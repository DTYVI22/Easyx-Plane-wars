#include<stdio.h>
#include<graphics.h>
#include<time.h>
#include<iostream>
#include<conio.h>

#define FONTTYPE "宋体"
//宏定义数据
#define BULLET_NUM	15
#define ENEMY_NUM	10

#define ENEMY_SMALL		0
#define ENEMY_MIDDLE	1
#define ENEMY_BIG		2

//枚举宽高
enum window
{
	width = 480,
	height = 700,
};


// 实现文本框控件
class EasyTextBox
{
private:
	int left = 0, top = 0, right = 0, bottom = 0;	// 控件坐标
	wchar_t* text = NULL;							// 控件内容
	size_t maxlen = 0;									// 文本框最大内容长度

public:
	void Create(int x1, int y1, int x2, int y2, int max)
	{
		maxlen = max;
		text = new wchar_t[maxlen];
		text[0] = 0;
		left = x1, top = y1, right = x2, bottom = y2;

		// 绘制用户界面
		Show();
	}

	~EasyTextBox()
	{
		if (text != NULL)
			delete[] text;
	}

	wchar_t* Text()
	{
		return text;
	}

	bool Check(int x, int y)
	{
		return (left <= x && x <= right && top <= y && y <= bottom);
	}

	// 绘制界面
	void Show()
	{
		// 备份环境值
		int oldlinecolor = getlinecolor();
		int oldbkcolor = getbkcolor();
		int oldfillcolor = getfillcolor();

		setlinecolor(LIGHTGRAY);		// 设置画线颜色
		setbkcolor(0xeeeeee);			// 设置背景颜色
		setfillcolor(0xeeeeee);			// 设置填充颜色
		fillrectangle(left, top, right, bottom);
		outtextxy(left + 10, top + 5, text);

		// 恢复环境值
		setlinecolor(oldlinecolor);
		setbkcolor(oldbkcolor);
		setfillcolor(oldfillcolor);
	}

	void OnMessage()
	{
		// 备份环境值
		int oldlinecolor = getlinecolor();
		int oldbkcolor = getbkcolor();
		int oldfillcolor = getfillcolor();

		setlinecolor(BLACK);			// 设置画线颜色
		setbkcolor(WHITE);				// 设置背景颜色
		setfillcolor(WHITE);			// 设置填充颜色
		fillrectangle(left, top, right, bottom);
		outtextxy(left + 10, top + 5, text);

		int width = textwidth(text);	// 字符串总宽度
		int counter = 0;				// 光标闪烁计数器
		bool binput = true;				// 是否输入中

		ExMessage msg;
		while (binput)
		{
			while (binput && peekmessage(&msg, EX_MOUSE | EX_CHAR, false))	// 获取消息，但不从消息队列拿出
			{
				if (msg.message == WM_LBUTTONDOWN)
				{
					// 如果鼠标点击文本框外面，结束文本输入
					if (msg.x < left || msg.x > right || msg.y < top || msg.y > bottom)
					{
						binput = false;
						break;
					}
				}
				else if (msg.message == WM_CHAR)
				{
					size_t len = wcslen(text);
					switch (msg.ch)
					{
					case '\b':				// 用户按退格键，删掉一个字符
						if (len > 0)
						{
							text[len - 1] = 0;
							width = textwidth(text);
							counter = 0;
							clearrectangle(left + 10 + width, top + 1, right - 1, bottom - 1);
						}
						break;
					case '\r':				// 用户按回车键，结束文本输入
					case '\n':
						binput = false;
						break;
					default:				// 用户按其它键，接受文本输入
						if (len < maxlen - 1)
						{
							text[len++] = msg.ch;
							text[len] = 0;
							clearrectangle(left + 10 + width + 1, top + 3, left + 10 + width + 1, bottom - 3);	// 清除画的光标
							width = textwidth(text);				// 重新计算文本框宽度
							counter = 0;
							outtextxy(left + 10, top + 5, text);		// 输出新的字符串
						}
					}
				}
				peekmessage(NULL, EX_MOUSE | EX_CHAR);				// 从消息队列抛弃刚刚处理过的一个消息
			}

			// 绘制光标（光标闪烁周期为 20ms * 32）
			counter = (counter + 1) % 32;
			if (counter < 16)
				line(left + 10 + width + 1, top + 3, left + 10 + width + 1, bottom - 3);				// 画光标
			else
				clearrectangle(left + 10 + width + 1, top + 3, left + 10 + width + 1, bottom - 3);		// 擦光标

			// 延时 20ms
			Sleep(20);
		}

		clearrectangle(left + 10 + width + 1, top + 3, left + 10 + width + 1, bottom - 3);	// 擦光标

		// 恢复环境值
		setlinecolor(oldlinecolor);
		setbkcolor(oldbkcolor);
		setfillcolor(oldfillcolor);

		Show();
	}
};



// 实现按钮控件
class EasyButton
{
private:
	int left = 0, top = 0, right = 0, bottom = 0;	// 控件坐标
	wchar_t* text = NULL;							// 控件内容
	void (*userfunc)() = NULL;						// 控件消息

public:
	void Create(int x1, int y1, int x2, int y2, const wchar_t* title, void (*func)())
	{
		text = new wchar_t[wcslen(title) + 1];
		wcscpy_s(text, wcslen(title) + 1, title);
		left = x1, top = y1, right = x2, bottom = y2;
		userfunc = func;

		// 绘制用户界面
		Show();
	}

	~EasyButton()
	{
		if (text != NULL)
			delete[] text;
	}

	bool Check(int x, int y)
	{
		return (left <= x && x <= right && top <= y && y <= bottom);
	}

	// 绘制界面
	void Show()
	{
		int oldlinecolor = getlinecolor();
		int oldbkcolor = getbkcolor();
		int oldfillcolor = getfillcolor();

		setlinecolor(BLACK);			// 设置画线颜色
		setbkcolor(WHITE);				// 设置背景颜色
		setfillcolor(WHITE);			// 设置填充颜色
		fillrectangle(left, top, right, bottom);
		outtextxy(left + (right - left - textwidth(text) + 1) / 2, top + (bottom - top - textheight(text) + 1) / 2, text);

		setlinecolor(oldlinecolor);
		setbkcolor(oldbkcolor);
		setfillcolor(oldfillcolor);
	}

	void OnMessage()
	{
		if (userfunc != NULL)
			userfunc();
	}
};



// 定义控件
EasyTextBox txtName;
EasyTextBox txtPwd;
EasyButton btnOK;



// 按钮 btnOK 的点击事件
void On_btnOk_Click()
{
	if (wcscmp(L"123", txtPwd.Text()))
		MessageBox(GetHWnd(), L"密码错误", L"错误", MB_OK);
	else
	{
		wchar_t s[100] = L"Hello, ";
		wcscat_s(s, 100, txtName.Text());
		MessageBox(GetHWnd(), s, L"Hello", MB_OK);
	}
}



// 主函数
int main()
{
	// 创建图形窗口
	initgraph(480, 700);

	// 简单绘制界面
	setbkcolor(0xeeeeee);
	cleardevice();
	settextcolor(BLACK);
	outtextxy(50, 55, L"用户名：");
	txtName.Create(120, 50, 400, 75, 10);						// 创建用户名文本框控件
	outtextxy(50, 105, L"密　码：");
	txtPwd.Create(120, 100, 400, 125, 10);						// 创建密码文本框控件
	btnOK.Create(320, 150, 400, 175, L"OK", On_btnOk_Click);	// 创建按钮控件

	ExMessage msg;
	while (true)
	{
		msg = getmessage(EX_MOUSE);			// 获取消息输入

		if (msg.message == WM_LBUTTONDOWN)
		{
			// 判断控件
			if (txtName.Check(msg.x, msg.y))	
				txtName.OnMessage();

			// 判断控件
			if (txtPwd.Check(msg.x, msg.y))		
				txtPwd.OnMessage();

			// 判断控件
			if (btnOK.Check(msg.x, msg.y))		
				btnOK.OnMessage();
		}
	}

	// 关闭绘图窗口
	closegraph();
	return 0;
}


#if 0
//飞机结构体
struct plane
{
	int x;
	int y;
	bool alive;	//是否存活
	int width;	//图片宽度
	int height;	//图片高度
	int hp;		//血量
	int type;	//敌机类型
	int fall;	//陨落
}player, bullet[BULLET_NUM], enemy[ENEMY_NUM];

IMAGE image_background;//背景图
IMAGE image_player[2];//飞机
IMAGE image_bullet;//子弹
IMAGE image_enemy[3];//三种敌机
IMAGE image_enemy_fall[3][6];//敌机坠毁

void drawAlpha(IMAGE* picture, int  picture_x, int picture_y);

bool timer_arrive(int id, int ms)
{
	static DWORD t[100];
	if (clock() - t[id] > ms)
	{
		t[id] = clock();
		return true;
	}
	return false;
}

//音乐
void BGM()//音乐 
{
#if 0
	// alias 取别名,repeat 重复播放
	mciSendString("open ./BGM/战斗.mp3 alias BGM", 0, 0, 0);//打开音乐
	mciSendString("play BGM repat", 0, 0, 0);//播放音乐
	if (0)
	{
		mciSendString("close BGM", 0, 0, 0);
	}
#endif 0
	PlaySound("./BGM/Bitte.WAV", NULL, SND_LOOP | SND_ASYNC);//调用windows函数播放音乐
}

//贴图
void load_image(void)
{
	loadimage(&image_background, "./images/background.png");

	loadimage(&image_player[0], "./images/me1.png");
	loadimage(&image_player[1], "./images/me2.png");

	loadimage(&image_bullet, "./images/bullet1.png");

	loadimage(&image_enemy[0], "./images/enemy1.png");
	loadimage(&image_enemy[1], "./images/enemy2.png");
	loadimage(&image_enemy[2], "./images/enemy3_n1.png");


	loadimage(&image_enemy_fall[0][0], "./images/enemy1_down1.png");
	loadimage(&image_enemy_fall[0][1], "./images/enemy1_down2.png");
	loadimage(&image_enemy_fall[0][2], "./images/enemy1_down3.png");
	loadimage(&image_enemy_fall[0][3], "./images/enemy1_down4.png");

	loadimage(&image_enemy_fall[1][0], "./images/enemy2_down1.png");
	loadimage(&image_enemy_fall[1][1], "./images/enemy2_down2.png");
	loadimage(&image_enemy_fall[1][2], "./images/enemy2_down3.png");
	loadimage(&image_enemy_fall[1][3], "./images/enemy2_down4.png");

	loadimage(&image_enemy_fall[2][0], "./images/enemy3_down1.png");
	loadimage(&image_enemy_fall[2][1], "./images/enemy3_down2.png");
	loadimage(&image_enemy_fall[2][2], "./images/enemy3_down3.png");
	loadimage(&image_enemy_fall[2][3], "./images/enemy3_down4.png");
	loadimage(&image_enemy_fall[2][4], "./images/enemy3_down3.png");
	loadimage(&image_enemy_fall[2][5], "./images/enemy3_down4.png");

}

//生成敌机参数
void enemy_hp(int index)
{
	int num = rand();
	if (num % 20 == 0)
	{
		enemy[index].type = ENEMY_BIG;
		enemy[index].hp = 3;
		enemy[index].height = 258;
		enemy[index].width = 169;
	}
	else if (num % 20 == 2 || num % 20 == 4)
	{
		enemy[index].type = ENEMY_MIDDLE;
		enemy[index].hp = 2;
		enemy[index].height = 99;
		enemy[index].width = 69;
	}
	else
	{
		enemy[index].type = ENEMY_SMALL;
		enemy[index].hp = 1;
		enemy[index].height = 43;
		enemy[index].width = 57;
	}
}

//初始化
void game_init(void)
{
	initgraph(width, height);
	player.x = width / 2 - 51;
	player.y = height - 126;
	player.alive = true;

	for (int i = 0; i < BULLET_NUM; i++)
	{
		bullet[i].x = 0;
		bullet[i].y = 0;
		bullet[i].alive = false;
	}

	for (int i = 0; i < ENEMY_NUM; i++)
	{
		enemy[i].alive = false;
		enemy_hp(i);
	}

}

//绘制
void game_draw(void)
{
	static UINT16 cnt;
	static int fall_cnt[ENEMY_NUM];
	int plane_type, plane_step;

	load_image();
	putimage(0, 0, &image_background);

	if (timer_arrive(0, 10))
	{
		cnt = (cnt + 1) % 10;
	}

	if (cnt < 5)
	{
		drawAlpha(&image_player[0], player.x, player.y);
	}
	else
	{
		drawAlpha(&image_player[1], player.x, player.y);
	}

	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bullet[i].alive)
		{
			drawAlpha(&image_bullet, bullet[i].x, bullet[i].y);
		}
	}

	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].alive)
		{
			if (enemy[i].type == ENEMY_BIG)
			{
				drawAlpha(&image_enemy[2], enemy[i].x, enemy[i].y);
			}
			else if (enemy[i].type == ENEMY_MIDDLE)
			{
				drawAlpha(&image_enemy[1], enemy[i].x, enemy[i].y);
			}
			else if (enemy[i].type == ENEMY_SMALL)
			{
				drawAlpha(&image_enemy[0], enemy[i].x, enemy[i].y);
			}
		}

		if (enemy[i].fall)
		{
			if (timer_arrive(20 + i, 250))
			{
				fall_cnt[i]++;
				plane_type = enemy[i].type;
				plane_step = fall_cnt[i];
				drawAlpha(&image_enemy_fall[plane_type][plane_step], enemy[i].x, enemy[i].y);

				if (fall_cnt[i] > 3)
				{
					enemy[i].fall = false;
					fall_cnt[i] = 0;
				}
			}
		}
	}

}



//
void creat_bullet(void)
{
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (!bullet[i].alive)
		{
			bullet[i].x = player.x + 60;
			bullet[i].y = player.y;
			bullet[i].alive = true;

			break;
		}
	}
}

void creat_enemy(void)
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (!enemy[i].alive)
		{
			enemy[i].x = rand() % (width - 100);
			enemy[i].y = rand() % 150;
			enemy[i].alive = true;
			enemy[i].fall = false;
			enemy_hp(i);
			break;
		}
	}
}


void bullet_move(int speed)
{
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bullet[i].alive)
		{
			bullet[i].y -= speed;

			if (bullet[i].y < 0)
			{
				bullet[i].x = 0;
				bullet[i].y = 0;
				bullet[i].alive = false;
			}
		}
	}

}

void enemy_move(int speed)
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].alive)
		{
			enemy[i].y += speed;

			if (enemy[i].y > height)
			{
				enemy[i].alive = false;
			}
		}
	}
}

void attack_plane(void)
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (!enemy[i].alive)
		{
			continue;
		}

		for (int j = 0; j < BULLET_NUM; j++)
		{
			if (!bullet[j].alive)
			{
				continue;
			}

			if (bullet[j].x > enemy[i].x && bullet[j].x < enemy[i].x + enemy[i].width
				&& bullet[j].y > enemy[i].y && bullet[j].y < enemy[i].y + enemy[i].height)
			{
				bullet[j].alive = false;
				enemy[i].hp--;

				if (enemy[i].hp <= 0)
				{
					enemy[i].alive = false;
					enemy[i].fall = true;
				}

			}
		}



	}
}

//角色移动,上下左右，斜向移动
void player_move(int speed)
{
	if (GetAsyncKeyState(VK_UP))
	{
		player.y -= speed;
		player.y = (player.y < 0) ? 0 : player.y;
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		player.y += speed;
		player.y = (player.y >= height - 126) ? height - 126 : player.y;
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		player.x -= speed;
		player.x = (player.x < -51) ? -51 : player.x;
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		player.x += speed;
		player.x = (player.x >= width - 51) ? width - 51 : player.x;
	}

	if (GetAsyncKeyState(VK_SPACE) && timer_arrive(2, 100))
	{
		creat_bullet();
	}
}

int main(void)
{
	game_init();
	BGM();
	BeginBatchDraw();
	while (1)
	{
		game_draw();
		FlushBatchDraw();
		player_move(5);
		bullet_move(2);
		if (timer_arrive(1, 1000))
		{
			creat_enemy();
		}

		if (timer_arrive(3, 10))
		{
			enemy_move(1);
		}

		attack_plane();
	}
	EndBatchDraw();
	return 0;
}
// 载入PNG图并去透明部分
void drawAlpha(IMAGE* picture, int  picture_x, int picture_y) //x为载入图片的X坐标，y为Y坐标
{
	// 变量初始化
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //获取picture的显存指针
	int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
	int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
	int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
	int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
	int dstX = 0;    //在显存里像素的角标

	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				if ((ix + picture_x) >= 0 && (ix + picture_x) <= graphWidth)	//防止出边界后循环显示
				{
					dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //在显存里像素的角标
					int dr = ((dst[dstX] & 0xff0000) >> 16);
					int dg = ((dst[dstX] & 0xff00) >> 8);
					int db = dst[dstX] & 0xff;
					draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
						| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
						| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
				}
			}
		}
	}
}
#endif

