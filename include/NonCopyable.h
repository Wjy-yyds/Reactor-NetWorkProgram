/**
 * Project 66th
 */


#ifndef _NONCOPYABLE_H
#define _NONCOPYABLE_H

class NonCopyable {
public:
    NonCopyable() = default;
    ~NonCopyable() = default;
    NonCopyable(const NonCopyable &) = delete;
};

#endif //_NONCOPYABLE_H