return {
    groupName = "IGModAudioChannel:GetPlaybackRate",

    cases = {
        {
            name = "Function exists when module enabled",
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                expect( FindMetaTable( "IGModAudioChannel" ).GetPlaybackRate ).to.beA( "function" )
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
            name = "GetPlaybackRate returns a number",
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

                    local rate = channel:GetPlaybackRate()
                    expect( rate ).to.beA( "number" )

                    done()
                end )
            end
        },
        {
            name = "Matches value set with SetPlaybackRate",
            when = HolyLib_IsModuleEnabled( "bass" ),
            async = true,
            timeout = 2,
            func = function()
                local filePath = "sound/bass_testsound.wav"
                local flags = ""
        
               bass.PlayFile( filePath, flags, function( channel, errCode, errMsg )
                    expect( channel ).to.beValid()

                    channel:SetPlaybackRate( 1.5 )
                    expect( channel:GetPlaybackRate() ).to.equal( 1.5 )

                    channel:SetPlaybackRate( 0.75 )
                    expect( channel:GetPlaybackRate() ).to.equal( 0.75 )

                    done()
                end)
            end
        },
    }
}