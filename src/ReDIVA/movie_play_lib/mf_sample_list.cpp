/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mf_sample_list.hpp"

namespace MoviePlayLib {
    MFSampleList::MFSampleList() : lock(), index(1), shutdown(), list() {

    }

    MFSampleList::~MFSampleList() {
        ClearList();
    }

    void MFSampleList::AddSample(IMFSample* mf_sample, bool check) {
        lock.Acquire();
        if (CheckVideoIndex(mf_sample, check)) {
            mf_sample->AddRef();
            list.push_back(mf_sample);
        }
        lock.Release();
    }

    bool MFSampleList::CheckVideoIndex(IMFSample* mf_sample, bool check) {
        if (!mf_sample)
            return false;

        uint32_t video_index = 0;
        if (!check || SUCCEEDED(mf_sample->GetUINT32(VideoIndexGUID, &video_index)) && video_index == index)
            return true;
        return false;
    }

    uint32_t MFSampleList::ClearList() {
        lock.Acquire();
        while (list.size()) {
            list.front()->Release();
            list.pop_front();
        }
        uint32_t index = ++this->index;
        lock.Release();
        return index;
    }

    IMFSample* MFSampleList::PopSample() {
        lock.Acquire();
        IMFSample* mf_sample = 0;
        if (list.size()) {
            mf_sample = list.front();
            list.pop_front();
        }
        lock.Release();
        return mf_sample;
    }

    int64_t MFSampleList::GetSampleTime() {
        lock.Acquire();
        int64_t sample_time = INT64_MAX;
        if (list.size())
            list.front()->GetSampleTime(&sample_time);
        lock.Release();
        return sample_time;
    }
}
