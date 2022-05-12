/**══════════════════════════════════╗
*作    者：LeeJiayi		                                               ║
*单	   位：SCUT，School of Automation Science and Engineering		   ║
* CSDN：https://blog.csdn.net/weixin_47006220?type=blog				   ║
*═══════════════════════════════════╣
* Copyright LeeJiayi 2022. All rights reserved.
*═══════════════════════════════════╝
*----------------------------------------------------------------------*/
#include "LZWCompress.h"

/**
 * @brief 将单个字符转为16进制字符串
 * @param ch			需要转换的数据
 *
 * @return ch	返回对应字符的16进制字符串
 */
string Char2Hex(uchar c) {
	const std::string hex = "0123456789ABCDEF";
	string ret;
	ret.push_back(hex[(c >> 4) & 0xf]); //取二进制高四位
	ret.push_back(hex[c & 0xf]);        //取二进制低四位
	return ret;
}

/**
 * @brief				用于分割字符串
 * @param str			需要分割的字符串
 * @param pattern		以什么符号来作为分割依据	
 *
 * @return resVec		返回分割后的字符串
 */
std::vector<std::string> splitStr(const std::string& str, const std::string& pattern)
{
	std::vector<std::string> resVec;

	if ("" == str)
	{
		return resVec;
	}
	//方便截取最后一段数据
	std::string strs = str + pattern;

	size_t pos = strs.find(pattern);
	size_t size = strs.size();

	while (pos != std::string::npos)
	{
		std::string x = strs.substr(0, pos);
		resVec.push_back(x);
		strs = strs.substr(pos + 1, size);
		pos = strs.find(pattern);
	}

	return resVec;
}

/**
 * @brief				Bitset格式转字符格式
 * @param bits			需要转换的字符
 *
 * @return char			返回转换后的字符
 */
char bitset2char(std::bitset<8> bits)
{
	return (char)bitset<8>(bits.to_string()).to_ulong();
}

/**
 * @brief				uint格式转vector<uchar>格式
 * @param  data			需要转换的data
 *
 * @return buf			返回转换后的vector<uchar>
 */
std::vector<uchar> Int2CharVector(uint data) {
	std::vector<uchar> buf;
	bitset<32> data_bit(data);
	string data_bit_str = data_bit.to_string();
	for (int i = 0; i < 4; i++) {
		bitset<8> data_bit_part(data_bit_str.substr(i * 8, 8));
		buf.push_back(bitset2char(data_bit_part));
	}
	return buf;
}

LZWcompress::LZWcompress(vector<uchar> data, int size) {
	for (uint i = 0; i < size; i++) {
		data_n.push_back(std::to_string(int(data[i])));
	}
}

//编码初始化
void LZWcompress::encode_init() {
	for (int i = 0; i < 256; i++) {
		encode_map.insert(std::pair<string, uint>(std::to_string(i), i));
	}

}

//解码初始化
void LZWcompress::decode_init() {
	for (int i = 0; i < 256; i++) {
		decode_map.insert(std::pair<uint, string>(i, std::to_string(i)));
	}
}

//LZW编码过程
void LZWcompress::LZW_encode() {
	encode_init();
	uint symbol = 256;
	string previous_char = "", current_char = "", p_and_c = "";
	for (auto yuvIter = data_n.begin(); yuvIter != data_n.end(); yuvIter++) {
		current_char = *yuvIter;
		if (previous_char != "") {
			p_and_c = previous_char + "-" + current_char;
		}
		else {
			p_and_c = current_char;
		}
		//cout<<"previous_:"<<previous_char<<"\tcurrent:"<<current_char<<"\tp_and_c"<<p_and_c<<endl;
		if (encode_map.find(p_and_c) != encode_map.end()) {
			//p_and_c在字典里
			previous_char = p_and_c;
		}
		else {
			//p_and_c不在字典里
			lzw_encode_output.push_back(encode_map[previous_char]);
			encode_map.insert(pair<string, uint>(p_and_c, symbol));
			symbol += 1;
			previous_char = current_char;
		}
	}
	lzw_encode_output.push_back(encode_map[p_and_c]);

	cout << "symbol:" << symbol << endl;


}

//LZW解码
void LZWcompress::LZW_decode() {
	decode_init();
	uint symbol = 256;

	cout << "encode_size:" << lzw_encode_output.size() << endl;

	int previous_symbol = -1, current_symbol = -1;
	string previous_char = "", current_char = "", p_and_c = "";

	lzw_decode_output.push_back(decode_map[lzw_encode_output[0]]);
	previous_symbol = lzw_encode_output[0];


	for (auto iter = (lzw_encode_output.begin() + 1); iter != lzw_encode_output.end(); iter++) {
		current_symbol = *iter;
		if (decode_map.find(current_symbol) != decode_map.end()) {
			//symbol在字典中
			auto decode_vector = splitStr(decode_map[current_symbol], "-");
			for (auto iter = decode_vector.begin(); iter != decode_vector.end(); iter++)
				lzw_decode_output.push_back(*iter);
			previous_char = decode_map[previous_symbol];
			current_char = decode_vector[0];
			if (previous_char != "")
				p_and_c = previous_char + "-" + current_char;
			else
				p_and_c = current_char;
			decode_map.insert(pair<int, string>(symbol, p_and_c));
			symbol += 1;

		}
		else {
			//symbol不在字典中
			previous_char = decode_map[previous_symbol];

			current_char = splitStr(decode_map[previous_symbol], "-")[0];
			if (previous_char != "")
				p_and_c = previous_char + "-" + current_char;
			else
				p_and_c = current_char;
			decode_map.insert(pair<int, string>(symbol, p_and_c));
			auto decode_vector_2 = splitStr(p_and_c, "-");
			for (auto iter = decode_vector_2.begin(); iter != decode_vector_2.end(); iter++)
				lzw_decode_output.push_back(*iter);

			symbol += 1;

		}
		previous_symbol = current_symbol;
	}
}


//获取编码数据，并返回编码数据（lzw_encode_data）及其数据大小（datasize）
uchar* LZWcompress::get_lzw_encode(int& datasize) {

	uchar* lzw_encode_data = new uchar[lzw_encode_output.size() * 4 + 4];
	datasize = lzw_encode_output.size() * 4 + 4;
	int k = 0;
	//保存lzw压缩文件大小
	for (int i = 0; i < 4; i++) {
		std::vector<uchar> char_vector = Int2CharVector(get_encode_size());
		lzw_encode_data[k++] = char_vector[i];
	}
	for (auto iter = lzw_encode_output.begin(); iter != lzw_encode_output.end(); iter++) {
		std::vector<uchar> char_vector = Int2CharVector(*iter);
		for (int i = 0; i < 4; i++) {
			lzw_encode_data[k++] = char_vector[i];
		}
	}
	return lzw_encode_data;
}



//获取解码数据，并返回解码数据（lzw_decode_data）及其数据大小（datasize）
uchar* LZWcompress::get_lzw_decode(int& datasize) {
	datasize = lzw_decode_output.size();
	uchar* lzw_decode_data = new uchar[lzw_decode_output.size()];
	for (int i = 0; i < lzw_decode_output.size(); i++) {
		lzw_decode_data[i] = uchar(stoi(lzw_decode_output[i]));
	}
	cout << "lzw_decode_data:" << lzw_decode_output.size() << endl;
	return lzw_decode_data;
}

//获取编码输出大小
uint LZWcompress::get_encode_size() {
	return lzw_encode_output.size() * 4;
}

//获取解码输出大小
uint LZWcompress::get_decode_size() {
	return lzw_decode_output.size();
}

//设置编码数据，准备解码
void LZWcompress::set_encode_data(uchar* lzw_encode_data, uint encode_size) {
	for (int i = 0; i < encode_size; i += 4) {
		string str = "";
		for (int j = 0; j < 4; j++) {
			str += Char2Hex(lzw_encode_data[i + j]);
		}
		lzw_encode_output.push_back(stoi(str, nullptr, 16));
	}
	//cout << lzw_encode_output.size() << endl;
}
