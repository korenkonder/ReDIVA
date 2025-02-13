/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"

namespace MoviePlayLib {
    class DECLSPEC_UUID("5D520F14-C168-43DA-B799-4FEC99669FB0")
        CommandArgs : public IUnknown {
    public:
        enum class Type {
            Open = 0,
            SetPosition,
            Play,
            Pause,
            Close,
            Max,
        };

    protected:
        RefCount ref_count;
        Lock lock;
        Type type;
        int64_t position;
        OLECHAR* str;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        CommandArgs(CommandArgs::Type type);
        virtual ~CommandArgs();

        virtual Type GetType();
        virtual int64_t GetPosition();
        virtual OLECHAR* GetPath();
        virtual HRESULT SetType(Type value);
        virtual HRESULT SetPosition(int64_t value);
        virtual HRESULT SetPath(const OLECHAR* value);

        static HRESULT Create(CommandArgs::Type type, CommandArgs*& ptr);
        static void Destroy(CommandArgs* ptr);

        inline void Destroy() {
            Destroy(this);
        }
    };
}
