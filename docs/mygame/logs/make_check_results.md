- Run 1 - initial run
  - Tests KNOWN_FAILING:   18
  - Tests TO_DO:           421
  - Tests EXPECT_FAILING:  9
  - Tests PASSED:          5413
  - Tests TOTAL:           5861

- Run 2 - save space freed (I could probably add padding, but that defeats the purpose of freeing space unless padding can dynamically deflate)
    FAILED tests:
    - test/save.c:17: SaveBlock2 is backwards compatible: 19: EXPECT_EQ(460, 3884) failed
    - test/save.c:12: SaveBlock1 is backwards compatible: 14: EXPECT_EQ(7708, 15568) failed
  - Tests FAILED:          2    Add TESTS='X' to run tests with the defined prefix.
  - Tests KNOWN_FAILING:   18
  - Tests TO_DO:           421
  - Tests EXPECT_FAILING:  9
  - Tests PASSED:          5411
  - Tests TOTAL:           5861