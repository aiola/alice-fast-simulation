#!/bin/bash
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
echo "========================================="
echo "############## PATH : ##############"
echo $PATH
echo "############## LD_LIBRARY_PATH : ##############"
echo $LD_LIBRARY_PATH
echo "############## ROOTSYS : ##############"
echo $ROOTSYS
echo "############## which root : ##############"
which root
echo "############## ALICE_ROOT : ##############"
echo $ALICE_ROOT
echo "############## which aliroot : ##############"
which aliroot
echo "############## system limits : ##############"
ulimit -a
echo "############## memory : ##############"
free 2> /dev/null || { [[ `uname` == Darwin ]] && top -l 1 -s 0 | head -8 | tail -3; }
echo "========================================="

# my program is here
files=(AnalysisResults*.root)
filename=${files[0]}

if [[ ! -f "${filename}" ]]; then
	echo "File '${filename}' does not exists!"
	echo "Validation failed!"
	exit 1
fi

minimumsize="1024"
actualsize=$(wc -c < "${filename}" | tr -d ' ')

if [[ "${actualsize}" -le "${minimumsize}" ]]; then
	echo "File '${filename}' has size ${actualsize} bytes!"
	echo "Validation failed!"
	rm "${filename}"
    exit 1
fi

echo "File '${filename}' exists and has size ${actualsize} bytes!"

echo "Validation OK!"

exit 0
# program ends

RET=$?
if [ "$RET" != "0" ];then
  echo "======== ERROR : analysis finished with NON zero code: $RET ========"
  if [ "$RET" -gt 128 ] && [ "$RET" -lt 160 ]; then
    let sig="$RET - 128"
    sigs='HUP INT QUIT ILL TRAP ABRT BUS FPE
    KILL USR1 SEGV USR2 PIPE ALRM TERM STKFLT
    CHLD CONT STOP TSTP TTIN TTOU URG XCPU
    XFSZ VTALRM PROF WINCH IO PWR SYS'
    sig=SIG`echo $sigs | awk '{ print $'"$sig"' }'`
    echo "======== it appears to have been killed with signal: $sig ========"
  fi
  exit $RET
fi

echo "======== analysis finished with exit code: $RET ========"
echo "############## memory after: ##############"
free -m