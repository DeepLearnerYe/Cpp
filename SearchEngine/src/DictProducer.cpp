#include "../include/DictProducer.h"
#include "../include/cppjieba/Jieba.hpp"
#include "../include/SplitTool.h"
#include <iostream>
#include <string>
#include <dirent.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <cctype>
#include <list>

using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::stringstream;
using std::list;

DictProducer::DictProducer(const string &dir, const string &path) 
{
    getFiles(dir, path);
}

void DictProducer::getFiles(const string &dir, const string &path)
{
    //获取中英文语料库的文件相对路径
    DIR *_dir = opendir(dir.c_str());
    if(_dir == NULL)
    {
        cerr << "Fail to open directory" << endl; 
    }
    struct dirent *entry;
    string folder_path = dir;
    while((entry = readdir(_dir)) != NULL)
    {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;//跳过当前目录和上级目录
        }

        _files.push_back(folder_path + entry->d_name);
    }
    closedir(_dir);

    //读入停用词
    ifstream stop_ifs(path);
    if(!stop_ifs.good())
    {
        cerr << "Fail to input stop words" << endl; 
    }
    string line;
    while(getline(stop_ifs,line))
    {
        _stop_word.insert(line);
    }
}

void DictProducer::buildEnDict() 
{
    ProduceEnDict();//生成_dict词典
    createEnIndex();

    return;
}

void DictProducer::ProduceEnDict()
{
    vector<string>::iterator it_files;
    for(it_files =  _files.begin(); it_files != _files.end(); ++it_files)
    {
        ifstream ifs(*it_files);
        if(!ifs.good())
        {
            cerr << "Fail to input file stream" << endl; 
            break;
        }
        string line;
        
        while(getline(ifs,line))
        {
            istringstream iss(line);
            string word;
            while(iss >> word)
            {
                //1.处理单词
                for(size_t i = 0; i != word.size(); ++i)
                {
                    if(!isalpha(word[i]))
                    {
                        word = string();
                        break;
                    }
                    word[i] = tolower(word[i]);
                }

                auto it = _stop_word.find(word); 

                if(it != _stop_word.end())
                {
                    continue;//是停用词,不将其放入，执行下一次循环
                
                }

                //2.将单词存入unordered_map
                ++_dict[word];

            }
        }

    }

}


void DictProducer::createEnIndex()
{
    for(auto &it:_dict)
    {
        _index_dict.emplace_back(it.first);//将map里的数据取出来放到vector里，以便建立索引库
    }

    size_t pos = 0;//记录在vector中的位置
    for(auto &it:_index_dict)
    {
        if(!it.empty())
        {
            //将单词拆分为字母
            for(size_t i = 0; i != it.size(); ++i)
            {
                _index[string(1, it[i])].insert(pos);
            }
        }
        ++pos;
    }


}

void DictProducer::buildCnDict() {
    ProduceCnDict();
    createCnIndex();

    return;
}

void DictProducer::ProduceCnDict()
{
    const char* const DICT_PATH = "../include/cppjieba/dict/jieba.dict.utf8";
    const char* const HMM_PATH = "../include/cppjieba/dict/hmm_model.utf8";
    const char* const USER_DICT_PATH = "../include/cppjieba/dict/user.dict.utf8";
    const char* const IDF_PATH = "../include/cppjieba/dict/idf.utf8";
    const char* const STOP_WORD_PATH = "../data/yuliao/stop_words_zh.txt";
    cppjieba::Jieba jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
    /* SplitTool splittool; */
    vector<string> cut_words;

    vector<string>::iterator it_files;
    for(it_files =  _files.begin(); it_files != _files.end(); ++it_files)
    {
        ifstream ifs(*it_files);
        if(!ifs.good())
        {
            cerr << "Fail to input file stream" << endl; 
            break;
        }
        string line;
        while(getline(ifs,line))
        {
            /* cut_words = splittool(line); */
            vector<string> cut_temp;
            jieba.CutAll(line, cut_temp);
            //去除停用词
            for(auto &it:_stop_word)
            {
                cut_temp.erase(remove(cut_temp.begin(),cut_temp.end(),it),cut_temp.end());
            }
            cut_words.insert(cut_words.end(),cut_temp.begin(), cut_temp.end());
        }
    }

    for(auto &elem:cut_words)
    {
        ++_dict[elem];
    }
}

void DictProducer::createCnIndex(){
    for(auto &it:_dict)
    {
        _index_dict.emplace_back(it.first);//将map里的数据取出来放到vector里，以便建立索引库
    }

    size_t pos = 0;//记录在vector中的位置
    for(auto &it:_index_dict)
    {
        //把词拆为单个字
        vector<string> temp = _cuttor->cut(it); 
        for(auto &it2:temp)
        {
            _index[it2].insert(pos);
        }
        ++pos;
    }

    return;
}

void DictProducer::storeEn() const{
    ofstream ofs("../data/english.data");
    if(!ofs.good())
    {
        cerr << "Fail to store" << endl;
    }
    stringstream oss;
    for(auto &it:_dict)
    {
        oss << it.first << " " << it.second << endl;
    }
    ofs << oss.str() << endl;

    ofstream ofs1("../data/english_index.data");
    if(!ofs1.good())
    {
        cerr << "Fail to store" << endl;
    }
    stringstream oss1;
    for(auto &it:_index)
    {
        oss1 << it.first << " ";
        for(auto &set_it:it.second)
        {
            oss1 << set_it << " ";
        }
        oss1 << endl;
    }
    ofs1 << oss1.str() << endl;

    return;
}


void DictProducer::storeCn() const
{
    ofstream ofs2("../data/chinese.data");
    if(!ofs2.good())
    {
        cerr << "Fail to store" << endl;
    }
    stringstream oss2;
    for(auto &it:_dict)
    {
        oss2 << it.first << " " << it.second << endl;
    }
    ofs2 << oss2.str() << endl;

    ofstream ofs3("../data/chinese_index.data");
    if(!ofs3.good())
    {
        cerr << "Fail to store" << endl;
    }
    stringstream oss3;
    for(auto &it:_index)
    {
        oss3 << it.first << " ";
        for(auto &set_it:it.second)
        {
            oss3 << set_it << " ";
        }
        oss3 << endl;
    }
    ofs3 << oss3.str() << endl;

    return;
}

//**************测试用****************
//void DictProducer::showFiles() const
//{
//    for(auto &elem:_files)
//    {
//        cout << elem  << endl;
//    }
//}
//
//void DictProducer::print() const
//{
//    ofstream ofs("test.data");
//    for(auto &it:_index)
//    {
//        for(auto &it1:it.second)
//        {
//            ofs << _index_dict[it1] << " ";
//        }
//        break;
//    }
//}
