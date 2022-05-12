/**══════════════════════════════════╗
*作    者：LeeJiayi		                                               ║
*单	   位：SCUT，School of Automation Science and Engineering		   ║
* CSDN：https://blog.csdn.net/weixin_47006220?type=blog				   ║
*═══════════════════════════════════╣
* Copyright LeeJiayi 2022. All rights reserved.
*═══════════════════════════════════╝
*----------------------------------------------------------------------*/
#include"BMPInfo.h"
#include"Compression.h"
#include<io.h>	//主要用于对文件夹下图片的读取
#include <direct.h>	//主要用于创建文件夹
#include<time.h>	//主要用于对压缩和解压缩的计时
using namespace LeeJiayi;

/**
 * @brief 本函数主要是用于读取特定目录下的特定格式文件，并存储其路径
 * @param path			将要读取的目录
 * @param files			用于存储文件路径的缓存
 * @param format		需要读取的文件格式
 *
 * @return 空
 */
void GetAllFormatFiles(string path, vector<string>& files, string format)
{
	//文件句柄  
	long long  hFile = 0;
	//_findnext()第一个参数”路径句柄”，返回的类型为intptr_t（long long），
	//如果定义为long，在win7中是没有问题，但是在win10中就要改为long long或者intptr_t
	
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*" + format).c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					//files.push_back(p.assign(path).append("\\").append(fileinfo.name) );
					GetAllFormatFiles(p.assign(path).append("\\").append(fileinfo.name), files, format);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
}

//程序的主入口
int main() {

		string prefix = "res";					//res文件夹主要用于存储解压后的图片
		if (_access(prefix.c_str(), 0) == -1)	//如果res文件夹不存在
			_mkdir(prefix.c_str());				//则创建

		prefix = "bmp";							//bmp文件夹主要用于存储压缩前的图片以及压缩后的文件！
		if (_access(prefix.c_str(), 0) == -1)	//如果文件夹不存在
			_mkdir(prefix.c_str());				//则创建

		Compression compressor;
		uint i = 0;
		string filePath = "bmp";	
		vector<string> files;
		clock_t start, finish;   //typedef long clock_t; 主要用于计时
		//FILE* stream1;
		//freopen_s(&stream1, "out.txt", "w", stdout); //输出重定向，输出数据将保存out.txt文件中

		cout << "* ---------------------------------------------------------" << endl;
		cout << "* 欢迎使用BMP图像压缩系统，本系统集成了动态规划和LZW压缩方法！" << endl;
		cout << "* 作者：LeeJiaiyi" << endl;
		cout << "* 单位：SCUT，School of Automation Science and Engineering	" << endl;
		cout << "* ---------------------------------------------------------" << endl;
		cout << "* Copyright LeeJiayi 2022. All rights reserved." << endl;
		cout << "* --------------------------------------------------------- " << endl;
		cout << "* 注意：BMP图片必须放在可执行文件目录下的bmp文件夹中！" << endl;

		string format = ".bmp";
		//读取bmp文件夹下的bmp文件
		GetAllFormatFiles(filePath, files, format);
		//如果存在文件就执行任务
		if (files.size()) {
			cout << endl << "* 请选择当前任务..." << endl;
			cout << "* 1.LZW压缩图片		* 2.动态规划压缩图片		* 3.结束程序" << endl;
			cout << "* --------------------------------------------------------- " << endl;
			cin >> i;
			if (i == 1) {
				cout <<endl<< "* --------------------------------------------------------- " << endl;
				cout << "开始批量LZW压缩..." << endl;
				for (int iter = 0; iter < files.size(); iter++) {
					string infile = files[iter];
					string outfile = infile + ".lzw";
					start = clock();												//开始计时
					compressor.LzwCompress(infile.c_str(), outfile.c_str());		//LZW压缩
					finish = clock();												//结束计时
					double totaltime = (double)(finish - start) / CLOCKS_PER_SEC;	//计算时间
					cout << "压缩时间为" << totaltime << "秒！" << endl;			//输出时间
					cout << "* --------------------------------------------------------- " << endl;
				}
				for (int iter = 0; iter < files.size(); iter++) {
					string infile = files[iter] + ".lzw";
					//分离字符串，将文件名提取出来
					vector<string> s = splitStr(files[iter], "\\");
					s = splitStr(s[1], ".");
					string outfile = "res//LZWDecompress_"+s[0] + ".bmp";
					cout << endl << "* --------------------------------------------------------- " << endl
						<< "开始解压缩...." << endl;
					start = clock();												//开始计时
					compressor.LzwDecompress(infile.c_str(), outfile.c_str());		//LZW解压缩
					finish = clock();												//结束计时
					double totaltime = (double)(finish - start) / CLOCKS_PER_SEC;	//计算时间
					cout << "解压缩时间为" << totaltime << "秒！" << endl;			//输出时间
				}
			}
			else if (i == 2) {
				cout << "* --------------------------------------------------------- " << endl;
				cout << "开始批量动态规划压缩..." << endl;
				for (int iter = 0; iter < files.size(); iter++) {
					string infile = files[iter];
					string outfile = infile + ".dp";
					start = clock();												//开始计时
					compressor.DPCompress(infile.c_str(), outfile.c_str());			//DP压缩
					finish = clock();												//结束计时
					double totaltime = (double)(finish - start) / CLOCKS_PER_SEC;	//计算时间
					cout << "压缩时间为" << totaltime << "秒！" << endl;			//输出时间
					cout << "* --------------------------------------------------------- " << endl;
				}
				for (int iter = 0; iter < files.size(); iter++) {
					string infile = files[iter] + ".dp";
					vector<string> s = splitStr(files[iter], "\\");
					s = splitStr(s[1], ".");
					string outfile = "res//DPDecompress_" + s[0] + ".bmp";
					cout << endl << "* --------------------------------------------------------- " << endl
						<< "开始解压缩...." << endl;
					start = clock();												//开始计时
					compressor.DPDecompress(infile.c_str(), outfile.c_str());		//DP解压缩
					finish = clock();												//结束计时
					double totaltime = (double)(finish - start) / CLOCKS_PER_SEC;	//计算时间
					cout << "解压缩时间为" << totaltime << "秒！" << endl;			//输出时间
				}
			}

		
		}
		else {
			cout << "bmp文件夹下无bmp图片！请检查！"<<endl;
			
		}
		//fclose(stdout);
		system("pause");
	return 0;
}