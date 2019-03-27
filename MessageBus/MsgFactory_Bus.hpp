#pragma once
//https://github.com/qicosmos/cosmos/blob/master/function_message_bus.hpp#L13
//此页程序之精妙，跪舔=。=

#include <string>
#include <map>
#include <functional>
#include <cassert>
#include "Func_traits.hpp"
#include "type_name.hpp"
#include "caller.h"

template <typename... Args, typename Func, std::size_t... Idx>
void for_each(const std::tuple<Args...>& t, Func&& f, std::index_sequence<Idx...>) 
{
	//https://blog.csdn.net/fengxinlinux/article/details/72614874
	//列表初始化，每个元素时一个函数对象及其返回值等...
	(void)std::initializer_list<int> { (f(std::get<Idx>(t)), void(), 0)...};
}

struct FnKey 
{
	std::string key;
};

class MsgFactoryBus 
{
private:
	//调用器
	std::map<std::string, std::function<void(void*, void*)>> m_Invokers_Map;

public:
	static MsgFactoryBus& GetFactory()
	{
		static MsgFactoryBus _res;
		return _res;
	}

	//注册句柄
	template<typename Function, typename = std::enable_if_t<!std::is_member_function_pointer<Function>::value>>
	void Register_handler(const Function& f, std::string const & additional = "")
	{
		using namespace std::placeholders;
		auto key = get_key<Function>(additional);
		check_duplicate(key);

		m_Invokers_Map[key] = { std::bind(&invoker<Function>::apply, f, _1, _2) };
	}

	template <typename Function, typename Self, typename = std::enable_if_t<std::is_member_function_pointer<Function>::value>>
	void Register_handler(Function f, Self* t, std::string const & additional = "") 
	{
		using namespace std::placeholders;
		auto key = get_key<Function>(additional);
		check_duplicate(key);

		m_Invokers_Map[key] = { std::bind(&invoker<Function>::template apply_mem<Self>, f, t, _1, _2) };
	}

	//non-void function
	template <typename T, typename U, typename ... Args>
	T Call(U&& u, Args&& ... args)
	{
		std::string key = get_key(std::forward<U>(u), std::forward<Args>(args)...);
		auto it = m_Invokers_Map.find(key);
		assert(it != m_Invokers_Map.end());

		T t;
		call_impl(it, &t, std::forward<U>(u), std::forward<Args>(args)...);
		return t;
	}

	//void function
	template <typename U, typename ... Args>
	void Call(U&& u, Args&& ... args)
	{
		std::string key = get_key(std::forward<U>(u), std::forward<Args>(args)...);
		auto it = m_Invokers_Map.find(key);
		assert(it != m_Invokers_Map.end());

		call_impl(it, nullptr, std::forward<U>(u), std::forward<Args>(args)...);
	}



private:
	MsgFactoryBus() {};
	MsgFactoryBus(const MsgFactoryBus&) = delete;
	MsgFactoryBus(MsgFactoryBus&&) = delete;


#pragma region 辅助函数

	template <typename T, typename U, typename ... Args>
	void call_impl(T it, void* ptr, U&& u, Args&& ... args) {
		auto args_tuple = get_args_tuple(std::integral_constant<bool, std::is_same<U, FnKey>::value>{},
			std::forward<U>(u), std::forward<Args>(args)...);
		using Tuple = decltype(args_tuple);
		using storage_type = typename std::aligned_storage<sizeof(Tuple), alignof(Tuple)>::type;
		storage_type data;
		Tuple* tp = new (&data) Tuple;
		*tp = args_tuple;

		it->second(tp, ptr);
	}

	//检验是否已经重复
	void check_duplicate(const std::string& key) 
	{
		auto it = m_Invokers_Map.find(key);
		if (it != m_Invokers_Map.end())
			assert("duplicate register");//assert最好重新定义防止运行时崩溃
	}

	template<typename Function>
	std::string get_key(std::string const & additional) 
	{
		if (!additional.empty())
			return additional;

		using tuple_type = typename function_traits<Function>::bare_tuple_type;
		auto key = get_name_from_tuple(tuple_type{});
		return key;
	}

	template <typename U, typename ... Args>
	std::enable_if_t<std::is_same<U, FnKey>::value, std::string> get_key(U&& u, Args&& ... args) 
	{
		return u.key;
	}

	template <typename U, typename ... Args>
	std::enable_if_t<!std::is_same<U, FnKey>::value, std::string> get_key(U&& u, Args&& ... args) 
	{
		return get_name(std::forward<U>(u), std::forward<Args>(args)...);
	}


#pragma endregion 辅助函数
	//返回类型后置
	template <typename U, typename ... Args>
	auto get_args_tuple(std::true_type, U&& u, Args&& ... args)-> decltype(std::make_tuple(std::forward<Args>(args)...)) {
		return std::make_tuple(std::forward<Args>(args)...);
	}

	//返回类型后置
	template <typename U, typename ... Args>
	auto get_args_tuple(std::false_type, U&& u, Args&& ... args) ->decltype(std::make_tuple(std::forward<U>(u), std::forward<Args>(args)...)) {
		return std::make_tuple(std::forward<U>(u), std::forward<Args>(args)...);
	}

	template<typename... Args>
	std::string get_name(Args&&... args) 
	{
		std::string name = "";
		std::initializer_list<int>{(name += type_name<Args>(), 0)...};
		return name;
	}

	template<typename T>
	std::string get_name_from_tuple(T t) 
	{
		std::string name = "";

		for_each(t, caller{ name }, std::make_index_sequence<std::tuple_size<T>::value>{});
		return name;
	}



	template<typename Function>
	struct invoker
	{
		static inline void apply(const Function& func, void* bl, void* result)
		{
			using tuple_type = typename function_traits<Function>::bare_tuple_type;
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
			if (result)
				*(decltype(r)*)result = r;
		}

		template<typename F, size_t... I, typename ... Args>
		static auto call_helper(const F& f, const std::index_sequence<I...>&, const std::tuple<Args...>& tup)-> typename std::result_of<F(Args...)>::type
		{
			return f(std::get<I>(tup)...);
		}

		template <typename Self>
		static inline void apply_mem(Function f, Self* self, void* bl, void* result)
		{
			using tuple_type = typename function_traits<Function>::bare_tuple_type;
			const tuple_type* tp = static_cast<tuple_type*>(bl);

			using return_type = typename function_traits<Function>::return_type;
			call_mem(f, self, *tp, result, std::integral_constant<bool, std::is_void<return_type>::value>{});
		}

		template<typename F, typename Self, typename ... Args>
		static void call_mem(F f, Self* self, const std::tuple<Args...>& tp, void*, std::true_type)
		{
			call_member_helper(f, self, std::make_index_sequence<sizeof...(Args)>{}, tp);
		}

		template<typename F, typename Self, typename ... Args>
		static void call_mem(F f, Self* self, const std::tuple<Args...>& tp, void* result, std::false_type)
		{
			auto r = call_member_helper(f, self, std::make_index_sequence<sizeof...(Args)>{}, tp);
			if (result)
				*(decltype(r)*)result = r;
		}

		template<typename F, typename Self, size_t... I, typename ... Args>
		static auto call_member_helper(F f, Self* self, const std::index_sequence<I...>&, const std::tuple<Args...>& tup)-> decltype((self->*f)(std::get<I>(tup)...))
		{
			return (self->*f)(std::get<I>(tup)...);
		}
	};

};