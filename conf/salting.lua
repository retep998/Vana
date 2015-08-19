-- What modifications should be made to the salt before hashing?
-- Note that this is separate from how it is stored in the database by design.
-- This increases the distribution of salting information to make it more difficult to attack.

-- Salt policy changes how the salt is distributed to the password and is one of the following:
-- system_salt_prepend
-- -- N/A
-- system_salt_append
-- -- N/A
-- system_salt_chopdistribute (split the salt and prepend a portion and append a portion)
-- -- The first parameter is the split point unless 2 parameters are provided, in which case, it defines how many characters from the beginning of the salt get prepended
-- -- The second parameter is how many characters from the end of the salt are appended to the password (optional)
-- system_salt_chopcrossdistribute (split the salt and prepend a portion and append a portion)
-- -- The first parameter is the split point unless 2 parameters are provided, in which case, it defines how many characters from the end of the salt get prepended
-- -- The second parameter is how many characters from the end of the salt are prepended to the password (optional)
-- system_salt_intersperse (take letters from salt and password alternately)
-- -- The first parameter is how many letters of the password to take before adding a character of salt
-- -- The second parameter is how many letters of the password to skip before starting to take salt characters
-- -- The third parameter is the salt leftover policy (system_salt_leftover_discard, system_salt_leftover_append, system_salt_leftover_prepend, system_salt_leftover_rollover; default system_salt_leftover_discard)

-- Salt modify policy transforms the salt physically before the application of salt policy
-- It is an array of objects consisting of a type key and args key (duplicates allowed) composed of the following:
-- system_salt_modify_xor_byte (performs a bitwise xor upon each byte of the salt with a specified value)
-- -- The first parameter indicates the value (an integer in the range [0, 255] or a single character) to xor each byte with
-- system_salt_modify_xor (performs a xor upon the salt byte-by-byte with a specified value)
-- -- The first parameter indicates the string/bytes to xor the string with. It will stop xoring when it runs out of string or out of salt.
-- system_salt_modify_bit_rotate (performs a bitwise right rotate upon each byte of the salt by a specified value)
-- -- The first parameter indicates how many bits to rotate
-- system_salt_modify_overall_bit_rotate (performs a bitwise right rotate upon the entire salt by a specified value)
-- -- The first parameter indicates how many bits to rotate
-- system_salt_modify_rotate (performs a byte rotate upon the salt by a specified value)
-- -- The first parameter indicates how many bytes to rotate
-- system_salt_modify_bit_reverse (performs a bitwise reverse upon each byte of the salt)
-- -- N/A
-- system_salt_modify_reverse (performs a byte reverse upon the salt)
-- -- N/A
-- system_salt_modify_overwrite (performs a byte replacement upon the salt with specified values and indexes)
-- -- The first parameter is a table of byte indexes and bytes/1-character strings to overwrite with. The behavior if the index doesn't exist is to ignore the value.
-- system_salt_modify_add (performs an addition on each byte of the salt with a specified value)
-- -- The first parameter indicates what to add to each byte
-- system_salt_modify_subtract (performs a subtraction on each byte of the salt with a specified value)
-- -- The first parameter indicates what to subtract from each byte
-- system_salt_modify_multiply (performs a multiplication on each byte of the salt with a specified value)
-- -- The first parameter indicates what to multiply each byte with
-- system_salt_modify_divide (performs a division on each byte of the salt with a specified value)
-- -- The first parameter indicates what to divide each byte with
-- system_salt_modify_append (performs a static append of salt bytes with configurable values)
-- -- The first parameter is the salt bytes/string to append
-- system_salt_modify_prepend (performs a static prepend of salt bytes with configurable values)
-- -- The first parameter is the salt bytes/string to prepend
-- system_salt_modify_intersperse (performs a static intersperse of salt bytes with configurable values)
-- -- The first parameter is the bytes/string to intersperse the salt with
-- -- The second parameter is how many bytes of the salt to take before adding a byte
-- -- The third parameter is how many bytes of the salt to skip before starting to add bytes
-- -- The fourth parameter is the salt leftover policy (system_salt_leftover_discard, system_salt_leftover_append, system_salt_leftover_prepend, system_salt_leftover_rollover; default system_salt_leftover_discard)

-- For example, if you wanted 2 system_salt_modify_reverse ciphers, it would look like:
-- ["salt_modify"] = {
--     makePolicy(system_salt_modify_reverse),
--     makePolicy(system_salt_modify_reverse),
-- },

function makePolicy(policyType, args)
	if args == nil then
		args = { };
	end
	if type(args) ~= "table" then
		args = { args };
	end
	return {
		["type"] = policyType,
		["args"] = args,
	};
end

-- Changes to account salt policies require resetting all salts and passwords in the database
account = {
	["salt"] = makePolicy(system_salt_prepend),
	["salt_modify"] = { },
	-- How large the generated salts should be
	-- A value of 0 uses a predefined random size
	-- You may also specify a min/max table to indicate a random size range (e.g. { ["min"] = 5, ["max"] = 10 })
	-- Changing this value requires no updates, it only affects new salts
	["salt_size"] = 0,
};

-- Changes to interserver salting policies requires a restart of all servers
interserver = {
	["salt"] = makePolicy(system_salt_prepend),
	["salt_modify"] = { },
};