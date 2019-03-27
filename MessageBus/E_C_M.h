#pragma once
//单元、算法、材料测试类
#include "BaseMesBus.h"
#include "MsgFactory_Bus.hpp"

struct FrameHead
{
	float _Syn[2];
};
FrameHead g_FrameHead;

struct Engng
{
	Engng()
	{
		BaseMsg_bus::GetInstance().Attach([this] { CalculateOK(); }, "CalOK" );
	}

	void InitialMsg(const FrameHead& _FrameHead)
	{
//		BaseMsg_bus::GetInstance().SendMsg<void, FrameHead>(_FrameHead, "Initial");
	}

	void SendMsg(const std::string& topic)
	{
		BaseMsg_bus::GetInstance().SendMsg<void, int>(50, "Calculate");
	}

	void CalculateOK()
	{
		std::cout << "" << "calculate ok" << std::endl;
	}

};

class BaseE
{
public:
	virtual void Initial(const FrameHead& _head) = 0;
	virtual void Calculate(int speed) = 0;
};

class Element1 :public BaseE
{
public:
	Element1()//注册
	{
		BaseMsg_bus::GetInstance().Attach([this](const FrameHead& _head){ Initial(_head); }, "Initial");
		BaseMsg_bus::GetInstance().Attach([this](int speed) { Calculate(speed); }, "Calculate");
	}

	void Initial(const FrameHead& _head)
	{
		std::cout << "Element1初始化成功" <<  std::endl;
		return;
	}

	void Calculate(int speed)//派生类的函数实体
	{
		std::cout << "Element1计算得" << speed << std::endl;
		BaseMsg_bus::GetInstance().SendMsg<void>( "CalOK" );
	}

};