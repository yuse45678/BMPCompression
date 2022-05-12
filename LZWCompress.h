/**══════════════════════════════════╗
*作    者：LeeJiayi		                                               ║
*单	   位：SCUT，School of Automation Science and Engineering		   ║
* CSDN：https://blog.csdn.net/weixin_47006220?type=blog				   ║
*═══════════════════════════════════╝
*/

#pragma once
typedef unsigned char uchar;
typedef unsigned int uint;

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <exception>
#include <string>
#include <bitset>
using namespace std;

string Char2Hex(uchar c);
std::vector<std::string> splitStr(const std::string& str, const std::string& pattern);
char bitset2char(std::bitset<8> bits);
std::vector<uchar> Int2CharVector(uint data);


class LZWcompress {
private:
	std::vector<string> data_n;
	std::map<string, uint> encode_map;
	std::map<uint, string> decode_map;
	std::vector<uint> lzw_encode_output;
	std::vector<string> lzw_decode_output;

public:
	LZWcompress(vector<uchar> data, int size);

	void encode_init();
	void decode_init();
	void LZW_encode();
	void LZW_decode();

	uchar* get_lzw_encode(int&datasize);
	uchar* get_lzw_decode(int& datasize);
	uint get_encode_size();
	uint get_decode_size();
	
	void set_encode_data(uchar* lzw_encode_data, uint encode_size);


};
