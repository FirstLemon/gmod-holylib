return {
    groupName = "bass.PlayFile",

    cases = {
        {
            name = "Function exists globally",
            when = HolyLib_IsModuleEnabled("bass"),
            func = function()
                expect( bass.PlayFile ).to.beA( "function" )
            end
        },
        {
            name = "Function doesn't exists globally",
            when = not HolyLib_IsModuleEnabled("bass"),
            func = function()
                expect( bass.PlayFile ).to.beA( "nil" )
            end
        },
        {
            name = "trying to play valid wav File",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 20,
            func = function()
                filePath = "buttons/blip1.wav"
                local flags = "mono"
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    print("channel: ", channel)
                    print("errorCode: ", errorCode)
                    print("errorMsg: ", errorMsg)

                    expect(channel).toNot.beNil()
                    expect(errorCode).to.beNil()
                    expect(errorMsg).to.beNil()
                    
                    done()
                end)
            end
        },
    }
}