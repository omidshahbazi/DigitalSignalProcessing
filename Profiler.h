#ifdef ENABLE_PROFILER

#pragma once
#ifndef PROFILER_H
#define PROFILER_H

#include "IHAL.h"
#include "Log.h"

class ProfilerFrame;

class Profiler
{
	friend class ProfilerFrame;

private:
	static constexpr uint8 FRAME_COUNT = 128;

	struct FrameInfo
	{
	public:
		uint8 DeepIndex;
		cstr Info;
		uint32 TotalTicks;
	};

public:
	static Profiler *Get(void)
	{
		static Profiler profiler;

		return &profiler;
	}

private:
	Profiler(void)
		: m_HAL(nullptr),
		  m_Frames{},
		  m_FrameCount(0),
		  m_Indent(0)
	{
	}

public:
	void Initialize(IHAL *HAL)
	{
		m_HAL = HAL;
	}

	void Print(void) const
	{
		const uint32 TimeFrequency = m_HAL->GetTimeFrequency();

		for (uint8 i = 0; i < m_FrameCount; ++i)
		{
			const FrameInfo &info = m_Frames[i];

			volatile int dummy = 0;
			// Log::WriteInfo("%*s %s %u %fus", info.DeepIndex, "", info.Info, info.TotalTicks, (float)info.TotalTicks / TimeFrequency);
		}
	}

	void Reset(void)
	{
		m_FrameCount = 0;
		m_Indent = 0;
	}

private:
	uint16 Begin(cstr Info)
	{
		if (m_HAL == nullptr)
			m_HAL->Crash();

		m_Frames[m_FrameCount++] = {m_Indent, Info, m_HAL->GetTimeSinceStartupTicks()};
		
		// ++m_Indent; Wrong way

		return m_FrameCount - 1;
	}

	void End(uint16 ID)
	{
		FrameInfo &info = m_Frames[ID];
		info.TotalTicks = m_HAL->GetTimeSinceStartupTicks() - info.TotalTicks;

		--m_Indent;
	}

private:
	IHAL *m_HAL;
	FrameInfo m_Frames[FRAME_COUNT];
	uint8 m_FrameCount;
	uint8 m_Indent;
};

class ProfilerFrame
{
public:
	ProfilerFrame(cstr Info)
	{
		m_ID = Profiler::Get()->Begin(Info);
	}

	~ProfilerFrame(void)
	{
		Profiler::Get()->End(m_ID);
	}

private:
	uint16 m_ID;
};

#endif

#define INITIALIZE_PROFILER(HAL) Profiler::Get()->Initialize(HAL)
#define PRINT_AND_RESET_PROFILER()            \
	{                                         \
		Profiler *profiler = Profiler::Get(); \
		profiler->Print();                    \
		profiler->Reset();                    \
	}
#define PROFILE() ProfilerFrame CONCAT(__profile_, __LINE__)(__PRETTY_FUNCTION__);
#else
#define INITIALIZE_PROFILER(HAL)
#define PRINT_AND_RESET_PROFILER()
#define PROFILE()
#endif