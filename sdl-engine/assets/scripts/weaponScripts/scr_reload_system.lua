local bullet_entity   = dofile("./assets/scripts/entities/e_reload_bullet.lua")
local drop_zone_entity = dofile("./assets/scripts/entities/e_bullet_drop_zone.lua")
local starting_ammo = 3

local bullets      = {}
local bullet_homes = {}
local drop_zones   = {}
local all_bullets  = {}
local all_zones    = {}

local GRID_COLS     = 6
local GRID_ROWS     = 2
local CELL_WIDTH    = 54
local CELL_HEIGHT   = 88
local GRID_ORIGIN_X = 190
local GRID_ORIGIN_Y = 401

local ZONE_COUNT    = 1
local ZONE_SPACING  = 200
local ZONE_ORIGIN_X = 566
local ZONE_ORIGIN_Y = 342
local ZONE_SLOTS    = 6
local ZONE_RADIUS   = 78

local RELOAD_UI_X   = 545.0
local RELOAD_UI_Y   = 280.0

local cylinder_index = 1 -- current chamber position; advances every shot like a real cylinder
local menu_initialized = false
local saved_bullet_positions = {}

function start()
    GameState.reload_menu_open = false
    GameState.drop_state = "idle"
    GameState.dropped_bullet = nil
    GameState.spend_casing = spend_casing
    GameState.set_reload_menu = set_reload_menu
    GameState.add_ammo = add_ammo
    set_position(this, -1000, -1000)
end

function save_bullet_positions()
    saved_bullet_positions = {}
    for _, bullet in ipairs(all_bullets) do
        local x, y = get_position(bullet)
        saved_bullet_positions[bullet] = { x = x, y = y }
    end
end

function restore_bullet_positions()
    for _, bullet in ipairs(all_bullets) do
        local pos = saved_bullet_positions[bullet]
        if pos then
            set_position(bullet, pos.x, pos.y)
        end
    end
end

function hide_reload_entities()
    save_bullet_positions()
    for _, bullet in ipairs(all_bullets) do
        set_position(bullet, -10000, -10000)
    end
    for _, zone in ipairs(all_zones) do
        set_position(zone, -10000, -10000)
    end
    set_position(this, -1000, -1000)
end

function show_reload_entities()
    print("[RELOAD] showing frame at " .. RELOAD_UI_X .. "," .. RELOAD_UI_Y)
    set_position(this, RELOAD_UI_X, RELOAD_UI_Y)
    restore_bullet_positions()
    for i, zone in ipairs(all_zones) do
        local cfg = GameState.zones and GameState.zones[i]
        if cfg then
            set_position(zone, cfg.x, cfg.y)
        end
    end
end

function set_reload_menu(open)
    if open then
        if GameState.reload_menu_open then
            return
        end
        GameState.reload_menu_open = true
        if not menu_initialized then
            spawn_drop_zones(ZONE_COUNT)
            place_bullets(starting_ammo)
            menu_initialized = true
        end
        show_reload_entities()
        print("[RELOAD] menu opened")
    else
        if not GameState.reload_menu_open then
            return
        end
        GameState.reload_menu_open = false
        hide_reload_entities()
        GameState.dropped_bullet = nil
        GameState.drop_state = "idle"
        print("[RELOAD] menu closed")
    end
end

function spawn_drop_zones(n)
    drop_zones = {}
    GameState.zone_config_queue = {}
    for i = 1, n do
        table.insert(GameState.zone_config_queue, {
            x          = ZONE_ORIGIN_X + (i - 1) * ZONE_SPACING,
            y          = ZONE_ORIGIN_Y,
            slot_count = ZONE_SLOTS,
            radius     = ZONE_RADIUS
        })
    end
    for i = 1, n do
        local zone = spawn_entity(drop_zone_entity)
        table.insert(drop_zones, zone)
        table.insert(all_zones, zone)
    end
end

function place_bullets(n)
    bullets      = {}
    bullet_homes = {}
    local count = math.min(n, GRID_COLS * GRID_ROWS)

    for i = 0, count - 1 do
        local col = i % GRID_COLS
        local row = math.floor(i / GRID_COLS)
        local x = GRID_ORIGIN_X + col * CELL_WIDTH
        local y = GRID_ORIGIN_Y + row * CELL_HEIGHT
        local bullet = spawn_entity(bullet_entity)
        set_position(bullet, x, y)
        table.insert(bullets, bullet)
        table.insert(bullet_homes, { x = x, y = y })
        table.insert(all_bullets, bullet)
    end
end

function append_bullets(n)
    local start_index = #bullets
    for i = 0, n - 1 do
        local grid_i = start_index + i
        local col = grid_i % GRID_COLS
        local row = math.floor(grid_i / GRID_COLS)
        local x = GRID_ORIGIN_X + col * CELL_WIDTH
        local y = GRID_ORIGIN_Y + row * CELL_HEIGHT
        local bullet = spawn_entity(bullet_entity)

        -- Register home position into saved state directly
        saved_bullet_positions[bullet] = { x = x, y = y }
        bullet_homes[grid_i + 1] = { x = x, y = y }

        -- Spawn hidden; show_reload_entities will place them correctly when menu opens
        if GameState.reload_menu_open then
            set_position(bullet, x, y)
        else
            set_position(bullet, -10000, -10000)
        end

        table.insert(bullets, bullet)
        table.insert(bullet_homes, { x = x, y = y })
        table.insert(all_bullets, bullet)
    end
end

function find_home(bullet)
    for i, b in ipairs(bullets) do
        if b == bullet then
            return bullet_homes[i]
        end
    end
    return nil
end

function reorder_grid()
    for i = 0, #bullets - 1 do
        local col = i % GRID_COLS
        local row = math.floor(i / GRID_COLS)
        local x = GRID_ORIGIN_X + col * CELL_WIDTH
        local y = GRID_ORIGIN_Y + row * CELL_HEIGHT
        bullet_homes[i + 1] = { x = x, y = y }
        saved_bullet_positions[bullets[i + 1]] = { x = x, y = y }
        if GameState.reload_menu_open then
            set_position(bullets[i + 1], x, y)
        end
    end
end

function remove_bullet_from_grid(bullet)
    for i, b in ipairs(bullets) do
        if b == bullet then
            table.remove(bullets, i)
            table.remove(bullet_homes, i)
            return
        end
    end
end

function find_slot_for_bullet(bullet)
    if GameState.zones == nil then return nil end
    for _, zone in ipairs(GameState.zones) do
        for _, slot in ipairs(zone.slots) do
            if slot.bullet == bullet then
                return slot, zone
            end
        end
    end
    return nil, nil
end

function find_slot_index(zone, slot)
    for i, s in ipairs(zone.slots) do
        if s == slot then
            return i
        end
    end
    return nil
end

function unslot_bullet(bullet)
    local slot, zone = find_slot_for_bullet(bullet)
    if slot then
        slot.occupied = false
        slot.spent = false
        slot.bullet = nil
        if GameState.slotted_bullets then
            GameState.slotted_bullets[bullet] = nil
        end
        print("[RELOAD] unslotted bullet from slot")
    end
end

function spend_casing()
    if GameState == nil or GameState.zones == nil then
        print("[RELOAD] Cylinder not loaded - open reload menu to load bullets")
        return false
    end

    local zone = GameState.zones[1]
    local slot = zone and zone.slots[cylinder_index]
    local fired = false

    if slot and slot.occupied and not slot.spent then
        mark_slot_spent(1, cylinder_index)
        print("[RELOAD] Fired slot #" .. cylinder_index .. "!")
        fired = true
    else
        print("[RELOAD] Click — slot #" .. cylinder_index .. " is empty or spent!")
    end

    cylinder_index = (cylinder_index % ZONE_SLOTS) + 1
    return fired
end

function update()
    if not initialized then
        initialized = true
        return
    end

    if not GameState.reload_menu_open then
        return
    end

    if #GameState.zones < ZONE_COUNT then
        print("[RELOAD] waiting for zones: " .. #GameState.zones .. "/" .. ZONE_COUNT)
        return
    end

    if GameState.drop_state ~= "pending" or GameState.dropped_bullet == nil then
        return
    end

    local dropped = GameState.dropped_bullet

    if GameState.slotted_bullets and GameState.slotted_bullets[dropped] then
        unslot_bullet(dropped)
    end

    local bx, by = get_position(dropped)
    local bcx = bx + 48
    local bcy = by + 48
    print("[RELOAD] drop at bcx=" .. bcx .. " bcy=" .. bcy)

    local accepted = false
    for _, zone in ipairs(GameState.zones) do
        print("[RELOAD] checking zone x=" .. zone.x .. " y=" .. zone.y .. " w=" .. zone.w .. " h=" .. zone.h)
        if bcx >= zone.x and bcx <= zone.x + zone.w
        and bcy >= zone.y and bcy <= zone.y + zone.h then

            local best_slot, best_dist = nil, math.huge
            for _, slot in ipairs(zone.slots) do
                if not slot.occupied then
                    local dx = slot.x - bcx
                    local dy = slot.y - bcy
                    local dist = math.sqrt(dx * dx + dy * dy)
                    if dist < best_dist then
                        best_dist = dist
                        best_slot = slot
                    end
                end
            end

            if best_slot then
                set_position(dropped, best_slot.x - 48, best_slot.y - 48)
                best_slot.occupied = true
                best_slot.spent = false
                best_slot.bullet = dropped
                accepted = true
                play_animation(best_slot.bullet, "full")

                if GameState.slotted_bullets == nil then
                    GameState.slotted_bullets = {}
                end
                GameState.slotted_bullets[dropped] = true

                cylinder_index = 1

                remove_bullet_from_grid(dropped)
                reorder_grid()
            end
            break
        end
    end

    if not accepted then
        local home = find_home(dropped)
        print("[RELOAD] accepted=" .. tostring(accepted) .. " home=" .. tostring(home))
        if home then
            set_position(dropped, home.x, home.y)
        end
    end

    GameState.dropped_bullet = nil
    GameState.drop_state = "idle"

end

function mark_slot_spent(zone_index, slot_index)
    if GameState.zones == nil then 
        return 
    end

    local zone = GameState.zones[zone_index]
    if zone == nil then 
        return 
    end

    local slot = zone.slots[slot_index]
    if slot == nil or not slot.occupied then 
        return 
    end

    slot.spent = true
    play_animation(slot.bullet, "empty")
    print("[RELOAD] slot " .. slot_index .. " in zone " .. zone_index .. " marked spent")
end

function add_ammo(n)
    if not menu_initialized then
        spawn_drop_zones(ZONE_COUNT)
        place_bullets(starting_ammo)
        menu_initialized = true
        hide_reload_entities()
    end

    local max = GRID_COLS * GRID_ROWS
    local available = max - #bullets
    local to_spawn = math.min(n, available)

    if to_spawn > 0 then
        append_bullets(to_spawn)
        reorder_grid()
    end
end


function clear_bullets()
    for _, bullet in ipairs(all_bullets) do
        delete_entity(bullet)
    end
    bullets      = {}
    bullet_homes = {}
    all_bullets  = {}
    GameState.slotted_bullets = {}
end

function clear_zones()
    for _, zone in ipairs(all_zones) do
        delete_entity(zone)
    end
    drop_zones      = {}
    all_zones       = {}
    GameState.zones = {}
end