#include <functional>
#include <map>
#include <tuple>
#include "Func_traits.hpp"
#include "BaseMesBus.h"
#include "ModernMsg_Bus.hpp"
#include "MsgFactory_Bus.hpp"
#include "E_C_M.h"

//��ͨ�ĺ���ת�������
void TestNormalFunc();

//�µ���Ϣ���������
void test_modern_messge_bus();

//TestBus
void TestBus();


void main()
{
	TestBus();

	TestNormalFunc();

	test_modern_messge_bus();

}


void TestNormalFunc()
{
	using namespace std;
//	auto f = to_function(print);	//ת��Ϊstd::function
//
//	cout << "args:" << function_traits<decltype(print)>::arity << endl;	//��ȡ��������
//	function_traits<decltype(print)>::args<0>::type value = 10;	//��ȡ��������
//	cout << "value:" << value << endl;
//	cout << typeid(f).name() << endl;
//
//	f(11);
//
//	auto pf = to_function_pointer(print);		//ת��ΪC���Ը�ʽ�ĺ���ָ��
//	cout << typeid(pf).name() << endl;
//	pf(20);


	auto g = to_function(CallObject());	//ת��Ϊstd::function

}


void test_modern_messge_bus()
{
	message_bus bus;
	bus.register_handler("test", [](int a, int b) { return a + b; });
	bus.register_handler("void", [] { std::cout << "void" << std::endl; });
	bus.register_handler("str", [](int a, const std::string& b) { return std::to_string(a) + b; });
	bus.register_handler("ptr", [](const char* data, int size) { std::cout << data << " " << size << std::endl; });
	auto r = bus.call<int>("test", 1, 2);
	std::cout << r << std::endl;
	auto s = bus.call<std::string>("str", 1, std::string("test"));
	bus.call_void("void");
	bus.call_void("ptr", "test", 4);
}


void TestBus()
{
	Engng subject;
	Element1 E1;
	subject.SendMsg("Initial");
	subject.SendMsg("Calculate");
}