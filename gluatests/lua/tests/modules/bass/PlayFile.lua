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
            name = "Does Soundfile exist?",
            when = not HolyLib_IsModuleEnabled("bass"),
            func = function()
                local exists1 = file.Exists("sound/buttons/button1.wav", "GAME")
                expect(exists1).to.beTrue()

                local exists2 = file.Exists("buttons/button1.wav", "GAME")
                expect(exists2).to.beTrue()

                local exists3 = file.Exists("button1.wav", "GAME")
                expect(exists3).to.beTrue()
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