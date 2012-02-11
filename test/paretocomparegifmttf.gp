set terminal gif
set output "`echo $TBL_FILE`".".gif"
set title "1/MTTF and communication cost"
set xlabel "1/MTTF"
set ylabel "communication cost"
plot "`echo $TBL_FILE`" u (-$4):3, "`echo $TBL_FILE2`" u (-$4):3

#plot for [i = 1:100:5] "pareto_set_gamapper_gen_".i.".tbl" u 2:3, "pareto_set_lpmapper_timelimit1000.tbl" u 2:3
