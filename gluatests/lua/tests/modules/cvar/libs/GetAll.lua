return {
    groupName = "CopyReadBuffer",
    cases = {
        {
            name = "Function exists globally",
            when = HolyLib_IsModuleEnabled("cvar"),
            func = function()
                expect( CreateEntityList ).to.beA( "function" )
            end
        },
        {
            name = "Function doesn't exists globally",
            when = not HolyLib_IsModuleEnabled("cvar"),
            func = function()
                expect( CreateEntityList ).to.beA( "nil" )
            end
        },
        {
            name = "Returns an Table object",
            when = HolyLib_IsModuleEnabled("cvar"),
            func = function()
                local cvar_list = {}
                expect( cvar_list ).to.beA( "table" )
            end
        },
    }
}
