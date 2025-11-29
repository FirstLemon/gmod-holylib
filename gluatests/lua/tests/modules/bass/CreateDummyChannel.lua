return {
    groupName = "bass.CreateDummyChannel",

    cases = {
        {
            name = "Function exists when module enabled",
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                expect( bass.CreateDummyChannel ).to.beA( "function" )
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
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                local dummyChannel, err = bass.CreateDummyChannel( 44000, 2, 0 )

                expect( dummyChannel ).to.exist()
                expect( dummyChannel ).to.beA( "userdata" )
                expect( err ).to.beNil()
            end
        },
        {
            name = "Testing function with invalid sample rate",
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                local dummyChannel, err = bass.CreateDummyChannel( -1, 1, 0 )

                expect( dummyChannel ).to.beNil()
                expect( err ).to.beA( "string" )
                expect( err ).to.equal( "BASS_ERROR_FORMAT" )
            end
        },
        {
            name = "Testing function with invalid channel count",
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                local dummyChannel, err = bass.CreateDummyChannel( 44000, 0, 0 )

                expect( dummyChannel ).to.beNil()
                expect( err ).to.beA( "string" )
                expect( err ).to.equal( "BASS_ERROR_FORMAT" )
            end
        },
        {
            name = "Testing function with invalid flag parameter",
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                local dummyChannel, err = bass.CreateDummyChannel( 44000, 1, -1 )

                expect( dummyChannel ).to.beNil()
                expect( err ).to.beA( "string" )
                expect( err ).to.equal( "BASS_ERROR_NO3D" )
            end
        },
        {
            name = "Testing function with invalid arguments",
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                expect( bass.CreateDummyChannel, "44000", 1, 0 ).to.err()
            end
        },
    }
}