
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>


//parse one line of text from file into single preferences list
std::vector<std::string> ParseLine(std::string line,const char spiltSymbol = ';')
{
    std::vector<std::string> pref;
    while(!line.empty())
    {
        size_t index = line.find_first_of(spiltSymbol);
        if(index == std::string::npos)
            return pref;
        pref.emplace_back(line.substr(0,index));
        line.erase(0,index + 1);
    }

    return pref;
}

std::string ExtractLabel(std::string pList)
{
    size_t index = pList.find_first_of(':');
    std::string label;
    if(index != std::string::npos)
    {
        label = pList.substr(0,index);
        pList.erase(0,label.size());
    }
    return label;
}

std::vector<std::string> ExtractPreferences(std::string pList)
{
    size_t start = pList.find_first_of('<');
    size_t end = pList.find_last_of('>');
    std::string pref;
    std::vector<std::string> result;

    if(start != std::string::npos && end != std::string::npos )
        pref = pList.substr(start + 1, end - 1);
    while(!pref.empty())
    {
        size_t index = pref.find_first_of(',');
        if(index != std::string::npos)
        {
            // last entry does not have a comma seperator so take wad is left
            size_t lastEnd = pref.find_last_of('>');
            pref.erase(lastEnd,1);
            result.emplace_back(pref);
            return result;
        }
        result.emplace_back(pref.substr(0,index));
        pref.erase(0,index + 1 );
    } 
    return  result;
}

int main(int argc,char* argv[])
{

    if(argc != 2)
    {
        printf("%s\n %s\n","Invalid agrument count","Provide preference list file as 2nd agrument");
    }    

    std::ifstream file { argv[1] };  

    if(!file)
    {
        printf("%s:%s\n","Unable to open file:", argv[1]);
        std::exit(0);
    }
    using PrefDataBase = std::vector<std::vector<std::string>>;
    //convert a name into index in the preference database
    std::unordered_map<std::string,uint32_t> nameToPrefMap;
    PrefDataBase dataBase;
    char temp[512];
    while(!file.eof())
    {
        file.getline(temp,sizeof(char) * 512);
        std::string line = temp;
        std::vector<std::string> pList = ParseLine(line);
        
        for(const std::string& s : pList)
        {
            std::string label = ExtractLabel(line);
            std::vector<std::string> pref = ExtractPreferences(s);
            
            nameToPrefMap.emplace(label,(uint32_t)dataBase.size());
            dataBase.emplace_back(pref);
        }
    }

    #ifdef _DEBUG
    for (const auto& prefList : dataBase)
    {
        for (const auto& data : prefList)
        {
            printf("%s,",data.c_str());
        }
        printf("\n");
    }
    #endif


    return 0;
}