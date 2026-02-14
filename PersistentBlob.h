#pragma once
#ifndef PERSISTENT_BLOB_H
#define PERSISTENT_BLOB_H

#include "IHAL.h"

class PersistentBlobBase
{
public:
	static void Initialize(IHAL *HAL)
	{
		GetHAL() = HAL;
	}

	static void EreasAll(void)
	{
		GetHAL()->EreasPersistentData();
	}

	static void SaveAll(void)
	{
		GetHAL()->SavePersistentData();
	}

protected:
	static IHAL *&GetHAL(void)
	{
		static IHAL *hal = nullptr;

		return hal;
	}
};

template <typename T>
class PersistentBlob : PersistentBlobBase
{
public:
	PersistentBlob(uint16 ID)
		: m_ID(ID),
		  m_Data{}
	{
	}

	void Initialize(const T &DefaultData = {})
	{
		if (GetHAL()->ContainsPersistentData(m_ID))
			GetHAL()->GetPersistentData(m_ID, &m_Data, sizeof(T));
		else
		{
			GetHAL()->InitializePersistentData(m_ID);
			m_Data = DefaultData;
		}
	}

	void Flush()
	{
		GetHAL()->SetPersistentData(m_ID, &m_Data, sizeof(T));
	}

	void Set(const T &Object)
	{
		m_Data = Object;

		Flush();
	}

	T &Get(void)
	{
		return m_Data;
	}

	const T &Get(void) const
	{
		return m_Data;
	}

	uint16 GetID(void) const
	{
		return m_ID;
	}

private:
	uint16 m_ID;
	T m_Data;
};

#endif