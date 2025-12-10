return {
    groupName = "IGModAudioChannel:SetPlaybackRate",

    cases = {
        {
            name = "Function exists when module enabled",
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                expect( FindMetaTable( "IGModAudioChannel" ).SetPlaybackRate ).to.beA( "function" )
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
            name = "Valid call sets the playback rate correctly",
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

                    channel:SetPlaybackRate( 2 )
                    expect( channel:GetPlaybackRate() ).to.equal( 2 )

                    done()
                end )
            end
        },
    }
}