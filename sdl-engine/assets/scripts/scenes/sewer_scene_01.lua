scene = {
    -- Sprite table
    sprites = {
        [0] =
        {asset_id = "cursor", file_path = "./assets/ui/cursors/cursor.png"},
        {asset_id = "player", file_path = "./assets/sprites/characters/player/player_sprite_sheet.png"},
        {asset_id = "revolver", file_path = "./assets/sprites/weapons/revolver.png"},
        {asset_id = "terrain_sewers", file_path = "./assets/sprites/tiles/sewers.png"}
    },

    -- Font table
    fonts = {
        [0] = 
        {font_id = "pixel_sleigh_24", file_path = "./assets/fonts/PixelSleigh.ttf", font_size = 24}
    },

    -- Key & action table
    keys = {
        [0] = 
        {name = "move_up", key = 119},
        {name = "move_left", key = 97},
        {name = "move_down", key = 115},
        {name = "move_right", key = 100}
    },

    -- Mouse buttons & actions table
    buttons = {
        [0] = 
        {name = "LMB", button = 1}
    },

    -- Map table
    maps = {
        map_path = "./assets/maps/sewers_01.tmx",
        tile_path = "./assets/maps/sewers.tsx",
        tile_name = "terrain_sewers"
    },

    -- Entity table
    entities = {
        [0] =
        { -- cursor
            components = {
                sprite = {assetId = "cursor", width = 4, height = 4, src_rect = {x = 0, y = 0}, z_index = 2, pivot = {x = 0, y = 0}, flip = false},
                cursor = {}
            }
        },
        { -- player
            name = "player",
            components = {
                animation = {
                    clips = {
                        idle = {row = 0, num_frames = 4, animation_speed = 9, loop = true},
                        walk = {row = 1, num_frames = 6, animation_speed = 12, loop = true}
                    }
                },
                script  = {path = "./assets/scripts/player.lua"},
                camera_follow = {},
                box_collider = {width = 12, height = 16, offset = {x = 9.5, y = 7.5}},
                rigidbody = {is_dynamic = false, is_solid = true, mass = 1.0},
                sprite = {assetId = "player", width = 30, height = 30, src_rect = {x = 0, y = 0}, z_index = 9, pivot = {x = 0, y = 0}, flip = true},
                transform = {position = {x = 360.0, y = 160.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
            }
        },
        { -- revolver
            name = "revolver",
            components = {
                sprite = {assetId = "revolver", width = 9, height = 8, src_rect = {x = 0, y = 0}, z_index = 10, pivot = {x = 1, y = 4}, flip = false},
                transform = {position = {x = 0.0, y = 0.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0},
                child_of = {parent = "player", offset = {x = 17.0, y = 18.0}},
                mouse_follow = {{origin_pivot = {x = 1, y = 4}}}
            }
        }
    }
}
