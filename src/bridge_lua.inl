#include <cstdint>
#include <cstdio>

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

bool ReadStaticObjectField(Il2CppClass* klass, const char* fieldName, Il2CppObject*& out) {
    out = nullptr;
    if (!klass || !fieldName) return false;
    for (Il2CppClass* c = klass; c; c = g_api.class_get_parent(c)) {
        FieldInfo* field = g_api.class_get_field_from_name(c, fieldName);
        if (!field || !StaticField(field)) continue;
        g_api.field_static_get_value(field, &out);
        return out != nullptr;
    }
    return false;
}

bool FindLuaSystemManager(Il2CppClass*& managerClass, Il2CppObject*& manager,
                          wchar_t* detail, std::size_t cap) {
    managerClass = nullptr;
    manager = nullptr;
    const Il2CppImage* image = GameImage();
    if (!image) {
        SetText(detail, cap, L"Không mở được Assembly-CSharp");
        return false;
    }
    managerClass = g_api.class_from_name(image, "FGStudio.LuaSystem", "LuaSystemManager");
    if (!managerClass) {
        SetText(detail, cap, L"Không resolve được FGStudio.LuaSystem.LuaSystemManager");
        return false;
    }

    const MethodInfo* getInstance = FindMethod(managerClass, "get_Instance", 0);
    if (getInstance && StaticMethod(getInstance)) {
        wchar_t ignored[128]{};
        (void)InvokeObjectNoArgs(getInstance, nullptr, manager, ignored, _countof(ignored));
    }

    if (!manager) {
        const char* candidates[] = {
            "Instance", "instance", "_instance", "s_Instance", "<Instance>k__BackingField"
        };
        for (const char* name : candidates) {
            if (ReadStaticObjectField(managerClass, name, manager)) break;
        }
    }

    if (!manager) {
        for (Il2CppClass* c = managerClass; c && !manager; c = g_api.class_get_parent(c)) {
            void* iter = nullptr;
            while (FieldInfo* field = g_api.class_get_fields(c, &iter)) {
                if (!StaticField(field)) continue;
                const Il2CppType* type = g_api.field_get_type(field);
                char* typeName = type ? g_api.type_get_name(type) : nullptr;
                const bool match = typeName && Contains(typeName, "LuaSystemManager");
                if (typeName) g_api.free_fn(typeName);
                if (!match) continue;
                g_api.field_static_get_value(field, &manager);
                if (manager) break;
            }
        }
    }

    if (!manager) {
        SetText(detail, cap, L"Không tìm thấy live LuaSystemManager singleton");
        return false;
    }
    return true;
}

bool FindLuaEnvField(Il2CppClass* managerClass, Il2CppObject* manager, Il2CppObject*& luaEnv) {
    luaEnv = nullptr;
    if (!managerClass || !manager) return false;
    const char* names[] = {"LuaEnv", "luaEnv", "_luaEnv", "<LuaEnv>k__BackingField"};
    for (const char* name : names) {
        for (Il2CppClass* c = managerClass; c; c = g_api.class_get_parent(c)) {
            FieldInfo* field = g_api.class_get_field_from_name(c, name);
            if (!field || StaticField(field)) continue;
            g_api.field_get_value(manager, field, &luaEnv);
            if (luaEnv) return true;
        }
    }
    for (Il2CppClass* c = managerClass; c; c = g_api.class_get_parent(c)) {
        void* iter = nullptr;
        while (FieldInfo* field = g_api.class_get_fields(c, &iter)) {
            if (StaticField(field)) continue;
            const Il2CppType* type = g_api.field_get_type(field);
            char* typeName = type ? g_api.type_get_name(type) : nullptr;
            const bool match = typeName && Contains(typeName, "LuaEnv");
            if (typeName) g_api.free_fn(typeName);
            if (!match) continue;
            g_api.field_get_value(manager, field, &luaEnv);
            if (luaEnv) return true;
        }
    }
    return false;
}

bool ResolveLuaEnv(Il2CppObject*& luaEnv, wchar_t* detail, std::size_t cap) {
    luaEnv = nullptr;
    if (!g_api.Load(detail, cap)) return false;
    Il2CppClass* managerClass = nullptr;
    Il2CppObject* manager = nullptr;
    if (!FindLuaSystemManager(managerClass, manager, detail, cap)) return false;

    const MethodInfo* getLuaEnv = FindMethod(managerClass, "get_LuaEnv", 0);
    if (getLuaEnv) {
        wchar_t ignored[128]{};
        if (InvokeObjectNoArgs(getLuaEnv, StaticMethod(getLuaEnv) ? nullptr : manager,
                               luaEnv, ignored, _countof(ignored)) && luaEnv) {
            return true;
        }
    }

    if (FindLuaEnvField(managerClass, manager, luaEnv) && luaEnv) return true;
    SetText(detail, cap, L"LuaSystemManager tìm thấy nhưng chưa resolve được live LuaEnv");
    return false;
}

const MethodInfo* FindDoString(Il2CppClass* luaEnvClass) {
    if (!luaEnvClass) return nullptr;
    for (std::uint32_t preferred = 3; preferred >= 1; --preferred) {
        for (Il2CppClass* c = luaEnvClass; c; c = g_api.class_get_parent(c)) {
            void* iter = nullptr;
            while (const MethodInfo* method = g_api.class_get_methods(c, &iter)) {
                if (!Eq(g_api.method_get_name(method), "DoString")) continue;
                if (g_api.method_get_param_count(method) != preferred) continue;
                if (!ParamType(method, 0, "System.String")) continue;
                return method;
            }
        }
        if (preferred == 1) break;
    }
    return nullptr;
}

bool CopyManagedString(Il2CppObject* object, wchar_t* out, std::size_t cap,
                       wchar_t* detail, std::size_t detailCap) {
    if (!out || cap == 0) return false;
    out[0] = 0;
    if (!object) {
        SetText(detail, detailCap, L"Lua trả về nil thay vì String");
        return false;
    }
    Il2CppClass* klass = g_api.object_get_class(object);
    const char* name = klass ? g_api.class_get_name(klass) : nullptr;
    if (!name || !Eq(name, "String")) {
        SetText(detail, detailCap, L"Lua result đầu tiên không phải System.String");
        return false;
    }
    auto* str = reinterpret_cast<Il2CppString*>(object);
    const std::int32_t len = g_api.string_length(str);
    const std::uint16_t* chars = g_api.string_chars(str);
    if (len < 0 || !chars) {
        SetText(detail, detailCap, L"Không đọc được chars của Lua String result");
        return false;
    }
    const std::size_t count = static_cast<std::size_t>(len);
    const std::size_t copyCount = count < cap - 1 ? count : cap - 1;
    for (std::size_t i = 0; i < copyCount; ++i) out[i] = static_cast<wchar_t>(chars[i]);
    out[copyCount] = 0;
    return true;
}

bool ExecuteLuaString(const char* code, wchar_t* result, std::size_t resultCap,
                      wchar_t* detail, std::size_t detailCap) {
    if (!code || !result || resultCap == 0) return false;
    result[0] = 0;

    Il2CppObject* luaEnv = nullptr;
    if (!ResolveLuaEnv(luaEnv, detail, detailCap)) return false;
    Il2CppClass* luaEnvClass = g_api.object_get_class(luaEnv);
    const MethodInfo* doString = FindDoString(luaEnvClass);
    if (!doString || StaticMethod(doString)) {
        SetText(detail, detailCap, L"Không resolve được XLua.LuaEnv.DoString instance overload");
        return false;
    }

    Il2CppString* codeString = g_api.string_new(code);
    Il2CppString* chunkName = g_api.string_new("ThanLongAutoTradeTest");
    if (!codeString || !chunkName) {
        SetText(detail, detailCap, L"Không tạo được Lua code/chunk string");
        return false;
    }
    RootHandle codeRoot;
    RootHandle chunkRoot;
    if (!codeRoot.Hold(reinterpret_cast<Il2CppObject*>(codeString)) ||
        !chunkRoot.Hold(reinterpret_cast<Il2CppObject*>(chunkName))) {
        SetText(detail, detailCap, L"Không root được Lua code/chunk string");
        return false;
    }

    Il2CppObject* nullObject = nullptr;
    void* args[3]{};
    const std::uint32_t argc = g_api.method_get_param_count(doString);
    for (std::uint32_t i = 0; i < argc; ++i) {
        const Il2CppType* type = g_api.method_get_param(doString, i);
        char* typeName = type ? g_api.type_get_name(type) : nullptr;
        if (i == 0) {
            args[i] = &codeString;
        } else if (typeName && Eq(typeName, "System.String")) {
            args[i] = &chunkName;
        } else {
            args[i] = &nullObject;
        }
        if (typeName) g_api.free_fn(typeName);
    }

    void* exception = nullptr;
    Il2CppObject* rawResult = g_api.runtime_invoke(doString, luaEnv, args, &exception);
    if (exception || !rawResult) {
        SetText(detail, detailCap, L"LuaEnv.DoString ném exception/null result");
        return false;
    }
    RootHandle resultRoot;
    if (!resultRoot.Hold(rawResult)) {
        SetText(detail, detailCap, L"Không root được Lua DoString result");
        return false;
    }

    // Frozen IL2CPP x64 array layout: bounds @ +0x10, max_length @ +0x18, data @ +0x20.
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(rawResult);
    const std::uintptr_t length = *reinterpret_cast<const std::uintptr_t*>(bytes + 0x18u);
    if (length == 0 || length > 64) {
        SetText(detail, detailCap, L"Lua DoString result Object[] length không hợp lệ/rỗng");
        return false;
    }
    auto* const* items = reinterpret_cast<Il2CppObject* const*>(bytes + 0x20u);
    if (!CopyManagedString(items[0], result, resultCap, detail, detailCap)) return false;
    return true;
}

bool RunLuaProbe(wchar_t* output, std::size_t outputCap,
                 wchar_t* detail, std::size_t detailCap) {
    const char* script =
        "return (function() "
        "local a=(C_OtherRoleCommand and C_OtherRoleCommand.Trade) or 'nil'; "
        "local b=(C_TradeCommand and C_TradeCommand.Request) or 'nil'; "
        "return 'LUA_OK|Trade='..tostring(a)..'|Request='..tostring(b) "
        "end)()";
    return ExecuteLuaString(script, output, outputCap, detail, detailCap);
}

bool RunQueryTeam(wchar_t* output, std::size_t outputCap,
                  wchar_t* detail, std::size_t detailCap) {
    const char* script =
        "return (function() "
        "local out={}; local selfId=nil; "
        "if Game and Game.RoleData then selfId=Game.RoleData.RoleID end; "
        "if C_TeamData and C_TeamData.TeamMember then "
        "for _,m in pairs(C_TeamData.TeamMember) do "
        "if m and m.RoleID and (not selfId or tostring(m.RoleID)~=tostring(selfId)) then "
        "out[#out+1]=tostring(m.RoleID)..'\\t'..tostring(m.RoleName or m.Name or '')..'\\t'..tostring(m.MapID or '') "
        "end end end; return table.concat(out,'\\n') "
        "end)()";
    return ExecuteLuaString(script, output, outputCap, detail, detailCap);
}

bool RunSelectTarget(std::uint64_t roleId, wchar_t* output, std::size_t outputCap,
                     wchar_t* detail, std::size_t detailCap) {
    char script[2048]{};
    std::snprintf(script, sizeof(script),
        "return (function() local rid=%llu; "
        "local ok,res=pcall(function() "
        "if not Game then return 'ERR|Game=nil' end; "
        "Game.SelectTarget(rid); local t=Game.SelectedTarget; "
        "if not t then return 'ERR|SelectedTarget=nil' end; "
        "if tostring(t.RoleID)~=tostring(rid) then return 'ERR|SelectedRoleID='..tostring(t.RoleID) end; "
        "return 'OK|TARGET='..tostring(t.RoleID)..'|NAME='..tostring(t.Name or '') end); "
        "if not ok then return 'ERR|LUA|'..tostring(res) end; return res end)()",
        static_cast<unsigned long long>(roleId));
    return ExecuteLuaString(script, output, outputCap, detail, detailCap);
}

bool RunSelectAndTrade(std::uint64_t roleId, wchar_t* output, std::size_t outputCap,
                       wchar_t* detail, std::size_t detailCap) {
    char script[3072]{};
    std::snprintf(script, sizeof(script),
        "return (function() local rid=%llu; "
        "local ok,res=pcall(function() "
        "if not Game then return 'ERR|Game=nil' end; "
        "if not Network then return 'ERR|Network=nil' end; "
        "if not C_OtherRoleCommand or C_OtherRoleCommand.Trade==nil then return 'ERR|TradeConstant=nil' end; "
        "if not C_TradeCommand or C_TradeCommand.Request==nil then return 'ERR|TradeRequestConstant=nil' end; "
        "Game.SelectTarget(rid); local t=Game.SelectedTarget; "
        "if not t then return 'ERR|SelectedTarget=nil' end; "
        "if tostring(t.RoleID)~=tostring(rid) then return 'ERR|SelectedRoleID='..tostring(t.RoleID) end; "
        "local payload=tostring(C_OtherRoleCommand.Trade)..':'..tostring(C_TradeCommand.Request)..':'..tostring(rid); "
        "Network.SendPacket(200051,payload); "
        "return 'OK|TARGET='..tostring(t.RoleID)..'|PACKET=200051|PAYLOAD='..payload end); "
        "if not ok then return 'ERR|LUA|'..tostring(res) end; return res end)()",
        static_cast<unsigned long long>(roleId));
    return ExecuteLuaString(script, output, outputCap, detail, detailCap);
}

bool RunQueryTradeUi(wchar_t* output, std::size_t outputCap,
                     wchar_t* detail, std::size_t detailCap) {
    const char* script =
        "return (function() "
        "if not GUI then return 'ERR|GUI=nil' end; "
        "local names={'Trade','Trade_Main','TradeFrame','TradeWindow','TradeUI','PlayerTrade','RoleTrade'}; "
        "for _,n in ipairs(names) do local u=GUI.FindUI(n); if u~=nil then return 'OPEN|'..n end end; "
        "return 'CLOSED' end)()";
    return ExecuteLuaString(script, output, outputCap, detail, detailCap);
}

} // namespace
