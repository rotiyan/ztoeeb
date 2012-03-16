# echo "setup TruthUtils TruthUtils-00-00-12 in /afs/cern.ch/user/n/narayan/testarea/17.0.5/ZeeB/share/Generators/GenAnalysisTools"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/cern.ch/atlas/software/releases/17.0.5/CMT/v1r23; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtTruthUtilstempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then cmtTruthUtilstempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=TruthUtils -version=TruthUtils-00-00-12 -path=/afs/cern.ch/user/n/narayan/testarea/17.0.5/ZeeB/share/Generators/GenAnalysisTools  -quiet -without_version_directory -no_cleanup $* >${cmtTruthUtilstempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/mgr/cmt setup -sh -pack=TruthUtils -version=TruthUtils-00-00-12 -path=/afs/cern.ch/user/n/narayan/testarea/17.0.5/ZeeB/share/Generators/GenAnalysisTools  -quiet -without_version_directory -no_cleanup $* >${cmtTruthUtilstempfile}"
  cmtsetupstatus=2
  /bin/rm -f ${cmtTruthUtilstempfile}
  unset cmtTruthUtilstempfile
  return $cmtsetupstatus
fi
cmtsetupstatus=0
. ${cmtTruthUtilstempfile}
if test $? != 0 ; then
  cmtsetupstatus=2
fi
/bin/rm -f ${cmtTruthUtilstempfile}
unset cmtTruthUtilstempfile
return $cmtsetupstatus

