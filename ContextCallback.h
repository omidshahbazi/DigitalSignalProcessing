#pragma once
#ifndef CONTEXT_CALLBACK_H
#define CONTEXT_CALLBACK_H

template <typename ReturnType, typename... ArgsType>
class ContextCallback
{
public:
	typedef ReturnType (*CallbackType)(void *Context, ArgsType...);

	ContextCallback(void)
		: m_Context(nullptr),
		  m_Callback(nullptr)
	{
	}

	ContextCallback(CallbackType Context)
		: m_Context(nullptr),
		  m_Callback(Context)
	{
	}

	ContextCallback(void *Context, CallbackType Callback)
		: m_Context(Context),
		  m_Callback(Callback)
	{
	}

	ReturnType Call(ArgsType... Args)
	{
		if (m_Callback == nullptr)
			return;

		return m_Callback(m_Context, Args...);
	}

	ReturnType Call(ArgsType... Args) const
	{
		if (m_Callback == nullptr)
			return {};

		return m_Callback(m_Context, Args...);
	}

	ReturnType operator()(ArgsType... Args)
	{
		return Call(Args...);
	}

	ReturnType operator()(ArgsType... Args) const
	{
		return Call(Args...);
	}

	ContextCallback<ReturnType, ArgsType...> &operator=(const ContextCallback<ReturnType, ArgsType...> &Other)
	{
		m_Context = Other.m_Context;
		m_Callback = Other.m_Callback;

		return *this;
	}

	bool operator==(const ContextCallback<ReturnType, ArgsType...> &Other)
	{
		return (m_Context == Other.m_Context && m_Callback == Other.m_Callback);
	}

	bool operator!=(const ContextCallback<ReturnType, ArgsType...> &&Other)
	{
		return !(*this == Other);
	}

private:
	void *m_Context;
	CallbackType m_Callback;
};

#endif