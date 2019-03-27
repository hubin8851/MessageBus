#pragma once
#include <iostream>
#include <type_traits>
#include <functional>
#include <tuple>
//普通函数.
//函数指针.
//function/lambda.
//成员函数.
//函数对象.

//转换为std::function和函数指针. 
template<typename T>
struct function_traits;

//普通函数.
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

	//先移除引用，再移除常量移除寄存器优化
	typedef std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...> tuple_type;
	typedef std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...> bare_tuple_type;
};

//萃取函数指针，将其返回值和入参提取出来
template<typename Ret, typename... Args>
struct function_traits<Ret(*)(Args...)> : function_traits<Ret(Args...)> {};

//提取std::function类
//std::function，问题来了？跟上面的有什么区别？
template <typename Ret, typename... Args>
struct function_traits<std::function<Ret(Args...)>> : function_traits<Ret(Args...)> {};

//成员函数转换为std::function
//成员函数的调用一定要有具体对象，除非函数时静态的，所以需要函数类型和被调用对象 https://blog.csdn.net/gx864102252/article/details/82634211
//将const等类型扩展到函数限定符位置
#define FUNCTION_TRAITS(...)\
template <typename ReturnType, typename ClassType, typename... Args>\
struct function_traits<ReturnType(ClassType::*)(Args...) __VA_ARGS__> : function_traits<ReturnType(Args...)>{};\

FUNCTION_TRAITS()
FUNCTION_TRAITS(const)
FUNCTION_TRAITS(volatile)
FUNCTION_TRAITS(const volatile)


//函数对象.模板实现时需要具体对象类型
//decltype 检查实体的声明类型或表达式的类型及值分类，返回相应类型。 https://zh.cppreference.com/w/cpp/language/decltype
//嵌套依赖名字operator()会导致解析困难 https://blog.csdn.net/dick_china/article/details/4522253
template<typename Callable >
struct function_traits : function_traits<decltype(&Callable::operator())>{};


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