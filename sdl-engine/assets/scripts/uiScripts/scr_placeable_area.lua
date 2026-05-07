function start()
    print("[DROP ZONE] start() called, queue size: " .. #GameState.zone_config_queue)
    if GameState.zone_config_queue == nil or #GameState.zone_config_queue == 0 then
        print("[DROP ZONE] queue empty, returning")
        return
    end

    local cfg = table.remove(GameState.zone_config_queue, 1)
    set_position(this, cfg.x, cfg.y)

    local w, h = get_collider_size(this)
    print("[DROP ZONE] collider size (scaled): w=" .. w .. " h=" .. h)

    local cx     = cfg.x + w / 2
    local cy     = cfg.y + h / 2
    local radius = cfg.radius or 55
    print("[DROP ZONE] center: cx=" .. cx .. " cy=" .. cy .. " radius=" .. radius)

    local slot_positions = {}
    local SLOT_COUNT     = cfg.slot_count or 6

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

    if GameState.zones == nil then GameState.zones = {} end
    table.insert(GameState.zones, {
        x     = cfg.x,
        y     = cfg.y,
        w     = w,
        h     = h,
        slots = slot_positions
    })
    print("[DROP ZONE] registered zone x=" .. cfg.x .. " y=" .. cfg.y .. " w=" .. w .. " h=" .. h)
end