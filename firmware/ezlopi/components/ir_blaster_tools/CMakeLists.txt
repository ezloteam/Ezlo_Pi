set(component_srcs "include/ir_init.c"
                   "src/ir_rmt_nec.c"
                   "src/ir_rmt_samsung.c"
                   "src/ir_rmt_panasonic.c"
                   "src/ir_rmt_lg.c"
                   "src/ir_rmt_sony.c"
                   "src/ir_rmt_lego.c"
                   "src/ir_rmt_jvc.c"
                   "src/ir_rmt_dish.c"
                   "src/ir_rmt_gree.c"
                   "src/ir_rmt_toshibaAC.c"
                   "src/ir_rmt_airton.c"
                   "src/ir_builder_rmt_rc5.c"
                   "src/ir_parser_rmt_rc5.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS "include"
                       REQUIRES trace)
