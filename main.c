#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "raylib.h"
#include "tinyfiledialogs.h"

// 按钮绘制函数
static void DrawButton(Vector2 pos, Vector2 size, Color bodyColor, Color outlineColor, Font font, char* text1, char* text2, Color text1Color, Color text2Color)
{
	float sideThick = 5;				//按钮边框厚度
	float sideLength = sideThick * 2;	//倒角直角边长度

	// 绘制按钮
	for (int i = 0; i < 2; i++)
	{
		// 定义顶点
		Vector2 Vertices[13] = {
			{ pos.x + sideLength, pos.y },
			{ pos.x, pos.y + sideLength },
			{ pos.x, pos.y + size.y - sideLength },
			{ pos.x + sideLength, pos.y + size.y },
			{ pos.x + size.x - sideLength, pos.y + size.y },
			{ pos.x + size.x, pos.y + size.y - sideLength },
			{ pos.x + size.x, pos.y + sideLength },
			{ pos.x + size.x - sideLength, pos.y },
			Vertices[0],
			Vertices[2],
			Vertices[4],
			Vertices[6],
			Vertices[0],
		};

		// 绘制按钮
		if (i == 0)
			// 绘制主体
			DrawTriangleStrip(Vertices, 13, outlineColor);
		else
			// 绘制轮廓
			DrawTriangleStrip(Vertices, 13, bodyColor);

		// 绘制文本
		if (i == 1)
		{
			// 计算字体大小
			int textLength = TextLength(text1) + TextLength(text2);
			float fontSize = size.x * 1.4 / textLength;
			Vector2 text1Size = MeasureTextEx(font, text1, fontSize, 1);
			Vector2 text2Size = MeasureTextEx(font, text2, fontSize, 1);

			// 限制最大字体大小
			if (fontSize >= 25)
				fontSize = 25;

			// 避免文本超出按钮框外
			while (text1Size.x + text2Size.x + 5 >= size.x)
			{
				fontSize -= 0.1;
				text1Size = MeasureTextEx(font, text1, fontSize, 1);
				text2Size = MeasureTextEx(font, text2, fontSize, 1);
			}

			// 调整文本位置
			text1Size = MeasureTextEx(font, text1, fontSize, 1);	//如果注释这行代码,文本就会错位
			text2Size = MeasureTextEx(font, text2, fontSize, 1);	// :(
			Vector2 textPos = { pos.x + (size.x - text1Size.x - text2Size.x) / 2.3, pos.y + (size.y - text1Size.y) / 2.1 };

			// 绘制主文本
			DrawTextEx(font, text1, textPos, fontSize, 1, text1Color);

			// 绘制选项文本
			textPos.x += text1Size.x;
			DrawTextEx(font, text2, textPos, fontSize, 1, text2Color);
		}

		pos.x += sideThick;
		pos.y += sideThick;
		size.x -= sideThick * 2;
		size.y -= sideThick * 2;
		sideLength -= sideThick / 2;
	}
}

// 纹理平铺绘制函数
static void TileTexture(Texture texture, Vector2 pos, int wholeSideLength, int number)
{
	float scale = 0.0;
	int maxTextureSide = 0;

	// 计算缩放
	if (texture.width >= texture.height)
		maxTextureSide = texture.width;
	else
		maxTextureSide = texture.height;

	scale = (float)wholeSideLength / (float)maxTextureSide / (float)number;

	// 居中坐标
	pos.x += ((float)wholeSideLength - (float)texture.width * scale * (float)number) / 2.0;
	pos.y += ((float)wholeSideLength - (float)texture.height * scale * (float)number) / 2.0;

	// 绘制纹理
	for (int y = 1; y <= number; y++)
	{
		for (int x = 1; x <= number; x++)
		{
			DrawTextureEx(texture, pos, 0, scale, WHITE);
			pos.x += (float)wholeSideLength / (float)number;
		}

		pos.x = 50;
		pos.y += (float)texture.height * scale;
	}
}

// 在逻辑显示屏中分块绘制纹理的函数
static Texture DrawImageInDisplay(Image image, Vector2 displayPos, float scale, int pixelNumber, int pixelSize, int displayNumber)
{
	Texture texture = LoadTextureFromImage(image);
	Rectangle source = { 0 };
	Rectangle dest = { 0 };
	Vector2 deviation = { 0, 0 };
	pixelSize = (int)pow(2.0, (double)(pixelSize - 1));

	// 设置源矩形尺寸
	source.width = (float)pixelNumber;
	source.height = (float)pixelNumber;

	// 设置目标矩形坐标
	displayPos.x += 9 * scale;
	displayPos.y += 9 * scale;
	dest = (Rectangle){ displayPos.x, displayPos.y, 0, 0 };

	// 计算裁切尺寸偏移值 与 居中坐标
	if (texture.width >= texture.height)
	{
		deviation.x = pixelNumber;
		deviation.y = pixelNumber - ((float)(pixelNumber * displayNumber - texture.height * pixelSize) / 2.0f);

		dest.y += ((float)pixelNumber - deviation.y) * scale;
	}
	else
	{
		deviation.x = pixelNumber - ((float)(pixelNumber * displayNumber - texture.width * pixelSize) / 2.0f);
		deviation.y = pixelNumber;

		dest.x += ((float)pixelNumber - deviation.x) * scale;
	}

	// 绘制纹理
	for (int y = 0; y < displayNumber; y++)
	{
		for (int x = 0; x < displayNumber; x++)
		{
			// 重置裁切尺寸
			source.width = (float)pixelNumber;
			source.height = (float)pixelNumber;

			// 裁切尺寸偏移
			if (texture.width >= texture.height && displayNumber > 1)
			{
				if (y == 0)
				{
					source.width = deviation.x;
					source.height = deviation.y;
				}
			}
			else
			{
				if (x == 0)
				{
					source.width = deviation.x;
					source.height = deviation.y;
				}
			}

			// 裁切尺寸缩放
			source.width /= (float)pixelSize;
			source.height /= (float)pixelSize;

			// 防止越界裁切
			if (source.x + source.width > texture.width)
				source.width = (float)texture.width - source.x;
			if (source.y + source.height > texture.height)
				source.height = (float)texture.height - source.y;

			// 设置目标矩形尺寸
			dest.width = ((source.width * scale * displayNumber) * (float)pixelSize / displayNumber);
			dest.height = ((source.height * scale * displayNumber) * (float)pixelSize / displayNumber);

			// 防止绘制空纹理
			if (source.width > 0 && source.height > 0)
				DrawTexturePro(texture, source, dest, (Vector2) { 0, 0 }, 0, WHITE);

			source.x += source.width;
			dest.x += (18.0f + source.width * pixelSize) * scale;
		}

		source.x = 0.0;
		source.y += source.height;

		if (texture.width > texture.height)
			dest.x = displayPos.x;
		else
			dest.x = displayPos.x + ((float)pixelNumber - deviation.x) * scale;

		dest.y += (18.0f + source.height * pixelSize) * scale;
	}

	return texture;
}

// 获取两个颜色的相似度
static float GetColorSimilarity(Color color1, Color color2)
{
	float deltaR = fabsf((float)color1.r - (float)color2.r);
	float deltaG = fabsf((float)color1.g - (float)color2.g);
	float deltaB = fabsf((float)color1.b - (float)color2.b);

	float distance = sqrt((pow(deltaR, 2.0) * 0.3 + pow(deltaG, 2.0) * 0.59 + pow(deltaB, 2.0) * 0.11));

	return distance;
}

// 获取颜色相似阈值
static float GetColorSimilarityThreshold(Image image, float initialValue)
{
	int logicNumber = 1000;
	float similarityThreshold = initialValue - 0.5;

	while (logicNumber > 998)
	{
		// 迭代
		logicNumber = 0;				//重置逻辑数量计数
		similarityThreshold += 0.5;		//增加相似范围

		bool isFlushed = 0;

		// 计算逻辑代码数量
		for (int y = 0; y < image.height; y++)
		{
			int similarNumber = 0;			//相似像素数量
			Color pixelColor = { 0 };		//当前像素颜色
			Color lastPixelColor = { 0 };	//上一个不同的像素颜色
			isFlushed = 0;					//重置刷新标记

			for (int x = 0; x < image.width; x++)
			{
				// 获取当前像素颜色
				pixelColor = GetImageColor(image, x, y);

				// 行首初始化
				if (x == 0)
				{
					lastPixelColor = pixelColor;
					similarNumber = 1;

					// 跳过当前循环, 等待下一个像素
					continue;
				}

				// 获取颜色相似度
				float similarityValue = GetColorSimilarity(pixelColor, lastPixelColor);

				// 判断像素颜色相似度
				if (similarityValue <= similarityThreshold)
				{
					// 增加相似像素计数
					similarNumber += 1;
				}
				else
				{
					// 增加逻辑数量计数
					logicNumber += 2;

					// 重置记数与记录
					lastPixelColor = pixelColor;
					similarNumber = 1;
				}

				// 行尾绘制
				if (x == image.width - 1)
					logicNumber += 2;
			}

			// 刷新绘制缓冲区 (逻辑代码)
			if ((y + 1) % 2 == 0)
			{
				logicNumber += 1;

				// 标记已刷新
				isFlushed = 1;
			}
		}
	}

	// 返回相似阈值
	return similarityThreshold;
}

// 生成逻辑代码
static void GenerateLogic(FILE* logicFile, Image image, Rectangle rectangle, Vector2 imagePos, int displayX, int displayY, int pixelSize, float similarityThreshold)
{
	int isFlushed = 0;

	// 生成逻辑代码
	for (int y = 0; y < image.height; y++)
	{
		int similarNumber = 0;			//相似像素数量
		int recordX = 0;				//相似像素的起始X坐标
		Color pixelColor = { 0 };		//当前像素颜色
		Color lastPixelColor = { 0 };	//上一个不同的像素颜色
		isFlushed = 0;					//重置刷新标记

		for (int x = 0; x < image.width; x++)
		{
			// 计算坐标
			int drawX = x * pixelSize;
			int drawY = y * pixelSize;
			if (image.width >= image.height)
			{
				if (displayY == 0)
					drawY = (y + imagePos.y) * pixelSize;
			}
			else
			{
				if (displayX == 0)
					drawX = (x + imagePos.x) * pixelSize;
			}

			// 获取当前像素颜色
			pixelColor = GetImageColor(image, x, y);

			// 行首初始化
			if (x == 0)
			{
				lastPixelColor = pixelColor;
				similarNumber = 1;
				recordX = drawX;

				// 跳过当前循环, 等待下一个像素
				continue;
			}

			// 获取颜色相似度
			float similarityValue = GetColorSimilarity(pixelColor, lastPixelColor);

			// 判断像素颜色相似度
			if (similarityValue <= similarityThreshold)
			{
				// 增加相似像素计数
				similarNumber += 1;
			}
			else
			{
				// 完成输出
				fprintf(logicFile, "draw color %d %d %d %d 0 0\n",
					lastPixelColor.r, lastPixelColor.g, lastPixelColor.b, lastPixelColor.a);
				fprintf(logicFile, "draw rect %d %d %d %d 0 0\n",
					recordX, drawY, pixelSize * similarNumber, pixelSize);

				// 重置记数与记录
				lastPixelColor = pixelColor;
				similarNumber = 1;
				recordX = drawX;
			}

			// 行尾绘制
			if (x == image.width - 1)
			{
				if (similarNumber > 1)
				{
					fprintf(logicFile, "draw color %d %d %d %d 0 0\n",
						lastPixelColor.r, lastPixelColor.g, lastPixelColor.b, lastPixelColor.a);
					fprintf(logicFile, "draw rect %d %d %d %d 0 0\n",
						recordX, drawY, pixelSize * similarNumber, pixelSize);
				}
				else
				{
					fprintf(logicFile, "draw color %d %d %d %d 0 0\n",
						pixelColor.r, pixelColor.g, pixelColor.b, pixelColor.a);
					fprintf(logicFile, "draw rect %d %d %d %d 0 0\n",
						drawX, drawY, pixelSize, pixelSize);
				}
			}

		}

		// 刷新绘制缓冲区 (逻辑代码)
		if ((y + 1) % 2 == 0)
		{
			fputs("drawflush display1\n", logicFile);

			// 标记已刷新
			isFlushed = 1;
		}
	}

	// 补充 "刷新绘制缓冲区" (逻辑代码)
	if (isFlushed == 0)
		fputs("drawflush display1\n", logicFile);

	// 停止运行 (逻辑代码)
	fputs("stop\n\n\n", logicFile);

	UnloadImage(image);
	rectangle.x += rectangle.width;
}

int WinMain(void)
{
	InitWindow(1080, 810, "ImageToLogic");
	SetTargetFPS(30);
	SetExitKey(KEY_NULL);
	SetTraceLogLevel(LOG_ERROR);

	// 按钮文本
	char buttonText[7][40] = {
		{"Import image"},
		{"Display type:"},
		{"Display Number:"},
		{"Pixel size:"},
		{"Interpolation:"},
		{"Generate logic"},
		{"Github link"}
	};

	// 选项文本列表
	char optionText[4][20] = {
		{" Middle"},
		{" 1x"},
		{" 1x"},
		{" Nearest"}
	};

	Font font = LoadFontEx("./res/font/font.ttf", 64, 0, 0);						//Mindustry样式字体
	Texture background = LoadTexture("./res/background/windowBack.png");			//窗口背景
	Texture middleDisplayBack = LoadTexture("./res/background/middleBack.png");		//中型逻辑显示屏背景
	Texture middleDisplayFront = LoadTexture("./res/display/middleFront.png");		//中型逻辑显示屏前景
	Texture largeDisplayBack = LoadTexture("./res/background/largeBack.png");		//大型逻辑显示屏背景
	Texture largeDisplayFront = LoadTexture("./res/display/largeFront.png");		//大型逻辑显示屏前景

	Image image = { 0 };			//原始图像
	Image newImage = { 0 };			//缩放后的新图像

	// 选项变量
	int displayType = 0;			//逻辑显示屏类型  0:中型 1:大型
	int displayNumber = 1;			//由逻辑显示屏构成的正方形的边长
	int pixelSize = 1;				//像素尺寸 (实际尺寸应为: 2的pixelSize - 1次方)
	int interpolationType = 0;		//缩放算法类型    0:最近邻 1:双三次

	// 按钮状态
	int state = 0;
	int lastState = 0;

	int wholeSideLength = 600;		//由逻辑显示屏构成的正方形 在屏幕上的最大边长
	int displaySideThick = 0;		//逻辑显示屏的边框线条粗细度
	bool flushImage = 0;			//图像刷新(缩放)标记  0:禁用  1:启用
	bool generateLogic = 0;			//逻辑代码生成标记    0:禁用  1:启用

	while (!WindowShouldClose())
	{
		// 逻辑显示屏纹理切换判断 与 像素计数
		Texture display[2] = { 0 };
		int pixelNumber = 0;

		UnloadTexture(display[0]);
		UnloadTexture(display[1]);

		if (displayType == 0)
		{
			display[0] = middleDisplayBack;
			display[1] = middleDisplayFront;
			pixelNumber = 80;
		}

		if (displayType == 1)
		{
			display[0] = largeDisplayBack;
			display[1] = largeDisplayFront;
			pixelNumber = 176;
		}

		// 逻辑显示屏纹理坐标
		Vector2 displayPos = { 50.0, (810.0 - wholeSideLength) / 2.0 };

		// 调整图像尺寸
		if (flushImage)
		{
			// 释放资源
			if (newImage.data != NULL)
			{
				UnloadImage(newImage);
				newImage.data = NULL;
			}

			// 计算缩放后图像宽高
			float ratio = (float)image.width / (float)image.height;
			float newWidth = 0;
			float newHeight = 0;
			newImage = ImageCopy(image);

			if (image.width >= image.height)
			{
				newWidth = pixelNumber * displayNumber / (float)pow(2.0, (double)(pixelSize - 1));
				newHeight = (float)newWidth / ratio;
			}
			else
			{
				newHeight = pixelNumber * displayNumber / (float)pow(2.0, (double)(pixelSize - 1));
				newWidth = (float)newHeight * ratio;
			}

			// 检查宽高
			if (newWidth < 1)
				newWidth = 1;
			if (newHeight < 1)
				newHeight = 1;

			// 缩放算法类型切换
			if (interpolationType == 0)
				ImageResizeNN(&newImage, (int)newWidth, (int)newHeight);
			if (interpolationType == 1)
				ImageResize(&newImage, (int)newWidth, (int)newHeight);

			flushImage = 0;
		}

		// 图像转换
		if (generateLogic)
		{
			// 文件存储过滤器
			char const* saveFilterPatterns[] = {"*.txt"};

			// 选择文件存储位置
			char* filePath = tinyfd_saveFileDialog(
				"Select the storage location",
				"Logic.txt",
				1,
				saveFilterPatterns,
				"Text file"
			);

			// 判断文件路径是否为空
			if (filePath != NULL)
			{
				// 打开/创建逻辑代码文件
				FILE* logicFile = fopen(filePath, "w");

				Vector2 imagePos = { 0 };			// 图像坐标, 用于居中修正
				Vector2 deviation = { 0 };			// 偏移值, 用于修正裁切范围
				Image croppedImage = { 0 };			// 裁切下的图像
				Rectangle rectangle = { 0 };		// 用于裁切的矩形
				int newPixelSize = (int)pow(2.0, (double)(pixelSize - 1));

				// 垂直翻转图像 (Mindustry 的原点在左下角)
				ImageFlipVertical(&newImage);

				// 计算偏移值
				if (newImage.width >= newImage.height)
				{
					deviation.x = (float)pixelNumber / (float)newPixelSize;
					deviation.y = (float)(pixelNumber - ((float)(pixelNumber * displayNumber - newImage.height * newPixelSize) / 2.0f)) / (float)newPixelSize;

					// 居中坐标
					imagePos.y += (float)pixelNumber / (float)newPixelSize - deviation.y;
				}
				else
				{
					deviation.x = (float)(pixelNumber - ((float)(pixelNumber * displayNumber - newImage.width * newPixelSize) / 2.0f)) / (float)newPixelSize;
					deviation.y = (float)pixelNumber / (float)newPixelSize;

					// 居中坐标
					imagePos.x += (float)pixelNumber / (float)newPixelSize - deviation.x;
				}

				// 生成逻辑代码
				for (int displayY = 0; displayY < displayNumber; displayY++)
				{
					for (int displayX = 0; displayX < displayNumber; displayX++)
					{
						// 重置裁切尺寸
						rectangle.width = (float)pixelNumber / (float)newPixelSize;
						rectangle.height = (float)pixelNumber / (float)newPixelSize;

						// 裁切尺寸偏移
						if (newImage.width >= newImage.height && displayNumber > 1)
						{
							if (displayY == 0)
							{
								rectangle.width = deviation.x;
								rectangle.height = deviation.y;
							}
						}
						else
						{
							if (displayX == 0)
							{
								rectangle.width = deviation.x;
								rectangle.height = deviation.y;
							}
						}

						// 防止越界裁切
						if (rectangle.x + rectangle.width > newImage.width)
							rectangle.width = newImage.width - rectangle.x;
						if (rectangle.y + rectangle.height > newImage.height)
							rectangle.height = newImage.height - rectangle.y;

						// 裁切图像
						croppedImage = ImageFromImage(newImage, rectangle);

						// 分割提示
						fprintf(logicFile, "#Display %d:\n", displayX + displayY * displayNumber + 1);
						// 清空屏幕 (逻辑代码)
						fputs("draw clear 86 86 102 0 0 0\ndrawflush display1\n", logicFile);

						// 获取相似阈值
						float similarityThreshold = GetColorSimilarityThreshold(croppedImage, 10.0);

						// 生成逻辑代码
						GenerateLogic(logicFile, croppedImage, rectangle, imagePos, displayX, displayY, newPixelSize, similarityThreshold);

						rectangle.x = 0.0f;
						rectangle.y += rectangle.height;
					}
				}

				// 还原图像方向
				ImageFlipVertical(&newImage);

				// 关闭文件 清空指针
				fclose(logicFile);
				logicFile = NULL;
			}

			generateLogic = 0;
		}

		// 按钮选项文本切换判断
		if (displayType == 0)
			strcpy(optionText[0], " Middle");
		if (displayType == 1)
			strcpy(optionText[0], " Large");

		if (displayNumber == 1)
			strcpy(optionText[1], " 1x");
		if (displayNumber == 2)
			strcpy(optionText[1], " 2x");
		if (displayNumber == 3)
			strcpy(optionText[1], " 3x");

		if (pixelSize == 1)
			strcpy(optionText[2], " 1x");
		if (pixelSize == 2)
			strcpy(optionText[2], " 2x");
		if (pixelSize == 3)
			strcpy(optionText[2], " 4x");
		if (pixelSize == 4)
			strcpy(optionText[2], " 8x");

		if (interpolationType == 0)
			strcpy(optionText[3], " Nearest");
		if (interpolationType == 1)
			strcpy(optionText[3], " Bicubic");

		int buttonNumber = 7;									//按钮数量
		int buttonSpacing = 60;									//按钮间距
		Color buttonOutlineColor = (Color){ 69, 69, 69, 255 };	//按钮轮廓颜色
		Color buttonText1Color = WHITE;							//按钮主文本颜色
		Vector2 buttonPos = { 60 + displayPos.x + wholeSideLength, displayPos.y - buttonSpacing };
		Vector2 buttonSize = { 300, 90 };
		
		// 检测按钮状态
		Vector2 buttonPosCopy = buttonPos;
		for (int i = 0; i < buttonNumber; i++)
		{
			// 是否选中按钮
			if (GetMouseX() >= buttonPosCopy.x && GetMouseX() <= buttonPosCopy.x + buttonSize.x &&
				GetMouseY() >= buttonPosCopy.y && GetMouseY() <= buttonPosCopy.y + buttonSize.y)
			{
				// 是否按下按钮
				if (IsMouseButtonDown(0))
					state = 2 * 10 + i;
				else
					state = 1 * 10 + i;
				break;
			}
			else
			{
				// 重置状态
				state = -1;
			}
			
			buttonPosCopy.y += buttonSize.y + (wholeSideLength + buttonSpacing * 2 - buttonSize.y * buttonNumber) / (buttonNumber - 1);
		}

		BeginDrawing();
		ClearBackground(WHITE);
		DrawTextureEx(background, (Vector2) { 0, 0 }, 0, (float)GetScreenWidth() / (float)background.width, WHITE);

		// 绘制提示文本
		DrawTextEx(font, "Logic display order:", (Vector2) { 100, 40 }, 24, 1, WHITE);
		DrawTextEx(font, "from left to right, bottom to top.", (Vector2) { 100, 70 }, 24, 1, YELLOW);

		// 绘制逻辑显示屏背景
		TileTexture(display[0], displayPos, wholeSideLength, displayNumber);

		// 绘制预览图像
		Texture imageTexture = DrawImageInDisplay(newImage, displayPos, ((float)wholeSideLength / (float)display[0].width / (float)displayNumber), pixelNumber, pixelSize, displayNumber);
		
		// 绘制逻辑显示屏前景
		TileTexture(display[1], displayPos, wholeSideLength, displayNumber);

		// 绘制按钮
		for (int i = 0; i < buttonNumber; i++)
		{
			// 切换轮廓颜色
			if (state % 10 == i)
			{
				if (state / 10 % 10 == 1)
					buttonOutlineColor = (Color){ 255, 211, 127, 255 };		//被选中
				else if (state / 10 % 10 == 2)
					buttonOutlineColor = (Color){ 255, 255, 255, 255 };		//被按下
			}
			else
				buttonOutlineColor = (Color){ 69, 69, 69, 255 };			//未选中

			// 切换文本颜色
			if (i == 0)
				buttonText1Color = (Color){ 170, 215, 65, 255 };	//"导入图像"
			else if (i == 5)
				buttonText1Color = (Color){ 125, 190, 220, 255 };	//"生成逻辑"
			else if (i == 6)
				buttonText1Color = (Color){ 190, 115, 225, 255 };	//"Github链接"
			else
				buttonText1Color = (Color){ 255, 255, 255, 255 };	//默认

			// 按钮类型判断
			if (i >= 1 && i <= 4)
			{
				// 绘制带有选项的按钮
				DrawButton(buttonPos, buttonSize, BLACK, buttonOutlineColor,
					font, buttonText[i], optionText[i - 1], buttonText1Color, YELLOW);
			}
			else
			{
				// 绘制没有选项的按钮
				DrawButton(buttonPos, buttonSize, BLACK, buttonOutlineColor,
					font, buttonText[i], "", buttonText1Color, YELLOW);
			}

			buttonPos.y += buttonSize.y + (wholeSideLength + buttonSpacing * 2 - buttonSize.y * buttonNumber) / (buttonNumber - 1);
		}
		
		EndDrawing();
		UnloadTexture(imageTexture);

		// 按钮状态判断
		if (state / 10 % 10 == 2 && lastState != state)
		{
			// 导入图像
			if (state % 10 == 0)
			{
				// 文件选择过滤器
				char const* selectFilterPatterns[5] = { "*.jpg", "*.jpeg", "*.png", "*.gif", "*.bmp" };

				// 打开文件选择窗口
				char* filePath = tinyfd_openFileDialog(
					"Select an image",
					"C:\\",
					5,
					selectFilterPatterns,
					"image",
					0
				);

				// 加载图像
				if (filePath != NULL)
				{
					UnloadImage(image);

					image = LoadImage(filePath);

					filePath = NULL;
					flushImage = 1;
				}
			}

			// 逻辑显示屏类型切换
			if (state % 10 == 1)
			{
				if (displayType == 0)
					displayType = 1;
				else
					displayType = 0;

				flushImage = 1;
			}

			// 逻辑显示屏数量切换
			if (state % 10 == 2)
			{
				if (displayNumber < 3)
					displayNumber += 1;
				else
					displayNumber = 1;

				flushImage = 1;
			}

			// 像素尺寸切换
			if (state % 10 == 3)
			{
				if (pixelSize < 4)
					pixelSize += 1;
				else
					pixelSize = 1;

				flushImage = 1;
			}

			// 缩放算法类型切换
			if (state % 10 == 4)
			{
				if (interpolationType == 0)
					interpolationType = 1;
				else
					interpolationType = 0;

				flushImage = 1;
			}

			// 逻辑生成
			if (state % 10 == 5)
				generateLogic = 1;

			// 网址跳转
			if (state % 10 == 6)
				OpenURL("https://github.com/KongAIR-Fan/ImageToLogic");

			//记录状态,避免重复切换
			lastState = state;
		}

		// 按钮状态重置
		if (state / 10 % 10 != 2)
		{
			lastState = -1;
			state = -1;
		}

	}

	CloseWindow();

	UnloadFont(font);
	UnloadTexture(background);
	UnloadTexture(middleDisplayBack);
	UnloadTexture(middleDisplayFront);
	UnloadTexture(largeDisplayBack);
	UnloadTexture(largeDisplayFront);
	UnloadImage(image);
	UnloadImage(newImage);

	return 0;
}