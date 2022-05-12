/**══════════════════════════════════╗
*作    者：LeeJiayi		                                               ║
* CSDN：https://blog.csdn.net/weixin_47006220?type=blog				   ║
*═══════════════════════════════════╣
* Copyright LeeJiayi 2022. All rights reserved.
*═══════════════════════════════════╝
*----------------------------------------------------------------------*/
#pragma pack(1) //这个非常重要！表示结构体按照1个字节来构建！！！！！！！！！
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

	typedef struct tagBITMAPFILEHEADER//BMP文件头(14字节)
	{
		unsigned short bfType; // 位图文件的类型，必须为BM(0-1字节)
		unsigned int bfSize; // 位图文件的大小，以字节为单位(2-5字节)
		unsigned short bfReserved1; // 位图文件保留字，必须为0(6-7字节)
		unsigned short bfReserved2; // 位图文件保留字，必须为0(8-9字节)
		unsigned int bfOffBits; // 位图数据的起始位置，以相对于位图(10-13字节)
						 // 文件头的偏移量表示，以字节为单位
	} BitMapFileHeader;

	typedef struct tagBITMAPINFOHEADER  //位图信息头(40字节)
	{
		unsigned int biSize; // 本结构所占用字节数(14-17字节)
		unsigned int biWidth; // 位图的宽度，以像素为单位(18-21字节)
		unsigned int biHeight; // 位图的高度，以像素为单位(22-25字节)
		unsigned short biPlanes; // 目标设备的级别，必须为1(26-27字节)
		unsigned short biBitCount;// 每个像素所需的位数，必须是1(双色),(28-29字节)
						// 4(16色)，8(256色)或24(真彩色)之一
		unsigned int biCompression; // 位图压缩类型，必须是 0(不压缩),(30-33字节)
							 // 1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一
		unsigned int biSizeImage; // 位图的大小，以字节为单位(34-37字节)
		unsigned int biXPelsPerMeter; // 位图水平分辨率，每米像素数(38-41字节)
		unsigned int biYPelsPerMeter; // 位图垂直分辨率，每米像素数(42-45字节)
		unsigned int biClrUsed;// 位图实际使用的颜色表中的颜色数(46-49字节)
		unsigned int biClrImportant;// 位图显示过程中重要的颜色数(50-53字节)
	} BitMapInfoHeader;

	typedef struct tagRGBQUAD {
		unsigned char rgbBlue;
		unsigned char rgbGreen;
		unsigned char rgbRed;
		unsigned char rgbReserved;
	}RGB;

	//用于测试读取BMP图片的类
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
		cout << "打开文件错误嘞！！" << endl;
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
		cout << "输出文件打开错误嘞！" << endl;
		return false;
	}

}



}
#endif // !BMPINFO_H
