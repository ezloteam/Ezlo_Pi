#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

SCENES_STROPS_COMP_OPERATORES(NONE, NULL, NULL, NULL)
//////////////////////////////////////////////////////////////////////////////
SCENES_STROPS_COMP_OPERATORES(BEGINS_WITH, "begin", "begins with", "stringOperation")
SCENES_STROPS_COMP_OPERATORES(ENDS_WITH, "end", "ends with", "stringOperation")
SCENES_STROPS_COMP_OPERATORES(CONTAINS, "contain", "contains", "stringOperation")
SCENES_STROPS_COMP_OPERATORES(LENGTH, "length", "length equal to", "stringOperation")
SCENES_STROPS_COMP_OPERATORES(NOT_BEGIN, "not_begin", "doesn't begin with", "stringOperation")
SCENES_STROPS_COMP_OPERATORES(NOT_END, "not_end", "doesn't end with", "stringOperation")
SCENES_STROPS_COMP_OPERATORES(NOT_CONTAIN, "not_contain", "doesn't contain", "stringOperation")
SCENES_STROPS_COMP_OPERATORES(NOT_LENGTH, "not_length", "length not equal to", "stringOperation")
//////////////////////////////////////////////////////////////////////////////
SCENES_STROPS_COMP_OPERATORES(MAX, NULL, NULL, NULL)

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS