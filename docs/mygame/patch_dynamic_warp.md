entry point?: https://chatgpt.com/s/t_6ab71962c08c8191a610a5507d8e7941

* HAVE LITERAL WARPID: patch into dynamic warp before it commits to dynamic warp's variables
  * capture warpId
    * then I don't need a trigger script with every warp
      * I'd just need a flag/enum that dictates the dynamic warp behavior
    * go through game data w/ warpId to determine where we're warping to
      * cave data should be configured to determine where to go via generated warpId connections
* DUPLICATE WARPID W/ TRIGGER: whereas now, I have poryscript callnative before I hit the warp
  * more error-prone having to coordinate two objects for one responsibility
  * I just have c forcing behavior for the cave entrance
    * it's functionally identical to patching but w/ more setup
  * would I need a script per warp index?
    * can you do a sort of poryscript compile-time variable value on an object?