/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"
#include <list>

namespace MoviePlayLib {
    struct SampleQueue {
        SlimLock m_lock;
        uint32_t m_token;
        bool m_eos;
        std::list<IMFSample*> m_list;

        SampleQueue();
        ~SampleQueue();

        uint32_t clear();
        bool is_valid(IMFSample* pSample, bool check);
        int64_t peek_time();
        IMFSample* pop();
        void push(IMFSample* pSample, bool check);

        inline size_t size() {
            m_lock.Acquire();
            size_t size = m_list.size();
            m_lock.Release();
            return size;
        }

        inline bool is_eos() {
            m_lock.Acquire();
            bool eos = m_eos && !m_list.size();
            m_lock.Release();
            return eos;
        }

        inline void reset_eos() {
            m_lock.Acquire();
            m_eos = FALSE;
            m_lock.Release();
        }

        inline void set_eos() {
            m_lock.Acquire();
            m_eos = TRUE;
            m_lock.Release();
        }
    };
}
