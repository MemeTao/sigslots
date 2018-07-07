#include <iostream>
class A{
public:
    void foo(){}
};

void bar(void* p)
{
    std::cout<<"size of p:"<<sizeof(p)<<std::endl;
}

int main()
{
    A a;
    void* p = nullptr;
    std::cout<<"size of p:"<<sizeof(p)<<std::endl;
    std::cout<<"size of mem:"<<sizeof(&A::foo)<<std::endl;
    auto pfun = &A::foo;
    std::cout<<"size of pfun:"<<sizeof(pfun)<<std::endl;
    bar(&pfun);
    return 0;
}
