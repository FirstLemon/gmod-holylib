#include "module.h"
#include "LuaInterface.h"
#include "lua.h"
#include "detours.h"
#include "Bootil/File/Changes.h"

#include <unordered_set>

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

/*
static Detouring::Hook detour_CAutoRefresh_HandleLuaFileChange;
static void hook_CAutoRefresh_HandleLuaFileChange(const std::string *fileRelPath, const std::string *fileContent)
{	
	if (!g_Lua) {
		return;
	}

	if (Lua::PushHook("HolyLib:OnLuaFileChange"))
	{	
		g_Lua->PushString(fileRelPath->c_str());
		g_Lua->CallFunctionProtected(2, 0, true);
	}
};
*/

LUA_FUNCTION_STATIC(AutoRefreshBlock)
{
	LUA->CheckType(1, GarrysMod::Lua::Type::String);
	LUA->CheckType(2, GarrysMod::Lua::Type::String);

	std::string relPath, filename = (LUA->GetString(1), (LUA->GetString(2)));
	// unfinished testing something
}

static Detouring::Hook detour_CAutoRefresh_HandleChange_Lua;
static void hook_CAutoRefresh_HandleChange_Lua(const std::string *arg1, const std::string *arg2, const std::string *arg3)
{
	// ->
	if (arg1 && arg2 && arg3) {
		Msg("----\nAutoRefresh Debug Dump\n----\n Arg1: %s\nArg2: %s\nArg3: %s\n----\n", arg1->c_str(), arg2->c_str(), arg3->c_str());
	}
	else {
		Msg("Received null pointer(s): arg1=%p, arg2=%p, arg3=%p\n", arg1, arg2, arg3);
	}

	// the problem has something to do with this fishy mcdouble chili cheese, I do not even know if what I'm trying to do is actually possible or valid
	// I guess it was
	return detour_CAutoRefresh_HandleChange_Lua.GetTrampoline<Symbols::GarrysMod_AutoRefresh_HandleChange_Lua>()(arg1, arg2, arg3);
};


void CAutoRefreshModule::LuaInit(GarrysMod::Lua::ILuaInterface* pLua, bool bServerInit)
{
	if (bServerInit)
		return;

	Util::StartTable(pLua);
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

	// HandleChange_Lua
	Detour::Create(
		&detour_CAutoRefresh_HandleChange_Lua, "CAutoRefresh_HandleLuaFileChange",
		server_loader.GetModule(), Symbols::GarrysMod_AutoRefresh_HandleChange_LuaSym,
		(void *)hook_CAutoRefresh_HandleChange_Lua, m_pID
	);

	/*
	// HandleLuaFileCHange
	Detour::Create(
		&detour_CAutoRefresh_HandleLuaFileChange, "CAutoRefresh_HandleLuaFileChange",
		server_loader.GetModule(), Symbols::GarrysMod_AutoRefresh_HandleLuaFileChangeSym,
		(void*)hook_CAutoRefresh_HandleLuaFileChange, m_pID
	);
	*/
}

void CAutoRefreshModule::Think(bool simulating)
{
}

void CAutoRefreshModule::Shutdown()
{
}