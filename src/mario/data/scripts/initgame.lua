-- game.currentlevel = "maps/mainmenu.json"
game.currentlevel = "maps/level1.json"
map:load(game.currentlevel)
map:setflag("updateoffscreen", false)

mainfont = game:createfont("SmallFont")

game.lives = 5
game.score = 0
game.coins = 0
