/**�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�[
*��    �ߣ�LeeJiayi		                                               �U
*��	   λ��SCUT��School of Automation Science and Engineering		   �U
* CSDN��https://blog.csdn.net/weixin_47006220?type=blog				   �U
*�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�g
* Copyright LeeJiayi 2022. All rights reserved.
*�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�a
*----------------------------------------------------------------------*/
#include "DPCompress.h"

/**
 * @brief ��������Ҫ�Ƿ��ض�Ӧ��������Ҫʹ�õĶ�����λ��
 * @param val			����
 *
 * @return count		��������Ҫʹ�õĶ�����λ��
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
 * @brief ��������Ҫ�����ڶ������ļ����ж�̬�ָ��㷨����ʵ��ѹ��
 * @param fileSize		�ļ��ܴ�С
 * @param data			�ļ����ݵ�ָ��
 * @param segment		ÿһ�����յķָ��
 * @param segmentBit	���ոö���ʹ�õ�λ��
 *
 * @return SegmentCount	���طָ��ܶ���
 */
int LeeJiayi::GetSegmentation(int fileSize, vector<uchar>& data, vector<int>& segment, vector<int>& segmentBit)
{
	//s:���1��i���ٴ洢λ��
	uint* s = (uint*)malloc(sizeof(uint) * (fileSize + 1));
	// l[i]��ĵĴ�1��i��������ĳ��λ��i-j+1���������i��һ�ε����ų���l[i]
	uint* l = (uint*)malloc(sizeof(uint) * (fileSize + 1));
	// b[i]Ӧ��Ҫ����Ӧ�ε��������λ������ʵ�ʴ����ÿ�����ص���ռλ��
	uint* b = (uint*)malloc(sizeof(uint) * (fileSize + 1));

	int Lmax = 255;  //����������󳤶�
	int header = 11; //ÿһ�ε�ͷ����Ϣ3��Ԫ�������8λ����������ʾ��+8������󳤶�255��=11
	s[0] = 0;
	cout << "��ʼ���ж�̬�滮..." << endl;
	cout << "��������ֵ��..." << endl;
	for (int i = 1; i <= fileSize; i++) {
		//�������ص�p��Ҫ�Ĵ洢λ��
		b[i] = length(data[i - 1]);
		int bMax = b[i];            //�������Ԫ����ռλ�����ֵ
		s[i] = s[i - 1] + 1 * bMax; //ֻ��һ��Ԫ�أ�����*1
		l[i] = 1;

		// jΪ���һ����Ԫ�ظ�����jȡi��255�е���Сֵ
		//�ɿ�����i��ǰ������i��i-1,i�ٵ�i-2,i-1,i...
		for (int j = 2; j <= i && j <= Lmax; j++) {

			//Ԫ������ǰ���£����ܻ���ֱ�bMax����ģ���Ҫ����
			if (bMax < b[i - j + 1]) {
				bMax = b[i - j + 1];
			}

			//�ҵ����õķֶ�  jΪ����Ԫ�ظ���
			if (s[i] > s[i - j] + j * bMax) {
				s[i] = s[i - j] + j * bMax;
				l[i] = j;
			}
		}

		//����ÿһ�ε�ͷ����Ϣ3��Ԫ�������8λ����������ʾ��+8������󳤶�255��
		s[i] += header;
	}

	cout << "��ʼ׷��ÿһ��..." << endl;
	int SegmentCount = 1; //����׷�ٵĶ������Ӻ���ǰ��������ǵ�һ��
	int* SegmentLength = (int*)malloc(sizeof(int) * (fileSize + 1));  //�滮�ֶεĵĳ���
	int* MaxBit = (int*)malloc(sizeof(int) * (fileSize + 1));    //�滮�ֶ�Ԫ���������λ��
	int n = fileSize;
	while (n) {
		SegmentLength[SegmentCount] = l[n];  //�Ӻ���ǰ׷�ٵĵ�j�γ���
		MaxBit[SegmentCount] = b[n];  //�Ӻ���ǰ׷�ٵĵ�j��Ԫ�����λ��
		//����ÿһ��������Ԫ�أ�ȡ���ֵ
		for (int i = n - l[n] + 1; i <= n; i++) {
			if (MaxBit[SegmentCount] < b[i]) {
				MaxBit[SegmentCount] = b[i];
			}
		}
		n = n - l[n]; // n����Ϊ��j��֮ǰ���г���
		SegmentCount++;
	}

	//delete[] s, l, b;
	cout << "�ѻ�ȡ���Ž⣡���ڸ�ͼ�񽫻���Ϊ" << SegmentCount - 1 << "��" << endl;

	for (int i = SegmentCount - 1; i >= 1; i--) {
		//cout << "�γ���:" << SegmentLength[i] << ",����洢λ����" << MaxBit[i] << endl;
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

//��ʼ��д���࣬����д��ѹ�������Ϣ
LeeJiayi::Writer::Writer(FILE* f, int n)
{
	datas = new char[n];
	datas[0] = 0;
	index = 0;
	pos = 0;
	size = n;
	fout = f;
}

//�洢��Ϣ��Ϊ֮��һ����д����׼��
void LeeJiayi::Writer::store(uchar data, int bits)
{
	int rest_bits = 8 - pos;
	if (rest_bits > bits) {
		//����Ҫд����һ���ֽ�
		data = data << (rest_bits - bits);
		datas[index] = datas[index] | data;
		pos += bits;
	}
	else {
		//��Ҫ��һ���ֽ�
		datas[index] = datas[index] | (data >> (bits - rest_bits));
		index++;
		datas[index] = data << (8 - bits + rest_bits);
		pos = (bits - rest_bits);
	}
}

//д��ѹ�������Ϣ
void LeeJiayi::Writer::Write(int SegmentCount, int DataSize, vector<int>& segmentLength, vector<int>& segmentBit, vector<uchar>& data)
{
	fwrite(&SegmentCount, sizeof(int), 1, fout);
	//cout << segmentLength[0];
	for (int i = 0; i < SegmentCount; i++)
	{

		store((unsigned char)segmentLength[i] - 1, 8); //��λ�γ�
		store((unsigned char)segmentBit[i] - 1, 3);//��λ����λ��
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

//���ڳ�ʼ���ָ���
LeeJiayi::Recover::Recover(int SegmentCount, uint* SegmentBit, uint* SegmentLength, uchar* Datas)
{
	m = SegmentCount;
	b = SegmentBit;
	q = SegmentLength;
	datas = Datas;
}

//�ָ��ֶ���Ϣ
int LeeJiayi::Recover::recoverBitInfo()
{
	int section_num = m * 11 % 8 == 0 ? m * 11 / 8 : m * 11 / 8 + 1;//����ȡ��
	unsigned int tmp_q = 0, tmp_b = 0; //�����ʱq��b��Ԫ��
	bool is_q = true;
	int rest_bit_q = 8, rest_bit_b = 3, rest_bit_datas = 8, cnt = 1;
	for (int i = 0; i < section_num; i++)
	{
		rest_bit_datas = 8;
		while (rest_bit_datas != 0)
		{
			if (is_q)   //�������׼����q
			{
				if (rest_bit_datas < rest_bit_q)//�����һ�ֲ���
				{
					ReadData(tmp_q, rest_bit_q, rest_bit_datas, datas[i]);
					rest_bit_q = rest_bit_q - rest_bit_datas; //���������λ��
					rest_bit_datas = 0; //��ǰ8λ������
				}
				else//�����һ�ֹ���
				{
					unsigned char datas_i = datas[i] << (8 - rest_bit_datas);
					datas_i = datas_i >> (rest_bit_datas - rest_bit_q);
					tmp_q = tmp_q + datas_i;
					rest_bit_datas -= rest_bit_q; //����ʣ��λ��
					rest_bit_q = 8; //�����µ�һ��
					is_q = false; //׼����3λ������λ
					q[cnt] = tmp_q + 1; //��ɵ�cnt��Ԫ��
					tmp_q = 0;
				}
			}
			else    //�������׼����b
			{
				if (rest_bit_datas < rest_bit_b)//�����һ�ֲ���
				{
					ReadData(tmp_b, rest_bit_b, rest_bit_datas, datas[i]);
					rest_bit_b = rest_bit_b - rest_bit_datas; //���������λ��
					rest_bit_datas = 0; //��ǰ8λ������
				}
				else//�����һ�ֹ���
				{
					//unsigned char c = datas[i] << (8 - rest_bit_datas);
					//c = c >>  (rest_bit_datas - rest_bit_b);
					//char datas_i =  (datas[i] << (8 - rest_bit_datas) >> (rest_bit_datas - rest_bit_b));
					unsigned char datas_i = datas[i] << (8 - rest_bit_datas);
					datas_i = datas_i >> (8 - rest_bit_b);
					tmp_b = tmp_b + datas_i;
					rest_bit_datas -= rest_bit_b; //����ʣ��λ��
					rest_bit_b = 3; //�����µ�һ��
					is_q = true; //׼����8λ�Ķγ�
					b[cnt] = tmp_b + 1; //��ɵ�cnt��Ԫ��
					cnt++;
					tmp_b = 0;
				}
			}
		}
	}
	return rest_bit_datas; //���ؿճ�������λ
}

//��ȡѹ����Ϣ�е�����
void LeeJiayi::Recover::ReadData(uint& tmp, int need, int give, uchar data)
{
	data = data << (8 - give);
	data = data >> (8 - need);
	tmp = data;
	return;
}

//�ָ�ͼ������
void LeeJiayi::Recover::getPixel(uchar* pBmpBuf, uchar* pDatas, uchar last, uint rest_bits)
{
	int cnt = 1, p_cnt = 0, tmp_need = 8, data_cnt = 0;
	int over_ = 0; //��¼һ���ж�������δ��ȡ
	unsigned char tmp = 0;

	//�������һ���ֽ���ʣ���λ
	while (rest_bits > 0)
	{
		unsigned int group = q[cnt] * b[cnt];  //����һ�����ص���λ��
		if (group <= rest_bits)
		{//�����һ����λ��������rest_bits
			for (unsigned int i = 0; i < q[cnt]; i++)
			{
				pBmpBuf[p_cnt++] = get_hex(rest_bits - i * b[cnt], b[cnt], last);
			}
			rest_bits -= group; //rest_bits����groupλ
			cnt++; //������һ��
		}
		else
		{//�����һ����λ����Խrest_bits
			while (rest_bits >= b[cnt])
			{//��û��Խ�Ĳ��ֶ���
				pBmpBuf[p_cnt++] = get_hex(rest_bits, b[cnt], last);
				rest_bits -= b[cnt];
				over_++;
			}
			over_ = q[cnt] - over_;
			if (rest_bits != 0)
			{//�������λ��Խrest_bits
				tmp = get_hex(rest_bits, rest_bits, last);
				tmp = tmp << (b[cnt] - rest_bits);
				tmp_need = b[cnt];
				tmp_need -= rest_bits; //������λ����Ҫtmp_needλ�����һ������ֵ
				rest_bits = 0; //ʣ��λ����0
			}
		}
	}
	if (over_ == 0)
	{
		cnt++; //�л���һ��
		over_ = q[cnt]; //����δ������������
	}
	//��ʼ��pData���д���
	rest_bits = 8; //��¼��������8λ��ʣ����
	while (cnt <= m)
	{
		while (rest_bits >= tmp_need)
		{//ʣ��λ�����㵱ǰλ����Ҫ
			tmp += get_hex(rest_bits, tmp_need, pDatas[data_cnt]);
			pBmpBuf[p_cnt++] = tmp;

			tmp = 0; //��tmp��0
			rest_bits -= tmp_need;
			over_--; //����δ����������-1
			if (over_ == 0)
			{
				if (cnt == m)
				{
					cnt++;
					break;
				}
				cnt++; //�л���һ��
				over_ = q[cnt]; //����δ������������
			}
			tmp_need = b[cnt];  //������Ҫ��������
		}
		if (rest_bits != 0)
		{//�������һ����û�ж��꣬ͬʱҲ����
			tmp = get_hex(rest_bits, rest_bits, pDatas[data_cnt]);
			tmp = tmp << (b[cnt] - rest_bits);
			tmp_need -= rest_bits; //������λ����Ҫtmp_needλ�����һ������ֵ
			rest_bits = 8; //ʣ��λ����8
		}
		rest_bits = 8; //������һ��8λ
		data_cnt++; //��ȡ��һ���ֽ�
	}
}

/**
 * @brief ��ôӵ�startλ��ʼ����bitsλ��ʮ����
 * @param start			��ʼλ
 * @param bits			����λ
 * @param ch			��Ҫת��������
 *
 * @return ch	���ض�Ӧ�ַ���ʮ����
 */
uchar LeeJiayi::Recover::get_hex(int start, int bits, uchar ch)
{
	ch = ch << (8 - start);
	ch = ch >> (8 - bits);
	return ch;
}
