#pragma once
#include "iprogress.h"

template <typename T>
class ProgressBase : public IProgress
{
public:
    ProgressBase()
        : m_maxValue()
        , m_showedPercents()
    {
    }

    void SetMaxValue(uint64_t maxValue) override
    {
        m_maxValue = maxValue;
        m_showedPercents = 0;
    }

    void ShowProgress(T currentValue) const override
    {
        T currentPercents = currentValue * 100 / (m_maxValue - 1);
        if (currentPercents != m_showedPercents)
        {
            DrawProgress(currentPercents);
            m_showedPercents = currentPercents;
        }
    }

protected:
    T m_maxValue;
    mutable T m_showedPercents;
};
