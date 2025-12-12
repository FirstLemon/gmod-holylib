return {
    groupName = "bass.CreateMixerChannel",

    cases = {
        {
            name = "Function exists when module enabled",
            when = HolyLib_IsModuleEnabled( "bass" ) and file.Exists("bin/libbass.so", "GAME"),
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
            name = "Returns a valid IGModAudioChannel and nil error for valid call",
            when = HolyLib_IsModuleEnabled("bass"),
            func = function()
                local mixerChannel, err = bass.CreateMixerChannel(44100, 2, 0)

                expect( mixerChannel ).to.beValid()
                expect( mixerChannel ).to.beA( "IGModAudioChannel [MIXER]")
                expect( err ).to.beNil()
            end
        }
    }
}