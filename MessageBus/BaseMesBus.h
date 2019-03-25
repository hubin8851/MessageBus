#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <map>
#include <tuple>
#include <type_traits>

#include "Func_traits.h"

//��Ϣ������
class BaseMes_bus
{
private:
	//������
	std::map<std::string, std::function<void(void*, void*)>> invokers_map;
protected:
public:
	//ע��Ļص�����
	template<typename Function>
	void Register_handler(std::string const & name, const Function& f)
	{
		using std::placeholders::_1;
		using std::placeholders::_2;

		using return_type = typename function_traits<Function>::result_type;

		this->invokers_map[name] = { std::bind(&invoker<Function>::apply, f, _1, _2) };
	}




};