#pragma once
#ifndef CONTEXT_CALLBACK_H
#define CONTEXT_CALLBACK_H

#include "DataTypes.h"
#include <utility>
#include <new>

template <typename Signature, uint8 StorageSize = 16>
class FixedFunction;

template <typename Ret, typename... ArgsT, uint8 StorageSize>
class FixedFunction<Ret(ArgsT...), StorageSize>
{
public:
	FixedFunction(void)
		: m_Invoker(nullptr)
	{}

	template <typename F>
	FixedFunction(F Callable)
	{
		static_assert(sizeof(F) <= StorageSize, "Callable size exceeds FixedFunction storage!");
		static_assert(alignof(F) <= alignof(std::max_align_t), "Callable alignment mismatch!");

		new (static_cast<void*>(m_Storage)) F(std::move(Callable));

		m_Invoker = [](void* src, ArgsT&&... Args) -> Ret
			{
				return (*reinterpret_cast<F*>(src))(std::forward<ArgsT>(Args)...);
			};
	}

	template <typename C>
	FixedFunction(C* Instance, Ret(C::* Method)(ArgsT...))
	{
		struct BoundMethod
		{
			using MemberFuctionT = Ret(C::* Method)(ArgsT...);

			C* Instance;
			MemberFuctionT MemberFunction;

			Ret operator()(ArgsT&&... Args)
			{
				return (Instance->*MemberFunction)(std::forward<ArgsT>(Args)...);
			}
		};

		static_assert(sizeof(BoundMethod) <= StorageSize, "Bound method exceeds storage size!");

		BoundMethod bound = { Instance, Method };

		new (static_cast<void*>(m_Storage)) BoundMethod(bound);

		m_Invoker = [](void* Src, ArgsT&&... Args) -> Ret
			{
				return (*reinterpret_cast<BoundMethod*>(Src))(std::forward<ArgsT>(Args)...);
			};
	}

	Ret operator()(ArgsT... Args) const
	{
		if (m_Invoker)
			return m_Invoker(const_cast<void*>(static_cast<const void*>(m_Storage)), std::forward<ArgsT>(Args)...);

		return Ret();
	}

	explicit operator bool() const
	{
		return m_Invoker != nullptr;
	}

private:
	using InvokerFuctionT = Ret(*)(void*, ArgsT&&...);

	InvokerFuctionT m_Invoker;
	alignas(std::max_align_t) char m_Storage[StorageSize];
};

#endif