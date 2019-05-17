// ================================================================= //
// @File: config_reader.h
// @Author: Qiao Rukun
// @Date: 2019.05.16
// @LastEditTime: 2019.05.16
// @LastEditors: Qiao Rukun
// ================================================================= //

// @brief the config_reader.h is used for load config files.
// The file suffix is .ini; most of the code is copied from internet. 
// The class can only load the ini file.

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <map>

using namespace std;

// Trim all space in the string.
string &TrimString(string &str) {
  string::size_type pos = 0;
  while (str.npos != (pos = str.find(" ")))
    str = str.replace(pos, pos + 1, "");
  return str;
}

// An INI Node for reading.
// Equivalent to every line in ini files. 
struct ININode{
  string root;
  string key;
  string value;
  ININode(string root, string key, string value) {
    this->root = root;
    this->key = key;
    this->value = value;
  }
};


//键值对结构体
class SubNode {
public:
  void InsertElement(string key, string value) {
    sub_node.insert(pair<string, string>(key, value));
  }
  map<string, string> sub_node;
};

//INI文件操作类
class ConfigReader
{
public:
  ConfigReader();
  ~ConfigReader();

public:
  int ReadINI(string path);
  string GetStringValue(string root, string key);
  int GetIntValue(string root, string key);
  void Clear() { root_node_.clear(); }	//清空
  void TravelFile();						//遍历打印INI文件
private:
  map<string, SubNode> root_node_;		//INI文件内容的存储变量

};

ConfigReader::ConfigReader() {
}


ConfigReader::~ConfigReader() {
}


// Read ini file and save all info into the class map.
// The ini file should like this:
// [some_root_string_here]
// key_here1 = value_here1
// key_here2 = value_here2
// [another_root_string]
// ...
// All the space is not necessary.
int ConfigReader::ReadINI(string path)
{
  ifstream file_stream(path.c_str());
  if (!file_stream) 
    return 0;
  string str_line = "";
  string str_root = "";
  vector<ININode> vec_ini;
  while (getline(file_stream, str_line)) {
    string::size_type left_pos = 0;
    string::size_type right_pos = 0;
    string::size_type equal_div_pos = 0;
    string str_key = "";
    string str_value = "";
    if ((str_line.npos != (left_pos = str_line.find("["))) && (str_line.npos != (right_pos = str_line.find("]")))) {
      //cout << str_line.substr(left_pos+1, right_pos-1) << endl;
      str_root = str_line.substr(left_pos + 1, right_pos - 1);
    }

    if (str_line.npos != (equal_div_pos = str_line.find("="))) {
      str_key = str_line.substr(0, equal_div_pos);
      str_value = str_line.substr(equal_div_pos + 1, str_line.size() - 1);
      str_key = TrimString(str_key);
      str_value = TrimString(str_value);
      //cout << str_key << "=" << str_value << endl;
    }

    if ((!str_root.empty()) && (!str_key.empty()) && (!str_value.empty())) {
      ININode ini_node(str_root, str_key, str_value);
      vec_ini.push_back(ini_node);
      //cout << vec_ini.size() << endl;
    }
  }
  file_stream.close();

  //vector convert to map
  map<string, string> map_tmp; // This map_tmp is used like a set
  for (vector<ININode>::iterator itr = vec_ini.begin(); itr != vec_ini.end(); ++itr) {
    map_tmp.insert(pair<string, string>(itr->root, ""));
  }
  for (map<string, string>::iterator itr = map_tmp.begin(); itr != map_tmp.end(); ++itr) {
    SubNode sn;
    for (vector<ININode>::iterator sub_itr = vec_ini.begin(); sub_itr != vec_ini.end(); ++sub_itr) {
      if (sub_itr->root == itr->first) {
        sn.InsertElement(sub_itr->key, sub_itr->value);
      }
    }
    root_node_.insert(pair<string, SubNode>(itr->first, sn));
  }
  return 1;
}


// Given root, key and type, get related result.
string ConfigReader::GetStringValue(string root, string key) {
  map<string, SubNode>::iterator itr = root_node_.find(root);
  map<string, string>::iterator sub_itr = itr->second.sub_node.find(key);
  if (!(sub_itr->second).empty())
    return sub_itr->second;
  return "";
}

int ConfigReader::GetIntValue(string root, string key) {
  map<string, SubNode>::iterator itr = root_node_.find(root);
  map<string, string>::iterator sub_itr = itr->second.sub_node.find(key);
  if (!(sub_itr->second).empty()) {
    stringstream ss;
    int value;
    ss << sub_itr->second;
    ss >> value;
    return value;
  }
  return 0;
}


// Print INI file. Just for debug.
void ConfigReader::TravelFile() {
  for (map<string, SubNode>::iterator itr = root_node_.begin(); itr != root_node_.end(); ++itr) {
    //root
    cout << "[" << itr->first << "]" << endl;
    for (map<string, string>::iterator itr1 = itr->second.sub_node.begin(); itr1 != itr->second.sub_node.end();
      ++itr1) {
      cout << "    " << itr1->first << " = " << itr1->second << endl;
    }
  }
}