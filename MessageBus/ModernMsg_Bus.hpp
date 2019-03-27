#pragma once
#include <string>
#include <map>
#include <functional>
#include "Func_traits.hpp"

class message_bus
{
public:
	template<typename Function>
	void register_handler(std::string const & name, const Function& f)
	{
		using std::placeholders::_1;
		using std::placeholders::_2;

		using return_type = typename function_traits<Function>::return_type;

		this->invokers_[name] = { std::bind(&invoker<Function>::apply, f, _1, _2) };
	}

	template <typename T, typename ... Args>
	T call(const std::string& name, Args&& ... args)
	{
		auto it = invokers_.find(name);
		if (it == invokers_.end())
			return{};

		//获取传参元组
		auto args_tuple = std::make_tuple(std::forward<Args>(args)...);

		char data[sizeof(std::tuple<Args...>)];
		std::tuple<Args...>* tp = new (data) std::tuple<Args...>;
		*tp = args_tuple;

		//存储成函数指针在此处计算
		T t;
		it->second(tp, &t);
		return t;
	}

	template <typename ... Args>
	void call_void(const std::string& name, Args&& ... args)
	{
		auto it = invokers_.find(name);
		if (it == invokers_.end())
			return;

		auto args_tuple = std::make_tuple(std::forward<Args>(args)...);
		it->second(&args_tuple, nullptr);
	}

private:
	template<typename Function>
	struct invoker
	{
		static inline void apply(const Function& func, void* bl, void* result)
		{
			using tuple_type = typename function_traits<Function>::tuple_type;
			const tuple_type* tp = static_cast<tuple_type*>(bl);

			call(func, *tp, result);
		}

		template<typename F, typename ... Args>
		static typename std::enable_if<std::is_void<typename std::result_of<F(Args...)>::type>::value>::type
			call(const F& f, const std::tuple<Args...>& tp, void*)
		{
			call_helper(f, std::make_index_sequence<sizeof... (Args)>{}, tp);
		}

		template<typename F, typename ... Args>
		static typename std::enable_if<!std::is_void<typename std::result_of<F(Args...)>::type>::value>::type
			call(const F& f, const std::tuple<Args...>& tp, void* result)
		{
			auto r = call_helper(f, std::make_index_sequence<sizeof... (Args)>{}, tp);
			*(decltype(r)*)result = r;
		}

		template<typename F, size_t... I, typename ... Args>
		static auto call_helper(const F& f, const std::index_sequence<I...>&, const std::tuple<Args...>& tup)
		{
			return f(std::get<I>(tup)...);
		}
	};

private:
	std::map<std::string, std::function<void(void*, void*)>> invokers_;
};
