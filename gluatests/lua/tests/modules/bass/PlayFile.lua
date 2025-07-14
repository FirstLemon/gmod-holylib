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
        -- #############
        -- Success Cases
        -- #############
        {
            name = "Success Case - Playing valid sample audio file with mono flag",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 2,
            func = function()
                filePath = "assets/sound/bass-test.wav"
                local flags = "mono"
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    print("- [GLuaTest] Channel: ", channel)
                    print("- [GLuaTest] ErrorCode: ", errorCode)
                    print("- [GLuaTest] ErrorMsg: ", errorMsg)

                    expect(channel).toNot.beNil()
                    expect(errorCode).to.equal(0)
                    expect(errorMsg).to.beNil()
                    
                    done()
                end)
            end
        },
        {
            name = "Success Case - Playing valid sample audio file with no flag",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 2,
            func = function()
                filePath = "assets/sound/bass-test.wav"
                local flags = ""
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    print("- [GLuaTest] Channel: ", channel)
                    print("- [GLuaTest] ErrorCode: ", errorCode)
                    print("- [GLuaTest] ErrorMsg: ", errorMsg)

                    expect(channel).toNot.beNil()
                    expect(errorCode).to.equal(0)
                    expect(errorMsg).to.beNil()
                    
                    done()
                end)
            end
        },
        {
            name = "Success Case - Playing valid sample audio file with noplay flag",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 2,
            func = function()
                filePath = "assets/sound/bass-test.wav"
                local flags = ""
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    print("- [GLuaTest] Channel: ", channel)
                    print("- [GLuaTest] ErrorCode: ", errorCode)
                    print("- [GLuaTest] ErrorMsg: ", errorMsg)

                    expect(channel).toNot.beNil()
                    expect(errorCode).to.equal(0)
                    expect(errorMsg).to.beNil()
                    
                    done()
                end)
            end
        },
        {
            name = "Success Case - Playing valid sample audio file with noplay flag",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 2,
            func = function()
                filePath = "assets/sound/bass-test.wav"
                local flags = "noblock"
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    print("- [GLuaTest] Channel: ", channel)
                    print("- [GLuaTest] ErrorCode: ", errorCode)
                    print("- [GLuaTest] ErrorMsg: ", errorMsg)

                    expect(channel).toNot.beNil()
                    expect(errorCode).to.equal(0)
                    expect(errorMsg).to.beNil()
                    
                    done()
                end)
            end
        },
        -- #############
        -- Failure Cases
        -- #############
        {
            name = "Failure - Invalid Path",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 2,
            func = function()
                filePath = "assets/sound/thisFileIsCool.wav"
                local flags = "noblock"
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    print("- [GLuaTest] Channel: ", channel)
                    print("- [GLuaTest] ErrorCode: ", errorCode)
                    print("- [GLuaTest] ErrorMsg: ", errorMsg)

                    expect(channel).toNot.beNil()
                    expect(errorCode).to.equal(0)
                    expect(errorMsg).to.beNil()
                    
                    done()
                end)
            end
        },
    }
}