/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"

namespace MoviePlayLib {
    class DECLSPEC_UUID("5D520F14-C168-43DA-B799-4FEC99669FB0")
        CommandArgs : public IUnknown {
    protected:
        RefCount m_ref;
        SlimLock m_lock;
        uint32_t m_command;
        int64_t m_value;
        OLECHAR* m_bstr;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        CommandArgs(uint32_t cmd);
        virtual ~CommandArgs();

        virtual uint32_t GetCommand();
        virtual int64_t GetValue();
        virtual OLECHAR* GetString();
        virtual HRESULT SetCommand(uint32_t val);
        virtual HRESULT SetValue(int64_t value);
        virtual HRESULT SetString(const OLECHAR* text);

        static void Destroy(CommandArgs* ptr);

        inline void Destroy() {
            Destroy(this);
        }
    };

    extern HRESULT CreateCommandArgs(uint32_t cmd, CommandArgs*& pp);
}
