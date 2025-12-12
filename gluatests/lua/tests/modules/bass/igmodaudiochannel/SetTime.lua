return {
    groupName = "IGModAudioChannel:SetTime",

    beforeEach = function( state )
        state.filePath = "sound/bass_testsound.wav"
        state.flags = "noplay"
    end,

    afterEach = function( state )
        if IsValid( state.channel ) then
            state.channel:Stop()
            state.channel = nil
        end
    end,

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
            name = "SetTime moves playback postion",
            when = HolyLib_IsModuleEnabled( "bass" ),
            async = true,
            timeout = 2,
            func = function()
                bass.PlayFile( state.filePath, state.flags, function( channel, errorCode, errorMsg )
                    expect( channel ).to.beValid()

                    state.channel = channel

                    channel:SetTime(1.5)
                    local time = channel:GetTime()
                    expect( time ).to.beA( "number" )
                    expect( time ).to.aboutEqual(1.5, 0.05)

                    done()
                end )
            end
        },
    }
}