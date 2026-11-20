#pragma once
#ifndef NOT_NULL_H
#define NOT_NULL_H

#include "DataTypes.h"
#include "Debug.h"

template<typename T>
class NotNull
{
public:
	NotNull(void) = delete;
	NotNull(decltype(nullptr)) = delete;
	NotNull& operator=(decltype(nullptr)) = delete;

	NotNull(T* Value)
		: m_Value{ Value }
	{
		ASSERT_NOT_NULL(m_Value);
	}

	template<typename U>
	NotNull(const NotNull<U>& Other)
		: m_Value{ Other.Get() }
	{}

	operator T* (void) const
	{
		return m_Value;
	}

	T* operator->(void) const
	{
		return m_Value;
	}

	T& operator*(void) const
	{
		return *m_Value;
	}

private:
	T* m_Value;
};

template<>
class NotNull<cstr>
{
public:
	NotNull(void) = delete;
	NotNull(decltype(nullptr)) = delete;
	NotNull& operator=(decltype(nullptr)) = delete;

	NotNull(cstr Value)
		: m_Value{ Value }
	{
		ASSERT_NOT_NULL(m_Value);
	}

	operator cstr(void) const
	{
		return m_Value;
	}

	cstr operator->(void) const
	{
		return m_Value;
	}

	cstr& operator*(void)
	{
		return m_Value;
	}

private:
	cstr m_Value;
};

#endif