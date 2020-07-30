#ifndef WAY_H
#define WAY_H

//途经的城市
class Way
{
public:
    Way();
    Way(int from, int to, int cost, int transposition, int start, int end);

    int from, to, cost, transposition;
    int start,end;

};

#endif // WAY_H
