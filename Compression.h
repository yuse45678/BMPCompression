/**¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨[
*×÷    Õß£ºLeeJiayi		                                               ¨U
* CSDN£ºhttps://blog.csdn.net/weixin_47006220?type=blog				   ¨U
*¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨g
* Copyright LeeJiayi 2022. All rights reserved.
*¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨a
*----------------------------------------------------------------------*/
#pragma once
#ifndef COMPRESSION_H
#define COMPRESSION_H
#include"BMPInfo.h"
#include<iomanip>
#include"LZWCompress.h"
#include "DPCompress.h"
using namespace std;
using namespace cv;
namespace LeeJiayi {

	class Compression
	{
	public:
		static void LzwCompress(const char* Filepath, const char* Savepath);
		static void LzwDecompress(const char* Filepath, const char* Savepath);
		static void DPCompress(const char* Filepath, const char* Savepath);
		static void DPDecompress(const char* Filepath, const char* Savepath);
	};


}
#endif // !COMPRESS_H
