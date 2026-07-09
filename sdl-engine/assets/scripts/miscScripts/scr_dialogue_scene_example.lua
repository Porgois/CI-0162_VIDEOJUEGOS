function start()
    start_dialogue(this, {
        "It began with a blinding flash\nof light.",
        "Plantlife withered, rivers and lakes\n dried up, and whatever water remained\nturned undrinkable.",
        "The land itself became inhospitable.",
        "Those most heavily exposed to it\nwarped and twisted to grotesque,\nnearly-unrecognizable forms.",
        "It wasn't long until hope dwindled...",
        "That was, until a rumour began to spread;\na rumour that eventually\nreached the elder's ears:\n",
        "\"Find it, speak your heart's truth and your plight will be heard.\"",
        "With nothing left to lose, your clan\nembarked on a pilgrimage to its lair.\n",
        "Many perished on the journey,\n leaving only you and a few others alive.",
        "There came a point where that number\nlowered to one.",
        "Your group was ambushed\nby mutants in the woods...",
        "You abandoned them and sought\nrefuge in a nearby sewer system.",
        "The search must continue.",
        "There is nothing to go back to."
        
    }, "assets/soundEffects/misc/generic_text_sound.wav", 150, 0.08, 43)
end

function on_click()
    advance_dialogue(this)
end
