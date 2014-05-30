#data/e.plot
reset
set xrange [0:200]
set yrange [0:200]
set term gif animate
set output 'E.gif'
do for [i=0:1938] {
	plot 'Model2D/E_'.i.'.dat' matrix with image;
}
#End of data/e.plot
