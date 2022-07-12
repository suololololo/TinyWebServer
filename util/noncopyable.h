#ifndef __NON_COPYABLE_H__
#define __NON_COPYABLE_H__


class NonCopyAble
{
protected:
    NonCopyAble() {}
    ~NonCopyAble() {}

private:
    NonCopyAble(const NonCopyAble &);
    const NonCopyAble& operator=(const NonCopyAble&);

};

#endif