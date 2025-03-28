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

LUA_FUNCTION_STATIC(noodles) 
{
	LUA->CheckType(1, GarrysMod::Lua::Type::Number);
	LUA->CheckType(2, GarrysMod::Lua::Type::Number);

	double arg1 = LUA->GetNumber(1);
	double arg2 = LUA->GetNumber(2);

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->GetField(-1, "print");
		LUA->PushNumber(arg1 + arg2);
		LUA->Call(1, 0);
	LUA->Pop();

	return 0;
}

/*
static Detouring::Hook detour_CAutoRefresh_HandleLuaFileChange;
void hook_CAutoRefresh_HandleLuaFileChange(void* something, const std::string* filecontent)
{
	Msg("Lua AutoRefresh - %s\n", filecontent->c_str());
}

struct string
{
	char* data;
	size_t size;
	size_t capacity;
};
*/
uintptr_t thisPointer;

static Detouring::Hook detour_CAutoRefresh_HandleLuaFileChange;
static void __fastcall hook_CAutoRefresh_HandleLuaFileChange() 
{
	__asm {
		mov eax, [ebp + 8]
		mov thisPointer, eax
	}
	Msg("thisPointer: %p\n", (void*)thisPointer);
}

void CAutoRefreshModule::LuaInit(GarrysMod::Lua::ILuaInterface* pLua, bool bServerInit)
{
	if (bServerInit)
		return;

	Util::StartTable();
		Util::AddFunc(noodles, "noodles");
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
	Detour::Create(
		&detour_CAutoRefresh_HandleLuaFileChange, "CAutoRefresh_HandleLuaFileChange",
		server_loader.GetModule(), Symbols::GarrysMod_AutoRefresh_HandleLuaFileChangeSym,
		(void*)hook_CAutoRefresh_HandleLuaFileChange, m_pID
	);
}

void CAutoRefreshModule::Think(bool simulating)
{
}

void CAutoRefreshModule::Shutdown()
{
}