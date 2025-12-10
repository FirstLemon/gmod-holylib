return {
    groupName = "IGModAudioChannel:SetTime",

    cases = {
        {
            name = "Function exists when module enabled",
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                expect( FindMetaTable( "IGModAudioChannel" ).SetTime ).to.beA( "function" )
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
            name = "Valid call sets the time correctly",
            when = HolyLib_IsModuleEnabled( "bass" ),
            async = true,
            timeout = 2,
            func = function()
                local filePath = "sound/bass_testsound.wav"
                local flags = ""
        
                bass.PlayFile( filePath, flags, function( channel, errorCode, errorMsg )
                    expect( channel ).to.beValid()
                    expect( errorCode ).to.equal( 0 )
                    expect( errorMsg ).to.beNil()

                    channel:SetTime( 0.5 )
                    local time = channel:GetTime()
                    expect( time ).to.beA( "number" )
                    expect( math.abs( time - 0.5 ) < 0.05).to.beTrue()

                    done()
                end )
            end
        },
    }
}