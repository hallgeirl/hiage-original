-- game.currentlevel = "maps/mainmenu.json"
-- game.currentlevel = "maps/testmap.json"
game.currentlevel = "maps/level2.json"
map:load(game.currentlevel)
map:setflag("updateoffscreen", false)

mainfont = game:createfont("SmallFont")
