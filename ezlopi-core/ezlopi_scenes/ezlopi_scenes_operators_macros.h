SCENES_OPERATORS(NONE, NULL, NULL, NULL)
//////////////////////////////////////////////////////////////////////////////
SCENES_OPERATORS(LESS, "<", "less", "compareNumbers")
SCENES_OPERATORS(GREATER, ">", "greater", "compareNumbers")
SCENES_OPERATORS(LESS_EQUAL, "<=", "less equal", "compareNumbers")
SCENES_OPERATORS(GREATER_EQUAL, ">=", "greater equal", "compareNumbers")
SCENES_OPERATORS(EQUAL, "==", "equal", "compareNumbers")
SCENES_OPERATORS(NOT_EQUAL, "!=", "not equal", "compareNumbers")
SCENES_OPERATORS(BETWEEN, "between", "between", "compareNumberRange")
SCENES_OPERATORS(NOT_BETWEEN, "not_between", "not between", "compareNumberRange")
SCENES_OPERATORS(ANY_OF, "any_of", "any of", "numbersArray")
SCENES_OPERATORS(NONE_OF, "none_of", "none of", "numbersArray")
//////////////////////////////////////////////////////////////////////////////
SCENES_OPERATORS(MAX, NULL, NULL, NULL)