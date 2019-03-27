#pragma once
#include <string>
#include <functional>

struct caller {
	caller(std::string& name) :name_(name) {};

	template<typename T>
	void operator()(const T& t) {
		name_ += type_name<T>();
	}

	std::string& name() {
		return name_;
	}

	std::string& name_;
};

