#ifndef SIGSLOTS_H_
#define SIGSLOTS_H_
#include <stdint.h>
#include <vector>
#include <cstring>
//考虑以下几个问题:
//1.如何存放信号槽函数
//2.发送信号时,如何判断信号槽函数"所属"对象是否已经不存在?
//问题2的回答:继承

template <typename Dest,typename...ArgTypes>
class Connected{
public:
    Connected(Dest* pdest,void* pfunc)
        :pdest_(pdest)
    {
        memset(pfunc_,0,sizeof(pfunc_));
        memcpy(&pfunc_[0],pfunc,sizeof(pfunc));
    }
    void operator()(ArgTypes...args)
    {
        ;
    }
private:
    Dest* pdest_;       //目标的this指针
    uint8_t pfunc_[16]; //函数地址
};
template <typename...ArgTypes>
class Signal{
#ifdef CXX11
    using ConnectedList = std::vector<Connected<void*,ArgTypes...>>;
#else
    typedef std::vector<Connected<void*,ArgTypes...>> ConnectedList;
#endif
public:
    //connet必须可以跨越多个类
    template <typename Dest>
    void connect(Dest* pdest,void (Dest::*pfunc)(ArgTypes...args))
    {
        Connected<Dest,ArgTypes...> conn(pdest,pfunc);
        slots_.push_back(conn);
    }
    void operator()(ArgTypes...args)
    {
        for(auto it : slots_)
        {
            it(args...);
        }
    }
private:
    ConnectedList slots_;
};


class Slots{
    
};
#endif


