local H = wesnoth.require "lua/helper.lua"
local AH = wesnoth.require "ai/lua/ai_helper.lua"

local function get_enemies(cfg)
    local scatter_distance = cfg.scatter_distance or 3
    local enemies = wesnoth.get_units {
        { "filter_side", {{"enemy_of", {side = wesnoth.current.side} }} },
        { "filter_location",
            { radius = scatter_distance, { "filter", { side = wesnoth.current.side } } }
        }
    }
    return enemies
end

local function get_swarm_units(cfg)
    local units = {}
    local all_units = wesnoth.get_units { side = wesnoth.current.side }
    for i,u in ipairs(all_units) do
        if (u.moves > 0) then table.insert(units, u) end
    end
    return units
end

local swarm_scatter = {}

function swarm_scatter:evaluation(ai, cfg)
    if (not get_enemies(cfg)[1]) then return 0 end
    if (not get_swarm_units(cfg)[1]) then return 0 end
    return cfg.ca_score
end

function swarm_scatter:execution(ai, cfg)
    local enemies = get_enemies(cfg)
    local units = get_swarm_units(cfg)
    local vision_distance = cfg.vision_distance or 12

    -- In this case we simply maximize the distance from all these close enemies
    -- but only for units that are within 'vision_distance' of one of those enemies
    for i,unit in ipairs(units) do
        local unit_enemies = {}
        for i,e in ipairs(enemies) do
            if (H.distance_between(unit.x, unit.y, e.x, e.y) <= vision_distance) then
                table.insert(unit_enemies, e)
            end
        end

        if unit_enemies[1] then
            local best_hex = AH.find_best_move(unit, function(x, y)
                local rating = 0
                for i,e in ipairs(unit_enemies) do
                    rating = rating + H.distance_between(x, y, e.x, e.y)
                end
                return rating
            end)

            AH.movefull_stopunit(ai, unit, best_hex)
        end
    end
end

return swarm_scatter
