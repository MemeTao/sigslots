#ifndef SIGSLOTS_H_
#define SIGSLOTS_H_
#include <stdint.h>
#include <vector>
#include <cstring>
//考虑以下几个问题:
//1.如何存放信号槽函数
//2.发送信号时,如何判断信号槽函数"所属"对象是否已经不存在?
//3.静态模板成员函数,什么时候实例化?
//问题1的回答:统一用字节存放
//问题2的回答:使用继承,利用"确定性析构"特性
//问题3的回答:暂时不知


//XXX:不能对整个类模板化,不然在signal
//构造的时候,会出现先有鸡还是先有蛋的问题
class Slots;
//FIXME:这个类是关键
class Connected{
    typedef void(Connected::*sm_t)(Connected*);
    template <typename From,typename To>
    union Caster{
        From from;
        To   to;
    };
    sm_t sm_;
public:
    //可以确定的是pdest必然是继承了Slots类的对象
    template <typename Dest,typename...ArgTypes>
    Connected(Dest* pdest,void (Dest::*pfunc)(ArgTypes...))
        :pdest_(pdest)
    {
        memset(pfunc_,0,sizeof(pfunc_));
        memcpy(&pfunc_[0],&pfunc,sizeof(pfunc));
        //这样,我们就保存了this指针和成员函数指针
        //但是还需要保存Dest的类型
        typedef void(*emit_t)(Connected*,ArgTypes...);
        Caster<emit_t,sm_t> caster;
        //实例化一个Dest类型的函数
        caster.from = &Connected::emiter<Dest,ArgTypes...>;
        sm_ = caster.to;
    }
    template<typename...ArgTypes>
    void operator()(ArgTypes...args)
    {
        //FIXME:如果不是静态成员函数,编译器
        //无法推导出对应的emiter()
        //emiter(this,args...);
        typedef void(*emit_t)(Connected*,ArgTypes...);
        Caster<sm_t,emit_t> caster;
        caster.from = sm_;
        caster.to(this,args...);
    }
    bool operator == (const Slots* pdest) const
    {
        return pdest_ == pdest;
    }
    
private:
    //FIXME:静态成员函数模板,什么时候实例化?
    template <typename Dest,typename...ArgTypes>
    static void emiter(Connected* self,ArgTypes...args)
    {
        typedef void(Dest::*pfcun_t)(ArgTypes...);
        pfcun_t pfunc;
        memcpy(&pfunc,self->pfunc_,sizeof(pfunc));
        //FIXME:这个推倒什么错?
        //pfunc(self->pdest_,args...);
        //(static_cast<Dest*>(self->pdest_)->pfunc(args...);
        //FIXME:这里不明白
        (static_cast<Dest*>(self->pdest_)->*(pfunc))(args...);
    }

    uint8_t pfunc_[16]; //函数地址
    Slots* pdest_;      //this指针
};

class SignalInterface{
public:
    virtual void disconnect(const Slots* dest) = 0;
    virtual ~SignalInterface() = default;
};

//FIXME:这个类实现有问题,ConnectList的类型不能为void*
//必须使用包裹,将其统一化
template <typename...ArgTypes>
class Signal : public SignalInterface{
    using ConnectedList = std::vector<Connected>;
public:
    Signal():SignalInterface() {}
    virtual ~Signal() = default;
    //connet必须可以跨越多个类
    template <typename Dest>
    void connect(Dest* pdest,void (Dest::*pfunc)(ArgTypes...args))
    {
        Connected conn(pdest,pfunc);
        pdest->registered(this);
        connectedList_.push_back(conn);
    }
    void operator()(ArgTypes...args)
    {
        for(auto& it : connectedList_)
        {
            it(args...);
        }
    }
    virtual void disconnect(const Slots* dest) override
    {
        std::vector<int> indexs;
        int i = 0;
        for(auto& conn : connectedList_)
        {
            i++;
            if(conn == dest)
            {
                indexs.push_back(i);
            }
        }
        for(auto j : indexs)
        {
            connectedList_.erase(connectedList_.begin() + j);
        }
    }
private:
    ConnectedList connectedList_;
};

class SlotsInterface{
public:
    virtual ~SlotsInterface(){}
    virtual void registered(SignalInterface*) = 0;
};
class Slots : public SlotsInterface{
    using SenderList = std::vector<SignalInterface*>;
public:
    Slots(){}
    virtual void registered(SignalInterface* sender) override
    {
        senders_.push_back(sender);
    }
    virtual ~Slots() override
    {
        for(auto it : senders_)
        {
            it->disconnect(this);
        }
    }     
private:
    SenderList senders_;
};
#endif


