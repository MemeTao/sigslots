#include "../sigslots.h"
#include <iostream>
class Send{
public:
    Send()
    {
        ;
    }
    Signal<int,double> signal_;
};

class Recive : public Slots{
public:
    Recive()
    {}
    void rec(int i,double d)
    {
        (void) i;
        (void) d;
        std::cout<<"i:"<<i<<",d:"<<d<<std::endl;
    }
};

int main()
{
    Send src;
    Recive handle;
    src.signal_.connect<Recive>(&handle,&Recive::rec);
    src.signal_(2,3);

    return 0;
}
