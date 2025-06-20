#include "module.h"
#include "LuaInterface.h"
#include "lua.h"
#include "detours.h"
#include <variant>
// #include "Bootil/File/Changes.h"

#include "tier0/memdbgon.h"

class CAutoRefreshModule : public IModule
{
public:
	virtual void Init(CreateInterfaceFn* appfn, CreateInterfaceFn* gamefn) OVERRIDE;
	virtual void LuaInit(GarrysMod::Lua::ILuaInterface* pLua, bool bServerInit) OVERRIDE;
	virtual void LuaShutdown(GarrysMod::Lua::ILuaInterface* pLua) OVERRIDE;
	virtual void InitDetour(bool bPreServer) OVERRIDE;
	virtual void Think(bool bSimulating) OVERRIDE;
	virtual void Shutdown() OVERRIDE;
	virtual const char* Name() { return "autorefresh"; };
	virtual int Compatibility() { return LINUX32; };
};

CAutoRefreshModule g_pAutoRefreshModule;
IModule* pAutoRefreshModule = &g_pAutoRefreshModule;

void CAutoRefreshModule::Init(CreateInterfaceFn* appfn, CreateInterfaceFn* gamefn)
{
}

bool InitHookBeforeRefresh(const std::string *pfileRelPath, const std::string *pfileName)
{
	if (!g_Lua)
	{
		return false;
	}

	bool bDenyRefresh = false;
	if (Lua::PushHook("HolyLib:GetBeforeRefresh"))
	{
		g_Lua->PushString(pfileRelPath->c_str());
		g_Lua->PushString(pfileName->c_str());
		
		if (g_Lua->CallFunctionProtected(3, 1, true)) {
			bDenyRefresh = g_Lua->GetBool(-1);
			g_Lua->Pop(1);
		}
	}

	return bDenyRefresh;
}

void InitLuaHookAfterRefresh()
{

}

/*
static Detouring::Hook detour_CAutoRefresh_HandleChange_Lua;
static void hook_CAutoRefresh_HandleChange_Lua(const std::string *pfileRelPath, const std::string *pfileName, const std::string *pfileExt)
{
	if (!pfileRelPath && !pfileName && !pfileExt) {
		Warning(PROJECT_NAME ": Autorefresh - HandleChange_Lua received invalid args!\n");
		
		return;
	}

	bool bDenyRefresh = InitLuaHookBeforeRefresh(pfileRelPath, pfileName);
	if (bDenyRefresh) {
		return;
	}

	return detour_CAutoRefresh_HandleChange_Lua.GetTrampoline<Symbols::GarrysMod_AutoRefresh_HandleChange_Lua>()(pfileRelPath, pfileName, pfileExt);
};
*/

static Detouring::Hook detour_CAutoRefresh_HandleChange;
static void hook_CAutoRefresh_HandleChange(const std::string *pfileRelPath, const std::string *pfileName, const std::string *pfileExt)
{
	using handleChange = int(__cdecl *)(const std::string *pfileRelPath, const std::string *pfileName, const std::string *pfileExt);
	auto originalFunction = detour_CAutoRefresh_HandleChange.GetTrampoline<handleChange>();
	int result = originalFunction(pfileRelPath, pfileName, pfileExt);

	// just for future me, sanity check because I got mad
	if (!pfileRelPath && !pfileName && !pfileExt) {
		Warning(PROJECT_NAME ": Autorefresh - HandleChange received invalid args!\n");

		return;
	}

	bool bDenyRefresh = InitHookBeforeRefresh(pfileRelPath, pfileName);
	if (bDenyRefresh) {
		return;
	}

	return result;
};

void CAutoRefreshModule::LuaInit(GarrysMod::Lua::ILuaInterface* pLua, bool bServerInit)
{
	if (bServerInit)
		return;

	Util::StartTable(pLua);
	Util::FinishTable(pLua, "Autorefresh");
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

	/*
	// HandleChange_Lua
	Detour::Create(
		&detour_CAutoRefresh_HandleChange_Lua, "CAutoRefresh_HandleChange_Lua",
		server_loader.GetModule(), Symbols::GarrysMod_AutoRefresh_HandleChange_LuaSym,
		(void *)hook_CAutoRefresh_HandleChange_Lua, m_pID
	);
	*/

	// HandleChange
	Detour::Create(
		&detour_CAutoRefresh_HandleChange, "CAutoRefresh_HandleChange_Lua",
		server_loader.GetModule(), Symbols::GarrysMod_AutoRefresh_HandleChangeSym,
		(void *)hook_CAutoRefresh_HandleChange, m_pID
	);
}

void CAutoRefreshModule::Think(bool simulating)
{
}

void CAutoRefreshModule::Shutdown()
{
}