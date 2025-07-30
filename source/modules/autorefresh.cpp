#include "module.h"
#include "LuaInterface.h"
#include "lua.h"
#include "detours.h"

#include <unordered_map>

#include "tier0/memdbgon.h"

class CAutoRefreshModule : public IModule
{
public:
	virtual void LuaInit(GarrysMod::Lua::ILuaInterface* pLua, bool bServerInit) OVERRIDE;
	virtual void LuaShutdown(GarrysMod::Lua::ILuaInterface* pLua) OVERRIDE;
	virtual void InitDetour(bool bPreServer) OVERRIDE;
	virtual const char* Name() { return "autorefresh"; };
	virtual int Compatibility() { return LINUX32 | LINUX64; };
};

CAutoRefreshModule g_pAutoRefreshModule;
IModule* pAutoRefreshModule = &g_pAutoRefreshModule;

static std::unordered_map<std::string, bool> blockedLuaFilesMap = {};
LUA_FUNCTION_STATIC(DenyLuaAutoRefresh)
{
	LUA->CheckType(2, GarrysMod::Lua::Type::Bool);
	bool blockStatus = LUA->GetBool(2);

	int arg1Type = LUA->GetType(1);
	if (arg1Type == GarrysMod::Lua::Type::String)
	{
		std::string filePath = LUA->GetString(1);
		blockedLuaFilesMap.insert_or_assign(filePath, blockStatus);
	} 
	else if (arg1Type == GarrysMod::Lua::Type::Table)
	{
		LUA->PushNil();
		while (LUA->Next(1))
		{
			LUA->CheckType(-1, GarrysMod::Lua::Type::String);
			std::string filePath = LUA->GetString(-1);
			blockedLuaFilesMap.insert_or_assign(filePath, blockStatus);
			LUA->Pop();
		}

		LUA->Pop();
	}
	else
	{
		LUA->ArgError(1, "expected string or table of strings");
	}

	return 0;
}

static Detouring::Hook detour_CAutoRefresh_HandleChange_Lua;
static bool hook_CAutoRefresh_HandleChange_Lua(const std::string* pfileRelPath, const std::string* pfileName, const std::string* pfileExt)
{
	using OriginalFunction = bool(*)(const std::string*, const std::string*, const std::string*);
	auto trampoline = detour_CAutoRefresh_HandleChange_Lua.GetTrampoline<OriginalFunction>();
	if (!g_Lua || !pfileRelPath || !pfileName || !pfileExt)
	{
		return trampoline(pfileRelPath, pfileName, pfileExt);
	}

	bool bDenyRefresh = false; 
	if (!(blockedLuaFilesMap.empty()))
	{
		if (auto fileSearch = blockedLuaFilesMap.find(pfileName->c_str()); fileSearch != blockedLuaFilesMap.end()) {
			bDenyRefresh = fileSearch->second;
			Msg("bDenyRefresh: %d\n", bDenyRefresh);
		}
	}

	if (!bDenyRefresh && Lua::PushHook("HolyLib:PreLuaAutoRefresh"))
	{
		g_Lua->PushString(pfileRelPath->c_str());
		g_Lua->PushString(pfileName->c_str());

		if (g_Lua->CallFunctionProtected(3, 1, true))
		{
			bDenyRefresh = g_Lua->GetBool(-1);
			g_Lua->Pop(1);
		}
	}

	if (bDenyRefresh)
	{
		return true;
	}

	bool originalResult = trampoline(pfileRelPath, pfileName, pfileExt);

	if (Lua::PushHook("HolyLib:PostLuaAutoRefresh"))
	{
		g_Lua->PushString(pfileRelPath->c_str());
		g_Lua->PushString(pfileName->c_str());

		g_Lua->CallFunctionProtected(3, 0, true);
	}

	return originalResult;
};

void CAutoRefreshModule::LuaInit(GarrysMod::Lua::ILuaInterface* pLua, bool bServerInit)
{
	if (bServerInit)
		return;

	Util::StartTable(pLua);
		Util::AddFunc(pLua, DenyLuaAutoRefresh, "DenyLuaAutoRefresh");
	Util::FinishTable(pLua, "autorefresh");
}

void CAutoRefreshModule::LuaShutdown(GarrysMod::Lua::ILuaInterface* pLua)
{
	Util::NukeTable(pLua, "autorefresh");
}

void CAutoRefreshModule::InitDetour(bool bPreServer)
{
	if (bPreServer)
		return;

	SourceSDK::FactoryLoader server_loader("server");
	Detour::Create(
		&detour_CAutoRefresh_HandleChange_Lua, "CAutoRefresh_HandleChange_Lua",
		server_loader.GetModule(), Symbols::GarrysMod_AutoRefresh_HandleChange_LuaSym,
		(void*)hook_CAutoRefresh_HandleChange_Lua, m_pID
	);
}
