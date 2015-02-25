#!/bin/bash
##################################################
validateout=`dirname $0`
validatetime=`date`
validated="0";
error=0
if [ -z $validateout ]
then
    validateout="."
fi

cd $validateout;
validateworkdir=`pwd`;

echo "*******************************************************"
echo "* Automatically generated validation script           *"

echo "* Time:    $validatetime "
echo "* Dir:     $validateout"
echo "* Workdir: $validateworkdir"
echo "* ----------------------------------------------------*"
ls -la ./
echo "* ----------------------------------------------------*"

##################################################

if [ ! -f stderr ] ; then
   error=1
   echo "* ########## Job not validated - no stderr  ###" 
   echo "Error = $error" 
fi
parArch=`grep -Ei "Cannot Build the PAR Archive" stderr`
segViol=`grep -Ei "Segmentation violation" stderr`
segFault=`grep -Ei "Segmentation fault" stderr`
glibcErr=`grep -Ei "*** glibc detected ***" stderr`

if [ "$parArch" != "" ] ; then
   error=1
   echo "* ########## Job not validated - PAR archive not built  ###" 
   echo "$parArch" 
   echo "Error = $error" 
fi
if [ "$segViol" != "" ] ; then
   error=1
   echo "* ########## Job not validated - Segment. violation  ###" 
   echo "$segViol" 
   echo "Error = $error" 
fi
if [ "$segFault" != "" ] ; then
   error=1
   echo "* ########## Job not validated - Segment. fault  ###" 
   echo "$segFault" 
   echo "Error = $error" 
fi
if [ "$glibcErr" != "" ] ; then
   error=1
   echo "* ########## Job not validated - *** glibc detected ***  ###" 
   echo "$glibcErr" 
   echo "Error = $error" 
fi
if ! [ -f AnalysisResults.root ] ; then
   error=1
   echo "Output file AnalysisResults.root not found. Job FAILED !"
   echo "Output file AnalysisResults.root not found. Job FAILED !" >> stderr
fi
if ! [ -f outputs_valid ] ; then
   error=1
   echo "Output files were not validated by the analysis manager" >> stdout
   echo "Output files were not validated by the analysis manager" >> stderr
fi
if [ $error = 0 ] ; then
   echo "* ----------------   Job Validated  ------------------*"
   echo "* === Logs std* will be deleted === "
   rm -f std*
fi
echo "* ----------------------------------------------------*"
echo "*******************************************************"
cd -
exit $error
