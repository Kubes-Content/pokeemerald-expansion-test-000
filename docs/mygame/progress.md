[ ] load monster's movement pattern

[ ] pickups/monsters should not spawn on top of warp
  because player may spawn onto it
  [ ] or in front of a north door .. so check if north is a transition tile?
  will the tile the player walks down onto from north door be occupied while entering?
    so that monsters know to avoid it
      or do I need to prevent monsters from ever touching a tile before a north door?

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
  - utilize GoneGlobTM Technology
    - prefer 2 adjacent rooms to 1 adjacent room to 3 adjacent rooms to 4
      - we want to link cells together without globbing
      - prefer to connect to rooms with less than 2 pre-existing connections
- [ ] spawn atla independently of pickups
  - how to assign as the drop of an overworld enemy in a cell within the cave
    - you'd have to have the overworld enemy stuff built
- [ ] gen. 'atla' pickups that persist per room
- [ ] test different pickups per room