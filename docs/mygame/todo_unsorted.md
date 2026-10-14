[ ] mon movement behavior
  [ ] aggro state/toggle/target
    [ ] MonsterIsAggro(args)
      [ ] if chase target index != OBJECT_EVENTS_COUNT
        - would having 0 be equivalent to null and offsetting the index by 1 be more futureproof?
          - if we ever changed OBJECT_EVENTS_COUNT and nothing else, it would break our save file if we used OBJECT_EVENTS_COUNT as null equivalent
            - I don't care that much about save compatibility.... but if we can do it....
    [ ] if aggro -> chase target (just player, for now)
      - remember, target's distance is not how far the monster needs to travel, subtract 1
    [ ] on player caught, trigger battle
  [ ] aggro triggered by aggro distance (retrieved via function)
    [ ] awareness range separate from movement range
  [ ] aggro drops if player goes outside their movement range (aggro should increase this in the future)
    - I was thinking they could keep chasing until the monster would have to go outside
      - this could stop some chases from ending prematurely
  [ ] aggro increases max distance from home cood in a function

---

- [ ] gen. dungeon. pass 2
  - [ ] spawn all objects in cell
    - [x] pickups
    - [ ] overworld monsters that wander relative to a home coordinate
      - [ ] only save home cood
      - will chase player a distance from coordinate
        - otherwise try to maintain a tighter distance from coordinate
          - stays within 5, but will go up to 9 if chasing
            - should give mobility abilities
              - some mons can dash two spaces in one turn
        - how do they move? I don't want to tie to player movement
          - they tick. they simply tick.
            - maybe we have a single task for game behavior that encapsulates ticking
  - a side room that gives a key to a drawbridge/gate/etc. before the end
    - a powerup that lasts the duration of the floor and lets you push select obstacles
      - something blocking a door can effectively be a drawbridge
        - just differentiate a permanent blockade from a pushable one
      - kill a mini boss for the powerup?
        - we could also have a Dark Cloud esque backroom behind an obstacle like this
          - or just behind a mini boss, i suppose
            - returning to an obstacle later does have something fun to it
              - now... if the backroom was blocked by a boss that gave a key to a different obstacle.... that'd be fine
                - I just don't want it to be linear if it doesn't need to be

[?] do we want to save mon location before calculating next movement step (to reload position)
- or is reloading positions LESS immersive? they shouldn't be frozen
  - it's really only immersive if you leave and immediately re-enter
  - what if the player moves something to trap a mon? why not preserve that?
  - they could return to their home position after a short time
    - we could setup a timer per cell index in temp data?
      - always counting down on tick, but...
        - if it's above zero when that cell is re-entered, reload the cached monster positions
        - else load their home positions (or find a valid coordinate within a range of it)
  - I want positions reloaded, but I'm also torn on that
    - what if a mon saves next to a warp so you enter to immediate battle?
      - I guess they'll have to suck it up?
      - that's gonna feel kinda strange if you leave the dungeon and come back to that
        - would saving in temp data instead of save data remedy that?
          - wouldn't it still feel strange if you run around the dungeon and return to that?
            - it would be nice if they reset... I like the timer in temp data idea, though it's totally unnecessary