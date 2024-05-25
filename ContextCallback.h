#pragma once
#ifndef CONTEXT_CALLBACK_H
#define CONTEXT_CALLBACK_H

template <typename ReturnType, typename... ArgTypes>
class ContextCallback
{
public:
	typedef ReturnType (*CallbackType)(void *Context, ArgTypes...);

public:
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

	ReturnType Call(ArgTypes... Args)
	{
		if (m_Callback == nullptr)
			return ReturnType();

		return m_Callback(m_Context, Args...);
	}

	ReturnType Call(ArgTypes... Args) const
	{
		if (m_Callback == nullptr)
			return {};

		return m_Callback(m_Context, Args...);
	}

	ReturnType operator()(ArgTypes... Args)
	{
		return Call(Args...);
	}

	ReturnType operator()(ArgTypes... Args) const
	{
		return Call(Args...);
	}

	ContextCallback<ReturnType, ArgTypes...> &operator=(const ContextCallback<ReturnType, ArgTypes...> &Other)
	{
		m_Context = Other.m_Context;
		m_Callback = Other.m_Callback;

		return *this;
	}

	bool operator==(const ContextCallback<ReturnType, ArgTypes...> &Other)
	{
		return (m_Context == Other.m_Context && m_Callback == Other.m_Callback);
	}

	bool operator!=(const ContextCallback<ReturnType, ArgTypes...> &&Other)
	{
		return !(*this == Other);
	}

private:
	void *m_Context;
	CallbackType m_Callback;
};

template <typename... ArgTypes>
class ContextCallback<void, ArgTypes...>
{
public:
	typedef void (*CallbackType)(void *Context, ArgTypes...);

public:
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

	void Call(ArgTypes... Args)
	{
		if (m_Callback == nullptr)
			return;

		m_Callback(m_Context, Args...);
	}

	void Call(ArgTypes... Args) const
	{
		if (m_Callback == nullptr)
			return;

		m_Callback(m_Context, Args...);
	}

	void operator()(ArgTypes... Args)
	{
		Call(Args...);
	}

	void operator()(ArgTypes... Args) const
	{
		Call(Args...);
	}

	ContextCallback<void, ArgTypes...> &operator=(const ContextCallback<void, ArgTypes...> &Other)
	{
		m_Context = Other.m_Context;
		m_Callback = Other.m_Callback;

		return *this;
	}

	bool operator==(const ContextCallback<void, ArgTypes...> &Other)
	{
		return (m_Context == Other.m_Context && m_Callback == Other.m_Callback);
	}

	bool operator!=(const ContextCallback<void, ArgTypes...> &&Other)
	{
		return !(*this == Other);
	}

private:
	void *m_Context;
	CallbackType m_Callback;
};

#endif