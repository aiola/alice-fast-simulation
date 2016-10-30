#!/bin/bash

files=(realequiv realequivregions virtequiv bornequiv pwhg_checklimits pwg-btlgrid.top 
pwgborngrid.top pwghistnorms.top FlavRegList pwgboundviolations.dat pwg-stat.dat pwgubound.dat 
pwgcounters.dat pwggrid.dat pwgxgrid.dat)

for f in ${files[@]}; do
	if [ -e "$f" ]; then
		echo "Deleting file ${f}"
		rm ${f}
	else
		echo "File ${f} does not exist"
	fi
done
exit 0
