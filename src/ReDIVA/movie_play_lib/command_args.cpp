/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "command_args.hpp"

namespace MoviePlayLib {
    HRESULT CommandArgs::QueryInterface(const IID& riid, void** ppvObject) {
        if (!ppvObject)
            return E_INVALIDARG;

        if (riid == __uuidof(CommandArgs) || riid == __uuidof(IUnknown)) {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }

        *ppvObject = 0;
        return E_NOINTERFACE;
    }

    ULONG CommandArgs::AddRef() {
        return ++ref_count;
    }

    ULONG CommandArgs::Release() {
        if (!--ref_count)
            Destroy();
        return ref_count;
    }

    CommandArgs::~CommandArgs() {
        if (str)
            SysFreeString(str);
    }

    CommandArgs::Type CommandArgs::GetType() {
        return type;
    }

    int64_t CommandArgs::GetPosition() {
        return position;
    }

    OLECHAR* CommandArgs::GetPath() {
        return str;
    }

    HRESULT CommandArgs::SetType(CommandArgs::Type value) {
        type = value;
        return S_OK;
    }

    HRESULT CommandArgs::SetPosition(int64_t value) {
        position = value;
        return S_OK;
    }

    CommandArgs::CommandArgs(CommandArgs::Type type)
        : ref_count(), lock(), type(type), position(), str() {

    }

    HRESULT CommandArgs::SetPath(const OLECHAR* value) {
        if (str) {
            SysFreeString(str);
            str = 0;
        }

        if (!value)
            return S_OK;

        str = SysAllocString(value);
        if (!str)
            return E_OUTOFMEMORY;
        return S_OK;
    }

    HRESULT CommandArgs::Create(CommandArgs::Type type, CommandArgs*& ptr) {
        CommandArgs* p = new CommandArgs(type);
        if (!p)
            return E_OUTOFMEMORY;

        ptr = p;
        p->AddRef();
        p->Release();
        return S_OK;
    }

    inline void CommandArgs::Destroy(CommandArgs* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }
}
