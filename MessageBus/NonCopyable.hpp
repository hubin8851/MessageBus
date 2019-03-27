#pragma once
//��Ϊboost���˷ǿ�����
#ifndef BOOST_VERSION

class NonCopyable
{
public:
	NonCopyable(const NonCopyable&) = delete; // deleted
	NonCopyable& operator = (const NonCopyable&) = delete; // deleted
	NonCopyable() = default;   // available
};

#endif