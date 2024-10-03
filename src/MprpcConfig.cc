#include "MprpcConfig.h"
#include <iostream>
// #include <string>

void MprpcConfig::LoadConfigFile(const char *config_file)
{
    FILE *pf = fopen(config_file, "r");
    if(pf == nullptr)
    {
        std::cout << config_file << "is not exist" << std::endl;
        exit(EXIT_FAILURE);
    }

    // 1.注释 2.正确配置项 =  3.去掉开头多余空格
    while(!feof(pf))
    {
        char buf[512] = {0};
        fgets(buf, 512, pf);

        // 去空格
        std::string read_buf(buf);
        
        // 判断有#的注释行
        if(read_buf[0] == '#' || read_buf.empty()) continue;

        // 解析配置项
        int idx = read_buf.find('=');
        if(idx == -1) continue;
        std::string key, value;
        
        int endidx = read_buf.find('\n', idx);
        key = read_buf.substr(0, idx);
        Trim(key);
        value = read_buf.substr(idx + 1, endidx - idx - 1);
        Trim(value); 
        m_configMap.insert({key, value});
    }
}

std::string MprpcConfig::Load(const std::string &key)
{
    auto it = m_configMap.find(key);
    if(it == m_configMap.end()) return "";
    return it->second;
}

void MprpcConfig::Trim(std::string &src_buf)
{
    int idx = src_buf.find_first_not_of(' ');
    if(idx != -1)
    {
        src_buf = src_buf.substr(idx, src_buf.size() - idx);
    }
    idx = src_buf.find_last_not_of(' ');
    if(idx != -1)
    {
        src_buf = src_buf.substr(0, idx + 1);
    }
}