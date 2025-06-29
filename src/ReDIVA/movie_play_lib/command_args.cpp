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
        return ++m_ref;
    }

    ULONG CommandArgs::Release() {
        if (!--m_ref)
            Destroy();
        return m_ref;
    }

    CommandArgs::~CommandArgs() {
        if (m_bstr)
            SysFreeString(m_bstr);
    }

    uint32_t CommandArgs::GetCommand() {
        return m_command;
    }

    int64_t CommandArgs::GetValue() {
        return m_value;
    }

    OLECHAR* CommandArgs::GetString() {
        return m_bstr;
    }

    HRESULT CommandArgs::SetCommand(uint32_t val) {
        m_command = val;
        return S_OK;
    }

    HRESULT CommandArgs::SetValue(int64_t value) {
        m_value = value;
        return S_OK;
    }

    CommandArgs::CommandArgs(uint32_t cmd)
        : m_ref(), m_lock(), m_command(cmd), m_value(), m_bstr() {

    }

    HRESULT CommandArgs::SetString(const OLECHAR* text) {
        if (m_bstr) {
            SysFreeString(m_bstr);
            m_bstr = 0;
        }

        if (!text)
            return S_OK;

        m_bstr = SysAllocString(text);
        if (!m_bstr)
            return E_OUTOFMEMORY;
        return S_OK;
    }

    inline void CommandArgs::Destroy(CommandArgs* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }

    HRESULT CreateCommandArgs(uint32_t cmd, CommandArgs*& pp) {
        CommandArgs* p = new CommandArgs(cmd);
        if (!p)
            return E_OUTOFMEMORY;

        pp = p;
        p->AddRef();
        p->Release();
        return S_OK;
    }
}
