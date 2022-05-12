/**�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�[
*��    �ߣ�LeeJiayi		                                               �U
*��	   λ��SCUT��School of Automation Science and Engineering		   �U
* CSDN��https://blog.csdn.net/weixin_47006220?type=blog				   �U
*�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�g
* Copyright LeeJiayi 2022. All rights reserved.
*�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�a
*----------------------------------------------------------------------*/
#pragma once
#include"BMPInfo.h"
namespace LeeJiayi {
	typedef unsigned char uchar;
	typedef unsigned int uint;

	int length(uchar val);
	int GetSegmentation(int, vector<uchar>&, vector<int>&, vector<int>&);

	class Writer
	{
	private:
		FILE* fout;
		int size;//д����ֽ�����
		char* datas;//����
		int index;//��ǰд�뵽���ֽ�λ��
		int pos;//��ǰд�뵽���ֽڵ�λλ��
	public:
		Writer(FILE* f, int n);
		void store(uchar data, int bits);
		void Write(int SegmentCount, int DataSize, vector<int>& segmentLength, vector<int>& segmentBit, vector<uchar>& data);
	};

	class Recover
	{
	private:
		int m;
		uint* b;
		uint* q;
		uchar* datas;
	public:
		Recover(int SegmentCount, uint* SegmentBit, uint* SegmentLenth, uchar* Datas);
		int recoverBitInfo();
		static void ReadData(uint& tmp, int need, int give, uchar data);
		void getPixel(uchar* pBmpBuf, uchar* pDatas, uchar last, uint rest_bits);
		static uchar get_hex(int start, int bits, uchar ch);
	};

}