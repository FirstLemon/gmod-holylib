return {
    groupName = "IGModAudioChannel:Destroy",
    cases = {
        {
            name = "Function exists on meta table",
            when = HolyLib_IsModuleEnabled("bass"),
            func = function()
                expect( FindMetaTable("IGModAudioChannel").Destroy ).to.beA( "function" )
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
            name = "Destroying Audio Channel",
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
                        return channel:IsValid()
                    end)

                    expect( ok ).to.beFalse()
                    expect( error ).to.equal( "Tried to use a NULL IGModAudioChannel!" )
                    
                    done()
                end)
            end
        },
        {
            name = "Destroyed Audio Channel is no longer valid",
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
                        return channel:IsValid()
                    end)

                    expect( ok ).to.beFalse()
                    expect( error ).to.equal( "Tried to use a NULL IGModAudioChannel!" )
                    
                    done()
                end)
            end
        },
    }
}