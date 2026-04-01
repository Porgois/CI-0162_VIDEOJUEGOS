scene = {
    -- Sprite table
    sprites = {
        [0] =
        {assetId = "cursor", file_path = "./assets/ui/cursors/cursor.png"},
        {assetId = "player", file_path = "./assets/sprites/characters/player/player_sprite_sheet.png"},
        {assetId = "player_ship", file_path = "./assets/sprites/enviroment/barrel_sprite.png"}
    },

    -- TODO: Font table


    -- Key & action table
    keys = {
        [0] = 
        {name = "move_up", key = 119},
        {name = "move_left", key = 97},
        {name = "move_down", key = 115},
        {name = "move_right", key = 100}
    },

    -- TODO: Mouse buttons & actions table

    -- Entity table
    entities = {
        [0] = -- cursor
        {
            components = {
                sprite = {assetId = "cursor", width = 4, height = 4, src_rect = {x = 0, y = 0}, flip = false},
                cursor = {}
            }
        },
        [1] = -- player
        {
            components = {
                animation = {
                    clips = {
                        idle = {row = 0, num_frames = 4, animation_speed = 8, loop = true},
                        walk = {row = 1, num_frames = 6, animation_speed = 13, loop = true}
                    }
                },
                script  = {path = "./assets/scripts/player.lua"},
                collider = {},
                circle_collider = {radius = 5, width = 30, height = 30},
                rigidbody = {velocity = {x = 0, y = 0}},
                sprite = {assetId = "player", width = 30, height = 30, src_rect = {x = 0, y = 0}, flip = true},
                transform = {position = {x = 325.0, y = 215.0}, scale = {x = 3.0, y = 3.0}, rotation = 0.0}
            }
        }
    }
}