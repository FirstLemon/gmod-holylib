#include "module.h"
#include "LuaInterface.h"
#include "lua.h"
#include "detours.h"
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

static Detouring::Hook detour_CAutoRefresh_HandleLuaFileChange;
static void hook_CAutoRefresh_HandleLuaFileChange(const std::string *fileRelPath, const std::string *fileContent)
{	
	Msg("HandleLuaFileChange: %s", fileRelPath->c_str());

	return detour_CAutoRefresh_HandleLuaFileChange.GetTrampoline<Symbols::GarrysMod_AutoRefresh_HandleLuaFileChange>()(fileRelPath, fileContent);
};

// ToDo: Think about this again, maybe 
// I don't like this approach but I don't know any better as the time of writing this
static std::unordered_map<std::string, std::string> blockedPaths;

// Should the user be able to push a whole table of paths or have to call the function for each path seperated... mhm :?
LUA_FUNCTION_STATIC(AddPathToBlockList)
{
	LUA->CheckType(1, GarrysMod::Lua::Type::String);
	LUA->CheckType(2, GarrysMod::Lua::Type::String);

	std::string relPath = LUA->GetString(1);
	std::string fileName = LUA->GetString(2);
	Msg("relPath: %s\nfilename: %s\n", relPath.c_str(), fileName.c_str());
	
	// ToDo: Implement checking if boths args are valid or banana
	// I guess maybe through checking if the files or paths actually exist but that would force weird limits

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->GetField(-1, "print");

		const std::string pathMsg = "Adding '" + relPath + fileName + "' to blocked";
		LUA->PushString(pathMsg.c_str());
		LUA->Call(1, 0);
	LUA->Pop();

	blockedPaths.insert({ relPath, fileName });

	return 0;
}

// decisions, decisions. Should I detour the bootil changemonitor and handle the autorefresh from there? Would that result in better performance but at what cost? Would that even work that way (future me find that out)
// or should I rather detour HandleChange_Lua, to handle my madness at a later point to get balance between perf and control, again would that even work like that
// or should I just detour HandleLuaFileChange to get the least heavy implementation, but that would mean that all the previous checks are dealt with even when we block a path or whatever
// or maybe just a mixture of all. 

static Detouring::Hook detour_CAutoRefresh_HandleChange_Lua;
static void hook_CAutoRefresh_HandleChange_Lua(const std::string *pfileRelPath, const std::string *pfileName, const std::string *pfileExt)
{
	// ->
	if (pfileRelPath && pfileName && pfileExt) {
		Msg("----\nAutoRefresh Debug Dump\n----\nArg1: %s\nArg2: %s\nArg3: %s\n----\n", pfileRelPath->c_str(), pfileName->c_str(), pfileExt->c_str());
	}
	else {
		Msg("Received something invalid: arg1=%p, arg2=%p, arg3=%p\n", pfileRelPath, pfileName, pfileExt);
		return;
	}

	std::string fileRelPath = "hey/";
	std::string fileName = "yeh";
	std::string fileExt = "hey";

	std::string fullFileRelPath = fileRelPath + fileName + "." + fileExt;

	for (auto iter = blockedPaths.begin(); iter != blockedPaths.end(); iter++)
	{
		Msg(" - BLOCKED PATHS: %s/%s\n", iter->first.c_str(), iter->second.c_str());
		
		auto findIter = blockedPaths.find(fullFileRelPath);

		if (findIter == blockedPaths.end()) {
			Msg(" - Path IS NOT Refresh blocked: [%s]", fullFileRelPath.c_str());
		}
		else {
			Msg(" - Path IS Refresh blocked, denying refresh: [%s]", fullFileRelPath.c_str());
			return;
		}
	}

	// the problem has something to do with this fishy mcdouble chili cheese, I do not even know if what I'm trying to do is actually possible or valid
	// I guess it was
	return detour_CAutoRefresh_HandleChange_Lua.GetTrampoline<Symbols::GarrysMod_AutoRefresh_HandleChange_Lua>()(pfileRelPath, pfileName, pfileExt);
};

void CAutoRefreshModule::LuaInit(GarrysMod::Lua::ILuaInterface* pLua, bool bServerInit)
{
	if (bServerInit)
		return;

	Util::StartTable(pLua);
		Util::AddFunc(pLua, AddPathToBlockList, "AddBlockPathLua");
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

	// HandleChange_Lua
	Detour::Create(
		&detour_CAutoRefresh_HandleChange_Lua, "CAutoRefresh_HandleLuaFileChange",
		server_loader.GetModule(), Symbols::GarrysMod_AutoRefresh_HandleChange_LuaSym,
		(void *)hook_CAutoRefresh_HandleChange_Lua, m_pID
	);


	// HandleLuaFileCHange
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