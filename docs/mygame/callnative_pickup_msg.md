poryscript:
  1. prep message (of any type) via callnative
     - text[], text[]+image, etc.
  2. show a message via poryscript
  3. give an item or do other thing via callnative
  4. show debug message via poryscript (if flag set)

- From C: set a flag that signals that a message and icon are pending
  - use later in poryscript
    - unless you know how to queue a msgbox from c