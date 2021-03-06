local AH = wesnoth.require "ai/lua/ai_helper.lua"

local function get_guardian(cfg)
    local filter = cfg.filter or { id = cfg.id }
    local guardian = wesnoth.get_units({
        side = wesnoth.current.side,
        { "and", filter },
        formula = '$this_unit.moves > 0' }
    )[1]
    return guardian
end

local ca_return_guardian = {}

function ca_return_guardian:evaluation(ai, cfg)
    local guardian = get_guardian(cfg)
    if guardian then
        if ((guardian.x ~= cfg.return_x) or (guardian.y ~= cfg.return_y)) then
            return cfg.ca_score
        else
            return cfg.ca_score - 20
        end
    end
    return 0
end

function ca_return_guardian:execution(ai, cfg)
    local guardian = get_guardian(cfg)

    -- In case the return hex is occupied:
    local x, y = cfg.return_x, cfg.return_y
    if (guardian.x ~= x) or (guardian.y ~= y) then
        x, y = wesnoth.find_vacant_tile(x, y, guardian)
    end

    local nh = AH.next_hop(guardian, x, y)
    AH.movefull_stopunit(ai, guardian, nh)
end

return ca_return_guardian
