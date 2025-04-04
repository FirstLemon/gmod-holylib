return {
    groupName = "cvars",
    cases = {
        {
            name = "Function exists globally",
            when = HolyLib_IsModuleEnabled("cvars"),
            func = function()
                expect( FindMetaTable("GetAll") ).to.beA( "function" )
            end
        },
        {
            name = "Function doesn't exists globally",
            when = not HolyLib_IsModuleEnabled("cvars"),
            func = function()
                expect( FindMetaTable("GetAll") ).to.beA( "nil" )
            end
        },
        {
            name = "Returns an Table object",
            when = HolyLib_IsModuleEnabled("cvars"),
            func = function()
                local cvar_list = {}
                expect( cvar_list ).to.beA( "table" )
            end
        },
    }
}
