#include "module.h"
#include "LuaInterface.h"
#include "lua.h"
#include "detours.h"
// #include "Bootil/File/Changes.h"

#include "tier0/memdbgon.h"

class CAutoRefreshModule : public IModule
{
public:
	virtual void Init(CreateInterfaceFn *appfn, CreateInterfaceFn *gamefn) OVERRIDE;
	virtual void LuaInit(GarrysMod::Lua::ILuaInterface *pLua, bool bServerInit) OVERRIDE;
	virtual void LuaShutdown(GarrysMod::Lua::ILuaInterface *pLua) OVERRIDE;
	virtual void InitDetour(bool bPreServer) OVERRIDE;
	virtual void Think(bool bSimulating) OVERRIDE;
	virtual void Shutdown() OVERRIDE;
	virtual const char *Name() { return "autorefresh"; };
	virtual int Compatibility() { return LINUX32; };
};

CAutoRefreshModule g_pAutoRefreshModule;
IModule *pAutoRefreshModule = &g_pAutoRefreshModule;

void CAutoRefreshModule::Init(CreateInterfaceFn *appfn, CreateInterfaceFn *gamefn)
{
}

static Detouring::Hook detour_CAutoRefresh_HandleChange_Lua;
static void hook_CAutoRefresh_HandleChange_Lua(const std::string *pfileRelPath, const std::string *pfileName, const std::string *pfileExt)
{
	if (!g_Lua)
	{
		return;
	}

	bool bDenyRefresh = false;
	if (Lua::PushHook("HolyLib:GetBeforeRefresh"))
	{
		g_Lua->PushString(pfileRelPath->c_str());
		g_Lua->PushString(pfileName->c_str());

		if (g_Lua->CallFunctionProtected(3, 1, true))
		{
			bDenyRefresh = g_Lua->GetBool(-1);
			g_Lua->Pop(1);
		}
	}

	if (bDenyRefresh) {
		return;
	}

	detour_CAutoRefresh_HandleChange_Lua.GetTrampoline<Symbols::GarrysMod_AutoRefresh_HandleChange_Lua>()(pfileRelPath, pfileName, pfileExt);

	if (Lua::PushHook("HolyLib:GetAfterRefresh"))
	{
		g_Lua->PushString(pfileRelPath->c_str());
		g_Lua->PushString(pfileName->c_str());

		g_Lua->CallFunctionProtected(3, 0, true);
	}

	return;
};

void CAutoRefreshModule::LuaInit(GarrysMod::Lua::ILuaInterface *pLua, bool bServerInit)
{
	if (bServerInit)
		return;

	Util::StartTable(pLua);
	Util::FinishTable(pLua, "autorefresh");
}

void CAutoRefreshModule::LuaShutdown(GarrysMod::Lua::ILuaInterface *pLua)
{
	Util::NukeTable(pLua, "autorefresh");
}

void CAutoRefreshModule::InitDetour(bool bPreServer)
{
	if (bPreServer)
		return;

	SourceSDK::FactoryLoader server_loader("server");

	// HandleChange_Lua
	Detour::Create(
		&detour_CAutoRefresh_HandleChange_Lua, "CAutoRefresh_HandleLuaFileChange",
		server_loader.GetModule(), Symbols::GarrysMod_AutoRefresh_HandleChange_LuaSym,
		(void *)hook_CAutoRefresh_HandleChange_Lua, m_pID
	);
}

void CAutoRefreshModule::Think(bool simulating)
{ 
}

void CAutoRefreshModule::Shutdown()
{
}