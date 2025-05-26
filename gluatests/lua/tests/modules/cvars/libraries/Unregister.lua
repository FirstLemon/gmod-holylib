return {
    groupName = "cvars:Unregister",
    cases = {
        {
            name = "Function exists globally",
            when = HolyLib_IsModuleEnabled("cvars"),
            func = function()
                expect(cvar.Unregister).to.beA("function")
            end
        },
        {
            name = "Function doesn't exists globally",
            when = not HolyLib_IsModuleEnabled("cvars"),
            func = function()
                expect(cvar.Unregister).to.beA("nil")
            end
        },
        {   
            name = "Unregister 'achievement_debug'",
            when = HolyLib_IsModuleEnabled("cvars"),
            func = function()
                local ConVar = GetConVar("achievement_debug")
                expect(ConVar).to.beValid()
                cvar.Unregister("achievement_debug")
                expect(ConVar).to.beInvalid()
            end
        },
    }
}
