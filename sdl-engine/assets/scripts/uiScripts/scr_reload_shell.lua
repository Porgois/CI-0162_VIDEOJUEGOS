local is_held = false
local is_ejecting = false
local eject_speed = 12
local offscreen_pos = 900
local eject_play_delay_time = 0.75

-- shell drop sfx delay
local shell_sound_pending = false
local shell_sound_timer = 0.0

function eject_bullet(slot)
    if slot then
        slot.bullet = nil
        slot.occupied = false
        slot.spent = false
    end

    local slotted = GameState.revolver_reload_menu_open and GameState.revolver_slotted_bullets or GameState.shotgun_slotted_bullets
    if slotted then
        slotted[this] = nil
    end

    is_ejecting = true
    play_animation(this, "eject")

    -- Delay the shell drop sound instead of playing it immediately
    shell_sound_pending = true
    shell_sound_timer = eject_play_delay_time

    print("[RELOAD SHELL] Ejecting spent bullet")
end

function on_click()
    -- Slotted bullets cannot be picked up (only spent ones are removed by clicking)
    local slotted = GameState.revolver_reload_menu_open and GameState.revolver_slotted_bullets or GameState.shotgun_slotted_bullets
    if slotted and slotted[this] then
        local zones = GameState.revolver_reload_menu_open and GameState.revolver_zones or GameState.shotgun_zones
        if zones then
            for _, zone in ipairs(zones) do
                for _, slot in ipairs(zone.slots) do
                    if slot.occupied and slot.bullet == this and slot.spent then
                        eject_bullet(slot)
                        return
                    end
                end
            end
        end
        print("[RELOAD BULLET] Cannot pick up unspent slotted bullet")
        return
    end

    -- Backup check: bullet is in a slot but not in slotted_bullets table
    local zones = GameState.revolver_reload_menu_open and GameState.revolver_zones or GameState.shotgun_zones
    if zones then
        for _, zone in ipairs(zones) do
            for _, slot in ipairs(zone.slots) do
                if slot.occupied and slot.bullet == this then
                    if slot.spent then
                        play_audio("assets/soundEffects/weapons/shotgun/shell_eject.wav")
                        eject_bullet(slot)
                    else
                        print("[RELOAD BULLET] Cannot pick up unspent bullet occupying a slot")
                    end
                    return
                end
            end
        end
    end

    -- Grab
    is_held = true
    GameState.drop_state = "idle"
    play_audio("assets/soundEffects/weapons/shotgun/shell_grab.wav")
end

function update()
    -- Tick the delayed shell-drop sound regardless of ejecting/held state
    if shell_sound_pending then
        shell_sound_timer = shell_sound_timer - get_delta_time()
        if shell_sound_timer <= 0.0 then
            play_audio("assets/soundEffects/weapons/shotgun/shell_drop.wav")
            shell_sound_pending = false
        end
    end

    if is_ejecting then
        local x, y = get_position(this)
        local new_y = y + eject_speed
        set_position(this, x, new_y)

        -- Don't delete the entity until the delayed sound has had a chance to play
        if new_y > offscreen_pos and not shell_sound_pending then
            delete_entity(this)
        end
        return
    end

    if is_held then
        if is_button_pressed("lmb") then
            local mx, my = get_mouse_position()
            set_position(this, mx - 48, my - 48)
            play_animation(this, "grab")
        else
            is_held = false
            GameState.dropped_bullet = this
            GameState.drop_state = "pending"
            play_animation(this, "default")
        end
    end
end