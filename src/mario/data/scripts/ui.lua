function printLives( x, y )
  game:print(mainfont, "Lives  " .. game.lives , screen:camerax() + x, screen:cameray() + y, 0.2, -0.2)  
end

function printCoins( x, y )
  game:print(mainfont, "Coins  " .. game.coins , screen:camerax() + x, screen:cameray() + y, 0.2, -0.2)  
end

function printScore( x, y )
  game:print(mainfont, "Score  " .. game.score , screen:camerax() + x, screen:cameray() + y, 0.2, -0.2)  
end
