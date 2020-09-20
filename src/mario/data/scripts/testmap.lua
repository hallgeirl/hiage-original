function initMap()
  screen:zoom(200)
  screen:camera(0,0)
  map:spawn("MarioBig", 50,200)
  --mario = map.objects[2]
  mario = findObject("MarioBig")
  --disable input
  mario.input = false
  maptimer = Timer(true)
  mario:velx(5)
  goomba = findObject("Goomba")
end

function updateMap()
--  screen:zoom(150+50*math.sin(gametimer:get()))
  if maptimer:reached(1) and not maptimer:reached(1.5) then
    if not mario.inair then
      jump(mario, 300)
    else
      mario:accel(800,1,0)
    end
  end
  
  mario.maxspeed = 150
  screen:camera(mario:x(), mario:y())

  if maptimer:reached(2) and not maptimer:reached(3) then
    mario:accel(1000, 1, 0)
  end

  if maptimer:reached(3.5) and mario:x() > goomba:x() then
    mario:accel(2000, -1, 0)
  end

  if maptimer:reached(3.5) and mario:x() < goomba:x() then
    mario:accel(2000, 1, 0)
  end

  if maptimer:reached(4.1) and not maptimer:reached(4.2) and not mario.inair then
    jump(mario, 500)
  end

  if maptimer:reached(5.3) and not maptimer:reached(6.1) then
    game:timefactor(6.15-maptimer:get())
  end

  if maptimer:reached(4.6) and not maptimer:reached(6) then
    screen:zoom(200*(2.72^(4.5-maptimer:get())))
  end

  if screen:camerax() < (screen:zoom() * screen:aspect()) then
    screen:camera(screen:zoom() * screen:aspect(), screen:cameray())
  end

  if screen:cameray() < screen:zoom() then
    screen:camera(screen:camerax(), screen:zoom())
  end
end
