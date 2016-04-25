//======================================================================
/**
*  @file      CBMPLoader.cpp
*
*  ��Ŀ������ ������պ͵���
*  �ļ�����:  ����λͼ�� 
*  ����ƽ̨�� Windows98/2000/NT/XP
*  
*  ���ߣ�     WWBOSS
*  �����ʼ�:  wwboss123@gmail.com
*  �������ڣ� 2006-11-23	
*  �޸����ڣ� 2006-11-26
*	
*/																			
//======================================================================

#include <stdio.h>
#include <windows.h>
#include <gl\gl.h>				 
#include <gl\glu.h>

#include "CBMPLoader.h"              /**< ����ͷ�ļ� */

/** ���캯�� */
CBMPLoader::CBMPLoader()
{
   /** ��ʼ����ԱֵΪ0 */
	image = 0;
	imageWidth = 0;
	imageHeight = 0;
}

/** �������� */
CBMPLoader::~CBMPLoader()
{
   FreeImage(); /**< �ͷ�ͼ������ռ�ݵ��ڴ� */
}

/** װ��һ��λͼ�ļ� */
bool CBMPLoader::LoadBitmap(const char *file)
{
	FILE *pFile = 0; /**< �ļ�ָ�� */
	
	/** ����λͼ�ļ���Ϣ��λͼ�ļ�ͷ�ṹ */
	BITMAPINFOHEADER bitmapInfoHeader;
	BITMAPFILEHEADER header;
  
	/*unsigned char *buffer;*/
	/*unsigned int width = image->sizeX;
	unsigned int height = TextureImage[i]->sizeY;*/

	unsigned char textureColors = 0;/**< ���ڽ�ͼ����ɫ��BGR�任��RGB */

   /** ���ļ�,�������� */
	pFile = fopen(file, "rb");
		if(pFile == 0) return false;

	/** ����λͼ�ļ�ͷ��Ϣ */ 
	fread(&header, sizeof(BITMAPFILEHEADER), 1, pFile);
	
	/** �����ļ��Ƿ�Ϊλͼ�ļ� */
	if(header.bfType != BITMAP_ID)
	   {
		   fclose(pFile);             /**< ������λͼ�ļ�,��ر��ļ������� */
		   return false;
	   }

	/** ����λͼ�ļ���Ϣ */
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, pFile);

	/** ����ͼ��Ŀ�Ⱥ͸߶� */
	imageWidth = bitmapInfoHeader.biWidth;
    imageHeight = bitmapInfoHeader.biHeight;

    /** ȷ����ȡ���ݵĴ�С */
   if(bitmapInfoHeader.biSizeImage == 0)
	   bitmapInfoHeader.biSizeImage = bitmapInfoHeader.biWidth *
	   bitmapInfoHeader.biHeight * 3;

	/** ��ָ���Ƶ����ݿ�ʼλ�� */
	fseek(pFile, header.bfOffBits, SEEK_SET);

	/** �����ڴ� */
	image = new unsigned char[bitmapInfoHeader.biSizeImage];

	/** ����ڴ�����Ƿ�ɹ� */
	if(!image)                        /**< �������ڴ�ʧ���򷵻� */
	   {
		   delete[] image;
		   fclose(pFile);
		   return false;
	   }

	/** ��ȡͼ������ */
	fread(image, 1, bitmapInfoHeader.biSizeImage, pFile);

	/** ��ͼ����ɫ���ݸ�ʽ���н���,��BGRת��ΪRGB */
	for(int index = 0; index < (int)bitmapInfoHeader.biSizeImage; index+=3)
	   {
		   textureColors = image[index];
		   image[index] = image[index + 2];
		   image[index + 2] = textureColors;
	   }
	//-----------------------------��RGB�޸ĳ�RGBA
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
  
	fclose(pFile);       /**< �ر��ļ� */
	return true;         /**< �ɹ����� */
}

/** ����λͼ�ļ������������� */
bool CBMPLoader::Load(const char* fileName)
{
	if (!LoadBitmap(fileName))
	{
		MessageBox(NULL, "����λͼ�ļ�ʧ��!", "����", MB_OK);
		exit(0);
	}

	/** ��������������� */
	glGenTextures(1, &ID);

	/** ����������� */
	glBindTexture(GL_TEXTURE_2D, ID);

	/** �����˲� */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	/** �������� */
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, imageWidth,
		imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
		image);
	return true;
}

/** �ͷ��ڴ� */
void CBMPLoader::FreeImage()
{
   /** �ͷŷ�����ڴ� */
   if(image)
      {
         delete[] image;
         image = 0;
      }
}
