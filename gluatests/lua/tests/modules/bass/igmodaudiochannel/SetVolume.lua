return {
    groupName = "IGModAudioChannel:SetVolume",
    cases = {
        {
            name = "Function exists on meta table",
            when = HolyLib_IsModuleEnabled("bass"),
            func = function()
                expect( FindMetaTable("IGModAudioChannel").SetVolume ).to.beA( "function" )
            end
        },
        {
            name = "Metatable doesn't exist",
            when = not HolyLib_IsModuleEnabled("bass"),
            func = function()
                expect( FindMetaTable("IGModAudioChannel") ).to.beA( "nil" )
            end
        },
        {
            name = "Sets volume to 0.5",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 2,
            func = function()
                local filePath = "sound/bass-test.wav"
                local flags = ""
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    expect( channel ).toNot.beNil()
                    expect( channel ).to.beValid()

                    channel:SetVolume( 0.5 )
                    expect(channel:GetVolume()).to.equal( 0.5 )
                    
                    done()
                end)
            end
        },
        {
            name = "Sets volume to a negative number ( -1 )",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 2,
            func = function()
                local filePath = "sound/bass-test.wav"
                local flags = ""
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    expect( channel ).toNot.beNil()
                    expect( channel ).to.beValid()

                    expect(channel:GetVolume()).to.equal( 1 )
                    channel:SetVolume( -1 )
                    expect(channel:GetVolume()).to.equal( 1 )
                    
                    done()
                end)
            end
        },
        {
            name = "SetVolume on destroyed channel should error",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 2,
            func = function()
                local filePath = "sound/bass-test.wav"
                local flags = ""
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    expect( channel ).toNot.beNil()
                    expect( channel ).to.beValid()

                    channel:Destroy()

                    local ok, error = pcall(function()
                        return channel:SetVolume( 0.5 )
                    end)

                    expect( ok ).to.beFalse()
                    expect( error ).to.equal( "Tried to use a NULL IGModAudioChannel!" )
                    
                    done()
                end)
            end
        },
    }
}