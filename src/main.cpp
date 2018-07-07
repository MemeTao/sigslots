#include "sigslots.h"


class Send{
public:
    Send();
    Signal<int,double> signal_;
};

class Recive : public Slots{
public:
    Recive();
    void rec(int i,double d)
    {
        (void) i;
        (void) d;
    }
};

int main()
{
    Send src;
    Recive handle;
    src.signal_.connect<Recive>(&handle,&Recive::rec);
    src.signal_();

    return 0;
}
