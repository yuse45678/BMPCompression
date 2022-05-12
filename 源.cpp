/**�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�[
*��    �ߣ�LeeJiayi		                                               �U
*��	   λ��SCUT��School of Automation Science and Engineering		   �U
* CSDN��https://blog.csdn.net/weixin_47006220?type=blog				   �U
*�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�g
* Copyright LeeJiayi 2022. All rights reserved.
*�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�a
*----------------------------------------------------------------------*/
#include"BMPInfo.h"
#include"Compression.h"
#include<io.h>	//��Ҫ���ڶ��ļ�����ͼƬ�Ķ�ȡ
#include <direct.h>	//��Ҫ���ڴ����ļ���
#include<time.h>	//��Ҫ���ڶ�ѹ���ͽ�ѹ���ļ�ʱ
using namespace LeeJiayi;

/**
 * @brief ��������Ҫ�����ڶ�ȡ�ض�Ŀ¼�µ��ض���ʽ�ļ������洢��·��
 * @param path			��Ҫ��ȡ��Ŀ¼
 * @param files			���ڴ洢�ļ�·���Ļ���
 * @param format		��Ҫ��ȡ���ļ���ʽ
 *
 * @return ��
 */
void GetAllFormatFiles(string path, vector<string>& files, string format)
{
	//�ļ����  
	long long  hFile = 0;
	//_findnext()��һ��������·������������ص�����Ϊintptr_t��long long����
	//�������Ϊlong����win7����û�����⣬������win10�о�Ҫ��Ϊlong long����intptr_t
	
	//�ļ���Ϣ  
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

//����������
int main() {

		string prefix = "res";					//res�ļ�����Ҫ���ڴ洢��ѹ���ͼƬ
		if (_access(prefix.c_str(), 0) == -1)	//���res�ļ��в�����
			_mkdir(prefix.c_str());				//�򴴽�

		prefix = "bmp";							//bmp�ļ�����Ҫ���ڴ洢ѹ��ǰ��ͼƬ�Լ�ѹ������ļ���
		if (_access(prefix.c_str(), 0) == -1)	//����ļ��в�����
			_mkdir(prefix.c_str());				//�򴴽�

		Compression compressor;
		uint i = 0;
		string filePath = "bmp";	
		vector<string> files;
		clock_t start, finish;   //typedef long clock_t; ��Ҫ���ڼ�ʱ
		//FILE* stream1;
		//freopen_s(&stream1, "out.txt", "w", stdout); //����ض���������ݽ�����out.txt�ļ���

		cout << "* ---------------------------------------------------------" << endl;
		cout << "* ��ӭʹ��BMPͼ��ѹ��ϵͳ����ϵͳ�����˶�̬�滮��LZWѹ��������" << endl;
		cout << "* ���ߣ�LeeJiaiyi" << endl;
		cout << "* ��λ��SCUT��School of Automation Science and Engineering	" << endl;
		cout << "* ---------------------------------------------------------" << endl;
		cout << "* Copyright LeeJiayi 2022. All rights reserved." << endl;
		cout << "* --------------------------------------------------------- " << endl;
		cout << "* ע�⣺BMPͼƬ������ڿ�ִ���ļ�Ŀ¼�µ�bmp�ļ����У�" << endl;

		string format = ".bmp";
		//��ȡbmp�ļ����µ�bmp�ļ�
		GetAllFormatFiles(filePath, files, format);
		//��������ļ���ִ������
		if (files.size()) {
			cout << endl << "* ��ѡ��ǰ����..." << endl;
			cout << "* 1.LZWѹ��ͼƬ		* 2.��̬�滮ѹ��ͼƬ		* 3.��������" << endl;
			cout << "* --------------------------------------------------------- " << endl;
			cin >> i;
			if (i == 1) {
				cout <<endl<< "* --------------------------------------------------------- " << endl;
				cout << "��ʼ����LZWѹ��..." << endl;
				for (int iter = 0; iter < files.size(); iter++) {
					string infile = files[iter];
					string outfile = infile + ".lzw";
					start = clock();												//��ʼ��ʱ
					compressor.LzwCompress(infile.c_str(), outfile.c_str());		//LZWѹ��
					finish = clock();												//������ʱ
					double totaltime = (double)(finish - start) / CLOCKS_PER_SEC;	//����ʱ��
					cout << "ѹ��ʱ��Ϊ" << totaltime << "�룡" << endl;			//���ʱ��
					cout << "* --------------------------------------------------------- " << endl;
				}
				for (int iter = 0; iter < files.size(); iter++) {
					string infile = files[iter] + ".lzw";
					//�����ַ��������ļ�����ȡ����
					vector<string> s = splitStr(files[iter], "\\");
					s = splitStr(s[1], ".");
					string outfile = "res//LZWDecompress_"+s[0] + ".bmp";
					cout << endl << "* --------------------------------------------------------- " << endl
						<< "��ʼ��ѹ��...." << endl;
					start = clock();												//��ʼ��ʱ
					compressor.LzwDecompress(infile.c_str(), outfile.c_str());		//LZW��ѹ��
					finish = clock();												//������ʱ
					double totaltime = (double)(finish - start) / CLOCKS_PER_SEC;	//����ʱ��
					cout << "��ѹ��ʱ��Ϊ" << totaltime << "�룡" << endl;			//���ʱ��
				}
			}
			else if (i == 2) {
				cout << "* --------------------------------------------------------- " << endl;
				cout << "��ʼ������̬�滮ѹ��..." << endl;
				for (int iter = 0; iter < files.size(); iter++) {
					string infile = files[iter];
					string outfile = infile + ".dp";
					start = clock();												//��ʼ��ʱ
					compressor.DPCompress(infile.c_str(), outfile.c_str());			//DPѹ��
					finish = clock();												//������ʱ
					double totaltime = (double)(finish - start) / CLOCKS_PER_SEC;	//����ʱ��
					cout << "ѹ��ʱ��Ϊ" << totaltime << "�룡" << endl;			//���ʱ��
					cout << "* --------------------------------------------------------- " << endl;
				}
				for (int iter = 0; iter < files.size(); iter++) {
					string infile = files[iter] + ".dp";
					vector<string> s = splitStr(files[iter], "\\");
					s = splitStr(s[1], ".");
					string outfile = "res//DPDecompress_" + s[0] + ".bmp";
					cout << endl << "* --------------------------------------------------------- " << endl
						<< "��ʼ��ѹ��...." << endl;
					start = clock();												//��ʼ��ʱ
					compressor.DPDecompress(infile.c_str(), outfile.c_str());		//DP��ѹ��
					finish = clock();												//������ʱ
					double totaltime = (double)(finish - start) / CLOCKS_PER_SEC;	//����ʱ��
					cout << "��ѹ��ʱ��Ϊ" << totaltime << "�룡" << endl;			//���ʱ��
				}
			}

		
		}
		else {
			cout << "bmp�ļ�������bmpͼƬ�����飡"<<endl;
			
		}
		//fclose(stdout);
		system("pause");
	return 0;
}