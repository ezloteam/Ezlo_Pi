
#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

SCENES_STRINGS_OPERATORS(NONE, NULL, NULL, NULL)
//////////////////////////////////////////////////////////////////////////////
SCENES_STRINGS_OPERATORS(LESS, "<", "less", "compareStrings")
SCENES_STRINGS_OPERATORS(GREATER, ">", "greater", "compareStrings")
SCENES_STRINGS_OPERATORS(LESS_EQUAL, "<=", "less equal", "compareStrings")
SCENES_STRINGS_OPERATORS(GREATER_EQUAL, ">=", "greater equal", "compareStrings")
SCENES_STRINGS_OPERATORS(EQUAL, "==", "equal", "compareStrings")
SCENES_STRINGS_OPERATORS(NOT_EQUAL, "!=", "not equal", "compareStrings")
//////////////////////////////////////////////////////////////////////////////
SCENES_STRINGS_OPERATORS(MAX, NULL, NULL, NULL)
#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS