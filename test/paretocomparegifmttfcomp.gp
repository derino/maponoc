set terminal gif
set output "`echo $TBL_FILE`".".gif"
set title "-MTTF and comp. time"
set xlabel "-MTTF"
set ylabel "comp. time"
plot "`echo $TBL_FILE`" u (-$4):2, "`echo $TBL_FILE2`" u (-$4):2

#plot for [i = 1:100:5] "pareto_set_gamapper_gen_".i.".tbl" u 2:3, "pareto_set_lpmapper_timelimit1000.tbl" u 2:3
