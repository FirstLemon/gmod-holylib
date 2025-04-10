#include "module.h"
#include "LuaInterface.h"
#include "lua.h"
#include "detours.h"


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

static Detouring::Hook detour_CAutoRefresh_HandleLuaFileChange;
static void hook_CAutoRefresh_HandleLuaFileChange(const std::string *fileRelativePath, const std::string *fileContent)
{
	if (Lua::PushHook("HolyLib::OnLuaFileChange"))
	{
		// g_Lua->PushString(fileRelativePath->c_str());
		if (g_Lua->CallFunctionProtected(1, 1, true)) {
			g_Lua->Pop(1);
			return;
		}
	}
};

/* //
static Detouring::Hook detour_CAutoRefresh_HandleLuaFileChange;
static void hook_CAutoRefresh_HandleLuaFileChange(const std::string *fileRelativePath, const std::string *fileContent)
{
	Msg("HandleLuaFileChange Arg1 - %s\n", fileRelativePath->c_str());
	Msg("HandleLuaFileChange Arg2 - %s\n", fileContent->c_str());

	if (Lua::PushHook("HolyLib::OnLuaFileChange"))
	{
		g_Lua->PushString(fileRelativePath->c_str());
		g_Lua->CallFunctionProtected(1, 0, true);
	}
}
*/

/*
static Detouring::Hook detour_CAutoRefresh_FindRootFile;
void hook_CAutoRefresh_FindRootFile(void* something, const std::string* unknown)
{
	Msg("Lua FindRootFile - %s\n", unknown->c_str());
}
*/

void CAutoRefreshModule::LuaInit(GarrysMod::Lua::ILuaInterface* pLua, bool bServerInit)
{
	if (bServerInit)
		return;

	/*
	Util::StartTable(pLua);
	Util::FinishTable(pLua, "autorefresh");
	*/
}

void CAutoRefreshModule::LuaShutdown(GarrysMod::Lua::ILuaInterface* pLua)
{
	// Util::NukeTable(pLua, "autorefresh");
}

void CAutoRefreshModule::InitDetour(bool bPreServer)
{
	if (bPreServer)
		return;

	SourceSDK::ModuleLoader server_loader("server");
	Detour::Create(
		&detour_CAutoRefresh_HandleLuaFileChange, "CAutoRefresh_HandleLuaFileChange",
		server_loader.GetModule(), Symbols::GarrysMod_AutoRefresh_HandleLuaFileChangeSym,
		(void*)hook_CAutoRefresh_HandleLuaFileChange, m_pID
	);

	/*
	Detour::Create(
		&detour_CAutoRefresh_FindRootFile, "CAutoRefresh_FindRootFile",
		server_loader.GetModule(), Symbols::GarrysMod_AutoRefresh_FindRootFileSym,
		(void*)hook_CAutoRefresh_FindRootFile, m_pID
	);
	*/
}

void CAutoRefreshModule::Think(bool simulating)
{
}

void CAutoRefreshModule::Shutdown()
{
}