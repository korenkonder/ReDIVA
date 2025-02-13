/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"
#include <list>

namespace MoviePlayLib {
    struct MFSampleList {
        Lock lock;
        uint32_t index;
        bool shutdown;
        std::list<IMFSample*> list;

        MFSampleList();
        ~MFSampleList();

        void AddSample(IMFSample* mf_sample, bool check);
        bool CheckVideoIndex(IMFSample* mf_sample, bool check);
        uint32_t ClearList();
        int64_t GetSampleTime();
        IMFSample* PopSample();

        inline bool CanShutdown() {
            lock.Acquire();
            bool can_shutdown = shutdown && !list.size();
            lock.Release();
            return can_shutdown;
        }

        inline size_t GetSamplesCount() {
            lock.Acquire();
            size_t size = list.size();
            lock.Release();
            return size;
        }

        inline void ResetShutdown() {
            lock.Acquire();
            shutdown = FALSE;
            lock.Release();
        }

        inline void SetShutdown() {
            lock.Acquire();
            shutdown = TRUE;
            lock.Release();
        }
    };
}
