/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <list>
#include <string>
#include <vector>
#include "../default.hpp"
#include "../prj/vector_pair_combine.hpp"

struct SprIdMember {
    uint16_t idx;
    uint16_t set_idx;

    inline SprIdMember() {
        idx = (uint16_t)-1;
        set_idx = (uint16_t)-1;
    }

    inline SprIdMember(uint16_t in_idx, uint16_t in_set_idx) {
        idx = in_idx;
        set_idx = in_set_idx;
    }
};

union SprId {
    SprIdMember m;
    uint32_t w;

    inline SprId() {
        w = (uint32_t)-1;
    }

    inline SprId(uint32_t in_id) {
        w = in_id;
    }

    inline SprId(uint16_t in_idx, uint16_t in_set_idx) {
        m.idx = in_idx;
        m.set_idx = in_set_idx;
    }

    inline bool is_null() const {
        return w == (uint32_t)-1;
    }

    inline bool not_null() const {
        return w != (uint32_t)-1;
    }
};

constexpr bool operator==(const SprId& left, const SprId& right) {
    return left.w == right.w;
}

constexpr bool operator!=(const SprId& left, const SprId& right) {
    return !(left == right);
}

constexpr bool operator<(const SprId& left, const SprId& right) {
    return left.w < right.w;
}

constexpr bool operator>(const SprId& left, const SprId& right) {
    return right < left;
}

constexpr bool operator<=(const SprId& left, const SprId& right) {
    return !(right < left);
}

constexpr bool operator>=(const SprId& left, const SprId& right) {
    return !(left < right);
}

struct SprDbFile {
    struct Spr {
        uint32_t uid;
        std::string name;
        uint16_t id;
        bool texture;

        Spr();
        ~Spr();
    };

    struct SprSet {
        uint32_t uid;
        std::string name;
        std::string file;
        std::vector<Spr> sprite;
        uint32_t id;

        SprSet();
        ~SprSet();
    };

    bool ready;
    bool modern;
    bool big_endian;
    bool is_x;

    std::vector<SprSet> set;

    SprDbFile();
    ~SprDbFile();

    void read(const char* path, bool modern);
    void read(const wchar_t* path, bool modern);
    void read(const void* data, size_t size, bool modern);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* dir, const char* file, uint32_t hash);
};

struct SprDb {
    struct Spr {
        uint32_t uid;
        std::string name;
        SprId id;
        int32_t reference;

        Spr();
        ~Spr();
    };

    struct SprSet {
        uint32_t uid;
        std::string name;
        std::string file;
        uint32_t id;

        SprSet();
        ~SprSet();
    };

    std::list<SprSet> set;
    prj::vector_pair_combine<uint32_t, SprSet*> set_uid;
    prj::vector_pair_combine<uint32_t, SprSet*> set_idx;
    prj::vector_pair_combine<std::string, SprSet*> set_name;
    std::list<Spr> spr;
    prj::vector_pair_combine<uint32_t, Spr*> spr_uid;
    prj::vector_pair_combine<std::string, Spr*> spr_name;
    prj::vector_pair_combine<SprId, Spr*> spr_idx;

    static const SprSet s_err_set;
    static const Spr s_err_spr;

    SprDb();
    ~SprDb();

    void add(SprDbFile* spr_db_file);
    void clear();

    void addSet(uint32_t set_uid, uint32_t idx);
    void freeSet(uint32_t set_uid, uint32_t idx,
        const char* set_name, std::vector<uint32_t>& sprite_ids);
    void parse(const SprDbFile::SprSet* set_file, std::vector<uint32_t>& sprite_ids);

    uint32_t getSetSize() const;

    const char* getSetNameFromUid(uint32_t uid) const;
    const char* getSetFileFromUid(uint32_t uid) const;
    uint32_t getSetIdFromUid(uint32_t uid) const;
    uint32_t getSetUidFromIdx(uint32_t idx) const;
    const char* getSetNameFromIdx(uint32_t idx) const;
    const char* getSetFileFromIdx(uint32_t idx) const;
    uint32_t getSetUidFromName(const std::string& name) const;
    uint32_t getSetIdFromName(const std::string& name) const;
    uint32_t getSetIdFromIdx(uint32_t idx) const;

    uint32_t getSprIdFromUid(uint32_t uid) const;
    uint32_t getSprUidFromName(const std::string& name) const;
    uint32_t getSprIdFromName(const std::string& name) const;
    uint32_t getSprUidFromIdx(uint32_t set_uid, uint32_t idx) const;
    uint32_t getSprIdFromIdx(uint32_t set_uid, uint32_t idx) const;
    uint32_t getTexUidFromIdx(uint32_t set_uid, uint32_t idx) const;
    uint32_t getTexIdFromIdx(uint32_t set_uid, uint32_t idx) const;

    const SprDb::SprSet& getSetFromUid(uint32_t uid) const;
    const SprDb::SprSet& getSetFromIdx(uint32_t idx) const;
    const SprDb::SprSet& getSetFromName(const std::string& name) const;
    const SprDb::Spr& getSprFromUid(uint32_t uid) const;
    const SprDb::Spr& getSprFromName(const std::string& name) const;
    const SprDb::Spr& getSprFromIdx(uint32_t set_uid, uint32_t idx) const;
    const SprDb::Spr& getTexFromIdx(uint32_t set_uid, uint32_t idx) const;
};
