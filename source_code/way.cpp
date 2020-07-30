#include "way.h"

Way::Way()
{
    from=-1;
}

Way::Way(int from, int to, int cost, int transposition, int start, int end)
{
    this->from=from;
    this->to=to;
    this->cost=cost;
    this->transposition=transposition;
    this->start=start;
    this->end=end;
}
