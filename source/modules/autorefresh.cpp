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

// testing
struct AutoRefresh {
	const char *pFileName;

	AutoRefresh(const std::string &lemon) {
		pFileName = lemon.c_str();
	}
};

/*
static Detouring::Hook detour_CAutoRefresh_HandleLuaFileChange;
void hook_CAutoRefresh_HandleLuaFileChange(void* something, const std::string *unknown_a, const std::string *unknown_b, const std::string *unknown_c)
{
	Msg("Test \n");
	AutoRefresh whatever(*unknown_a);
	Msg("Lua AutoRefresh 1 - %s\n", whatever.pFileName);
	Msg("Lua AutoRefresh 2 - %s\n", unknown_b->c_str());
	Msg("Lua AutoRefresh 3 - %s\n", unknown_c->c_str());
}
*/

/*
static Detouring::Hook detour_CAutoRefresh_FindRootFile;
void hook_CAutoRefresh_FindRootFile(void* something, const std::string* unknown)
{
	Msg("Lua FindRootFile - %s\n", unknown->c_str());
}
*/

// /*
static Detouring::Hook detour_CAutoRefresh_HandleChange_Lua;
bool hook_CAutoRefresh_HandleChange_Lua(void* something, const std::string* strFolder, const std::string* strFilename, const std::string* strExtension)
{	
	Msg("--- <> ---: %s", *strExtension);
	std::string test = "lua";
	const std::string* ext = &test;
	if (strExtension = ext) return false;


	Msg("Lua HandleChange_Lua Folder - %s\n", strFolder->c_str());
	Msg("Lua HandleChange_Lua Filename - %s\n", strFilename->c_str());

	return true;
}
// */

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
	/*
	Detour::Create(
		&detour_CAutoRefresh_HandleLuaFileChange, "CAutoRefresh_HandleLuaFileChange",
		server_loader.GetModule(), Symbols::GarrysMod_AutoRefresh_HandleLuaFileChangeSym,
		(void*)hook_CAutoRefresh_HandleLuaFileChange, m_pID
	);
	*/

	/*
	Detour::Create(
		&detour_CAutoRefresh_FindRootFile, "CAutoRefresh_FindRootFile",
		server_loader.GetModule(), Symbols::GarrysMod_AutoRefresh_FindRootFileSym,
		(void*)hook_CAutoRefresh_FindRootFile, m_pID
	);
	*/

	// /*
	Detour::Create(
		&detour_CAutoRefresh_HandleChange_Lua, "CAutoRefresh_HandleChange_Lua",
		server_loader.GetModule(), Symbols::GarrysMod_AutoRefresh_HandleChange_LuaSym,
		(void*)hook_CAutoRefresh_HandleChange_Lua, m_pID
	);
	// */
}

void CAutoRefreshModule::Think(bool simulating)
{
}

void CAutoRefreshModule::Shutdown()
{
}