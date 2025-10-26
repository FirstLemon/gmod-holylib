return {
    groupName = "IGModAudioChannel:Stop",
    cases = {
        {
            name = "Function exists on meta table",
            when = HolyLib_IsModuleEnabled("bass"),
            func = function()
                expect( FindMetaTable("IGModAudioChannel").Stop ).to.beA( "function" )
            end
        },
        {
            name = "Metatable doesn't exist",
            when = not HolyLib_IsModuleEnabled("bass"),
            func = function()
                expect( FindMetaTable("IGModAudioChannel") ).to.beA( "nil" )
            end
        },
        -- Enums returned from GetState()
        -- https://wiki.facepunch.com/gmod/Enums/GMOD_CHANNEL
        {
            name = "Stops playing the audio channel",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 2,
            func = function()
                local filePath = "sound/bass_testsound.wav"
                local flags = ""
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    expect( channel ).toNot.beNil()
                    expect( channel:GetState() ).to.equal( 1 )

                    channel:Stop()
                    expect( channel:GetState() ).to.equal( 0 )
                    
                    done()
                end)
            end
        },
        {
            name = "Calling Stop() twice is safe",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 2,
            func = function()
                local filePath = "sound/bass_testsound.wav"
                local flags = ""
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    expect( channel ).toNot.beNil()

                    channel:Stop()
                    channel:Stop()
                    expect( channel:GetState() ).to.equal( 0 )
                    
                    done()
                end)
            end
        },
        {
            name = "Audio channel remains valid after stopping",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 2,
            func = function()
                local filePath = "sound/bass_testsound.wav"
                local flags = ""
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    expect( channel ).toNot.beNil()
                    expect( channel ).to.beValid()

                    channel:Stop()

                    expect( channel ).to.beValid()
                    
                    done()
                end)
            end
        },
        {
            name = "Stopped Audio channel can only be resumed with Play()",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 2,
            func = function()
                local filePath = "sound/bass_testsound.wav"
                local flags = ""
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    expect( channel ).toNot.beNil()
                    expect( channel:GetState() ).to.equal( 1 )

                    channel:Stop()
                    expect( channel:GetState() ).to.equal( 0 )

                    channel:Pause()
                    expect( channel:GetState() ).to.equal( 0 )

                    channel:Play()
                    expect( channel:GetState() ).to.equal( 1 )
                    
                    done()
                end)
            end
        },
    }
}