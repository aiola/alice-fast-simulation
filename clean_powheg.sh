#!/bin/bash

files=(realequiv realequivregions virtequiv bornequiv pwhg_checklimits pwg-btlgrid.top 
pwgborngrid.top pwghistnorms.top FlavRegList pwgboundviolations.dat pwg-stat.dat pwgubound.dat 
pwgcounters.dat pwggrid.dat pwgxgrid.dat)

tarfile=$1

if [ -n "${tarfile}" ]; then
	tar cf "${tarfile}" --files-from /dev/null
fi

for f in ${files[@]}; do
	if [ -e "$f" ]; then
		if [ -n "${tarfile}" ]; then
			echo "Archiving file ${f}"
			tar rf "${tarfile}" "${f}"
		fi
		echo "Deleting file ${f}"
		rm ${f}
	else
		echo "File ${f} does not exist"
	fi
done

exit 0
