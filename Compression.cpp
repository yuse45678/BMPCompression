/**══════════════════════════════════╗
*作    者：LeeJiayi		                                               ║
*单	   位：SCUT，School of Automation Science and Engineering		   ║
* CSDN：https://blog.csdn.net/weixin_47006220?type=blog				   ║
*═══════════════════════════════════╣
* Copyright LeeJiayi 2022. All rights reserved.
*═══════════════════════════════════╝
*----------------------------------------------------------------------*/
#include "Compression.h"

/**
 * @brief LZW压缩过程
 * @param Filepath		需要压缩的文件路径
 * @param Savepath		压缩后文件存储路径
 *
 * @return 空
 */
void LeeJiayi::Compression::LzwCompress(const char* Filepath, const char* Savepath)
{
	BitMapFileHeader bmpFileHeaderData;	//BMP文件头
	BitMapInfoHeader bmpInfoHeaderData;	//BMP信息头
	RGB colorMap[256];					//BMP颜色表（如果是24位真彩色不存在这玩意）
	vector<uchar> imgData;
	int channels = 0;
	FILE* inFile;
	errno_t err = fopen_s(&inFile, Filepath, "rb");
	if (err == 0) {
		cout << "已加载" << Filepath << endl;
		cout << "开始读取图像数据" << endl;
		fread(&bmpFileHeaderData, sizeof(BitMapFileHeader), 1, inFile);
		fread(&bmpInfoHeaderData, sizeof(BitMapInfoHeader), 1, inFile);
		
		//如果是8位位图就读取颜色表
		//本次任务的图片都是8位位图
		if (bmpInfoHeaderData.biBitCount == 8) {
			fread(&colorMap, sizeof(RGB), 256, inFile);
		}

		uchar pixVal;//用于临时存储读取的像素信息
		int imgSize = bmpInfoHeaderData.biHeight * bmpInfoHeaderData.biWidth * bmpInfoHeaderData.biBitCount / 8;
		//读取像素信息并存储在imgData中
		for (int i = 0; i < imgSize; i++) {
			fread(&pixVal, sizeof(uchar), 1, inFile);
			imgData.push_back(pixVal);
			}
		//计算图像文件的大小，用于后续压缩比的计算！
		float inFileSize = ftell(inFile);
		fclose(inFile);
		//定义LZW压缩类
		LZWcompress lzw(imgData, imgData.size());
		//LZW编码
		lzw.LZW_encode();
		int dataSize=0;
		//获取LZW编码后的数据
		uchar* lzwData=lzw.get_lzw_encode(dataSize);
		
		cout << "开始进行压缩文件的写入...." << endl;
		FILE* fout;

		errno_t err = fopen_s(&fout, Savepath, "wb");
		if (!err) {
			//写入文件头
			fwrite(&bmpFileHeaderData, sizeof(bmpFileHeaderData), 1, fout);
			//写入信息头
			fwrite(&bmpInfoHeaderData, sizeof(bmpInfoHeaderData), 1, fout);
			//8位位图就写入颜色表
			if (bmpInfoHeaderData.biBitCount == 8) {
				fwrite(&colorMap, sizeof(RGB), 256, fout);
			}
			//写入压缩后的像素数据
			fwrite(lzwData,dataSize,1,fout);
			//计算压缩后的文件大小
			float FileSize = ftell(fout);
			fclose(fout);

			cout << "已完成对"<<Savepath <<"的写入！" << endl;
			cout << "* ---------------------------------------------------------" << endl;
			cout << Filepath<<"的压缩比为" << FileSize / inFileSize << endl;;
			cout << "* ---------------------------------------------------------" << endl;
		}
		else {
			cout << "无法打开将要写入的文件！请检查文件路径！！" << endl;
		}
	}
	else {
		cout << "无法打开图像文件！请检查文件路径！！" << endl;
	}
}

/**
 * @brief LZW解压缩过程
 * @param Filepath		需要解压缩的文件路径
 * @param Savepath		解压缩后文件存储路径
 *
 * @return 空
 */
void LeeJiayi::Compression::LzwDecompress(const char* Filepath, const char* Savepath)
{
	BitMapFileHeader fileHeader;
	BitMapInfoHeader infoHeader;
	RGB colorMap[256];
	FILE* inFile;
	errno_t err = fopen_s(&inFile, Filepath, "rb");
	if (!err) {
		cout << "开始读取 "<<Filepath<<" 压缩文件数据" << endl;
		//获取 位图信息 中的图像宽度 高度 和 位深度
		fread(&fileHeader, sizeof(BitMapFileHeader), 1, inFile);
		fread(&infoHeader, sizeof(BitMapInfoHeader), 1, inFile);

		if (infoHeader.biBitCount == 8) {
			fread(&colorMap, sizeof(RGB), 256, inFile);
		}

		uchar* lzwSizeInfo = new uchar[4];
		fread(lzwSizeInfo, sizeof(uchar), 4, inFile);

		cout << "开始还原信息.." << endl;
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
			cout << "开始写入图像数据" << endl;
			fwrite(&fileHeader, sizeof(BitMapFileHeader), 1, fout);
			fwrite(&infoHeader, sizeof(BitMapInfoHeader), 1, fout);

			if (infoHeader.biBitCount == 8) {
				fwrite(&colorMap, sizeof(RGB), 256, fout);
			}

			fwrite(data, dataSize, 1, fout);
			fclose(fout);
	
			cout << "已完成对"<<Savepath<<"的写入！"<< endl;
			cout << "* ---------------------------------------------------------" << endl;
		}
		else {
			cout << "无法打开将要写入的文件！请检查文件路径！！" << endl;
		}
	}
	else {
		cout << "无法打开将要解压的文件！请检查文件路径！！" << endl;
	}
}

/**
 * @brief DP压缩过程
 * @param Filepath		需要压缩的文件路径
 * @param Savepath		压缩后文件存储路径
 *
 * @return 空
 */
void LeeJiayi::Compression::DPCompress(const char* Filepath, const char* Savepath)
{
	BitMapFileHeader bmpFileHeaderData;	//BMP文件头
	BitMapInfoHeader bmpInfoHeaderData;	//BMP信息头
	RGB colorMap[256];					//BMP颜色表（如果是24位真彩色不存在这玩意）
	vector<uchar> imgData;
	int channels = 0;
	FILE* inFile;
	errno_t err = fopen_s(&inFile, Filepath, "rb");
	if (err == 0) {
		cout << "开始读取"<< Filepath<<"图像数据" << endl;
		fread(&bmpFileHeaderData, sizeof(BitMapFileHeader), 1, inFile);
		fread(&bmpInfoHeaderData, sizeof(BitMapInfoHeader), 1, inFile);

		//如果是8位位图就读取颜色表
		//本次任务的图片都是8位位图
		if (bmpInfoHeaderData.biBitCount == 8) {
			fread(&colorMap, sizeof(RGB), 256, inFile);
		}


		uchar pixVal;//用于临时存储读取的像素信息
		int imgSize = bmpInfoHeaderData.biHeight * bmpInfoHeaderData.biWidth * bmpInfoHeaderData.biBitCount / 8;
		
		//读取像素信息
		for (int i = 0; i < imgSize; i++) {
			fread(&pixVal, sizeof(uchar), 1, inFile);
			imgData.push_back(pixVal);
		}


		//动态规划计算出分段信息及其所使用的位数
		vector<int> segment;
		vector<int> segmentBit;
		int SegmentCount = GetSegmentation(imgData.size(), imgData, segment, segmentBit);

		cout << "开始进行压缩文件的写入...." << endl;

		//32位是描述有多少段（即SegmentCount）;
		//SegmentCount*11是有SegmentCount段，每段的8位用来描述段的长度，3位用来描述像素位数
		int total_bits = 32 + SegmentCount * 11; 

		for (int i = 0; i < SegmentCount; i++)
		{
			total_bits += segment[i] * segmentBit[i]; //算上所有像素有多少位
		}
		int total_bytes = total_bits % 8 == 0 ? total_bits / 8 : total_bits / 8 + 1; //向上取整计算字节数
		FILE* fout;

		errno_t err = fopen_s(&fout, Savepath, "wb");
		if (!err) {
			fwrite(&bmpFileHeaderData, sizeof(bmpFileHeaderData), 1, fout);
			fwrite(&bmpInfoHeaderData, sizeof(bmpInfoHeaderData), 1, fout);

			if (bmpInfoHeaderData.biBitCount == 8) {
				fwrite(&colorMap, sizeof(RGB), 256, fout);
			}
			
			//写入压缩后的数据
			Writer w(fout, total_bytes);
			w.Write(SegmentCount, imgData.size(), segment, segmentBit, imgData);
			fclose(fout);
			cout << "已完成" << Savepath << "的写入！" << endl;
		}
		else {
			cout << "无法打开将要写入的文件！请检查文件路径！！" << endl;
		}
	}
	else {
		cout << "无法打开图像文件！请检查文件路径！！" << endl;
	}
}

/**
 * @brief DP解压缩过程
 * @param Filepath		需要解压缩的文件路径
 * @param Savepath		解压缩后文件存储路径
 *
 * @return 空
 */
void LeeJiayi::Compression::DPDecompress(const char* Filepath, const char* Savepath)
{
	BitMapFileHeader fileHeader;
	BitMapInfoHeader infoHeader;
	RGB colorMap[256];
	FILE* inFile;
	errno_t err = fopen_s(&inFile, Filepath, "rb");
	if (!err) {
		cout << "开始读取"<< Filepath<<"数据" << endl;
		
		fread(&fileHeader, sizeof(BitMapFileHeader), 1, inFile);
		fread(&infoHeader, sizeof(BitMapInfoHeader), 1, inFile);

		if (infoHeader.biBitCount == 8) {
			fread(&colorMap, sizeof(RGB), 256, inFile);
		}

		int SegmentCount = 0; //记录有多少段

		fread(&SegmentCount, sizeof(int), 1, inFile); //读取有多少段
		uint* SegmentBit = new uint[SegmentCount + 1];  //第i段像素所占位数
		uint* SegmentLength = new uint[SegmentCount + 1];  //第i段有几个像素

		cout << "开始还原信息" << endl;
		//还原SegmentBit、SegmentLength信息
		int rest_bits = 0;//记录剩下的位数
		int section_num = SegmentCount * 11 % 8 == 0 ? SegmentCount * 11 / 8 : SegmentCount * 11 / 8 + 1;//向上取整
		if (SegmentCount * 11 % 8 != 0) { rest_bits = 8 - (SegmentCount * 11 - SegmentCount * 11 / 8 * 8); }
		uchar* datas = new uchar[section_num];
		fread(datas, sizeof(char), section_num, inFile);
		Recover r(SegmentCount, SegmentBit, SegmentLength, datas);//创建恢复类
		r.recoverBitInfo();//恢复分段位数与分段个数
		
		int p_sum = 0;//计算每一段的长度
		for (int i = 1; i <= SegmentCount; i++)
		{
			p_sum += SegmentLength[i];
		}
		//计算最终的总位数以及最后的总字节数，用于恢复使用
		int pBit_sum = 0, qByte_sum = 0;
		for (int i = 1; i <= SegmentCount; i++)
		{
			pBit_sum += SegmentLength[i] * SegmentBit[i];
		}
		qByte_sum = pBit_sum % 8 == 0 ? pBit_sum / 8 : pBit_sum / 8 + 1;

		uchar* pBmpBuf = new uchar[p_sum]; //存储最终像素数据
		uchar* pBmpDatas = new uchar[qByte_sum];//存储未解析的像素数据
		fread(pBmpDatas, sizeof(uchar), qByte_sum, inFile); //读取数据

		//恢复最终的像素数据
		r.getPixel(pBmpBuf, pBmpDatas, datas[section_num - 1], rest_bits);
		fclose(inFile);

		FILE* fout;
		err = fopen_s(&fout, Savepath, "wb");
		if (!err) {
			cout << "开始写入图像数据" << endl;
			fwrite(&fileHeader, sizeof(BitMapFileHeader), 1, fout);
			fwrite(&infoHeader, sizeof(BitMapInfoHeader), 1, fout);

			if (infoHeader.biBitCount == 8) {
				fread(&colorMap, sizeof(RGB), 256, fout);
			}

			fwrite(pBmpBuf, p_sum, 1, fout); //写入像素信息
			fclose(fout);
			cout << "已完成" << Savepath <<"的写入！" << endl;
		}
		else {
			cout << "无法打开将要写入的文件！请检查文件路径！！" << endl;
		}
	}
	else {
		cout << "无法打开将要解压的文件！请检查文件路径！！" << endl;
	}
}

