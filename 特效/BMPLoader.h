//======================================================================
/**
*  @file      BMPLoader.h
*
*  ��Ŀ������ ����ϵͳ
*  �ļ�����:  ����λͼ�� 
*  ����ƽ̨�� Windows98/2000/NT/XP
*  
*  ���ߣ�     WWBOSS
*  �����ʼ�:  wwboss123@gmail.com
*  �������ڣ� 2006-11-23	
*  �޸����ڣ� 2007-07-10
*	
*/																			
//======================================================================

#ifndef __BMPLOADER_H__
#define __BMPLOADER_H__

#include "stdafx.h"

#define BITMAP_ID 0x4D42	/**< λͼ�ļ��ı�־ */

/** λͼ������ */
class CBMPLoader
{
   public:
      CBMPLoader();
      ~CBMPLoader();

      bool LoadBitmap(const char *filename); /**< װ��һ��bmp�ļ� */
      void FreeImage();                      /**< �ͷ�ͼ������ */
	  bool Load(const char* fileName);       /**< ����λͼ���������� */

      unsigned int ID;                 /**< ���������ID�� */
      int imageWidth;                  /**< ͼ���� */
      int imageHeight;                 /**< ͼ��߶� */
      unsigned char *image;            /**< ָ��ͼ�����ݵ�ָ�� */
};

#endif //__BMPLOADER_H__

