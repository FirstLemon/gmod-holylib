#include "module.h"
#include "LuaInterface.h"
#include "lua.h"
#include "detours.h"

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
void hook_CAutoRefresh_HandleLuaFileChange(const std::string *unknown_a, const std::string *unknown_b, const std::string *unknown_c)
{
	Msg("HandleLuaFileChange Arg1 - %s\n", unknown_a->c_str());
	Msg("HandleLuaFileChange Arg2 - %s\n", unknown_b->c_str());
	Msg("HandleLuaFileChange Arg3 - %s\n", unknown_c->c_str());
}


/*
static Detouring::Hook detour_CAutoRefresh_FindRootFile;
void hook_CAutoRefresh_FindRootFile(void* something, const std::string* unknown)
{
	Msg("Lua FindRootFile - %s\n", unknown->c_str());
}
*/

/*
static Detouring::Hook detour_CAutoRefresh_HandleChange_Lua;
bool hook_CAutoRefresh_HandleChange_Lua(const std::string* strFolder, const std::string* strFilename, const std::string* strExtension)
{	
	const std::string ext = "lua";
	if (*strExtension == ext) return false;

	Msg("Lua HandleChange_Lua dir - %s\n", strFolder->c_str());
	Msg("Lua HandleChange_Lua name - %s\n", strFilename->c_str());
	Msg("Lua HandleChange_Lua ext - %s\n", strExtension->c_str());

	return true;
}
*/

void CAutoRefreshModule::LuaInit(GarrysMod::Lua::ILuaInterface* pLua, bool bServerInit)
{
	if (bServerInit)
		return;

	Util::StartTable();
	Util::FinishTable("autorefresh");
}

void CAutoRefreshModule::LuaShutdown(GarrysMod::Lua::ILuaInterface* pLua)
{
	Util::NukeTable("autorefresh");
}

void CAutoRefreshModule::InitDetour(bool bPreServer)
{
	if (bPreServer)
		return;

	SourceSDK::ModuleLoader server_loader("server");
	// /*
	Detour::Create(
		&detour_CAutoRefresh_HandleLuaFileChange, "CAutoRefresh_HandleLuaFileChange",
		server_loader.GetModule(), Symbols::GarrysMod_AutoRefresh_HandleLuaFileChangeSym,
		(void*)hook_CAutoRefresh_HandleLuaFileChange, m_pID
	);
	// */

	/*
	Detour::Create(
		&detour_CAutoRefresh_FindRootFile, "CAutoRefresh_FindRootFile",
		server_loader.GetModule(), Symbols::GarrysMod_AutoRefresh_FindRootFileSym,
		(void*)hook_CAutoRefresh_FindRootFile, m_pID
	);
	*/

	/*
	Detour::Create(
		&detour_CAutoRefresh_HandleChange_Lua, "CAutoRefresh_HandleChange_Lua",
		server_loader.GetModule(), Symbols::GarrysMod_AutoRefresh_HandleChange_LuaSym,
		(void*)hook_CAutoRefresh_HandleChange_Lua, m_pID
	);
	*/
}

void CAutoRefreshModule::Think(bool simulating)
{
}

void CAutoRefreshModule::Shutdown()
{
}