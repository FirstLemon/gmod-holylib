#include "module.h"
#include "LuaInterface.h"
#include "lua.h"
#include "detours.h"
#include "Bootil/File/Changes.h"


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


static Detouring::Hook detour_CAutoRefresh_GetChange;
static void hook_CAutoRefresh_GetChange()
{

};


static Detouring::Hook detour_CAutoRefresh_CheckForChanges;
static void hook_CAutoRefresh_CheckForChanges()
{

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

	// HandleLuaFileCHange
	Detour::Create(
		&detour_CAutoRefresh_HandleLuaFileChange, "CAutoRefresh_HandleLuaFileChange",
		server_loader.GetModule(), Symbols::GarrysMod_AutoRefresh_HandleLuaFileChangeSym,
		(void*)hook_CAutoRefresh_HandleLuaFileChange, m_pID
	);

	// Bootil::ChangeMonitor::GetChange
	Detour::Create(
		&detour_CAutoRefresh_GetChange, "CAutoRefresh_GetChangeSym",
		server_loader.GetModule(), Symbols::Bootil_File_ChangeMonitor_GetChangeSym,
		(void *)hook_CAutoRefresh_GetChange, m_pID
	);

	// Bootil::ChangeMonitor::CheckForChanges
	Detour::Create(
		&detour_CAutoRefresh_CheckForChanges, "CAutoRefresh_CheckForChanges",
		server_loader.GetModule(), Symbols::Bootil_File_ChangeMonitor_CheckForChangesSym,
		(void *)hook_CAutoRefresh_CheckForChanges, m_pID
	);
}

void CAutoRefreshModule::Think(bool simulating)
{
}

void CAutoRefreshModule::Shutdown()
{
}