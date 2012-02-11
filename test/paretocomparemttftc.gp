set title "1/MTTF and communication cost"
set xlabel "1/MTTF"
set ylabel "communication cost"
#plot "`echo $TBL_FILE`" u (-$4):3, "`echo $TBL_FILE2`" u (-$4):3

plot "`echo $TBL_FILE`" u ($2 < `echo $TC` ? (-$4) : 1/0):($2 < `echo $TC` ? $3 : 1/0), "`echo $TBL_FILE2`" u ($2 < `echo $TC` ? (-$4) : 1/0):($2 < `echo $TC` ? $3 : 1/0)

# plot for [i = 1:100:5] "pareto_set_gamapper_gen_".i.".tbl" u 2:3, "pareto_set_lpmapper_timelimit1000.tbl" u 2:3

# pause mouse any


#plot "resultsall.tbl" u 4:5, "" u 4:5:2 w labels
#plot "resultsall.tbl" u 4:5, "" u 4:5:2 w labels
#plot "resultsall.tbl" u 4:5, "" u 4:5:((stringcolumn(2) eq '1' && stringcolumn(1) eq 'Mapping1') ? 'x' : '') w labels
#plot "resultsall.tbl" u 4:5, "" u 4:5:((stringcolumn(2) eq '1' && stringcolumn(1) eq 'Mapping1' && stringcolumn(3) eq 'Pareto') ? 'x' : '') w labels

#plot "resultsall.tbl" u 4:5:((stringcolumn(2) eq '1' && stringcolumn(1) eq 'Mapping1' && stringcolumn(3) eq 'Pareto') ? 'x' : '') w labels, "" u 4:5:((stringcolumn(2) eq '1' && stringcolumn(1) eq 'Mapping1' && stringcolumn(3) eq 'CoG') ? 'o' : '') w labels, "" u 4:5:((stringcolumn(2) eq '1' && stringcolumn(1) eq 'Mapping1' && stringcolumn(3) eq 'NMSA') ? 'a' : '') w labels, "" u 4:5:((stringcolumn(2) eq '1' && stringcolumn(1) eq 'Mapping1' && stringcolumn(3) eq 'NMSB') ? 'b' : '') w labels, "" u 4:5:((stringcolumn(2) eq '1' && stringcolumn(1) eq 'Mapping1' && stringcolumn(3) eq 'NMSC') ? 'c' : '') w labels, "" u 4:5:((stringcolumn(2) eq '1' && stringcolumn(1) eq 'Mapping1' && stringcolumn(3) eq 'LNMS-C') ? '+' : '') w labels

#plot "resultsall-wo10-overhead.tbl" u 6:7:((stringcolumn(2) eq '1' && stringcolumn(1) eq 'Mapping1' && stringcolumn(3) eq 'Pareto') ? 'x' : '') w labels, "" u 6:7:((stringcolumn(2) eq '1' && stringcolumn(1) eq 'Mapping1' && stringcolumn(3) eq 'CoG') ? 'o' : '') w labels, "" u 6:7:((stringcolumn(2) eq '1' && stringcolumn(1) eq 'Mapping1' && stringcolumn(3) eq 'NMSA') ? 'a' : '') w labels, "" u 6:7:((stringcolumn(2) eq '1' && stringcolumn(1) eq 'Mapping1' && stringcolumn(3) eq 'NMSB') ? 'b' : '') w labels, "" u 6:7:((stringcolumn(2) eq '1' && stringcolumn(1) eq 'Mapping1' && stringcolumn(3) eq 'NMSC') ? 'c' : '') w labels, "" u 6:7:((stringcolumn(2) eq '1' && stringcolumn(1) eq 'Mapping1' && stringcolumn(3) eq 'LNMS-C') ? '+' : '') w labels

#plot "resultsall-wo10-overhead.tbl" u 6:7:((stringcolumn(3) eq 'LNMS(3)') ? 'x' : '') w labels




# # # bi algoritmanin iki eksendeki overhead'i
# set title "Overhead in execution time and communication cost"
# set xlabel "Overhead in execution time (%)"
# set ylabel "Overhead in communication cost (%)"

# plot "resultsall-wo10-overhead.tbl" u 8:9:((stringcolumn(5) eq 'Pareto') ? '+' : '') w labels t "CoG"
# pause mouse any

# plot "resultsall-wo10-overhead.tbl" u 8:9:((stringcolumn(5) eq 'CoG') ? '+' : '') w labels t "CoG"
# pause mouse any

# plot "resultsall-wo10-overhead.tbl" u 8:9:((stringcolumn(5) eq 'NMSA') ? '+' : '') w labels t "NMSA"
# pause mouse any

# plot "resultsall-wo10-overhead.tbl" u 8:9:((stringcolumn(5) eq 'NMSB') ? '+' : '') w labels t "NMSB"
# pause mouse any

# plot "resultsall-wo10-overhead.tbl" u 8:9:((stringcolumn(5) eq 'NMSC') ? '+' : '') w labels t "NMSC"
# pause mouse any

# plot "resultsall-wo10-overhead.tbl" u 8:9:((stringcolumn(5) eq 'LNMS(1)') ? '+' : '') w labels t "LNMS(1)"
# pause mouse any

# plot "resultsall-wo10-overhead.tbl" u 8:9:((stringcolumn(5) eq 'LNMS(2)') ? '+' : '') w labels t "LNMS(2)"
# pause mouse any

# plot "resultsall-wo10-overhead.tbl" u 8:9:((stringcolumn(5) eq 'LNMS(3)') ? '+' : '') w labels t "LNMS(3)"
# pause mouse any

# plot "resultsall-wo10-overhead.tbl" u 8:9:((stringcolumn(5) eq 'LNMS(4)') ? '+' : '') w labels t "LNMS(4)"
# pause mouse any

# plot "resultsall-wo10-overhead.tbl" u 8:9:((stringcolumn(5) eq 'LNMS(5)') ? '+' : '') w labels t "LNMS(5)"
# pause mouse any

# plot "resultsall-wo10-overhead.tbl" u 8:9:((stringcolumn(5) eq 'LNMS(6)') ? '+' : '') w labels t "LNMS(6)"
# pause mouse any



# # herbir fault icin paretolar ve heuristicler
# plot "resultsall-wo10-overhead.tbl" u 8:9:((stringcolumn(4) eq '1' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'Pareto') ? 'o' : '') w labels t "Pareto o", "" u 8:9:((stringcolumn(4) eq '1' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'CoG') ? '+' : '') w labels t "CoG +", "" u 8:9:((stringcolumn(4) eq '1' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'NMSA') ? '>' : '') w labels t "NMSA >", "" u 8:9:((stringcolumn(4) eq '1' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'NMSB') ? '<' : '') w labels t "NMSB <", "" u 8:9:((stringcolumn(4) eq '1' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'NMSC') ? '<>' : '') w labels t "NMSC <>", "" u 8:9:((stringcolumn(4) eq '1' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'LNMS(1)') ? 'x' : '') w labels t "LNMS(1) x", "" u 8:9:((stringcolumn(4) eq '1' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'LNMS(2)') ? 'x' : '') w labels t "LNMS(2) x", "" u 8:9:((stringcolumn(4) eq '1' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'LNMS(3)') ? 'x' : '') w labels t "LNMS(3) x", "" u 8:9:((stringcolumn(4) eq '1' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'LNMS(4)') ? 'x' : '') w labels t "LNMS(4) x", "" u 8:9:((stringcolumn(4) eq '1' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'LNMS(5)') ? 'x' : '') w labels t "LNMS(5) x"

# pause mouse any

# plot "resultsall-wo10-overhead.tbl" u 8:9:((stringcolumn(4) eq '2' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'Pareto') ? 'o' : '') w labels t "Pareto o", "" u 8:9:((stringcolumn(4) eq '2' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'CoG') ? '+' : '') w labels t "CoG +", "" u 8:9:((stringcolumn(4) eq '2' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'NMSA') ? '>' : '') w labels t "NMSA >", "" u 8:9:((stringcolumn(4) eq '2' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'NMSB') ? '<' : '') w labels t "NMSB <", "" u 8:9:((stringcolumn(4) eq '2' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'NMSC') ? '<>' : '') w labels t "NMSC <>", "" u 8:9:((stringcolumn(4) eq '2' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'LNMS(1)') ? 'x' : '') w labels t "LNMS(1) x", "" u 8:9:((stringcolumn(4) eq '2' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'LNMS(2)') ? 'x' : '') w labels t "LNMS(2) x", "" u 8:9:((stringcolumn(4) eq '2' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'LNMS(3)') ? 'x' : '') w labels t "LNMS(3) x", "" u 8:9:((stringcolumn(4) eq '2' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'LNMS(4)') ? 'x' : '') w labels t "LNMS(4) x", "" u 8:9:((stringcolumn(4) eq '2' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'LNMS(5)') ? 'x' : '') w labels t "LNMS(5) x"

# pause mouse any

# plot "resultsall_processed.tbl" u 8:9:((stringcolumn(4) eq '7' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'Pareto') ? 'o' : '') w labels t "Pareto o", "" u 8:9:((stringcolumn(4) eq '7' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'CoG') ? '+' : '') w labels t "CoG +", "" u 8:9:((stringcolumn(4) eq '7' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'NMSA') ? '>' : '') w labels t "NMSA >", "" u 8:9:((stringcolumn(4) eq '7' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'NMSB') ? '<' : '') w labels t "NMSB <", "" u 8:9:((stringcolumn(4) eq '7' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'NMSC') ? '<>' : '') w labels t "NMSC <>", "" u 8:9:((stringcolumn(4) eq '7' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'LNMS(1)') ? 'x' : '') w labels t "LNMS(1) x", "" u 8:9:((stringcolumn(4) eq '7' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'LNMS(2)') ? 'x' : '') w labels t "LNMS(2) x", "" u 8:9:((stringcolumn(4) eq '7' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'LNMS(3)') ? 'x' : '') w labels t "LNMS(3) x", "" u 8:9:((stringcolumn(4) eq '7' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'LNMS(4)') ? 'x' : '') w labels t "LNMS(4) x", "" u 8:9:((stringcolumn(4) eq '7' && stringcolumn(3) eq 'Mapping1' && stringcolumn(5) eq 'LNMS(5)') ? 'x' : '') w labels t "LNMS(5) x"

# pause mouse any
