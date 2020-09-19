function initMap()
  screen:zoom(150)
  map.maptimer = Timer(true)
  map.currentitem = 1
end

function updateMap()
  -- Print out the menu alternatives and the header
  game:print(mainfont, "Retry level?", screen:camerax()-100, screen:cameray() + 50, 0.5, -0.2)
  game:print(mainfont, "Yes!", screen:camerax()-40, screen:cameray() + 20, 0.3, -0.2)
  game:print(mainfont, "No!", screen:camerax()-40, screen:cameray() + 0, 0.3, -0.2)
  
  if map.currentitem == 1 then
    game:drawtexture("MenuCursor", screen:camerax() - 60, screen:cameray() + 28)
  elseif map.currentitem == 2 then
    game:drawtexture("MenuCursor", screen:camerax() - 60, screen:cameray() + 8)
  end
  
  if input:keydown(274) then
    map.currentitem = 2
  elseif input:keydown(273) then
    map.currentitem = 1
  end
  
  if input:keydown(13) or input:keydown(32) then
    if map.currentitem == 1 then
      map:load(game.currentlevel)
    elseif map.currentitem == 2 then
      map:load("data/maps/mainmenu.map")
    end
  end
  screen:camera(screen:zoom() * screen:aspect(), screen:zoom())
end
