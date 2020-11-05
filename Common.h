#pragma once 
#include <string>

// noncopyable  is 无复制与等于 
class noncopyable
{
    protected:
        noncopyable(){}

    private:
        noncopyable(const noncopyable&)=delete;
        void operator=(const noncopyable&)=delete;

};

class copyable
{

};

// 统一sting 与const char×
class StringPiece
{
    public:
        StringPiece(const char* str):str_(str)
        {

        }
        StringPiece(const std::string&  str):str_(str.c_str())
        {

        }
        const char* c_str() const
        {
            return str_;
        } 
    private:
        const char* str_;
};

//类型转化
template<typename To,typename From>
inline To implict_cast(const From& f)
{
    return f;
}
