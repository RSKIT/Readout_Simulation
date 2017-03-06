/*
  Hilfsmethoden für den Umgang mit tinyxml2
*/
#include "tinyxml2.h"
#include <string>

tinyxml2::XMLNode* getNode(tinyxml2::XMLNode* parent, std::string value, tinyxml2::XMLNode* start)
{
    tinyxml2::XMLNode* child;
    if(start != 0)
        child = start;
    else
        child = parent->FirstChildElement();

    while(1)
    {
        if(child->Value() == value)
            return child;

        if(child != parent->LastChildElement())
            child = child->NextSiblingElement();
        else
            break;
    }
    return 0;
}

//TODO: does not work with tinyxml2 as it does some escaping by itself...
//it can be skipped... (work for nothing, but the check from unescapeStringCheck() is still
//  necessary)
std::string escapeString(std::string text)
{
    std::string convert = text;
    const char escaping[5] = {'&','\"','\'','<','>'};
    const std::string replacement[5] = {"&amp;","&quot;","&apos;","&lt;","&gt;"};
    int index = 0;
    int symbol = 0;

    for(int symbol = 1; symbol < 5; ++symbol)   
            //exclude '&' from the replacement as it is done by tinyxml2
    {
        index = convert.find(escaping[symbol], 0);
        while(index != std::string::npos)
        {
            convert.replace(index,1,replacement[symbol]);
            index = convert.find(escaping[symbol],index+1);
        }
    }

    return convert;
}

std::string unescapeString(std::string escapedtext)
{
    std::string convert = escapedtext;
    const char escaping[5] = {'\"','\'','<','>','&'};
    const std::string replacement[5] = {"&quot;","&apos;","&lt;","&gt;","&amp;"};
    int index = 0;
    int symbol = 0;

    for(int symbol = 0; symbol < 4; ++symbol)   //exclude '&' from the replacement (done by tinyxml2)
    {
        index = convert.find(replacement[symbol], 0);
        while(index != std::string::npos)
        {
            convert.replace(index, (replacement[symbol]).length(), &(escaping[symbol]));
            index = convert.find(replacement[symbol],index+1);
        }
    }

    return convert;    
}

std::string unescapeStringCheck(const char* escapedtext)
{
    if(escapedtext == 0)
        return "";

    std::string convert = escapedtext;
    return unescapeString(convert);
}

std::string checkString(const char* text)
{
    if(text == 0)
        return "";
    else
        return std::string(text);
}
