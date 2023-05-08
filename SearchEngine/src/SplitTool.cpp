#include "../include/SplitTool.h"
#include <iostream>
#include <string>
#include <vector>

using std::vector;
using std::string;
using std::cout;
using std::endl;

vector<string>SplitTool::cut(const string& input) {
    vector<string> return_unicode;
    for (size_t i = 0; i < input.size();) {
        if ((input[i] & 0x80) == 0) {
            //0x80二进制表示为1000 0000, 可以判断ASCII字符
            /* cout << "ASCII:" << input[i] << endl; */
            ++i;
        }   
        else {
            /* 中文字符 */
            /* UTF-8编码中，n字节编码，第1个字节的前n+1位固定，后续字节的前2位固定 */
            /* 1字节编码: 0xxxxxxx */
            /* 2字节编码: 110xxxxx 10xxxxxx*/
            /* 3字节编码：1110xxxx 10xxxxxx 10xxxxxx */
            /* 4字节编码：11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
            /* 利用第1个字节固定，分别按位与可以确定是几字节编码*/
            /*      0xE0：11100000 */
            /*      0xF0：11110000 */
            /*      0xC0：11000000 */
            /*      0xF8：11111000*/

            int nBytes = 0;
            if ((input[i] & 0xE0) == 0xC0) { // 2字节编码
                nBytes = 2;
            }   
            else if ((input[i] & 0xF0) == 0xE0) { // 3字节编码
                nBytes = 3;
            }   
            else if ((input[i] & 0xF8) == 0xF0) { // 4字节编码
                nBytes = 4;
            }   
            else { // 非法编码
                cout << "Invalid UTF-8 encoding" << endl;
            }   

            /*  
             * 2字节编码，input[i] = 110* ****
             * 0xFF向右位移3位后，   0001 1111，
             * 按位与可以得到码位信息
             */
            string unicode;//用于存单个中文字符
            int code = input[i];
            unicode = code;
            for (int j = 1; j < nBytes; j++) {
                code =  input[i + j]; // 将后续字节信息存入unicode
                unicode += code;
            }
            return_unicode.push_back(unicode);
            i += nBytes;
        }
    }
    return return_unicode;
}


