-- game.currentlevel = "data/maps/mainmenu.map"
game.currentlevel = "maps/mainmenu.map"
map:load(game.currentlevel)
map:setflag("updateoffscreen", false)

mainfont = game:createfont("SmallFont")

game.lives = 5
game.score = 0
game.coins = 0
