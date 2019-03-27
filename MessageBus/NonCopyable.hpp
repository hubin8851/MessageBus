#pragma once
//因为boost有了非拷贝类
#ifndef BOOST_VERSION

class NonCopyable
{
public:
	NonCopyable(const NonCopyable&) = delete; // deleted
	NonCopyable& operator = (const NonCopyable&) = delete; // deleted
	NonCopyable() = default;   // available
};

#endif