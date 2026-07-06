function start()
    local zone_config_queue = nil
    if GameState.revolver_reload_menu_open then
        zone_config_queue = GameState.revolver_zone_config_queue
    elseif GameState.shotgun_reload_menu_open then
        zone_config_queue = GameState.shotgun_zone_config_queue
    end

    print("[DROP ZONE] start() called, queue size: " .. tostring(zone_config_queue and #zone_config_queue or 0))
    if zone_config_queue == nil or #zone_config_queue == 0 then
        print("[DROP ZONE] queue empty, returning")
        return
    end

    local cfg = table.remove(zone_config_queue, 1)
    local w, h = get_collider_size(this)
    if cfg.w and cfg.h then
        w = cfg.w
        h = cfg.h
    end

    if cfg.layout == "horizontal" then
        set_position(this, cfg.x - w / 2, cfg.y - h / 2)
    else
        set_position(this, cfg.x, cfg.y)
    end

    local x, y = get_position(this)
    print("[DROP ZONE] collider size (scaled): w=" .. w .. " h=" .. h)

    local cx = x + w / 2
    local cy = y + h / 2
    local radius = cfg.radius or 55
    print("[DROP ZONE] center: cx=" .. cx .. " cy=" .. cy .. " radius=" .. radius)

    local slot_positions = {}
    local SLOT_COUNT     = cfg.slot_count or 6
    local layout         = cfg.layout or "circle"

    if layout == "horizontal" then
        local slot_gap = cfg.slot_gap or cfg.slot_spacing or 48
        local total_width = (SLOT_COUNT - 1) * slot_gap
        local start_x = cx - total_width / 2
        for i = 1, SLOT_COUNT do
            slot_positions[i] = {
                x        = start_x + (i - 1) * slot_gap,
                y        = cy,
                occupied = false,
                spent    = false,
                bullet   = nil
            }
            print("[DROP ZONE] slot " .. i .. ": x=" .. slot_positions[i].x .. " y=" .. slot_positions[i].y)
        end
    else
        for i = 1, SLOT_COUNT do
            local angle = (2 * math.pi / SLOT_COUNT) * (i - 1) - (math.pi / 2)
            slot_positions[i] = {
                x        = cx + math.cos(angle) * radius,
                y        = cy + math.sin(angle) * radius,
                occupied = false,
                spent    = false,
                bullet   = nil
            }
            print("[DROP ZONE] slot " .. i .. ": x=" .. slot_positions[i].x .. " y=" .. slot_positions[i].y)
        end
    end

    local target_zones = nil
    if GameState.revolver_reload_menu_open then
        if GameState.revolver_zones == nil then GameState.revolver_zones = {} end
        target_zones = GameState.revolver_zones
    elseif GameState.shotgun_reload_menu_open then
        if GameState.shotgun_zones == nil then GameState.shotgun_zones = {} end
        target_zones = GameState.shotgun_zones
    end

    if target_zones == nil then
        if GameState.revolver_zones == nil then GameState.revolver_zones = {} end
        if GameState.shotgun_zones == nil then GameState.shotgun_zones = {} end
        target_zones = GameState.revolver_zones
    end

    table.insert(target_zones, {
        x     = x,
        y     = y,
        w     = w,
        h     = h,
        slots = slot_positions
    })
    print("[DROP ZONE] registered zone x=" .. cfg.x .. " y=" .. cfg.y .. " w=" .. w .. " h=" .. h)

    if GameState.reload_menu_open and GameState.pending_revolver_restore and GameState.restore_cylinder_state then
        print("[DROP ZONE] restoring revolver cylinder immediately after zone registration")
        GameState.restore_cylinder_state()
        GameState.pending_revolver_restore = false
    end

    if GameState.reload_menu_open and GameState.pending_shotgun_restore and GameState.restore_shotgun_state then
        print("[DROP ZONE] restoring shotgun barrel immediately after zone registration")
        GameState.restore_shotgun_state()
        GameState.pending_shotgun_restore = false
    end
end