/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "sample_queue.hpp"

namespace MoviePlayLib {
    SampleQueue::SampleQueue() : m_lock(), m_token(1), m_eos(), m_list() {

    }

    SampleQueue::~SampleQueue() {
        clear();
    }

    uint32_t SampleQueue::clear() {
        m_lock.Acquire();
        while (m_list.size()) {
            m_list.front()->Release();
            m_list.pop_front();
        }
        uint32_t token = ++m_token;
        m_lock.Release();
        return token;
    }

    bool SampleQueue::is_valid(IMFSample* pSample, bool check) {
        if (!pSample)
            return false;

        uint32_t token = 0;
        if (!check || SUCCEEDED(pSample->GetUINT32(SAMPLE_RESET_TOKEN, &token)) && token == m_token)
            return true;
        return false;
    }

    int64_t SampleQueue::peek_time() {
        m_lock.Acquire();
        int64_t hnsSampleTime = INT64_MAX;
        if (m_list.size())
            m_list.front()->GetSampleTime(&hnsSampleTime);
        m_lock.Release();
        return hnsSampleTime;
    }

    IMFSample* SampleQueue::pop() {
        m_lock.Acquire();
        IMFSample* pSample = 0;
        if (m_list.size()) {
            pSample = m_list.front();
            m_list.pop_front();
        }
        m_lock.Release();
        return pSample;
    }

    void SampleQueue::push(IMFSample* pSample, bool check) {
        m_lock.Acquire();
        if (is_valid(pSample, check)) {
            pSample->AddRef();
            m_list.push_back(pSample);
        }
        m_lock.Release();
    }
}
