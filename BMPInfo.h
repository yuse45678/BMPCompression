/**�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�[
*��    �ߣ�LeeJiayi		                                               �U
*��	   λ��SCUT��School of Automation Science and Engineering		   �U
* CSDN��https://blog.csdn.net/weixin_47006220?type=blog				   �U
*�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�g
* Copyright LeeJiayi 2022. All rights reserved.
*�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�a
*----------------------------------------------------------------------*/
#pragma pack(1) //����ǳ���Ҫ����ʾ�ṹ�尴��1���ֽ�������������������������
#ifndef BMPINFO_H
#define BMPINFO_H
#define BMPHEADSIZE 14
#define BMPINFOSIZE 40
#include<fstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/opencv.hpp>

using namespace cv;
using namespace std;
namespace LeeJiayi {

	typedef struct tagBITMAPFILEHEADER//BMP�ļ�ͷ(14�ֽ�)
	{
		unsigned short bfType; // λͼ�ļ������ͣ�����ΪBM(0-1�ֽ�)
		unsigned int bfSize; // λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ(2-5�ֽ�)
		unsigned short bfReserved1; // λͼ�ļ������֣�����Ϊ0(6-7�ֽ�)
		unsigned short bfReserved2; // λͼ�ļ������֣�����Ϊ0(8-9�ֽ�)
		unsigned int bfOffBits; // λͼ���ݵ���ʼλ�ã��������λͼ(10-13�ֽ�)
						 // �ļ�ͷ��ƫ������ʾ�����ֽ�Ϊ��λ
	} BitMapFileHeader;

	typedef struct tagBITMAPINFOHEADER  //λͼ��Ϣͷ(40�ֽ�)
	{
		unsigned int biSize; // ���ṹ��ռ���ֽ���(14-17�ֽ�)
		unsigned int biWidth; // λͼ�Ŀ�ȣ�������Ϊ��λ(18-21�ֽ�)
		unsigned int biHeight; // λͼ�ĸ߶ȣ�������Ϊ��λ(22-25�ֽ�)
		unsigned short biPlanes; // Ŀ���豸�ļ��𣬱���Ϊ1(26-27�ֽ�)
		unsigned short biBitCount;// ÿ�����������λ����������1(˫ɫ),(28-29�ֽ�)
						// 4(16ɫ)��8(256ɫ)��24(���ɫ)֮һ
		unsigned int biCompression; // λͼѹ�����ͣ������� 0(��ѹ��),(30-33�ֽ�)
							 // 1(BI_RLE8ѹ������)��2(BI_RLE4ѹ������)֮һ
		unsigned int biSizeImage; // λͼ�Ĵ�С�����ֽ�Ϊ��λ(34-37�ֽ�)
		unsigned int biXPelsPerMeter; // λͼˮƽ�ֱ��ʣ�ÿ��������(38-41�ֽ�)
		unsigned int biYPelsPerMeter; // λͼ��ֱ�ֱ��ʣ�ÿ��������(42-45�ֽ�)
		unsigned int biClrUsed;// λͼʵ��ʹ�õ���ɫ���е���ɫ��(46-49�ֽ�)
		unsigned int biClrImportant;// λͼ��ʾ��������Ҫ����ɫ��(50-53�ֽ�)
	} BitMapInfoHeader;

	typedef struct tagRGBQUAD {
		unsigned char rgbBlue;
		unsigned char rgbGreen;
		unsigned char rgbRed;
		unsigned char rgbReserved;
	}RGB;

	//���ڲ��Զ�ȡBMPͼƬ����
	class ImgBMP
	{
	public:
		BitMapFileHeader bmpFileHeaderData;
		BitMapInfoHeader bmpInfoHeaderData;
		RGB colorMap[256];
		std::vector<uint8_t> imgData;

		inline bool LoadImage(const char* path);
		inline bool SaveImage(const char* path);
	};
#pragma once// reset to default


bool ImgBMP::LoadImage(const char* path)
{
	FILE* pFile;
	errno_t err = fopen_s(&pFile,path, "rb");
	if (err==0)
	{
		// Processing
		fread(&bmpFileHeaderData, sizeof(BitMapFileHeader), 1, pFile);
		if (bmpFileHeaderData.bfType == 0x4D42) // Check is it an RGB file
		{
			// Get Channel num of a pixel
			int channels = 0;
			fread(&bmpInfoHeaderData, sizeof(BitMapInfoHeader), 1, pFile);
			if (bmpInfoHeaderData.biBitCount == 8)// grayscale format
			{

				channels = 1;
				fread(&colorMap, sizeof(RGB), 256, pFile);
			}
			else if (bmpInfoHeaderData.biBitCount == 24)// RGB format
			{
				channels = 3;
			}
			// Get offset of every scanline,length(scanline)=length(pixel)+offset
			int offset = 0;
			int linelength = bmpInfoHeaderData.biWidth * channels;
			offset = linelength % 4;
			if (offset > 0)
			{
				offset = 4 - offset;
			}
			// Read Pixel
			uint8_t pixVal;
			for (int i = 0; i < bmpInfoHeaderData.biHeight; i++)
			{
				for (int j = 0; j < linelength; j++)
				{
					fread(&pixVal, sizeof(uint8_t), 1, pFile);
					imgData.push_back(pixVal);
				}
				for (int k = 0; k < offset; k++)
				{
					fread(&pixVal, sizeof(uint8_t), 1, pFile);
				}
			}
		}
		else
		{
			return false;
		}


		fclose(pFile);
		return true;
	}
	else {
		cout << "���ļ������ϣ���" << endl;
		return false;
	}
}

bool ImgBMP::SaveImage(const char* path)
{
	FILE* pFile;
	errno_t err = fopen_s(&pFile,path, "wb");
	if (!err)
	{
		// Processing
		fwrite(&bmpFileHeaderData, sizeof(BitMapFileHeader), 1, pFile);
		fwrite(&bmpInfoHeaderData, sizeof(BitMapInfoHeader), 1, pFile);
		// Get Channel num of a pixel
		int channels = 0;
		if (bmpInfoHeaderData.biBitCount == 8)
		{
			channels = 1;
			fwrite(&colorMap, sizeof(RGB), 256, pFile);
		}
		else if (bmpInfoHeaderData.biBitCount == 24)
		{
			channels = 3;
		}
		// Get offset of every scanline,length(scanline)=length(pixel)+offset
		int offset = 0;
		int linelength = bmpInfoHeaderData.biWidth * channels;
		offset = (channels * bmpInfoHeaderData.biWidth) % 4;
		if (offset > 0)
		{
			offset = 4 - offset;
		}
		// Write Pixel
		uint8_t pixVal;
		auto iter = imgData.begin();
		for (int i = 0; i < bmpInfoHeaderData.biHeight; i++)
		{
			for (int j = 0; j < linelength; j++)
			{
				pixVal = *iter;
				fwrite(&pixVal, sizeof(uint8_t), 1, pFile);
				iter += 1;
			}
			pixVal = 0;
			for (int k = 0; k < offset; k++)
			{
				fwrite(&pixVal, sizeof(uint8_t), 1, pFile);
			}
		}
		fclose(pFile);
		return true;
	}
	else {
		cout << "����ļ��򿪴����ϣ�" << endl;
		return false;
	}

}



}
#endif // !BMPINFO_H
