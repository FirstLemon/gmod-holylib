return {
    groupName = "IGModAudioChannel:GetTable",
    cases = {
        {
            name = "Function exists when module enabled",
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                expect( FindMetaTable( "IGModAudioChannel" ).GetVolume ).to.beA( "function" )
            end
        },
        {
            name = "Function is nil when module disabled",
            when = not HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                expect( FindMetaTable( "IGModAudioChannel" ) ).to.beA( "nil" )
            end
        },
        {
            name = "GetVolume returns a number",
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()

                bass.PlayFile( filePath, flags, function( channel, errorCode, errorMsg )
                    expect( channel ).toNot.beNil()
                    expect( channel ).to.beValid()

                    expect( channel:GetVolume() ).to.beA( "number" )
                    
                    done()
                end )
            end
        },
    }
}