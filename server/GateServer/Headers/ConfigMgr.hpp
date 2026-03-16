#pragma oncey
#include <string>
#include <unordered_map>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>

struct SectionInfo {
    SectionInfo() = default;
    ~SectionInfo(){ 
        _section_datas.clear();
    }
    SectionInfo(const SectionInfo& src) { 
        _section_datas = src._section_datas; 
    }
    SectionInfo& operator=(const SectionInfo& src) {
        if (&src == this) return *this;
        _section_datas = src._section_datas; 
        return *this;
    }

    std::unordered_map<std::string, std::string> _section_datas;
    std::string operator[](const std::string &key){  // 表示这里面不能有： 修改  map 不能变
        // 所以 if 判断的时候，  不能直接用 [] 访问
        // auto it = _section_datas.find(key);
        if (!_section_datas.count(key)) return std::string("");
        return _section_datas[key];
    }
};

class ConfigMgr {
public:
    ConfigMgr();
    ~ConfigMgr() { 
        _config_map.clear();
    }

    SectionInfo operator[](const std::string& section){
        if (!_config_map.count(section)) return SectionInfo();
        return _config_map[section];
    }

    ConfigMgr& operator=(const ConfigMgr& src) {
        if (&src == this) return *this;
        _config_map = src._config_map;
        return *this;
    }

    ConfigMgr(const ConfigMgr& src) {
        _config_map = src._config_map;
    }

private:
    std::unordered_map<std::string, SectionInfo> _config_map;
};

