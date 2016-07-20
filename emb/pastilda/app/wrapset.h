#ifndef __WRAPSET__
#define __WRAPSET__
template<typename T> void ZeroIt(T& value)
{
    memset(&value,0,sizeof(value));
}
#endif