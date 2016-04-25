//======================================================================
/**
*  @file      CBMPLoader.cpp
*
*  项目描述： 构造天空和地面
*  文件描述:  载入位图类 
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-11-23	
*  修改日期： 2006-11-26
*	
*/																			
//======================================================================

#include <stdio.h>
#include <windows.h>
#include <gl\gl.h>				 
#include <gl\glu.h>

#include "CBMPLoader.h"              /**< 包含头文件 */

/** 构造函数 */
CBMPLoader::CBMPLoader()
{
   /** 初始化成员值为0 */
	image = 0;
	imageWidth = 0;
	imageHeight = 0;
}

/** 析构函数 */
CBMPLoader::~CBMPLoader()
{
   FreeImage(); /**< 释放图像数据占据的内存 */
}

/** 装载一个位图文件 */
bool CBMPLoader::LoadBitmap(const char *file)
{
	FILE *pFile = 0; /**< 文件指针 */
	
	/** 创建位图文件信息和位图文件头结构 */
	BITMAPINFOHEADER bitmapInfoHeader;
	BITMAPFILEHEADER header;
  
	/*unsigned char *buffer;*/
	/*unsigned int width = image->sizeX;
	unsigned int height = TextureImage[i]->sizeY;*/

	unsigned char textureColors = 0;/**< 用于将图像颜色从BGR变换到RGB */

   /** 打开文件,并检查错误 */
	pFile = fopen(file, "rb");
		if(pFile == 0) return false;

	/** 读入位图文件头信息 */ 
	fread(&header, sizeof(BITMAPFILEHEADER), 1, pFile);
	
	/** 检查该文件是否为位图文件 */
	if(header.bfType != BITMAP_ID)
	   {
		   fclose(pFile);             /**< 若不是位图文件,则关闭文件并返回 */
		   return false;
	   }

	/** 读入位图文件信息 */
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, pFile);

	/** 保存图像的宽度和高度 */
	imageWidth = bitmapInfoHeader.biWidth;
    imageHeight = bitmapInfoHeader.biHeight;

    /** 确保读取数据的大小 */
   if(bitmapInfoHeader.biSizeImage == 0)
	   bitmapInfoHeader.biSizeImage = bitmapInfoHeader.biWidth *
	   bitmapInfoHeader.biHeight * 3;

	/** 将指针移到数据开始位置 */
	fseek(pFile, header.bfOffBits, SEEK_SET);

	/** 分配内存 */
	image = new unsigned char[bitmapInfoHeader.biSizeImage];

	/** 检查内存分配是否成功 */
	if(!image)                        /**< 若分配内存失败则返回 */
	   {
		   delete[] image;
		   fclose(pFile);
		   return false;
	   }

	/** 读取图像数据 */
	fread(image, 1, bitmapInfoHeader.biSizeImage, pFile);

	/** 将图像颜色数据格式进行交换,由BGR转换为RGB */
	for(int index = 0; index < (int)bitmapInfoHeader.biSizeImage; index+=3)
	   {
		   textureColors = image[index];
		   image[index] = image[index + 2];
		   image[index + 2] = textureColors;
	   }
	//-----------------------------将RGB修改成RGBA
	//buffer = (unsigned char *)malloc(imageWidth*imageHeight * 4);
	//for (int x = 0, int y = 0; x < (*image->SizeX*imageHeight * 3); x += 3, y += 4)
	//{
	//	buffer[y] = image[x];
	//	buffer[y + 1] = image[x + 1];
	//	buffer[y + 2] = image[x + 2];
	//	if (image[x] == 255 && image[x + 1] == 255 && image[x + 2] == 255)
	//	{
	//		buffer[y + 3] = 0;
	//	}
	//	else
	//		buffer[y + 3] = 1;
	//}
  
	fclose(pFile);       /**< 关闭文件 */
	return true;         /**< 成功返回 */
}

/** 载入位图文件，并创建纹理 */
bool CBMPLoader::Load(const char* fileName)
{
	if (!LoadBitmap(fileName))
	{
		MessageBox(NULL, "载入位图文件失败!", "错误", MB_OK);
		exit(0);
	}

	/** 生成纹理对象名称 */
	glGenTextures(1, &ID);

	/** 创建纹理对象 */
	glBindTexture(GL_TEXTURE_2D, ID);

	/** 控制滤波 */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	/** 创建纹理 */
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, imageWidth,
		imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
		image);
	return true;
}

/** 释放内存 */
void CBMPLoader::FreeImage()
{
   /** 释放分配的内存 */
   if(image)
      {
         delete[] image;
         image = 0;
      }
}
