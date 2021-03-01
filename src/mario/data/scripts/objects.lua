-- Main Object script file - Defines the actions of all the in-game objects (enemies, player, blocks etc.)

key_space = 44
key_right = 79
key_left = 80
key_shift = 225

key_a = 4



timedTexts = {}

-- Construct some text that disappears when some time has passed
function TimedText ( text, lifetime, x, y, vx, vy )
  local timedText = {}
  timedText.text = text
  timedText.timer = Timer(true)
  timedText.lifetime = lifetime
  timedText.x = x
  timedText.y = y
  timedText.vx = vx
  timedText.vy = vy
  
  return timedText
end

function updateTimedTexts ()
  for i, _ in timedTexts do
    game:print(mainfont, timedTexts[i].text, timedTexts[i].x, timedTexts[i].y, 0.2, -0.2)
    timedTexts[i].x = timedTexts[i].x + timedTexts[i].vx * frametime
    timedTexts[i].y = timedTexts[i].y + timedTexts[i].vy * frametime
    if timedTexts[i].timer:reached(timedTexts[i].lifetime) then
      table.remove(timedTexts, i)
    end
  end
end

-- Common update handlers for many objects

-- Apply physics to the object
function applyPhysics( o )
  -- Destroy monsters that has falled down
  if o:y() < -250 and o:type() ~= "Mario" then
    map:destroy(o.id)
  end
end

-- Limit the speed to the maximum speed
function limitSpeed ( o )
  if o:x() < screen:camerax() - (screen:zoom() * screen:aspect()) - 32 then
    if o:type() == "Shell" and o.moving then
      map:destroy(o.id)
    end
    o:velx(0)
  elseif o:x() > screen:camerax() + (screen:zoom() * screen:aspect()) + 32 then
    if o:type() == "Shell" and o.moving then
      map:destroy(o.id)
    end
    o:velx(0)
  end

end

-- Play animations
function updateAnimations( o )
  if o.dying then
    return
  end
end

-- Monsters
function updateGroundAI ( o )
  if o.dying then
    return
  end

  o:accel(2000, o.direction, 0)
  limitSpeed(o)

  -- Turn at map edge
  if o:x() <= 0 then 
    turn(o)
  end

  if o.reachedwall then
    turn(o)
    o.reachedwall = false
  end
end

-- Generic handler for monster vs object collision
function groundMonsterCollide ( o1, o2, pos1, pos2 )
  if o2.groundmonster and not o1.dying and not o2.dying then
    local vel1 = o1:velx()
    local vel2
    if o1:velx() < 0 and o2:velx() <= 0 or o1:velx() > 0 and o2:velx() > 0 then
      vel2 = -o2:velx()
    else
      vel2 = o2:velx()
    end
    
    -- Check their positions and velocities and handle accordingly
    if pos1:x() + o1:hbright() - 4 < pos2:x() + o2:hbleft() and vel1 > vel2 then 
      if o1:type() ~= "Shell" then turn(o1) end
      if o2:type() ~= "Shell" then turn(o2) end
    elseif pos1:x() + o1:hbleft() + 4 > pos2:x() + o2:hbright() and vel2 > vel1 then
      if o1:type() ~= "Shell" then turn(o1) end
      if o2:type() ~= "Shell" then turn(o2) end
    end
  end
  
end

-- Turn the monster around (if it hits anything)
function turn ( o )
    o.direction = -o.direction
    o:velx(-o:velx())
    o:x(o:x()+o.direction*2)
end

-- Goomba
function initGoomba ( o )
  o.maxspeed = 50
  o.score = 100
  o.direction = -1
  o.dying = false
  o.deathtimer = Timer()
  o.groundmonster = true
end

function updateGoomba ( o )
  if o.deathtimer:reached(3) then
    map:destroy(o.id)
  end
end

-- Koopa
function initKoopa( o )
  o.maxspeed = 70
  o.direction = -1
  o.groundmonster = true
  o.score = 100
end

-- Koopa shells
function initShell ( o )
  o.maxspeed = 250
  o.groundmonster = false
  o.direction = -1
  o.groundmonster = true
  o.moving = false
  o.coltimer = Timer(true)
  o.score = 100
  o.combo = 0
end

function updateShell ( o )
  if o.moving and not o.dying then
    o:accel(10000, o.direction, 0)
    updateGroundAI(o)
  end
  o.coltimer:update()
  
  if math.abs(o:velx()) < 1 then
    o.combo = 0
  end
end

-- Flying koopa
function initFlyingKoopa ( o )
  o.maxspeed = 50
  o.direction = -1
end

function updateFlyingKoopa ( o )
  o:accel(500, o.direction, 0)
  limitSpeed(o)
end

-- Collision between moving shells and objects
function shellCollide ( shell, object, pos1, pos2 )
  if (math.abs(shell:velx()) > 30 or math.abs(shell:vely()) > 30) and not object.dying then
    if object.groundmonster or object:type() == "FlyingKoopa" then
      object:flipy(true)
      object.dying = true
      object:cancollide(false)
      object:vely(100)
      object:velx(0)
      normalHit(shell, object)
    end
  end
end

-- Player
function initPlayer ( o )
  o.maxspeed = 200
  o.inair = false
  map.player = o
  o.input = true
  o.big = (o:name() == "MarioBig")
  o.dying = false          -- "Dying" in this case means "shrinking" when big and dying when small
  o.recovertimer = Timer() -- Recovery timer after shrinking (a time when you can't be touched)
  o.shrinktimer = Timer()
  o.deathtimer = Timer()
  o.deathtimer:stop()
  o.jumped = false
  o.combo = 0
end

-- Inflict damage on Mario (mag == 1 - normal damage (shrink when big, die otherwise), mag == 2 - instant death (for instance when being crushed)
function hurtMario ( pl, mag )
  if not pl.dying then
  --if not true then
    if pl.big and not pl.dying and mag == 1 then 
      pl.shrinktimer:start()
      pl.dying = true
      pl:playanimation("die", false, 3)
      audio:play("Shrink")
      pl:velx(0)
    elseif mag == 2 or (not pl.big and mag == 1) then
      pl.deathtimer:start()
      pl.dying = true
      pl:playanimation("die", false, 4)
      pl:velx(0)
      audio:play("MarioDie")
      audio:stopmusic()
    end
  end
end

function updatePlayer ( o )
  if o:y() <= -50 then
    o:vely(0)
    o:y(-50)
    hurtMario(o, 2)
  end

  if o.dying and not o.deathtimer:reached(0.7) then
    o:vely(0)
  end

  if o.deathtimer:reached(0.5) and not o.deathtimer:reached(0.7) then
    o:vely(250)
    o:cancollide(false)
  end

  if o.shrinktimer:reached(3) and o.big then
    map:spawn("MarioSmall", o:x(), o:y())
    local mario = findObject("MarioSmall")
    mario.big = false
    map:destroy(o.id)
  end

  -- Load the appropriate map when you die
  if o.deathtimer:reached(4) then
    game.lives = game.lives - 1
    if game.lives == 0 then
      map:load("data/maps/gameover.json")
    else
      map:load("data/maps/deathmenu.json")
    end
  end

  if o.input == true and not o.dying then
    -- right button
    if input:keydown(key_right) then
      o:accel(1000,1,0)
    end

    -- left button
    if input:keydown(key_left) then
      o:accel(1000,-1,0)
    end

    updateTimedTexts()

    if input:keydown(key_shift) then
      o.maxspeed = 220
    else
      o.maxspeed = 110
    end
  end

  limitSpeed(o)

  -- Check map boundaries
  if o:x() < 0 then
    o:x(0)
    o:velx(0)
  end

  if o:x() <= -100 then
    o:vely(0)
    o:y(-100)
  end

end

-- Function that plays the hit sound and gives the player score for kills
function normalHit(hitter, object)
  local jumpmag
  local scoreinc
  
  if input:keydown(key_space) then
    jumpmag = 350
  else
    jumpmag = 200
  end
  
  if hitter:type() == "Mario" then
    jump(hitter,jumpmag)
  end
  hitter.combo = hitter.combo + 1
  
-- Increment score
  if hitter.combo < 8 and hitter.combo > 0 then
    audio:play("NormalHit" .. hitter.combo)
    if hitter.combo <= 3 then
      scoreinc = object.score * 2^hitter.combo
    elseif hitter.combo >= 3 then
      scoreinc = 10 * object.score * 2^(hitter.combo-4)
    end
    game.score = game.score + scoreinc
    table.insert(timedTexts, TimedText("" .. scoreinc, 1, object:x(), object:y(), 0, 50))
  else
    audio:play("HardHit")
    gainLife(1)
  end  
end

-- Collision handler for collision between player and objects
function playerCollide( player, object, pos1, pos2 )
  local colrange = 5
  -- Determine how high the player should jump when hitting an enemy - jump button down = high, otherwise = low
  local jumpmag

  if input:keydown(key_space) then
    jumpmag = 350
  else
    jumpmag = 200
  end

  -- Collision with goomba
  if not object.dying then
    if object:type() == "Goomba" then
      if object:y() + object:hbtop() - colrange <= player:y() + player:hbbottom() then
        -- Player is at sufficient relative height to do some damage
        object.dying = true
        object.deathtimer:start()
        object:playanimation("die", false, 10)
        normalHit(player, object)
      else
        hurtMario(player, 1)
      end
      
    -- Collision with koopa
    elseif object:type() == "Koopa" then
      if object:y() + object:hbtop() - colrange <= player:y() + player:hbbottom() then
        -- Create a shell and point it in the right direction
        local shell = map:spawn("GreenShell", object:x()+5, object:y())
        shell.direction = object.direction
        if shell.direction == 1 then shell:flipx(false) else shell:flipx(true) end
        map:destroy(object.id)
        normalHit(player, object)
      else
        hurtMario(player, 1)
      end
    
    -- Collision with flying koopa
    elseif object:type() == "FlyingKoopa" then
        if object:y() + object:hbtop() - colrange <= player:y() + player:hbbottom() then
        -- Create a koopa and point it in the right direction
        local koopa = map:spawn("GreenKoopa", object:x()+10, object:y())
        koopa.direction = object.direction
        if koopa.direction == 1 then koopa:flipx(false) else koopa:flipx(true) end
        map:destroy(object.id)
        normalHit(player, object)
      else
        hurtMario(player, 1)
      end
      
    -- Collision with shell (kind of complex)
    elseif object:type() == "Shell" then
      if not object.moving then
        if object.coltimer:reached(0.3) then
          if player:x() + player:hbright() >= object:x() + (object:hbright() - object:hbleft()) / 2 then
            object.moving = true
            object.direction = -1
          else
            object.moving = true
            object.direction = 1
          end
          object.coltimer:reset()
          audio:play("Kick")
        end
      else
        if object.coltimer:reached(0.3) then
          if object:y() + object:hbtop() - colrange <= player:y() + player:hbbottom() then
            object.moving = false
            normalHit(player, object)
            object:velx(0)
            object.coltimer:reset()
          else
            hurtMario(player, 1)
          end
        end
      end
      
    -- Collision with mushroom
    elseif object:type() == "Mushroom" then
      map:destroy(object.id)
      if not player.big then
        map:destroy(player.id)
        map:spawn("MarioBig", player:x(), player:y())
      end
      audio:play("Grow")
      
    -- Collision with coin
    elseif object:type() == "Coin" then
--      game.coins = game.coins + 1
      map:destroy(object.id)
      gainCoin(1)
      coin = map:spawn("CoinFading", object:x(), object:y())
      coin:vely(100)
    elseif object:type() == "Goal" and not hasfinished then
      player.goal = true
      hasfinished = true
      audio:playmusic("Finish", false)
    end -- if object:type()
  end -- if object.dying
end

-- Collision handler for collision between object and tile
function landcreatureCollide( o, x, y, b, ox, oy )
  -- Full blocking tile
  local tilesize = map:tilesize()
  local left = x * tilesize
  local right = x * tilesize + tilesize
  local top = y * tilesize + tilesize
  local bottom = y * tilesize
  local collided = false

  local insidetilex = (ox + o:hbright() > left + 1 and ox + o:hbleft() < right - 1)
  local insidetiley = (oy + o:hbtop() > bottom + 1 and oy + o:hbbottom() < top - 1)

  -- Note: Two checks are required because: 
  -- - With only the first check, stuff tends to fall through walls and floors because of the 1-pixel error in the checks wether or not the object is inside the tile (see insidetiley and insidetilex).
  -- - With only the last check, collisions that shouldn't happen, happens. For instance, when falling and trying to accellerate against a wall, a collision with the tile's top is detected and you can stand in mid-air.
  
  -- Full blocking tile
  if b == 1 then
    -- Collision with tile top
    if (oy + o:hbbottom() - (frametime * o:vely()) + 1) >= top and o:vely() <= 0 and insidetilex then
      o:vely(0)
      o:y(top-o:hbbottom())
      o.inair = false
      if o:type() == "Mario" then
        o.combo = 0
      end
      collided = true
      
    -- Collision with tile bottom
    elseif (oy + o:hbtop() - (frametime * o:vely()) - 1) <= bottom and o:vely() >= 0 and insidetilex then
      o:vely(0)
      o:y(bottom-o:hbtop())
      collided = true
      
    -- Collision with tile left
    elseif (ox + o:hbright() - (frametime * o:velx()) - 1) <= left and o:velx() >= 0 and insidetiley then
      o:x(left-o:hbright())
      o:velx(0)
      o.reachedwall = true -- For AI
      if o:type() == "Shell" then
        audio:play("Thomp")
      end
      collided = true
      
    -- Collision with tile right
    elseif (ox + o:hbleft() - (frametime * o:velx()) + 1) >= right and o:velx() <= 0 and insidetiley then
      o:x(right-o:hbleft())
      o:velx(0)
      o.reachedwall = true -- For AI
      if o:type() == "Shell" then
        audio:play("Thomp")
      end
      collided = true
    end

  -- One-way tile - top
  elseif b == 2 then
    if (oy + o:hbbottom() - (frametime * o:vely()) + 1) >= top and o:vely() <= 0 and insidetilex then
      o:vely(0)
      o:y(top-o:hbbottom())
      o.inair = false
      collided = true
      if o:type() == "Mario" then
        o.combo = 0
      end
    end
  end
  
  -- If no collision was detected - perform a looser check
  if not collided then
    -- Full blocking tile
    if b == 1 then
      -- Collision with tile top
      if (oy + o:hbbottom() - (frametime * o:vely()) + 3) >= top and o:vely() <= 0 and insidetilex then
        o:vely(0)
        o:y(top-o:hbbottom())
        o.inair = false
        
      -- Collision with tile bottom
      elseif (oy + o:hbtop() - (frametime * o:vely()) - 3) <= bottom and o:vely() >= 0 and insidetilex then
        o:vely(0)
        o:y(bottom-o:hbtop())
        
      -- Collision with tile left
      elseif (ox + o:hbright() - (frametime * o:velx()) - 3) <= left and o:velx() >= 0 and insidetiley then
        o:x(left-o:hbright())
        o:velx(0)
        o.reachedwall = true -- For AI
        if o:type() == "Shell" then
          audio:play("Thomp")
        end
        
      -- Collision with tile right
      elseif (ox + o:hbleft() - (frametime * o:velx()) + 3) >= right and o:velx() <= 0 and insidetiley then
        o:x(right-o:hbleft())
        o:velx(0)
        o.reachedwall = true -- For AI
        if o:type() == "Shell" then
          audio:play("Thomp")
        end
      end

    -- One-way tile - top
    elseif b == 2 then
      if (oy + o:hbbottom() - (frametime * o:vely()) + 3) >= top and o:vely() <= 0 and insidetilex then
        o:vely(0)
        o:y(top-o:hbbottom())
        o.inair = false
      end
    end
  end
end

-- Blocks
function initBlock( o )
  o.orgy = o:y()
  o.orgx = o:x()
end

function updateBlock( o )
  if o:y() > o.orgy then
    o:accel(2000, 0, -1)
  else
    o:y(o.orgy)
    o:vely(0)
  end
  
  if o:x() > o.orgx then
    o:accel(2000, -1, 0)
  elseif o:x() < o.orgx then
    o:accel(2000, 1, 0)
  else
    o:x(o.orgx)
    o:velx(0)
  end
  
  if o:name() == "BlockSingleCoin" or o:name() == "BlockMultipleCoins" then
    o.cointimer:update()
  end
end

function activateBlock( block, newblockname )
  -- Make a new block in place of the current one
  map:destroy(block.id)
  newblock = map:spawn(newblockname, block:x(), block:y())
  newblock:y(newblock:y() + 10)

  return newblock
end

function spawnObjectFromBlock( block, name )
  mush = map:spawn("Mushroom", block:x(), block:y()+16)
  mush:vely(100)
  audio:play("MushroomSpawn")
end

function gainCoinFromBlock( block, amount )
  if block.cointimer:reached(0.1) then
    coin = map:spawn("CoinFading", block:x()+2, block:y()+10)
    if block.coins <= 1 then
      activateBlock(block, "BlockInactive")
    else
      block.coins = block.coins - 1
      block:y(block:y() + 10)
    end
    gainCoin(amount)
    coin:vely(100)
    block.cointimer:reset()
  end
end

function blockCollide( block, object, blockpos, objectpos )
  local left = blockpos:x()
  local insideblockx = (objectpos:x() + object:hbright() > blockpos:x() + 1 and objectpos:x() + object:hbleft() < blockpos:x() + 16 - 1)
  local insideblocky = (objectpos:y() + object:hbtop() > blockpos:y() + 1 and objectpos:y() + object:hbbottom() < blockpos:y() + 16 - 1)
  local objectmid = objectpos:x() + (object:hbright() + object:hbleft()) / 2

  -- Collision from below
  if objectpos:y() + object:hbtop() - 2 - (frametime * object:vely()) < blockpos:y() then
    --if objectpos:y() + object:hbtop() - 2 - (frametime * object:vely()) < blockpos:y() then
      if objectmid >= left-2 and objectmid <= left + 16 + 2 then
        -- If the object colliding is Mario, then give him some stuff
        if object:type() == "Mario" or object:type() == "Shell" then
          audio:play("Thomp")
          -- Give him mushroom
          if block:name() == "BlockMushroom" then
            spawnObjectFromBlock(block, "Mushroom")
            activateBlock(block, "BlockInactive")
          -- Give him money
          elseif block:name() == "BlockMultipleCoins" or block:name() == "BlockSingleCoin" then
            gainCoinFromBlock(block, 1)
          end
        end
        object:vely(0)
        object:y(blockpos:y() - object:hbtop())
      end
    --else
     -- object:vely(0)
     -- object:y(blockpos:y() - object:hbtop())
    --end    
  elseif objectpos:y() + object:hbtop() - 4 - (frametime * object:vely()) < blockpos:y() then
    object:vely(0)
    object:y(blockpos:y() - object:hbtop())
  -- From above
  elseif objectpos:y() + object:hbbottom() + 2 - (frametime * object:vely()) > blockpos:y() + 16 and object:vely() <= 0 and insideblockx then
    object:vely(0)
    object:y(blockpos:y() - object:hbbottom() + 16)
    object.inair = false
    
  -- From the left
  elseif objectpos:x() + object:hbright() - 2 - (frametime * object:velx()) < block:x() and insideblocky then
    object:x(blockpos:x() - object:hbright())
    if object.groundmonster or object:name() == "Mushroom" or object:type() == "FlyingKoopa" then
      if object:type() == "Shell" then
        audio:play("Thomp")
        if block:name() == "BlockMushroom" then
          spawnObjectFromBlock(block, "Mushroom")
          activateBlock(block, "BlockInactive")
        elseif block:name() == "BlockMultipleCoins" or block:name() == "BlockSingleCoin" then
          gainCoinFromBlock(block, 1)
        end
      end
      turn(object)
    else
      object:velx(0)
    end
    
  -- From the right
  elseif objectpos:x() + object:hbleft() + 2 - (frametime * object:velx()) > blockpos:x() + 16 and insideblocky then
    object:x(blockpos:x() + 16 - object:hbleft())
    if object.groundmonster or object:name() == "Mushroom" or object:type() == "FlyingKoopa" then
      if object:type() == "Shell" then 
        audio:play("Thomp")
        if block:name() == "BlockMushroom" then
          spawnObjectFromBlock(block, "Mushroom")
          activateBlock(block, "BlockInactive")
        elseif block:name() == "BlockMultipleCoins" or block:name() == "BlockSingleCoin" then
          gainCoinFromBlock(block, 1)
        end
      end
      turn(object)
    else
      object:velx(0)
    end
  end
end

function initFadingCoin(o)
  o.fadetimer = Timer(true)
end

function updateFadingCoin(o)
  if o.fadetimer:reached(0.3) then map:destroy(o.id) end
end

-- Coin blocks
function initCoinBlock( o )
  if o:name() == "BlockSingleCoin" then
    o.coins = 1
  else
    o.coins = 10
  end
  o.cointimer = Timer(true)
end

-- Mushroom
function initMushroom( o )
  o.maxspeed = 50
  o.direction = 1
end


