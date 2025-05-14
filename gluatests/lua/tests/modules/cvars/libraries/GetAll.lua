return {
    groupName = "cvars:GetAll",
    cases = {
        {
            name = "Function exists globally",
            when = HolyLib_IsModuleEnabled("cvars"),
            func = function()
                expect( cvar.GetAll ).to.beA( "function" )
            end
        },
        {
            name = "Function doesn't exists globally",
            when = not HolyLib_IsModuleEnabled("cvars"),
            func = function()
                expect( cvar.GetAll ).to.beA( "nil" )
            end
        },
        {
            name = "Returns an Table object",
            when = HolyLib_IsModuleEnabled("cvars"),
            func = function()
                expect( cvar.GetAll() ).to.beA( "table" )
            end
        },
    }
}
