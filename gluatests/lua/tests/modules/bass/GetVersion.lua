return {
    groupName = "bass.GetVersion",

    cases = {
        {
            name = "Function exists when module enabled",
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                expect( bass.Update ).to.beA( "function" )
            end
        },
        {
            name = "Function is nil when module disabled",
            when = not HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                expect( bass ).to.beA( "nil" )
            end
        },
        {
            name = "GetVersion returns a version string",
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                expect( bass.GetVersion() ).to.beA( "String" )
            end
        },
    }
}