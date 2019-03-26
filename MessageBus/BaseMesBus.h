#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <map>
#include <tuple>
#include <type_traits>

#include "Func_traits.h"
#include "RedefAny.h"

//消息总线类
class BaseMsg_bus
{

private:
	std::multimap< std::string, Any > m_MsgMap;
	typedef std::multimap< std::string, Any >::iterator Iterater;
	

protected:
public:
	//注册消息
	template<typename F>
	void Attach(F&& f, const std::string& strTopic = "")
	{
		auto func = to_function(std::forward<F>(f));
		Add(strTopic, std::move(func));
	}

	//发送消息
	template<typename R>
	void SendMsg(const std::string& strTopic = "")
	{
		using function_type = std::function<R()>;
		std::string strMsgType = strTopic + typeid(function_type).name();
		auto range = m_MsgMap.equal_range(strMsgType);
		for (Iterater it = range.first; it != range.second; ++it)
		{
			auto f = it->second.AnyCast < function_type >();
			f();
		}
	}

	//移除某个主题, 需要主题和消息类型
	template<typename R, typename... Args>
	void RemoveMsg(const std::string& strTopic = "")
	{
		using function_type = std::function<R(Args...)>; //typename function_traits<void(CArgs)>::stl_function_type;

		std::string strMsgType = strTopic + typeid(function_type).name();
		int count = m_MsgMap.count( strMsgType );
		auto range = m_MsgMap.equal_range( strMsgType );
		m_MsgMap.erase( range.first, range.second );
	}

private:
	template<typename F>
	void Add(const std::string& strTopic, F&& f)
	{
		std::string strMsgType = strTopic + typeid(F).name();
		m_MsgMap.emplace(std::move(strMsgType), std::forward<F>(f));
	}

};