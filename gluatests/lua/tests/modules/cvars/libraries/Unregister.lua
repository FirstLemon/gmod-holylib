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
                expect(cvar).to.beA("nil")
            end
        },
        {   
            name = "Unregister 'achievement_debug'",
            when = HolyLib_IsModuleEnabled("cvars"),
            func = function()
                local convar = CreateConVar("holylib_unregister_test", "0")
                expect(convar).to.beA("ConVar")
                cvar.Unregister("holylib_unregister_test")
                expect(convar).to.beInvalid()
            end
        },
    }
}
