#pragma once
#ifndef WAVE_SHAPER_FILTER_H
#define WAVE_SHAPER_FILTER_H

#include "Filter.h"
#include "../Math.h"
#include "../Debug.h"

template <typename T>
class WaveShaperFilter : public Filter<T, MIN_SAMPLE_RATE>
{
public:
    struct TablePoints
    {
    public:
        T InputValue;
        T OutputValue;
    };

public:
    WaveShaperFilter(void)
        : m_Table(nullptr),
          m_Length(0),
          m_MinInput(-1.0f),
          m_MaxInput(1.0f),
          m_InputRangeInv(0)
    {
    }

    void SetTable(const TablePoints *Table, uint8 Length)
    {
        ASSERT(Table != nullptr, "Invalid Table");
        ASSERT(Length > 1, "Invalid Length %f", Length);

        m_Table = Table;
        m_Length = Length;

        m_MinInput = Table[0].InputValue;
        m_MaxInput = Table[Length - 1].InputValue;
        
        m_InputRangeInv = (float)(Length - 1) / (m_MaxInput - m_MinInput);
    }
    const TablePoints *GetTable(void) const
    {
        return m_Table;
    }
    uint8 GetLength(void) const
    {
        return m_Length;
    }

    void Process(T *Buffer, uint8 Count) override
    {
        const float minIn = m_MinInput;
        const float maxIn = m_MaxInput;
        const float rangeInv = m_InputRangeInv;
        const uint8 maxIdx = m_Length - 1;

        for (uint8 i = 0; i < Count; ++i)
        {
            float input = Math::Clamp(Buffer[i], minIn, maxIn);
            
            float pos = (input - minIn) * rangeInv;
            uint8 index = static_cast<uint8>(pos);
            float frac = pos - index;

            if (index >= maxIdx) 
            {
                Buffer[i] = m_Table[maxIdx].OutputValue;
            }
            else 
            {
                Buffer[i] = Math::Lerp(m_Table[index].OutputValue, m_Table[index + 1].OutputValue, frac);
            }
        }
    }

private:
    const TablePoints *m_Table;
    uint8 m_Length;
    float m_MinInput;
    float m_MaxInput;
    float m_InputRangeInv;
};
#endif