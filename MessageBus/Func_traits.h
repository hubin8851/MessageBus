#pragma once
#include <iostream>
#include <functional>
#include <tuple>
//��ͨ����.
//����ָ��.
//function/lambda.
//��Ա����.
//��������.

//ת��Ϊstd::function�ͺ���ָ��. 
template<typename T>
struct function_traits;

//��ͨ����.
template<typename Ret, typename... Args>
struct function_traits<Ret(Args...)>
{
public:
	enum { arity = sizeof...(Args) };
	typedef Ret function_type(Args...);
	typedef Ret return_type;
	using stl_function_type = std::function<function_type>;
	typedef Ret(*pointer)(Args...);

	template<size_t I>
	struct args
	{
		static_assert(I < arity, "index is out of range, index must less than sizeof Args");
		using type = typename std::tuple_element<I, std::tuple<Args...>>::type;
	};

	//���Ƴ����ã����Ƴ������Ƴ��Ĵ����Ż�
	typedef std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...> tuple_type;
	typedef std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...> bare_tuple_type;
};

//��ȡ����ָ�룬���䷵��ֵ�������ȡ����
template<typename Ret, typename... Args>
struct function_traits<Ret(*)(Args...)> : function_traits<Ret(Args...)> {};

//��ȡstd::function��
//std::function���������ˣ����������ʲô����
template <typename Ret, typename... Args>
struct function_traits<std::function<Ret(Args...)>> : function_traits<Ret(Args...)> {};

//��Ա����ת��Ϊstd::function
//��Ա�����ĵ���һ��Ҫ�о�����󣬳��Ǻ���ʱ��̬�ģ�������Ҫ�������ͺͱ����ö��� https://blog.csdn.net/gx864102252/article/details/82634211
//��const��������չ�������޶���λ��
#define FUNCTION_TRAITS(...)\
template <typename ReturnType, typename ClassType, typename... Args>\
struct function_traits<ReturnType(ClassType::*)(Args...) __VA_ARGS__> : function_traits<ReturnType(Args...)>{};\

FUNCTION_TRAITS()
FUNCTION_TRAITS(const)
FUNCTION_TRAITS(volatile)
FUNCTION_TRAITS(const volatile)


//��������.ģ��ʵ��ʱ��Ҫ�����������
//template<typename Callable>
//struct function_traits : function_traits<decltype(&Callable::operator())> { };

template <typename Function>
typename function_traits<Function>::stl_function_type to_function(const Function& lambda)
{
	return static_cast<typename function_traits<Function>::stl_function_type>(lambda);
}

template <typename Function>
typename function_traits<Function>::stl_function_type to_function(Function&& lambda)
{
	return static_cast<typename function_traits<Function>::stl_function_type>(std::forward<Function>(lambda));
}

template <typename Function>
typename function_traits<Function>::pointer to_function_pointer(const Function& lambda)
{
	return static_cast<typename function_traits<Function>::pointer>(lambda);
}


void print(int a)
{
	std::cout << "print:" << a << std::endl;
}

struct CallObject
{
	void operator()()
	{
		std::cout << "CallObject" << std::endl;
	}
};