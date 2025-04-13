return {
    groupName = "cvars",
    cases = {
        {
            name = "Function exists globally",
            when = HolyLib_IsModuleEnabled("cvars"),
            func = function()
                expect( cvar.GetAll ).to.beA( "fucntion" )
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
