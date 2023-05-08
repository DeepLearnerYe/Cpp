#include "../include/DictProducer.h"
#include "../include/cppjieba/Jieba.hpp"
#include <iostream>

using namespace cppjieba;

using std::cout;
using std::endl;

void EnglishTest()
{
    DictProducer dict("../data/yuliao/english/", "../data/yuliao/stop_words_eng.txt");
    dict.buildEnDict();
    dict.storeEn();
}

void ChineseTest()
{
    DictProducer dict("../data/yuliao/art/", "../data/yuliao/stop_words_zh.txt");
    dict.buildCnDict();
    dict.storeCn();
}


int main(void)
{
    time_t begin = time(NULL);
    EnglishTest();
    ChineseTest();
    time_t end = time(NULL);
    cout << "4 files generated!" << endl;
    cout << "time consumes " << end - begin << "s." << endl;

    return 0;
}

