
#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

SCENES_VALUES_WITH_LESS_OPERATORS(NONE, NULL, NULL, NULL)
//////////////////////////////////////////////////////////////////////////////
SCENES_VALUES_WITH_LESS_OPERATORS(LESS, "<", "less", "compareValues")
SCENES_VALUES_WITH_LESS_OPERATORS(GREATER, ">", "greater", "compareValues")
SCENES_VALUES_WITH_LESS_OPERATORS(LESS_EQUAL, "<=", "less equal", "compareValues")
SCENES_VALUES_WITH_LESS_OPERATORS(GREATER_EQUAL, ">=", "greater equal", "compareValues")
SCENES_VALUES_WITH_LESS_OPERATORS(EQUAL, "==", "equal", "compareValues")
SCENES_VALUES_WITH_LESS_OPERATORS(NOT_EQUAL, "!=", "not equal", "compareValues")
//////////////////////////////////////////////////////////////////////////////
SCENES_VALUES_WITH_LESS_OPERATORS(MAX, NULL, NULL, NULL)

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS