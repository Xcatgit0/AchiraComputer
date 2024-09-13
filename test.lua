-- Function to convert a number to a binary string with leading zeros
local function toBinaryString(num, bits)
    local bin = ""
    while num > 0 do
        bin = (num % 2) .. bin
        num = math.floor(num / 2)
    end
    return bin:format("%0" .. bits .. "d")
end

-- Functions to convert instructions to binary
local function assembleInstruction(instruction)
    local parts = {}
    for part in string.gmatch(instruction, "%S+") do
        table.insert(parts, part)
    end

    local command = parts[1]:upper()
    local binary = ""

    if command == "SET_PIXEL" then
        local x, y, r, g, b, a = tonumber(parts[2]), tonumber(parts[3]), tonumber(parts[4]), tonumber(parts[5]), tonumber(parts[6]), tonumber(parts[7])
        binary = toBinaryString(0x01, 4) .. toBinaryString(x, 8) .. toBinaryString(y, 8) .. toBinaryString(r, 8) .. toBinaryString(g, 8) .. toBinaryString(b, 8) .. toBinaryString(a, 8)
    elseif command == "KEY_PRESS" then
        local k = tonumber(parts[2])
        binary = toBinaryString(0x02, 4) .. toBinaryString(k, 8)
    elseif command == "POWER" then
        local state = parts[2]:upper() == "ON" and 1 or 0
        binary = toBinaryString(0x03, 4) .. "00000000" .. "00000000" .. toBinaryString(state, 8)
    elseif command == "CLEAR_SCREEN" or command == "UPDATE_SCREEN" then
        local opcode = command == "CLEAR_SCREEN" and 0x04 or 0x05
        binary = toBinaryString(opcode, 4) .. "00000000" .. "00000000" .. "00000000"
    elseif command == "ADD" or command == "SUB" or command == "MUL" or command == "DIV" or command == "MOD" then
        local opcodes = {ADD=0x06, SUB=0x07, MUL=0x08, DIV=0x09, MOD=0x0A}
        local opcode = opcodes[command]
        local r1, r2, r3 = tonumber(parts[2]), tonumber(parts[3]), tonumber(parts[4])
        binary = toBinaryString(opcode, 4) .. toBinaryString(r1, 4) .. toBinaryString(r2, 4) .. toBinaryString(r3, 4) .. "0000"
    else
        error("Unknown instruction: " .. command)
    end

    return binary
end

-- Function to read assembly code from a file and convert to binary
local function convertAssemblyToMicrocode(inputFile, outputFile)
    local file = io.open(inputFile, "r")
    if not file then
        error("Unable to open input file.")
    end

    local binaryCode = ""
    for line in file:lines() do
        -- Skip empty lines and comments
        if line ~= "" and not line:match("^%s*;") then
            local instructionBinary = assembleInstruction(line)
            if instructionBinary then
                binaryCode = binaryCode .. instructionBinary
            end
        end
    end
    file:close()

    -- Convert binary string to bytes
    local file = io.open(outputFile, "wb")
    local raw = io.open("raw"..outputFile, "w")
    raw:write(tostring(binaryCode).."\n")
    if not file then
        error("Unable to open output file.")
    end

    for i = 1, #binaryCode, 8 do
        local byte = binaryCode:sub(i, i + 7)
        if #byte < 8 then
            byte = byte .. string.rep("0", 8 - #byte)
        end
        local charCode = tonumber(byte, 2)
        if charCode then
            file:write(string.char(charCode))
        else
            error("Invalid binary code.")
        end
    end
    file:close()
    raw:close()
    print("Microcode saved to " .. outputFile)
end

-- Example usage
convertAssemblyToMicrocode("example.as", "output.microcode")