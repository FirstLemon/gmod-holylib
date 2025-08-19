--[[
	This file is called by our workflow to create a summary after all builds & tests finished.
]]

dofile("utils.lua")

json = require("json")
require("http")

local settings = {...}
local loki_host = settings[1]
local loki_api = settings[2]
local run_number = tonumber(settings[3])

if not loki_host or not loki_api or not run_number then
	WriteFile("generated_summary.md", "Got no results") -- Test
	return
end

-- Let's fetch the current run data.
function FetchLokiResults(runNumber, callback)
	JSONHTTP({
		blocking = true,
		failed = function(reason)
			print("Failed to get performance results from Loki for " .. runNumber .. "!", reason)
		end,
		success = function(json)
			print("Successfully got performance results from Loki for " .. runNumber .. " :3")
			callback(json)
		end,
		method = "GET",
		url = loki_host .. "/loki/api/v1/query_range",
		params = {
			"query={run_number=\\\"" .. runNumber .. "\\\"}",
			"since=30d",
			"limit=1000",
		},
		headers = {
			["X-Api-Key"] = loki_api
		},
	})
end

currentLokiResults = {}
FetchLokiResults(run_number, function(jsonTable)
	currentLokiResults = jsonTable
end)

lastLokiResults = {} -- Results of the last run.
lastLokiRun = -1
nextLokiSearchID = run_number - 1
while (lastLokiRun == -1) and (run_number - nextLokiSearchID) < 100 do -- NUKE IT >:3
	FetchLokiResults(nextLokiSearchID, function(jsonTable)
		if not jsonTable or not jsonTable.data or not jsonTable.data.result or #jsonTable.data.result == 0 then continue end -- Useless!
		if nextLokiSearchID < lastLokiRun then continue end -- We got newer results already!

		-- We got newer shit :D
		-- NOTE: This code previously was intented to be nuked in a for k=1, 100 loop though Loki's CPU usage spiked to 900% for 15 seconds :sob:
		lastLokiRun = nextLokiSearchID
		lastLokiResults = jsonTable
		print("Found the last performance results " .. nextLokiSearchID)
	end)

	print("stuck")
	--HTTP_WaitForAll() -- This will also wait in the first iteration for our currentLokiResults
	nextLokiSearchID = nextLokiSearchID - 1
	break
end

if not currentLokiResults or not currentLokiResults.data or not currentLokiResults.data.result or #currentLokiResults.data.result == 0 then
	WriteFile("generated_summary.md", "Failed to fetch results!") -- Test
	error("Failed to fetch results!")
	return
end

--[[
	The loki entry will be the following structure:

	[1] = (string) nano second time of the log entry
	[2] = (string) json string containing our data

	resultTable structure:
		[branch name] = (table) {
			[function name] = {
				totalCalls = total calls,
				averageCalls = average time per calls,
				runCount = how many times performance results got collected/now many we merged together,
			}
		}

	logEntry structure:
		totalCalls = number
		timePerCall = number
		gmodBranch = string
		name = string
]]

function CalculateMergedResults(lokiResults)
	local resultTable = {}
	local lokiData = lokiResults.data.result[1]
	for _, lokiEntry in ipairs(lokiData.values) do
		local logEntry = json.decode(lokiEntry[2])
		if not logEntry then continue end

		local branchResults = resultTable[logEntry.gmodBranch]
		if not branchResults then
			branchResults = {}
			resultTable[logEntry.gmodBranch] = branchResults
		end

		local funcResults = branchResults[logEntry.name]
		if not funcResults then
			funcResults = {}
			branchResults[logEntry.name] = funcResults

			funcResults.totalCalls = tonumber(logEntry.totalCalls)
			funcResults.averageCalls = tonumber(logEntry.timePerCall)
			funcResults.runCount = 1
			continue
		end

		funcResults.totalCalls = funcResults.totalCalls + tonumber(logEntry.totalCalls)
		funcResults.averageCalls = (funcResults.averageCalls + tonumber(logEntry.timePerCall)) / 2
		funcResults.runCount = funcResults.runCount + 1
	end

	return resultTable
end

PrintTable(CalculateMergedResults(currentLokiResults))
PrintTable(CalculateMergedResults(lastLokiResults))

local markdown = "# Results"
markdown = markdown .. "# Results"

WriteFile("generated_summary.md", markdown) -- Test