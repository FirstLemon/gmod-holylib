return {
    groupName = "IGModAudioChannel:__tostring",
    cases = {
        {
            name = "Function exists globally",
            when = HolyLib_IsModuleEnabled("bass"),
            func = function()
                expect( FindMetaTable("IGModAudioChannel").__tostring ).to.beA( "function" )
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
            name = "__tostring on valid channel returns correct string",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 2,
            func = function()
                local filePath = "sound/bass-test.wav"
                local flags = ""
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    local output = channel:__tostring()
                    expect( output ).to.beA( "string" ) 
                    
                    done()
                end)
            end
        },
        {
            name = "__tostring on invlalid channel is being handled correctly",
            when = HolyLib_IsModuleEnabled("bass"),
            func = function()
                local filePath = "sound/bass-test.wav"
                local flags = ""

                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    local invalidChannel = bass.IGModAudioChannel()
                    local output = invalidChannel:__tostring()
                    expect( output ).to.beA( "string" ) 
                end)
        }
    }
}