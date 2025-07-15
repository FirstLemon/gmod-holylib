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
                local filePath = "sound/bass-test.wav"
                local flags = "mono"
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    expect( channel ).toNot.beNil()
                    expect( errorCode ).to.equal( 0 )
                    expect( errorMsg ).to.beNil()

                    expect( channel:GetState() ).to.equal( 1 )
                    
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
                local filePath = "sound/bass-test.wav"
                local flags = ""
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    expect( channel ).toNot.beNil()
                    expect( errorCode ).to.equal( 0 )
                    expect( errorMsg ).to.beNil()

                    expect( channel:GetState() ).to.equal( 1 )
                    
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
                local filePath = "sound/bass-test.wav"
                local flags = "noplay"
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    expect( channel ).toNot.beNil()
                    expect( errorCode ).to.equal( 0 )
                    expect( errorMsg ).to.beNil()

                    expect( channel:GetState() ).to.equal( 0 )
                    
                    done()
                end)
            end
        },
        {
            name = "Success Case - Playing valid sample audio file with noblock flag",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 2,
            func = function()
                local filePath = "sound/bass-test.wav"
                local flags = "noblock"
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    expect( channel ).toNot.beNil()
                    expect( errorCode ).to.equal( 0 )
                    expect( errorMsg ).to.beNil()

                    expect( channel:GetState() ).to.equal( 1 )
                    
                    done()
                end)
            end
        },
        {
            name = "Success Case - Ignores invalid / nonsense flag",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 2,
            func = function()
                local filePath = "sound/bass-test.wav"
                local flags = "holy noplay hello"
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    expect( channel ).toNot.beNil()
                    expect( errorCode ).to.equal( 0 )
                    expect( errorMsg ).to.beNil()

                    expect( channel:GetState() ).to.equal( 0 )
                    
                    done()
                end)
            end
        },
        -- #############
        -- Failure Cases
        -- #############
        {
            name = "Failure Case - Invalid Path",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 2,
            func = function()
                local filePath = "sound/thisFileIsCool.wav"
                local flags = "mono"
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    expect( channel ).to.beNil()
                    expect( errorCode ).to.equal( 2 )
                    expect( errorMsg ).to.equal( "BASS_ERROR_FILEOPEN" )
                    
                    done()
                end)
            end
        },
        {
            name = "Failure Case - incorrect 3d usage",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 2,
            func = function()
                local filePath = "sound/bass-test.wav"
                local flags = "3d mono"
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    expect( channel ).to.beNil()
                    expect( errorCode ).to.equal( 21 )
                    expect( errorMsg ).to.equal( "BASS_ERROR_NO3D" )
                    
                    done()
                end)
            end
        },
        {
            name = "Failure Case - Not an audio file",
            when = HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 2,
            func = function()
                local filePath = "sound/not-real.txt"
                local flags = ""
        
                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    expect( channel ).to.beNil()
                    expect( errorCode ).to.equal( 41 )
                    expect( errorMsg ).to.equal( "BASS_ERROR_FILEFORM" )
                    
                    done()
                end)
            end
        },
    }
}