#include <cstdint>
#include <cstdio>
#include <cwchar>
#include <cmath>

namespace {

struct RootHandle {
    std::uint32_t value = 0;
    ~RootHandle() {
        if (value && g_api.gchandle_free) g_api.gchandle_free(value);
    }
    bool Hold(Il2CppObject* object) {
        if (!object) return false;
        value = g_api.gchandle_new(object, false);
        return value != 0;
    }
};

struct MethodTarget {
    Il2CppClass* klass = nullptr;
    Il2CppObject* instance = nullptr;
    const MethodInfo* method = nullptr;
    const char* owner = nullptr;
};

const MethodInfo* FindMethodExact(Il2CppClass* klass, const char* name,
                                  const char* const* paramTypes, std::uint32_t count) {
    if (!klass || !name) return nullptr;
    for (Il2CppClass* c = klass; c; c = g_api.class_get_parent(c)) {
        void* iter = nullptr;
        while (const MethodInfo* method = g_api.class_get_methods(c, &iter)) {
            if (!Eq(g_api.method_get_name(method), name) ||
                g_api.method_get_param_count(method) != count) continue;
            bool match = true;
            for (std::uint32_t i = 0; i < count; ++i) {
                if (!ParamType(method, i, paramTypes[i])) {
                    match = false;
                    break;
                }
            }
            if (match) return method;
        }
    }
    return nullptr;
}

bool InvokeObject(const MethodInfo* method, void* instance, void** args,
                  Il2CppObject*& out, wchar_t* detail, std::size_t cap) {
    out = nullptr;
    if (!method) {
        SetText(detail, cap, L"Method chưa resolve");
        return false;
    }
    void* exception = nullptr;
    out = g_api.runtime_invoke(method, instance, args, &exception);
    if (exception) {
        SetText(detail, cap, L"runtime_invoke ném managed exception");
        out = nullptr;
        return false;
    }
    return true;
}

bool CopyManagedString(Il2CppObject* object, wchar_t* out, std::size_t cap) {
    if (!out || cap == 0) return false;
    out[0] = 0;
    if (!object) return true;
    Il2CppClass* klass = g_api.object_get_class(object);
    const char* name = klass ? g_api.class_get_name(klass) : nullptr;
    if (!name || !Eq(name, "String")) return false;
    auto* str = reinterpret_cast<Il2CppString*>(object);
    const std::int32_t length = g_api.string_length(str);
    const std::uint16_t* chars = g_api.string_chars(str);
    if (length < 0 || !chars) return false;
    const std::size_t n = static_cast<std::size_t>(length);
    const std::size_t copyCount = n < cap - 1 ? n : cap - 1;
    for (std::size_t i = 0; i < copyCount; ++i) out[i] = static_cast<wchar_t>(chars[i]);
    out[copyCount] = 0;
    return true;
}

bool BoxedNumberToU64(Il2CppObject* object, std::uint64_t& out) {
    out = 0;
    if (!object) return false;
    Il2CppClass* klass = g_api.object_get_class(object);
    const char* name = klass ? g_api.class_get_name(klass) : nullptr;
    void* raw = g_api.object_unbox(object);
    if (!name || !raw) return false;
    if (Eq(name, "Int32")) {
        const std::int32_t v = *reinterpret_cast<const std::int32_t*>(raw);
        if (v < 0) return false;
        out = static_cast<std::uint64_t>(v);
        return true;
    }
    if (Eq(name, "UInt32")) {
        out = *reinterpret_cast<const std::uint32_t*>(raw);
        return true;
    }
    if (Eq(name, "Int64")) {
        const std::int64_t v = *reinterpret_cast<const std::int64_t*>(raw);
        if (v < 0) return false;
        out = static_cast<std::uint64_t>(v);
        return true;
    }
    if (Eq(name, "UInt64")) {
        out = *reinterpret_cast<const std::uint64_t*>(raw);
        return true;
    }
    if (Eq(name, "Int16")) {
        const std::int16_t v = *reinterpret_cast<const std::int16_t*>(raw);
        if (v < 0) return false;
        out = static_cast<std::uint64_t>(v);
        return true;
    }
    if (Eq(name, "UInt16")) {
        out = *reinterpret_cast<const std::uint16_t*>(raw);
        return true;
    }
    if (Eq(name, "Double")) {
        const double v = *reinterpret_cast<const double*>(raw);
        if (!std::isfinite(v) || v < 0.0) return false;
        const std::uint64_t iv = static_cast<std::uint64_t>(v);
        if (static_cast<double>(iv) != v) return false;
        out = iv;
        return true;
    }
    if (Eq(name, "Single")) {
        const float v = *reinterpret_cast<const float*>(raw);
        if (!std::isfinite(v) || v < 0.0f) return false;
        const std::uint64_t iv = static_cast<std::uint64_t>(v);
        if (static_cast<float>(iv) != v) return false;
        out = iv;
        return true;
    }
    return false;
}

bool InvokeIntegerGetter(const MethodInfo* method, void* instance,
                         std::uint64_t& out, wchar_t* detail, std::size_t cap) {
    Il2CppObject* boxed = nullptr;
    if (!InvokeObject(method, instance, nullptr, boxed, detail, cap) || !boxed) return false;
    if (!BoxedNumberToU64(boxed, out)) {
        SetText(detail, cap, L"Getter số trả type không hỗ trợ");
        return false;
    }
    return true;
}

bool ReadNumericMember(Il2CppObject* object, const char* name, std::uint64_t& out) {
    out = 0;
    if (!object || !name) return false;
    Il2CppClass* klass = g_api.object_get_class(object);
    if (!klass) return false;

    char getterName[96]{};
    std::snprintf(getterName, sizeof(getterName), "get_%s", name);
    if (const MethodInfo* getter = FindMethod(klass, getterName, 0)) {
        wchar_t ignored[96]{};
        if (InvokeIntegerGetter(getter, object, out, ignored, _countof(ignored))) return true;
    }

    for (Il2CppClass* c = klass; c; c = g_api.class_get_parent(c)) {
        FieldInfo* field = g_api.class_get_field_from_name(c, name);
        if (!field || StaticField(field)) continue;
        const Il2CppType* type = g_api.field_get_type(field);
        char* typeName = type ? g_api.type_get_name(type) : nullptr;
        std::uint64_t storage = 0;
        g_api.field_get_value(object, field, &storage);
        bool ok = false;
        if (typeName) {
            if (Eq(typeName, "System.Int32")) {
                const auto v = static_cast<std::int32_t>(storage & 0xffffffffu);
                if (v >= 0) { out = static_cast<std::uint64_t>(v); ok = true; }
            } else if (Eq(typeName, "System.UInt32")) {
                out = static_cast<std::uint32_t>(storage & 0xffffffffu); ok = true;
            } else if (Eq(typeName, "System.Int64")) {
                const auto v = static_cast<std::int64_t>(storage);
                if (v >= 0) { out = static_cast<std::uint64_t>(v); ok = true; }
            } else if (Eq(typeName, "System.UInt64")) {
                out = storage; ok = true;
            }
            g_api.free_fn(typeName);
        }
        if (ok) return true;
    }
    return false;
}

bool ReadStringMember(Il2CppObject* object, const char* name, wchar_t* out, std::size_t cap) {
    if (!out || cap == 0) return false;
    out[0] = 0;
    if (!object || !name) return false;
    Il2CppClass* klass = g_api.object_get_class(object);
    if (!klass) return false;

    char getterName[96]{};
    std::snprintf(getterName, sizeof(getterName), "get_%s", name);
    if (const MethodInfo* getter = FindMethod(klass, getterName, 0)) {
        wchar_t ignored[96]{};
        Il2CppObject* value = nullptr;
        if (InvokeObject(getter, object, nullptr, value, ignored, _countof(ignored)) &&
            CopyManagedString(value, out, cap)) return true;
    }

    for (Il2CppClass* c = klass; c; c = g_api.class_get_parent(c)) {
        FieldInfo* field = g_api.class_get_field_from_name(c, name);
        if (!field || StaticField(field)) continue;
        const Il2CppType* type = g_api.field_get_type(field);
        char* typeName = type ? g_api.type_get_name(type) : nullptr;
        const bool isString = typeName && Eq(typeName, "System.String");
        if (typeName) g_api.free_fn(typeName);
        if (!isString) continue;
        Il2CppObject* value = nullptr;
        g_api.field_get_value(object, field, &value);
        return CopyManagedString(value, out, cap);
    }
    return false;
}

bool ReadStaticObjectField(Il2CppClass* klass, const char* fieldName, Il2CppObject*& out) {
    out = nullptr;
    if (!klass || !fieldName) return false;
    for (Il2CppClass* c = klass; c; c = g_api.class_get_parent(c)) {
        FieldInfo* field = g_api.class_get_field_from_name(c, fieldName);
        if (!field || !StaticField(field)) continue;
        g_api.field_static_get_value(field, &out);
        if (out) return true;
    }
    return false;
}

bool FindLiveInstance(Il2CppClass* klass, Il2CppObject*& out) {
    out = nullptr;
    if (!klass) return false;
    const MethodInfo* getInstance = FindMethod(klass, "get_Instance", 0);
    if (getInstance && StaticMethod(getInstance)) {
        wchar_t ignored[96]{};
        if (InvokeObjectNoArgs(getInstance, nullptr, out, ignored, _countof(ignored)) && out) return true;
    }
    const char* fieldNames[] = {
        "Instance", "instance", "_instance", "s_Instance", "<Instance>k__BackingField"
    };
    for (const char* fieldName : fieldNames) {
        if (ReadStaticObjectField(klass, fieldName, out)) return true;
    }

    const char* className = g_api.class_get_name(klass);
    for (Il2CppClass* c = klass; c && !out; c = g_api.class_get_parent(c)) {
        void* iter = nullptr;
        while (FieldInfo* field = g_api.class_get_fields(c, &iter)) {
            if (!StaticField(field)) continue;
            const Il2CppType* type = g_api.field_get_type(field);
            char* typeName = type ? g_api.type_get_name(type) : nullptr;
            const bool match = typeName && className && Contains(typeName, className);
            if (typeName) g_api.free_fn(typeName);
            if (!match) continue;
            g_api.field_static_get_value(field, &out);
            if (out) return true;
        }
    }
    return false;
}

bool ResolveTarget(const char* methodName, int argc, MethodTarget& target,
                   wchar_t* detail, std::size_t cap) {
    target = {};
    const Il2CppImage* image = GameImage();
    if (!image) {
        SetText(detail, cap, L"Không mở được Assembly-CSharp");
        return false;
    }
    struct Candidate { const char* ns; const char* klass; const char* label; };
    const Candidate candidates[] = {
        {"FGStudio.LuaSystem.API", "LuaSystemAPI_Game", "LuaSystemAPI_Game"},
        {"FGStudio.LuaSystem", "LuaSystemSharedData", "LuaSystemSharedData"},
    };
    for (const auto& candidate : candidates) {
        Il2CppClass* klass = g_api.class_from_name(image, candidate.ns, candidate.klass);
        if (!klass) continue;
        const MethodInfo* method = FindMethod(klass, methodName, argc);
        if (!method) continue;
        Il2CppObject* instance = nullptr;
        if (!StaticMethod(method) && !FindLiveInstance(klass, instance)) continue;
        target.klass = klass;
        target.instance = instance;
        target.method = method;
        target.owner = candidate.label;
        return true;
    }
    SetText(detail, cap, L"Không resolve được semantic Game/SharedData method hoặc live instance");
    return false;
}

bool InvokeOneInteger(const MethodInfo* method, void* instance, std::uint64_t value,
                      Il2CppObject*& result, wchar_t* detail, std::size_t cap) {
    result = nullptr;
    if (!method || g_api.method_get_param_count(method) != 1) {
        SetText(detail, cap, L"Method không có đúng 1 tham số");
        return false;
    }
    const Il2CppType* paramType = g_api.method_get_param(method, 0);
    char* typeName = paramType ? g_api.type_get_name(paramType) : nullptr;
    if (!typeName) {
        SetText(detail, cap, L"Không đọc được type tham số");
        return false;
    }
    std::int32_t i32 = static_cast<std::int32_t>(value);
    std::uint32_t u32 = static_cast<std::uint32_t>(value);
    std::int64_t i64 = static_cast<std::int64_t>(value);
    std::uint64_t u64 = value;
    void* arg = nullptr;
    if (Eq(typeName, "System.Int32")) arg = &i32;
    else if (Eq(typeName, "System.UInt32")) arg = &u32;
    else if (Eq(typeName, "System.Int64")) arg = &i64;
    else if (Eq(typeName, "System.UInt64")) arg = &u64;
    g_api.free_fn(typeName);
    if (!arg) {
        SetText(detail, cap, L"Type tham số RoleID/limit không được hỗ trợ");
        return false;
    }
    void* args[] = {arg};
    return InvokeObject(method, instance, args, result, detail, cap);
}

bool CollectionCount(Il2CppObject* collection, std::int32_t& count,
                     wchar_t* detail, std::size_t cap) {
    count = 0;
    if (!collection) {
        SetText(detail, cap, L"Collection null");
        return false;
    }
    Il2CppClass* klass = g_api.object_get_class(collection);
    const MethodInfo* getter = klass ? FindMethod(klass, "get_Count", 0) : nullptr;
    if (!getter && klass) getter = FindMethod(klass, "get_Length", 0);
    if (!getter) {
        SetText(detail, cap, L"Collection không có get_Count/get_Length");
        return false;
    }
    std::uint64_t value = 0;
    if (!InvokeIntegerGetter(getter, collection, value, detail, cap) || value > 4096) {
        SetText(detail, cap, L"Collection count không hợp lệ");
        return false;
    }
    count = static_cast<std::int32_t>(value);
    return true;
}

bool CollectionItem(Il2CppObject* collection, std::int32_t index, Il2CppObject*& item,
                    wchar_t* detail, std::size_t cap) {
    item = nullptr;
    if (!collection) return false;
    Il2CppClass* klass = g_api.object_get_class(collection);
    if (!klass) return false;
    const char* intParam[] = {"System.Int32"};
    const MethodInfo* getter = FindMethodExact(klass, "get_Item", intParam, 1);
    if (!getter) getter = FindMethodExact(klass, "GetValue", intParam, 1);
    if (!getter) {
        SetText(detail, cap, L"Collection không có get_Item/GetValue(Int32)");
        return false;
    }
    void* args[] = {&index};
    if (!InvokeObject(getter, collection, args, item, detail, cap)) return false;
    return true;
}

bool RunSemanticProbe(wchar_t* output, std::size_t outputCap,
                      wchar_t* detail, std::size_t detailCap) {
    if (output && outputCap) output[0] = 0;
    if (!g_api.Load(detail, detailCap)) return false;
    const Il2CppImage* image = GameImage();
    if (!image) {
        SetText(detail, detailCap, L"Assembly-CSharp chưa sẵn sàng");
        return false;
    }
    Il2CppClass* gameApi = g_api.class_from_name(image, "FGStudio.LuaSystem.API", "LuaSystemAPI_Game");
    Il2CppClass* shared = g_api.class_from_name(image, "FGStudio.LuaSystem", "LuaSystemSharedData");
    Il2CppClass* network = g_api.class_from_name(image, "FGStudio.LuaSystem.API", "LuaSystemAPI_Network");
    Il2CppClass* gui = g_api.class_from_name(image, "FGStudio.LuaSystem.API", "LuaSystemAPI_GUI");
    if (!gameApi || !shared || !network || !gui) {
        SetText(detail, detailCap, L"IL2CPP PASS nhưng thiếu một class semantic Game/SharedData/Network/GUI");
        return false;
    }
    SetText(detail, detailCap, L"IL2CPP semantic resolver PASS; không chạy LuaEnv.DoString");
    SetText(output, outputCap, L"SEMANTIC_READY|Game|SharedData|Network|GUI");
    return true;
}

bool RunQueryTeam(wchar_t* output, std::size_t outputCap,
                  wchar_t* detail, std::size_t detailCap) {
    if (!output || outputCap == 0) return false;
    output[0] = 0;
    MethodTarget target{};
    if (!ResolveTarget("GetNearByPeacePlayers", 1, target, detail, detailCap)) return false;

    Il2CppObject* collection = nullptr;
    if (!InvokeOneInteger(target.method, target.instance, 32, collection, detail, detailCap) || !collection) {
        SetText(detail, detailCap, L"GetNearByPeacePlayers(32) trả null/exception");
        return false;
    }
    RootHandle root;
    if (!root.Hold(collection)) {
        SetText(detail, detailCap, L"Không root được nearby-player collection");
        return false;
    }

    std::int32_t count = 0;
    if (!CollectionCount(collection, count, detail, detailCap)) return false;
    const std::int32_t capped = count > 32 ? 32 : count;
    std::int32_t written = 0;
    for (std::int32_t i = 0; i < capped; ++i) {
        Il2CppObject* player = nullptr;
        if (!CollectionItem(collection, i, player, detail, detailCap) || !player) continue;
        std::uint64_t roleId = 0;
        if (!ReadNumericMember(player, "RoleID", roleId) || roleId == 0) continue;
        wchar_t name[160]{};
        (void)ReadStringMember(player, "Name", name, _countof(name));
        wchar_t line[320]{};
        std::swprintf(line, _countof(line), L"%llu\t%s\tAOI\n",
                      static_cast<unsigned long long>(roleId), name);
        if (std::wcslen(output) + std::wcslen(line) + 1 >= outputCap) break;
        Append(output, outputCap, line);
        ++written;
    }
    wchar_t msg[256]{};
    std::swprintf(msg, _countof(msg), L"Direct GetNearByPeacePlayers PASS: %d/%d record usable", written, count);
    SetText(detail, detailCap, msg);
    return true;
}

bool GetSelectedTarget(Il2CppObject*& selected, wchar_t* detail, std::size_t cap) {
    selected = nullptr;
    MethodTarget getter{};
    if (ResolveTarget("get_SelectedTarget", 0, getter, detail, cap)) {
        if (InvokeObject(getter.method, getter.instance, nullptr, selected, detail, cap) && selected) return true;
    }

    const Il2CppImage* image = GameImage();
    if (!image) return false;
    struct Candidate { const char* ns; const char* klass; };
    const Candidate candidates[] = {
        {"FGStudio.LuaSystem.API", "LuaSystemAPI_Game"},
        {"FGStudio.LuaSystem", "LuaSystemSharedData"},
    };
    for (const auto& candidate : candidates) {
        Il2CppClass* klass = g_api.class_from_name(image, candidate.ns, candidate.klass);
        if (!klass) continue;
        const char* fields[] = {"SelectedTarget", "selectedTarget", "_selectedTarget", "<SelectedTarget>k__BackingField"};
        for (const char* fieldName : fields) {
            FieldInfo* field = g_api.class_get_field_from_name(klass, fieldName);
            if (!field) continue;
            if (StaticField(field)) {
                g_api.field_static_get_value(field, &selected);
            } else {
                Il2CppObject* instance = nullptr;
                if (!FindLiveInstance(klass, instance)) continue;
                g_api.field_get_value(instance, field, &selected);
            }
            if (selected) return true;
        }
    }
    SetText(detail, cap, L"SelectTarget đã gọi nhưng chưa đọc được SelectedTarget");
    return false;
}

bool DirectSelectTarget(std::uint64_t roleId, Il2CppObject*& selected,
                        wchar_t* detail, std::size_t cap) {
    selected = nullptr;
    MethodTarget target{};
    if (!ResolveTarget("SelectTarget", 1, target, detail, cap)) return false;
    Il2CppObject* ignoredResult = nullptr;
    if (!InvokeOneInteger(target.method, target.instance, roleId, ignoredResult, detail, cap)) return false;
    if (!GetSelectedTarget(selected, detail, cap)) return false;
    std::uint64_t selectedId = 0;
    if (!ReadNumericMember(selected, "RoleID", selectedId)) {
        SetText(detail, cap, L"SelectedTarget không đọc được RoleID");
        return false;
    }
    if (selectedId != roleId) {
        wchar_t msg[192]{};
        std::swprintf(msg, _countof(msg), L"SelectedTarget sai RoleID: got=%llu expected=%llu",
                      static_cast<unsigned long long>(selectedId),
                      static_cast<unsigned long long>(roleId));
        SetText(detail, cap, msg);
        return false;
    }
    return true;
}

bool RunSelectTarget(std::uint64_t roleId, wchar_t* output, std::size_t outputCap,
                     wchar_t* detail, std::size_t detailCap) {
    if (output && outputCap) output[0] = 0;
    Il2CppObject* selected = nullptr;
    if (!DirectSelectTarget(roleId, selected, detail, detailCap)) return false;
    RootHandle root;
    (void)root.Hold(selected);
    wchar_t name[160]{};
    (void)ReadStringMember(selected, "Name", name, _countof(name));
    std::swprintf(output, outputCap, L"OK|TARGET=%llu|NAME=%s",
                  static_cast<unsigned long long>(roleId), name);
    SetText(detail, detailCap, L"Direct Game.SelectTarget(RoleID) + SelectedTarget proof PASS");
    return true;
}

bool FindLuaSystemManager(Il2CppClass*& managerClass, Il2CppObject*& manager,
                          wchar_t* detail, std::size_t cap) {
    managerClass = nullptr;
    manager = nullptr;
    const Il2CppImage* image = GameImage();
    if (!image) return false;
    managerClass = g_api.class_from_name(image, "FGStudio.LuaSystem", "LuaSystemManager");
    if (!managerClass) {
        SetText(detail, cap, L"Không resolve LuaSystemManager");
        return false;
    }
    if (!FindLiveInstance(managerClass, manager)) {
        SetText(detail, cap, L"Không tìm thấy live LuaSystemManager singleton");
        return false;
    }
    return true;
}

bool ResolveLuaEnv(Il2CppObject*& luaEnv, wchar_t* detail, std::size_t cap) {
    luaEnv = nullptr;
    Il2CppClass* managerClass = nullptr;
    Il2CppObject* manager = nullptr;
    if (!FindLuaSystemManager(managerClass, manager, detail, cap)) return false;
    const MethodInfo* getter = FindMethod(managerClass, "get_LuaEnv", 0);
    if (getter && InvokeObjectNoArgs(getter, StaticMethod(getter) ? nullptr : manager,
                                     luaEnv, detail, cap) && luaEnv) return true;
    const char* fields[] = {"LuaEnv", "luaEnv", "_luaEnv", "<LuaEnv>k__BackingField"};
    for (const char* fieldName : fields) {
        FieldInfo* field = g_api.class_get_field_from_name(managerClass, fieldName);
        if (!field || StaticField(field)) continue;
        g_api.field_get_value(manager, field, &luaEnv);
        if (luaEnv) return true;
    }
    SetText(detail, cap, L"LuaSystemManager có nhưng chưa lấy được LuaEnv");
    return false;
}

bool LuaTableIndex(Il2CppObject* table, const char* key, Il2CppObject*& value,
                   wchar_t* detail, std::size_t cap) {
    value = nullptr;
    if (!table || !key) return false;
    Il2CppClass* klass = g_api.object_get_class(table);
    if (!klass) return false;
    const char* oneString[] = {"System.String"};
    const MethodInfo* getItem = FindMethodExact(klass, "get_Item", oneString, 1);
    if (!getItem) {
        SetText(detail, cap, L"XLua.LuaTable thiếu get_Item(String)");
        return false;
    }
    Il2CppString* managedKey = g_api.string_new(key);
    if (!managedKey) return false;
    RootHandle keyRoot;
    if (!keyRoot.Hold(reinterpret_cast<Il2CppObject*>(managedKey))) return false;
    void* args[] = {&managedKey};
    if (!InvokeObject(getItem, table, args, value, detail, cap) || !value) {
        SetText(detail, cap, L"LuaTable key không tồn tại/null");
        return false;
    }
    return true;
}

bool ReadTradeConstants(std::uint64_t& trade, std::uint64_t& request,
                        wchar_t* detail, std::size_t cap) {
    trade = 0;
    request = 0;
    Il2CppObject* luaEnv = nullptr;
    if (!ResolveLuaEnv(luaEnv, detail, cap)) return false;
    RootHandle envRoot;
    (void)envRoot.Hold(luaEnv);
    Il2CppClass* envClass = g_api.object_get_class(luaEnv);
    const MethodInfo* getGlobal = envClass ? FindMethod(envClass, "get_Global", 0) : nullptr;
    if (!getGlobal) {
        SetText(detail, cap, L"LuaEnv thiếu get_Global");
        return false;
    }
    Il2CppObject* global = nullptr;
    if (!InvokeObjectNoArgs(getGlobal, luaEnv, global, detail, cap) || !global) return false;
    RootHandle globalRoot;
    (void)globalRoot.Hold(global);

    Il2CppObject* otherRoleCommands = nullptr;
    Il2CppObject* tradeCommands = nullptr;
    if (!LuaTableIndex(global, "C_OtherRoleCommand", otherRoleCommands, detail, cap) ||
        !LuaTableIndex(global, "C_TradeCommand", tradeCommands, detail, cap)) return false;
    RootHandle otherRoot;
    RootHandle tradeRoot;
    (void)otherRoot.Hold(otherRoleCommands);
    (void)tradeRoot.Hold(tradeCommands);

    Il2CppObject* tradeValue = nullptr;
    Il2CppObject* requestValue = nullptr;
    if (!LuaTableIndex(otherRoleCommands, "Trade", tradeValue, detail, cap) ||
        !LuaTableIndex(tradeCommands, "Request", requestValue, detail, cap)) return false;
    if (!BoxedNumberToU64(tradeValue, trade) || !BoxedNumberToU64(requestValue, request)) {
        SetText(detail, cap, L"Trade/Request runtime constants không phải số nguyên hỗ trợ");
        return false;
    }
    if (trade != 7 || request > 1000) {
        SetText(detail, cap, L"Runtime Trade/Request constants không qua sanity guard");
        return false;
    }
    return true;
}

bool SendOtherRoleTradePacket(std::uint64_t roleId, std::uint64_t trade,
                              std::uint64_t request, wchar_t* detail, std::size_t cap) {
    const Il2CppImage* image = GameImage();
    if (!image) return false;
    Il2CppClass* network = g_api.class_from_name(image, "FGStudio.LuaSystem.API", "LuaSystemAPI_Network");
    if (!network) {
        SetText(detail, cap, L"Không resolve LuaSystemAPI_Network");
        return false;
    }
    const char* signature[] = {"System.Int32", "System.String"};
    const MethodInfo* send = FindMethodExact(network, "SendPacket", signature, 2);
    if (!send || !StaticMethod(send)) {
        SetText(detail, cap, L"Không resolve static Network.SendPacket(Int32,String)");
        return false;
    }
    wchar_t payloadWide[192]{};
    std::swprintf(payloadWide, _countof(payloadWide), L"%llu:%llu:%llu",
                  static_cast<unsigned long long>(trade),
                  static_cast<unsigned long long>(request),
                  static_cast<unsigned long long>(roleId));
    char payload[192]{};
    std::size_t i = 0;
    while (payloadWide[i] && i + 1 < sizeof(payload)) {
        const wchar_t ch = payloadWide[i];
        if (ch > 0x7f) return false;
        payload[i] = static_cast<char>(ch);
        ++i;
    }
    payload[i] = 0;
    Il2CppString* data = g_api.string_new(payload);
    if (!data) return false;
    RootHandle dataRoot;
    if (!dataRoot.Hold(reinterpret_cast<Il2CppObject*>(data))) return false;
    std::int32_t packet = 200051;
    void* args[] = {&packet, &data};
    Il2CppObject* ignored = nullptr;
    if (!InvokeObject(send, nullptr, args, ignored, detail, cap)) return false;
    return true;
}

bool RunSelectAndTrade(std::uint64_t roleId, wchar_t* output, std::size_t outputCap,
                       wchar_t* detail, std::size_t detailCap) {
    if (output && outputCap) output[0] = 0;
    Il2CppObject* selected = nullptr;
    if (!DirectSelectTarget(roleId, selected, detail, detailCap)) return false;
    RootHandle selectedRoot;
    (void)selectedRoot.Hold(selected);

    std::uint64_t trade = 0;
    std::uint64_t request = 0;
    if (!ReadTradeConstants(trade, request, detail, detailCap)) return false;
    if (!SendOtherRoleTradePacket(roleId, trade, request, detail, detailCap)) return false;

    std::swprintf(output, outputCap, L"OK|TARGET=%llu|PACKET=200051|PAYLOAD=%llu:%llu:%llu",
                  static_cast<unsigned long long>(roleId),
                  static_cast<unsigned long long>(trade),
                  static_cast<unsigned long long>(request),
                  static_cast<unsigned long long>(roleId));
    SetText(detail, detailCap, L"Target proof PASS + runtime Trade/Request constants + direct SendPacket PASS");
    return true;
}

bool FindUiByName(const char* uiName, Il2CppObject*& ui,
                  wchar_t* detail, std::size_t cap) {
    ui = nullptr;
    const Il2CppImage* image = GameImage();
    if (!image) return false;
    Il2CppClass* gui = g_api.class_from_name(image, "FGStudio.LuaSystem.API", "LuaSystemAPI_GUI");
    if (!gui) return false;
    const char* signature[] = {"System.String"};
    const MethodInfo* find = FindMethodExact(gui, "FindUI", signature, 1);
    if (!find) find = FindMethodExact(gui, "MainFindUI", signature, 1);
    if (!find || !StaticMethod(find)) {
        SetText(detail, cap, L"Không resolve GUI.FindUI/MainFindUI(String)");
        return false;
    }
    Il2CppString* name = g_api.string_new(uiName);
    if (!name) return false;
    RootHandle nameRoot;
    if (!nameRoot.Hold(reinterpret_cast<Il2CppObject*>(name))) return false;
    void* args[] = {&name};
    return InvokeObject(find, nullptr, args, ui, detail, cap);
}

bool RunQueryTradeUi(wchar_t* output, std::size_t outputCap,
                     wchar_t* detail, std::size_t detailCap) {
    if (!output || outputCap == 0) return false;
    output[0] = 0;
    const char* names[] = {
        "Trade", "Trade_Main", "TradeFrame", "TradeWindow", "TradeUI", "PlayerTrade", "RoleTrade"
    };
    for (const char* name : names) {
        Il2CppObject* ui = nullptr;
        wchar_t ignored[128]{};
        if (!FindUiByName(name, ui, ignored, _countof(ignored))) continue;
        if (!ui) continue;
        wchar_t wideName[64]{};
        std::size_t i = 0;
        while (name[i] && i + 1 < _countof(wideName)) {
            wideName[i] = static_cast<wchar_t>(static_cast<unsigned char>(name[i]));
            ++i;
        }
        wideName[i] = 0;
        std::swprintf(output, outputCap, L"OPEN|%s", wideName);
        SetText(detail, detailCap, L"GUI.FindUI phát hiện candidate Trade UI đang mở");
        return true;
    }
    SetText(output, outputCap, L"CLOSED");
    SetText(detail, detailCap, L"Không phát hiện Trade UI trong candidate semantic names");
    return true;
}

} // namespace
