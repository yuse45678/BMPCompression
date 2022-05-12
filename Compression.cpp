/**�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�[
*��    �ߣ�LeeJiayi		                                               �U
*��	   λ��SCUT��School of Automation Science and Engineering		   �U
* CSDN��https://blog.csdn.net/weixin_47006220?type=blog				   �U
*�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�g
* Copyright LeeJiayi 2022. All rights reserved.
*�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�a
*----------------------------------------------------------------------*/
#include "Compression.h"

/**
 * @brief LZWѹ������
 * @param Filepath		��Ҫѹ�����ļ�·��
 * @param Savepath		ѹ�����ļ��洢·��
 *
 * @return ��
 */
void LeeJiayi::Compression::LzwCompress(const char* Filepath, const char* Savepath)
{
	BitMapFileHeader bmpFileHeaderData;	//BMP�ļ�ͷ
	BitMapInfoHeader bmpInfoHeaderData;	//BMP��Ϣͷ
	RGB colorMap[256];					//BMP��ɫ�������24λ���ɫ�����������⣩
	vector<uchar> imgData;
	int channels = 0;
	FILE* inFile;
	errno_t err = fopen_s(&inFile, Filepath, "rb");
	if (err == 0) {
		cout << "�Ѽ���" << Filepath << endl;
		cout << "��ʼ��ȡͼ������" << endl;
		fread(&bmpFileHeaderData, sizeof(BitMapFileHeader), 1, inFile);
		fread(&bmpInfoHeaderData, sizeof(BitMapInfoHeader), 1, inFile);
		
		//�����8λλͼ�Ͷ�ȡ��ɫ��
		//���������ͼƬ����8λλͼ
		if (bmpInfoHeaderData.biBitCount == 8) {
			fread(&colorMap, sizeof(RGB), 256, inFile);
		}

		uchar pixVal;//������ʱ�洢��ȡ��������Ϣ
		int imgSize = bmpInfoHeaderData.biHeight * bmpInfoHeaderData.biWidth * bmpInfoHeaderData.biBitCount / 8;
		//��ȡ������Ϣ���洢��imgData��
		for (int i = 0; i < imgSize; i++) {
			fread(&pixVal, sizeof(uchar), 1, inFile);
			imgData.push_back(pixVal);
			}
		//����ͼ���ļ��Ĵ�С�����ں���ѹ���ȵļ��㣡
		float inFileSize = ftell(inFile);
		fclose(inFile);
		//����LZWѹ����
		LZWcompress lzw(imgData, imgData.size());
		//LZW����
		lzw.LZW_encode();
		int dataSize=0;
		//��ȡLZW����������
		uchar* lzwData=lzw.get_lzw_encode(dataSize);
		
		cout << "��ʼ����ѹ���ļ���д��...." << endl;
		FILE* fout;

		errno_t err = fopen_s(&fout, Savepath, "wb");
		if (!err) {
			//д���ļ�ͷ
			fwrite(&bmpFileHeaderData, sizeof(bmpFileHeaderData), 1, fout);
			//д����Ϣͷ
			fwrite(&bmpInfoHeaderData, sizeof(bmpInfoHeaderData), 1, fout);
			//8λλͼ��д����ɫ��
			if (bmpInfoHeaderData.biBitCount == 8) {
				fwrite(&colorMap, sizeof(RGB), 256, fout);
			}
			//д��ѹ�������������
			fwrite(lzwData,dataSize,1,fout);
			//����ѹ������ļ���С
			float FileSize = ftell(fout);
			fclose(fout);

			cout << "����ɶ�"<<Savepath <<"��д�룡" << endl;
			cout << "* ---------------------------------------------------------" << endl;
			cout << Filepath<<"��ѹ����Ϊ" << FileSize / inFileSize << endl;;
			cout << "* ---------------------------------------------------------" << endl;
		}
		else {
			cout << "�޷��򿪽�Ҫд����ļ��������ļ�·������" << endl;
		}
	}
	else {
		cout << "�޷���ͼ���ļ��������ļ�·������" << endl;
	}
}

/**
 * @brief LZW��ѹ������
 * @param Filepath		��Ҫ��ѹ�����ļ�·��
 * @param Savepath		��ѹ�����ļ��洢·��
 *
 * @return ��
 */
void LeeJiayi::Compression::LzwDecompress(const char* Filepath, const char* Savepath)
{
	BitMapFileHeader fileHeader;
	BitMapInfoHeader infoHeader;
	RGB colorMap[256];
	FILE* inFile;
	errno_t err = fopen_s(&inFile, Filepath, "rb");
	if (!err) {
		cout << "��ʼ��ȡ "<<Filepath<<" ѹ���ļ�����" << endl;
		//��ȡ λͼ��Ϣ �е�ͼ���� �߶� �� λ���
		fread(&fileHeader, sizeof(BitMapFileHeader), 1, inFile);
		fread(&infoHeader, sizeof(BitMapInfoHeader), 1, inFile);

		if (infoHeader.biBitCount == 8) {
			fread(&colorMap, sizeof(RGB), 256, inFile);
		}

		uchar* lzwSizeInfo = new uchar[4];
		fread(lzwSizeInfo, sizeof(uchar), 4, inFile);

		cout << "��ʼ��ԭ��Ϣ.." << endl;
		uint encode_size;
		string encodeSizeStr = "";
		for (int i = 0; i < 4; i++) {
			encodeSizeStr += Char2Hex(lzwSizeInfo[i]);
		}
		encode_size = stoi(encodeSizeStr, nullptr, 16);
		uchar* lzw_encode_data = new uchar[encode_size];
		fread(lzw_encode_data, sizeof(uchar), encode_size, inFile);
		fclose(inFile);

		LZWcompress* lzw = new LZWcompress();
		lzw->set_encode_data(lzw_encode_data, encode_size);
		lzw->LZW_decode();
		int dataSize = 0;
		uchar* data=lzw->get_lzw_decode(dataSize);

		FILE* fout;
		err = fopen_s(&fout, Savepath, "wb");
		if (!err) {
			cout << "��ʼд��ͼ������" << endl;
			fwrite(&fileHeader, sizeof(BitMapFileHeader), 1, fout);
			fwrite(&infoHeader, sizeof(BitMapInfoHeader), 1, fout);

			if (infoHeader.biBitCount == 8) {
				fwrite(&colorMap, sizeof(RGB), 256, fout);
			}

			fwrite(data, dataSize, 1, fout);
			fclose(fout);
	
			cout << "����ɶ�"<<Savepath<<"��д�룡"<< endl;
			cout << "* ---------------------------------------------------------" << endl;
		}
		else {
			cout << "�޷��򿪽�Ҫд����ļ��������ļ�·������" << endl;
		}
	}
	else {
		cout << "�޷��򿪽�Ҫ��ѹ���ļ��������ļ�·������" << endl;
	}
}

/**
 * @brief DPѹ������
 * @param Filepath		��Ҫѹ�����ļ�·��
 * @param Savepath		ѹ�����ļ��洢·��
 *
 * @return ��
 */
void LeeJiayi::Compression::DPCompress(const char* Filepath, const char* Savepath)
{
	BitMapFileHeader bmpFileHeaderData;	//BMP�ļ�ͷ
	BitMapInfoHeader bmpInfoHeaderData;	//BMP��Ϣͷ
	RGB colorMap[256];					//BMP��ɫ�������24λ���ɫ�����������⣩
	vector<uchar> imgData;
	int channels = 0;
	FILE* inFile;
	errno_t err = fopen_s(&inFile, Filepath, "rb");
	if (err == 0) {
		cout << "��ʼ��ȡ"<< Filepath<<"ͼ������" << endl;
		fread(&bmpFileHeaderData, sizeof(BitMapFileHeader), 1, inFile);
		fread(&bmpInfoHeaderData, sizeof(BitMapInfoHeader), 1, inFile);

		//�����8λλͼ�Ͷ�ȡ��ɫ��
		//���������ͼƬ����8λλͼ
		if (bmpInfoHeaderData.biBitCount == 8) {
			fread(&colorMap, sizeof(RGB), 256, inFile);
		}


		uchar pixVal;//������ʱ�洢��ȡ��������Ϣ
		int imgSize = bmpInfoHeaderData.biHeight * bmpInfoHeaderData.biWidth * bmpInfoHeaderData.biBitCount / 8;
		
		//��ȡ������Ϣ
		for (int i = 0; i < imgSize; i++) {
			fread(&pixVal, sizeof(uchar), 1, inFile);
			imgData.push_back(pixVal);
		}


		//��̬�滮������ֶ���Ϣ������ʹ�õ�λ��
		vector<int> segment;
		vector<int> segmentBit;
		int SegmentCount = GetSegmentation(imgData.size(), imgData, segment, segmentBit);

		cout << "��ʼ����ѹ���ļ���д��...." << endl;

		//32λ�������ж��ٶΣ���SegmentCount��;
		//SegmentCount*11����SegmentCount�Σ�ÿ�ε�8λ���������εĳ��ȣ�3λ������������λ��
		int total_bits = 32 + SegmentCount * 11; 

		for (int i = 0; i < SegmentCount; i++)
		{
			total_bits += segment[i] * segmentBit[i]; //�������������ж���λ
		}
		int total_bytes = total_bits % 8 == 0 ? total_bits / 8 : total_bits / 8 + 1; //����ȡ�������ֽ���
		FILE* fout;

		errno_t err = fopen_s(&fout, Savepath, "wb");
		if (!err) {
			fwrite(&bmpFileHeaderData, sizeof(bmpFileHeaderData), 1, fout);
			fwrite(&bmpInfoHeaderData, sizeof(bmpInfoHeaderData), 1, fout);

			if (bmpInfoHeaderData.biBitCount == 8) {
				fwrite(&colorMap, sizeof(RGB), 256, fout);
			}
			
			//д��ѹ���������
			Writer w(fout, total_bytes);
			w.Write(SegmentCount, imgData.size(), segment, segmentBit, imgData);
			fclose(fout);
			cout << "�����" << Savepath << "��д�룡" << endl;
		}
		else {
			cout << "�޷��򿪽�Ҫд����ļ��������ļ�·������" << endl;
		}
	}
	else {
		cout << "�޷���ͼ���ļ��������ļ�·������" << endl;
	}
}

/**
 * @brief DP��ѹ������
 * @param Filepath		��Ҫ��ѹ�����ļ�·��
 * @param Savepath		��ѹ�����ļ��洢·��
 *
 * @return ��
 */
void LeeJiayi::Compression::DPDecompress(const char* Filepath, const char* Savepath)
{
	BitMapFileHeader fileHeader;
	BitMapInfoHeader infoHeader;
	RGB colorMap[256];
	FILE* inFile;
	errno_t err = fopen_s(&inFile, Filepath, "rb");
	if (!err) {
		cout << "��ʼ��ȡ"<< Filepath<<"����" << endl;
		
		fread(&fileHeader, sizeof(BitMapFileHeader), 1, inFile);
		fread(&infoHeader, sizeof(BitMapInfoHeader), 1, inFile);

		if (infoHeader.biBitCount == 8) {
			fread(&colorMap, sizeof(RGB), 256, inFile);
		}

		int SegmentCount = 0; //��¼�ж��ٶ�

		fread(&SegmentCount, sizeof(int), 1, inFile); //��ȡ�ж��ٶ�
		uint* SegmentBit = new uint[SegmentCount + 1];  //��i��������ռλ��
		uint* SegmentLength = new uint[SegmentCount + 1];  //��i���м�������

		cout << "��ʼ��ԭ��Ϣ" << endl;
		//��ԭSegmentBit��SegmentLength��Ϣ
		int rest_bits = 0;//��¼ʣ�µ�λ��
		int section_num = SegmentCount * 11 % 8 == 0 ? SegmentCount * 11 / 8 : SegmentCount * 11 / 8 + 1;//����ȡ��
		if (SegmentCount * 11 % 8 != 0) { rest_bits = 8 - (SegmentCount * 11 - SegmentCount * 11 / 8 * 8); }
		uchar* datas = new uchar[section_num];
		fread(datas, sizeof(char), section_num, inFile);
		Recover r(SegmentCount, SegmentBit, SegmentLength, datas);//�����ָ���
		r.recoverBitInfo();//�ָ��ֶ�λ����ֶθ���
		
		int p_sum = 0;//����ÿһ�εĳ���
		for (int i = 1; i <= SegmentCount; i++)
		{
			p_sum += SegmentLength[i];
		}
		//�������յ���λ���Լ��������ֽ��������ڻָ�ʹ��
		int pBit_sum = 0, qByte_sum = 0;
		for (int i = 1; i <= SegmentCount; i++)
		{
			pBit_sum += SegmentLength[i] * SegmentBit[i];
		}
		qByte_sum = pBit_sum % 8 == 0 ? pBit_sum / 8 : pBit_sum / 8 + 1;

		uchar* pBmpBuf = new uchar[p_sum]; //�洢������������
		uchar* pBmpDatas = new uchar[qByte_sum];//�洢δ��������������
		fread(pBmpDatas, sizeof(uchar), qByte_sum, inFile); //��ȡ����

		//�ָ����յ���������
		r.getPixel(pBmpBuf, pBmpDatas, datas[section_num - 1], rest_bits);
		fclose(inFile);

		FILE* fout;
		err = fopen_s(&fout, Savepath, "wb");
		if (!err) {
			cout << "��ʼд��ͼ������" << endl;
			fwrite(&fileHeader, sizeof(BitMapFileHeader), 1, fout);
			fwrite(&infoHeader, sizeof(BitMapInfoHeader), 1, fout);

			if (infoHeader.biBitCount == 8) {
				fread(&colorMap, sizeof(RGB), 256, fout);
			}

			fwrite(pBmpBuf, p_sum, 1, fout); //д��������Ϣ
			fclose(fout);
			cout << "�����" << Savepath <<"��д�룡" << endl;
		}
		else {
			cout << "�޷��򿪽�Ҫд����ļ��������ļ�·������" << endl;
		}
	}
	else {
		cout << "�޷��򿪽�Ҫ��ѹ���ļ��������ļ�·������" << endl;
	}
}

