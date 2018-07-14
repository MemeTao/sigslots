### sigslots项目说明
```C++
class Send{
    Send(){}
    Signal<int,double> signal_;
};
class Recive : public Slots{
    Recive(){}
    void rec(int i,double d){
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
```
