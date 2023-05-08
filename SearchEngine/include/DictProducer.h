#ifndef _DICTPRODUCER_H
#define _DICTPRODUCER_H

#include <vector>
#include <unordered_map>
#include <string>
#include <map>
#include <set>

using std::vector;
using std::unordered_map;
using std::string;
using std::pair;
using std::map;
using std::set;

class SplitTool;//前向声明

class DictProducer {
public: 
    DictProducer(const string &dir, const string &path);
    DictProducer(const string &dir, SplitTool * spliTool);
    void buildEnDict();
    void buildCnDict();
    void storeEn() const;
    void storeCn() const;
    //测试函数
    void showFiles() const;
    void print() const;

private: 
    void getFiles(const string &dir, const string &path);//获取文件相对路径，并存入停用词 
    void ProduceEnDict();//生成英文词典
    void ProduceCnDict();//生成中文词典
    void createEnIndex();//创建英文索引
    void createCnIndex();//创建中文索引
    vector<string> _files;//存文件相对路径
    set<string> _stop_word;//存停用词
    unordered_map<string,size_t> _dict;//存中英文词典
    vector<string> _index_dict;//将unordered_map里的数据存到vector里，才能将下标存入索引
    map<string,set<int>> _index;//存中英文索引
    SplitTool* _cuttor;
};

#endif //_DICTPRODUCER_H
