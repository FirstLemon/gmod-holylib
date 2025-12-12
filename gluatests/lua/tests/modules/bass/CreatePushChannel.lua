return {
    groupName = "bass.CreatePushChannel",

    cases = {
        {
            name = "Function exists when module enabled",
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                expect( bass.CreatePushChannel ).to.beA( "function" )
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
            name = "Returns a valid igmodaudiochannel and nil for a valid call",
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                local channel, err = bass.CreatePushChannel( 44100, 2, 0 )

                expect( channel ).to.exist()
                expect( channel ).to.beA( "'IGModAudioChannel [PUSH]'" )
                expect( err ).to.beNil()
            end
        },
        {
            name = "Invalid sample rate returns nil and BASS_ERROR_FORMAT",
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                local channel, err = bass.CreatePushChannel( -1, 2, 0 )

                expect( channel ).to.beNil()
                expect( err ).to.beA( "string" )
                expect( err ).to.equal( "BASS_ERROR_FORMAT" )
            end
        },
        {
            name = "Invalid channel count returns nil and BASS_ERROR_FORMAT",
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                local channel, err = bass.CreatePushChannel( 44100, 0, 0 )

                expect( channel ).to.beNil()
                expect( err ).to.beA( "string" )
                expect( err ).to.equal( "BASS_ERROR_FORMAT" )
            end
        },
        {
            name = "Invalid arg types throw lua error",
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                expect( bass.CreatePushChannel, "We are definitely not horses", 1, 0 ).to.err()
            end
        },
    }
}