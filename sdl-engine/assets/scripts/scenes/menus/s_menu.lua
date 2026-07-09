scene = {
    -- Sprite table
    sprites = {
        [0] =
        {asset_id = "cursor", file_path = "./assets/ui/cursors/cursor.png"}
    },

    -- Font table
    fonts = {
        [0] = 
        {font_id = "pixel_sleigh_18", file_path = "./assets/fonts/PixelSleigh.ttf", font_size = 18},
        {font_id = "pixel_sleigh_24", file_path = "./assets/fonts/PixelSleigh.ttf", font_size = 24},
        {font_id = "pixel_sleigh_32", file_path = "./assets/fonts/PixelSleigh.ttf", font_size = 32}
    },

    -- Key & action table
    keys = {},

    -- Mouse buttons & actions table
    buttons = {
        [0] = 
        {name = "LMB", button = 1}
    },

    -- Map table
    maps = {
        width = 800,
        height = 600
    },

    -- Entity table
    entities = {
        [0] = 
        { -- cursor
            components = {
                sprite = {assetId = "cursor", width = 4, height = 4, src_rect = {x = 0, y = 0}, flip = false},
                cursor = {}
            }
        },
        { -- Game title
            components = {
                text = {text = "Warped Grace", font_id = "pixel_sleigh_32", r = 255, g = 255, b = 255, a = 255, outline_thickness = 3, is_ui = true},
                transform = {position = {x = 40.0, y = 120.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
            }
        },
        { -- Start 
            components = {
                clickable = {width = 210, height = 55, offset_x = 0, offset_y = 0},
                script = {path = "./assets/scripts/menuScripts/scr_menu_button.lua"},
                text = {text = "Start game", font_id = "pixel_sleigh_24", r = 255, g = 255, b = 255, a = 255, outline_thickness = 3, is_ui = true},
                transform = {position = {x = 40.0, y = 200.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0},
                tag = {tag = "t_start_button"}
            }
        },
        { -- Controls
            components = {
                clickable = {width = 210, height = 55, offset_x = 0, offset_y = 0},
                script = {path = "./assets/scripts/menuScripts/scr_menu_button.lua"},
                text = {text = "Controls", font_id = "pixel_sleigh_24", r = 255, g = 255, b = 255, a = 255, outline_thickness = 3, is_ui = true},
                transform = {position = {x = 40.0, y = 280.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0},
                tag = {tag = "t_controls_button"}
            }
        },
        { -- Exit
            components = {
                clickable = {width = 120, height = 55, offset_x = 0, offset_y = 0},
                script = {path = "./assets/scripts/menuScripts/scr_menu_button.lua"},
                text = {text = "Exit", font_id = "pixel_sleigh_24", r = 255, g = 255, b = 255, a = 255, outline_thickness = 3, is_ui = true},
                transform = {position = {x = 40.0, y = 360.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0},
                tag = {tag = "t_exit_button"}
            }
        },
        { -- Version
            components = {
                script = {path = "./assets/scripts/menuScripts/scr_menu_button.lua"},
                text = {text = "Version 0.6", font_id = "pixel_sleigh_18", r = 255, g = 255, b = 255, a = 255, outline_thickness = 3, is_ui = true},
                transform = {position = {x = 620.0, y = 560.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
            }
        }
    }
}

function scene.start()
    play_music("assets/soundEffects/misc/music/track_01.wav", -1, 20)
end