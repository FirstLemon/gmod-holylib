#pragma once

#include "interface.h"

class IServerPluginCallbacks;
class IModuleManager;
class IHolyUtil;

namespace GarrysMod::Lua
{
	class ILuaInterface;
}

class IHolyLib
{
public:
	// Returns the HolyLib plugin.
	virtual IServerPluginCallbacks* GetPlugin() = 0;

	// Returns the GameFactory for Interfaces from the server dll
	virtual CreateInterfaceFn GetGameFactory() = 0;

	// Returns the AppFactory for Interfaces by the engine and other things
	virtual CreateInterfaceFn GetAppFactory() = 0;

	// Returns the IModuleManager so you could add your own modules
	virtual IModuleManager* GetModuleManager() = 0;

	// Returns the Server ILuaInterface
	virtual GarrysMod::Lua::ILuaInterface* GetLuaInterface() = 0;

	// Calls the ghostinj entrypoint
	virtual void PreLoad() = 0;

	// For thoes who don't want to get it by the interface
	virtual IHolyUtil* GetHolyUtil() = 0;
};

#define INTERFACEVERSION_HOLYLIB "IHOLYLIB001"