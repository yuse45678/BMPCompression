/**══════════════════════════════════╗
*作    者：LeeJiayi		                                               ║
*单	   位：SCUT，School of Automation Science and Engineering		   ║
* CSDN：https://blog.csdn.net/weixin_47006220?type=blog				   ║
*═══════════════════════════════════╣
* Copyright LeeJiayi 2022. All rights reserved.
*═══════════════════════════════════╝
*----------------------------------------------------------------------*/
#include "DPCompress.h"

/**
 * @brief 本函数主要是返回对应数据所需要使用的二进制位数
 * @param val			数据
 *
 * @return count		返回所需要使用的二进制位数
 */
int LeeJiayi::length(uchar val)
{
	int count = 1;
	val /= 2;

	while (val > 0) {
		count++;
		val /= 2;
	}

	return count;
}

/**
 * @brief 本函数主要是用于对数据文件进行动态分割算法，以实现压缩
 * @param fileSize		文件总大小
 * @param data			文件数据的指针
 * @param segment		每一段最终的分割长度
 * @param segmentBit	最终该段所使用的位数
 *
 * @return SegmentCount	返回分割总段数
 */
int LeeJiayi::GetSegmentation(int fileSize, vector<uchar>& data, vector<int>& segment, vector<int>& segmentBit)
{
	//s:存从1到i最少存储位数
	uint* s = (uint*)malloc(sizeof(uint) * (fileSize + 1));
	// l[i]存的的从1到i的序列中某个位置i-j+1隔开到最后i这一段的最优长度l[i]
	uint* l = (uint*)malloc(sizeof(uint) * (fileSize + 1));
	// b[i]应该要存相应段的最大像素位数，但实际存的是每个像素的所占位数
	uint* b = (uint*)malloc(sizeof(uint) * (fileSize + 1));

	int Lmax = 255;  //像素序列最大长度
	int header = 11; //每一段的头部信息3（元素最多用8位二进制数表示）+8（段最大长度255）=11
	s[0] = 0;
	cout << "开始进行动态规划..." << endl;
	cout << "计算最优值中..." << endl;
	for (int i = 1; i <= fileSize; i++) {
		//计算像素点p需要的存储位数
		b[i] = length(data[i - 1]);
		int bMax = b[i];            //后面段中元素所占位数最大值
		s[i] = s[i - 1] + 1 * bMax; //只有一个元素，所以*1
		l[i] = 1;

		// j为最后一段中元素个数，j取i和255中的最小值
		//可看作从i往前逐渐扩大i到i-1,i再到i-2,i-1,i...
		for (int j = 2; j <= i && j <= Lmax; j++) {

			//元素逐渐向前更新，可能会出现比bMax更大的，需要更新
			if (bMax < b[i - j + 1]) {
				bMax = b[i - j + 1];
			}

			//找到更好的分段  j为段中元素个数
			if (s[i] > s[i - j] + j * bMax) {
				s[i] = s[i - j] + j * bMax;
				l[i] = j;
			}
		}

		//加上每一段的头部信息3（元素最多用8位二进制数表示）+8（段最大长度255）
		s[i] += header;
	}

	cout << "开始追踪每一段..." << endl;
	int SegmentCount = 1; //正在追踪的段数，从后往前，最后面是第一段
	int* SegmentLength = (int*)malloc(sizeof(int) * (fileSize + 1));  //存划分段的的长度
	int* MaxBit = (int*)malloc(sizeof(int) * (fileSize + 1));    //存划分段元素所需最大位数
	int n = fileSize;
	while (n) {
		SegmentLength[SegmentCount] = l[n];  //从后往前追踪的第j段长度
		MaxBit[SegmentCount] = b[n];  //从后往前追踪的第j段元素最大位数
		//遍历每一个段所有元素，取最大值
		for (int i = n - l[n] + 1; i <= n; i++) {
			if (MaxBit[SegmentCount] < b[i]) {
				MaxBit[SegmentCount] = b[i];
			}
		}
		n = n - l[n]; // n更新为第j段之前序列长度
		SegmentCount++;
	}

	//delete[] s, l, b;
	cout << "已获取最优解！对于该图像将划分为" << SegmentCount - 1 << "段" << endl;

	for (int i = SegmentCount - 1; i >= 1; i--) {
		//cout << "段长度:" << SegmentLength[i] << ",所需存储位数：" << MaxBit[i] << endl;
		segment.push_back(SegmentLength[i]);
		segmentBit.push_back(MaxBit[i]);
	}

	free(s);
	free(l);
	free(b);
	free(SegmentLength);
	free(MaxBit);
	s = NULL; l = NULL; b = NULL; SegmentLength = NULL; MaxBit = NULL;
	return SegmentCount - 1;
}

//初始化写入类，用于写入压缩后的信息
LeeJiayi::Writer::Writer(FILE* f, int n)
{
	datas = new char[n];
	datas[0] = 0;
	index = 0;
	pos = 0;
	size = n;
	fout = f;
}

//存储信息，为之后一次性写入做准备
void LeeJiayi::Writer::store(uchar data, int bits)
{
	int rest_bits = 8 - pos;
	if (rest_bits > bits) {
		//不需要写入下一个字节
		data = data << (rest_bits - bits);
		datas[index] = datas[index] | data;
		pos += bits;
	}
	else {
		//需要下一个字节
		datas[index] = datas[index] | (data >> (bits - rest_bits));
		index++;
		datas[index] = data << (8 - bits + rest_bits);
		pos = (bits - rest_bits);
	}
}

//写入压缩后的信息
void LeeJiayi::Writer::Write(int SegmentCount, int DataSize, vector<int>& segmentLength, vector<int>& segmentBit, vector<uchar>& data)
{
	fwrite(&SegmentCount, sizeof(int), 1, fout);
	//cout << segmentLength[0];
	for (int i = 0; i < SegmentCount; i++)
	{

		store((unsigned char)segmentLength[i] - 1, 8); //八位段长
		store((unsigned char)segmentBit[i] - 1, 3);//三位像素位数
	}
	int pBit = segmentBit[0], cnt = 0, pLen = segmentLength[0];
	for (int i = 0; i < DataSize; i++)
	{
		if (pLen == 0) { pBit = segmentBit[cnt]; pLen = segmentLength[cnt]; cnt++; }
		store(data[i], pBit);
		pLen--;
	}
	fwrite(datas, sizeof(char), size, fout);
}

//用于初始化恢复类
LeeJiayi::Recover::Recover(int SegmentCount, uint* SegmentBit, uint* SegmentLength, uchar* Datas)
{
	m = SegmentCount;
	b = SegmentBit;
	q = SegmentLength;
	datas = Datas;
}

//恢复分段信息
int LeeJiayi::Recover::recoverBitInfo()
{
	int section_num = m * 11 % 8 == 0 ? m * 11 / 8 : m * 11 / 8 + 1;//向上取整
	unsigned int tmp_q = 0, tmp_b = 0; //存放临时q、b的元素
	bool is_q = true;
	int rest_bit_q = 8, rest_bit_b = 3, rest_bit_datas = 8, cnt = 1;
	for (int i = 0; i < section_num; i++)
	{
		rest_bit_datas = 8;
		while (rest_bit_datas != 0)
		{
			if (is_q)   //如果现在准备读q
			{
				if (rest_bit_datas < rest_bit_q)//如果这一轮不够
				{
					ReadData(tmp_q, rest_bit_q, rest_bit_datas, datas[i]);
					rest_bit_q = rest_bit_q - rest_bit_datas; //更新需求的位数
					rest_bit_datas = 0; //当前8位被读完
				}
				else//如果这一轮够了
				{
					unsigned char datas_i = datas[i] << (8 - rest_bit_datas);
					datas_i = datas_i >> (rest_bit_datas - rest_bit_q);
					tmp_q = tmp_q + datas_i;
					rest_bit_datas -= rest_bit_q; //更新剩余位数
					rest_bit_q = 8; //开启新的一轮
					is_q = false; //准备读3位的像素位
					q[cnt] = tmp_q + 1; //完成第cnt个元素
					tmp_q = 0;
				}
			}
			else    //如果现在准备读b
			{
				if (rest_bit_datas < rest_bit_b)//如果这一轮不够
				{
					ReadData(tmp_b, rest_bit_b, rest_bit_datas, datas[i]);
					rest_bit_b = rest_bit_b - rest_bit_datas; //更新需求的位数
					rest_bit_datas = 0; //当前8位被读完
				}
				else//如果这一轮够了
				{
					//unsigned char c = datas[i] << (8 - rest_bit_datas);
					//c = c >>  (rest_bit_datas - rest_bit_b);
					//char datas_i =  (datas[i] << (8 - rest_bit_datas) >> (rest_bit_datas - rest_bit_b));
					unsigned char datas_i = datas[i] << (8 - rest_bit_datas);
					datas_i = datas_i >> (8 - rest_bit_b);
					tmp_b = tmp_b + datas_i;
					rest_bit_datas -= rest_bit_b; //更新剩余位数
					rest_bit_b = 3; //开启新的一轮
					is_q = true; //准备读8位的段长
					b[cnt] = tmp_b + 1; //完成第cnt个元素
					cnt++;
					tmp_b = 0;
				}
			}
		}
	}
	return rest_bit_datas; //返回空出来多少位
}

//读取压缩信息中的数据
void LeeJiayi::Recover::ReadData(uint& tmp, int need, int give, uchar data)
{
	data = data << (8 - give);
	data = data >> (8 - need);
	tmp = data;
	return;
}

//恢复图像数据
void LeeJiayi::Recover::getPixel(uchar* pBmpBuf, uchar* pDatas, uchar last, uint rest_bits)
{
	int cnt = 1, p_cnt = 0, tmp_need = 8, data_cnt = 0;
	int over_ = 0; //记录一组有多少像素未读取
	unsigned char tmp = 0;

	//处理最后一个字节中剩余的位
	while (rest_bits > 0)
	{
		unsigned int group = q[cnt] * b[cnt];  //计算一段像素的总位数
		if (group <= rest_bits)
		{//如果这一段总位数不超过rest_bits
			for (unsigned int i = 0; i < q[cnt]; i++)
			{
				pBmpBuf[p_cnt++] = get_hex(rest_bits - i * b[cnt], b[cnt], last);
			}
			rest_bits -= group; //rest_bits减少group位
			cnt++; //进入下一组
		}
		else
		{//如果这一组总位数跨越rest_bits
			while (rest_bits >= b[cnt])
			{//把没跨越的部分读完
				pBmpBuf[p_cnt++] = get_hex(rest_bits, b[cnt], last);
				rest_bits -= b[cnt];
				over_++;
			}
			over_ = q[cnt] - over_;
			if (rest_bits != 0)
			{//如果像素位跨越rest_bits
				tmp = get_hex(rest_bits, rest_bits, last);
				tmp = tmp << (b[cnt] - rest_bits);
				tmp_need = b[cnt];
				tmp_need -= rest_bits; //该像素位还需要tmp_need位来填充一个像素值
				rest_bits = 0; //剩余位数置0
			}
		}
	}
	if (over_ == 0)
	{
		cnt++; //切换下一组
		over_ = q[cnt]; //更新未读的像素数量
	}
	//开始对pData进行处理
	rest_bits = 8; //记录待解析的8位还剩多少
	while (cnt <= m)
	{
		while (rest_bits >= tmp_need)
		{//剩余位数满足当前位数需要
			tmp += get_hex(rest_bits, tmp_need, pDatas[data_cnt]);
			pBmpBuf[p_cnt++] = tmp;

			tmp = 0; //将tmp置0
			rest_bits -= tmp_need;
			over_--; //该组未读的像素量-1
			if (over_ == 0)
			{
				if (cnt == m)
				{
					cnt++;
					break;
				}
				cnt++; //切换下一组
				over_ = q[cnt]; //更新未读的像素数量
			}
			tmp_need = b[cnt];  //更新需要的像素量
		}
		if (rest_bits != 0)
		{//如果留了一部分没有读完，同时也不够
			tmp = get_hex(rest_bits, rest_bits, pDatas[data_cnt]);
			tmp = tmp << (b[cnt] - rest_bits);
			tmp_need -= rest_bits; //该像素位还需要tmp_need位来填充一个像素值
			rest_bits = 8; //剩余位数置8
		}
		rest_bits = 8; //进入下一个8位
		data_cnt++; //读取下一个字节
	}
}

/**
 * @brief 获得从第start位开始，共bits位的十进制
 * @param start			开始位
 * @param bits			最终位
 * @param ch			需要转换的数据
 *
 * @return ch	返回对应字符的十进制
 */
uchar LeeJiayi::Recover::get_hex(int start, int bits, uchar ch)
{
	ch = ch << (8 - start);
	ch = ch >> (8 - bits);
	return ch;
}
