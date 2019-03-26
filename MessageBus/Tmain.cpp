#include <functional>
#include <map>
#include <tuple>
#include <type_traits>
#include "BaseMesBus.h"
#include "Func_traits.h"

//普通的函数转换类测试
void TestNormalFunc();

//新的消息总线类测试
void test_messge_bus();

void main()
{
	TestNormalFunc();

//	bus.Register_handler("test", [](int a, int b) { return a + b; });
//	bus.Register_handler("void", [] { std::cout << "void" << std::endl; });
//	bus.Register_handler("str", [](int a, const std::string& b) { return std::to_string(a) + b; });
//	bus.Register_handler("ptr", [](const char* data, int size) { std::cout << data << " " << size << std::endl; });
//	auto r = bus.call<int>("test", 1, 2);
//	std::cout << r << std::endl;
//	auto s = bus.call<std::string>("str", 1, std::string("test"));
//	bus.call_void("void");
//	bus.call_void("ptr", "test", 4);

}


void TestNormalFunc()
{
	using namespace std;
	auto f = to_function(print);	//转换为std::function

	cout << "args:" << function_traits<decltype(print)>::arity << endl;	//获取参数数量
	function_traits<decltype(print)>::args<0>::type value = 10;	//获取参数类型
	cout << "value:" << value << endl;
	cout << typeid(f).name() << endl;

	f(11);

	auto pf = to_function_pointer(print);		//转换为C语言格式的函数指针
	cout << typeid(pf).name() << endl;
	pf(20);


//	auto g = to_function(CallObject());	//转换为std::function

}


void test_messge_bus()
{
//	BaseMsg_bus bus;
//	bus.Register_handler("test", [](int a, int b) { return a + b; });
//	bus.Register_handler("void", [] { std::cout << "void" << std::endl; });
//	bus.Register_handler("str", [](int a, const std::string& b) { return std::to_string(a) + b; });
//	bus.Register_handler("ptr", [](const char* data, int size) { std::cout << data << " " << size << std::endl; });
//	auto r = bus.call<int>("test", 1, 2);
//	std::cout << r << std::endl;
//	auto s = bus.call<std::string>("str", 1, std::string("test"));
//	bus.call_void("void");
//	bus.call_void("ptr", "test", 4);
}