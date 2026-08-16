#include <windows.h>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include "protocol.h"

using namespace autotradetest;

namespace {

using Il2CppDomain = void;
using Il2CppAssembly = void;
using Il2CppImage = void;
using Il2CppClass = void;
using MethodInfo = void;
using FieldInfo = void;
using Il2CppType = void;
using Il2CppObject = void;
using Il2CppString = void;
using Il2CppArray = void;

HANDLE g_mapping = nullptr;
SharedBlock* g_shared = nullptr;

template <typename T>
bool Resolve(HMODULE module, const char* name, T& out) {
    out = nullptr;
    FARPROC raw = GetProcAddress(module, name);
    if (!raw) return false;
    static_assert(sizeof(raw) == sizeof(out), "pointer-size mismatch");
    const auto* src = reinterpret_cast<const unsigned char*>(&raw);
    auto* dst = reinterpret_cast<unsigned char*>(&out);
    for (std::size_t i = 0; i < sizeof(out); ++i) dst[i] = src[i];
    return out != nullptr;
}

bool Eq(const char* a, const char* b) {
    if (!a || !b) return false;
    while (*a && *b) {
        if (*a++ != *b++) return false;
    }
    return *a == *b;
}

bool Contains(const char* text, const char* needle) {
    if (!text || !needle || !*needle) return false;
    for (const char* p = text; *p; ++p) {
        const char* a = p;
        const char* b = needle;
        while (*a && *b && *a == *b) { ++a; ++b; }
        if (!*b) return true;
    }
    return false;
}

void SetText(wchar_t* out, std::size_t cap, const wchar_t* text) {
    if (!out || cap == 0) return;
    std::size_t i = 0;
    if (text) {
        while (i + 1 < cap && text[i]) {
            out[i] = text[i];
            ++i;
        }
    }
    out[i] = 0;
}

void Append(wchar_t* out, std::size_t cap, const wchar_t* text) {
    if (!out || !text || cap == 0) return;
    std::size_t n = 0;
    while (n + 1 < cap && out[n]) ++n;
    std::size_t i = 0;
    while (n + 1 < cap && text[i]) out[n++] = text[i++];
    out[n] = 0;
}

struct Api {
    HMODULE module = nullptr;
    Il2CppDomain* (__cdecl* domain_get)() = nullptr;
    const Il2CppAssembly* (__cdecl* domain_assembly_open)(Il2CppDomain*, const char*) = nullptr;
    const Il2CppImage* (__cdecl* assembly_get_image)(const Il2CppAssembly*) = nullptr;
    const Il2CppImage* (__cdecl* get_corlib)() = nullptr;
    Il2CppClass* (__cdecl* class_from_name)(const Il2CppImage*, const char*, const char*) = nullptr;
    Il2CppClass* (__cdecl* class_get_parent)(Il2CppClass*) = nullptr;
    const MethodInfo* (__cdecl* class_get_method_from_name)(Il2CppClass*, const char*, int) = nullptr;
    const MethodInfo* (__cdecl* class_get_methods)(Il2CppClass*, void**) = nullptr;
    FieldInfo* (__cdecl* class_get_fields)(Il2CppClass*, void**) = nullptr;
    FieldInfo* (__cdecl* class_get_field_from_name)(Il2CppClass*, const char*) = nullptr;
    const char* (__cdecl* class_get_name)(Il2CppClass*) = nullptr;
    const char* (__cdecl* method_get_name)(const MethodInfo*) = nullptr;
    std::uint32_t (__cdecl* method_get_flags)(const MethodInfo*, std::uint32_t*) = nullptr;
    std::uint32_t (__cdecl* method_get_param_count)(const MethodInfo*) = nullptr;
    const Il2CppType* (__cdecl* method_get_param)(const MethodInfo*, std::uint32_t) = nullptr;
    const Il2CppType* (__cdecl* method_get_return_type)(const MethodInfo*) = nullptr;
    const char* (__cdecl* field_get_name)(FieldInfo*) = nullptr;
    const Il2CppType* (__cdecl* field_get_type)(FieldInfo*) = nullptr;
    std::uint32_t (__cdecl* field_get_flags)(FieldInfo*) = nullptr;
    void (__cdecl* field_static_get_value)(FieldInfo*, void*) = nullptr;
    void (__cdecl* field_get_value)(Il2CppObject*, FieldInfo*, void*) = nullptr;
    char* (__cdecl* type_get_name)(const Il2CppType*) = nullptr;
    void (__cdecl* free_fn)(void*) = nullptr;
    Il2CppObject* (__cdecl* runtime_invoke)(const MethodInfo*, void*, void**, void**) = nullptr;
    void* (__cdecl* object_unbox)(Il2CppObject*) = nullptr;
    Il2CppClass* (__cdecl* object_get_class)(Il2CppObject*) = nullptr;
    Il2CppString* (__cdecl* string_new)(const char*) = nullptr;
    const std::uint16_t* (__cdecl* string_chars)(Il2CppString*) = nullptr;
    std::int32_t (__cdecl* string_length)(Il2CppString*) = nullptr;
    std::uint32_t (__cdecl* gchandle_new)(Il2CppObject*, bool) = nullptr;
    void (__cdecl* gchandle_free)(std::uint32_t) = nullptr;

    bool Load(wchar_t* detail, std::size_t cap) {
        if (module) return true;
        module = GetModuleHandleW(L"GameAssembly.dll");
        if (!module) {
            SetText(detail, cap, L"GameAssembly.dll chưa sẵn sàng");
            return false;
        }
#define NEED(symbol) do { \
    if (!Resolve(module, "il2cpp_" #symbol, symbol)) { \
        SetText(detail, cap, L"Thiếu IL2CPP export bắt buộc cho Auto Trade test"); \
        return false; \
    } \
} while (0)
        NEED(domain_get);
        NEED(domain_assembly_open);
        NEED(assembly_get_image);
        NEED(get_corlib);
        NEED(class_from_name);
        NEED(class_get_parent);
        NEED(class_get_method_from_name);
        NEED(class_get_methods);
        NEED(class_get_fields);
        NEED(class_get_field_from_name);
        NEED(class_get_name);
        NEED(method_get_name);
        NEED(method_get_flags);
        NEED(method_get_param_count);
        NEED(method_get_param);
        NEED(method_get_return_type);
        NEED(field_get_name);
        NEED(field_get_type);
        NEED(field_get_flags);
        NEED(field_static_get_value);
        NEED(field_get_value);
        NEED(type_get_name);
        NEED(runtime_invoke);
        NEED(object_unbox);
        NEED(object_get_class);
        NEED(string_new);
        NEED(string_chars);
        NEED(string_length);
        NEED(gchandle_new);
        NEED(gchandle_free);
#undef NEED
        if (!Resolve(module, "il2cpp_free", free_fn)) {
            SetText(detail, cap, L"Thiếu il2cpp_free");
            return false;
        }
        return true;
    }
};

Api g_api;

const Il2CppImage* AssemblyImage(const char* name, const char* fallback = nullptr) {
    Il2CppDomain* domain = g_api.domain_get ? g_api.domain_get() : nullptr;
    if (!domain || !name) return nullptr;
    const Il2CppAssembly* assembly = g_api.domain_assembly_open(domain, name);
    if (!assembly && fallback) assembly = g_api.domain_assembly_open(domain, fallback);
    return assembly ? g_api.assembly_get_image(assembly) : nullptr;
}

const Il2CppImage* GameImage() {
    return AssemblyImage("Assembly-CSharp", "Assembly-CSharp.dll");
}

bool StaticMethod(const MethodInfo* method) {
    if (!method) return false;
    constexpr std::uint32_t StaticFlag = 0x0010;
    std::uint32_t iflags = 0;
    return (g_api.method_get_flags(method, &iflags) & StaticFlag) != 0;
}

bool StaticField(FieldInfo* field) {
    if (!field) return false;
    constexpr std::uint32_t StaticFlag = 0x0010;
    return (g_api.field_get_flags(field) & StaticFlag) != 0;
}

const MethodInfo* FindMethod(Il2CppClass* klass, const char* name, int argc) {
    for (Il2CppClass* c = klass; c; c = g_api.class_get_parent(c)) {
        if (const MethodInfo* method = g_api.class_get_method_from_name(c, name, argc)) return method;
    }
    return nullptr;
}

bool ParamType(const MethodInfo* method, std::uint32_t index, const char* expected) {
    if (!method || index >= g_api.method_get_param_count(method)) return false;
    const Il2CppType* type = g_api.method_get_param(method, index);
    char* name = type ? g_api.type_get_name(type) : nullptr;
    if (!name) return false;
    const bool ok = Eq(name, expected);
    g_api.free_fn(name);
    return ok;
}

bool InvokeObjectNoArgs(const MethodInfo* method, void* instance, Il2CppObject*& out,
                        wchar_t* detail, std::size_t cap) {
    out = nullptr;
    if (!method) {
        SetText(detail, cap, L"Object getter chưa resolve");
        return false;
    }
    void* exception = nullptr;
    out = g_api.runtime_invoke(method, instance, nullptr, &exception);
    if (exception || !out) {
        SetText(detail, cap, L"Object getter ném exception/null");
        out = nullptr;
        return false;
    }
    return true;
}

bool InvokeBool(const MethodInfo* method, void* instance, bool& out,
                wchar_t* detail, std::size_t cap) {
    out = false;
    if (!method) {
        SetText(detail, cap, L"Bool getter chưa resolve");
        return false;
    }
    const Il2CppType* returnType = g_api.method_get_return_type(method);
    char* typeName = returnType ? g_api.type_get_name(returnType) : nullptr;
    if (!typeName || !Eq(typeName, "System.Boolean")) {
        if (typeName) g_api.free_fn(typeName);
        SetText(detail, cap, L"Bool getter có return type không đúng");
        return false;
    }
    g_api.free_fn(typeName);
    void* exception = nullptr;
    Il2CppObject* boxed = g_api.runtime_invoke(method, instance, nullptr, &exception);
    if (exception || !boxed) {
        SetText(detail, cap, L"Bool getter ném exception/null");
        return false;
    }
    void* raw = g_api.object_unbox(boxed);
    if (!raw) {
        SetText(detail, cap, L"Không unbox được Boolean");
        return false;
    }
    out = *reinterpret_cast<const std::uint8_t*>(raw) != 0;
    return true;
}

bool InvokeInt32(const MethodInfo* method, void* instance, std::int32_t& out,
                 wchar_t* detail, std::size_t cap) {
    out = 0;
    if (!method) {
        SetText(detail, cap, L"Int32 getter chưa resolve");
        return false;
    }
    const Il2CppType* returnType = g_api.method_get_return_type(method);
    char* typeName = returnType ? g_api.type_get_name(returnType) : nullptr;
    if (!typeName || !Eq(typeName, "System.Int32")) {
        if (typeName) g_api.free_fn(typeName);
        SetText(detail, cap, L"Int32 getter có return type không đúng");
        return false;
    }
    g_api.free_fn(typeName);
    void* exception = nullptr;
    Il2CppObject* boxed = g_api.runtime_invoke(method, instance, nullptr, &exception);
    if (exception || !boxed) {
        SetText(detail, cap, L"Int32 getter ném exception/null");
        return false;
    }
    void* raw = g_api.object_unbox(boxed);
    if (!raw) {
        SetText(detail, cap, L"Không unbox được Int32");
        return false;
    }
    out = *reinterpret_cast<const std::int32_t*>(raw);
    return true;
}

bool ProveUnityMainThread(wchar_t* detail, std::size_t cap) {
    const Il2CppImage* core = g_api.get_corlib();
    if (!core) {
        SetText(detail, cap, L"Không lấy được corlib để proof main thread");
        return false;
    }
    Il2CppClass* sync = g_api.class_from_name(core, "System.Threading", "SynchronizationContext");
    Il2CppClass* threadClass = g_api.class_from_name(core, "System.Threading", "Thread");
    if (!sync || !threadClass) {
        SetText(detail, cap, L"Thiếu SynchronizationContext/Thread cho proof main thread");
        return false;
    }
    const MethodInfo* getCurrentContext = FindMethod(sync, "get_Current", 0);
    const MethodInfo* getCurrentThread = FindMethod(threadClass, "get_CurrentThread", 0);
    if (!getCurrentContext || !getCurrentThread ||
        !StaticMethod(getCurrentContext) || !StaticMethod(getCurrentThread)) {
        SetText(detail, cap, L"Không resolve được current context/thread");
        return false;
    }
    Il2CppObject* context = nullptr;
    Il2CppObject* thread = nullptr;
    if (!InvokeObjectNoArgs(getCurrentContext, nullptr, context, detail, cap) ||
        !InvokeObjectNoArgs(getCurrentThread, nullptr, thread, detail, cap)) return false;

    Il2CppClass* contextClass = g_api.object_get_class(context);
    const char* contextName = contextClass ? g_api.class_get_name(contextClass) : nullptr;
    if (!contextName || !Eq(contextName, "UnitySynchronizationContext")) {
        SetText(detail, cap, L"Callback không nằm trong UnitySynchronizationContext");
        return false;
    }
    const MethodInfo* getMainThreadId = FindMethod(contextClass, "get_MainThreadId", 0);
    const MethodInfo* getManagedThreadId = FindMethod(threadClass, "get_ManagedThreadId", 0);
    std::int32_t mainId = 0;
    std::int32_t currentId = 0;
    if (!InvokeInt32(getMainThreadId, context, mainId, detail, cap) ||
        !InvokeInt32(getManagedThreadId, thread, currentId, detail, cap)) return false;
    if (mainId != currentId) {
        SetText(detail, cap, L"Managed thread ID không phải Unity main thread");
        return false;
    }
    return true;
}

bool SafeForWorldAction(wchar_t* detail, std::size_t cap) {
    const Il2CppImage* image = GameImage();
    if (!image) {
        SetText(detail, cap, L"Không mở được Assembly-CSharp");
        return false;
    }
    Il2CppClass* gameApi = g_api.class_from_name(image, "FGStudio.LuaSystem.API", "LuaSystemAPI_Game");
    Il2CppClass* session = g_api.class_from_name(image, "FGStudio.Game.Logic", "SessionData");
    if (!gameApi || !session) {
        SetText(detail, cap, L"Thiếu Game/Session class cho world guard");
        return false;
    }
    const MethodInfo* mapReady = FindMethod(gameApi, "IsMapReady", 0);
    const MethodInfo* waiting = FindMethod(session, "get_WaitingChangeMap", 0);
    if (!mapReady || !waiting || !StaticMethod(mapReady) || !StaticMethod(waiting)) {
        SetText(detail, cap, L"Không resolve được guard chuyển map");
        return false;
    }
    bool ready = false;
    bool changing = false;
    if (!InvokeBool(mapReady, nullptr, ready, detail, cap)) return false;
    if (!InvokeBool(waiting, nullptr, changing, detail, cap)) return false;
    if (!ready || changing) {
        SetText(detail, cap, L"Action bị chặn: map chưa ready/đang chuyển map");
        return false;
    }
    return true;
}

} // namespace
