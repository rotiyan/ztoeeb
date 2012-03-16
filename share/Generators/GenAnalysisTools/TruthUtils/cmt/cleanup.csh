# echo "cleanup TruthUtils TruthUtils-00-00-12 in /afs/cern.ch/user/n/narayan/testarea/17.0.5/ZeeB/share/Generators/GenAnalysisTools"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/atlas/software/releases/17.0.5/CMT/v1r23
endif
source ${CMTROOT}/mgr/setup.csh
set cmtTruthUtilstempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtTruthUtilstempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt cleanup -csh -pack=TruthUtils -version=TruthUtils-00-00-12 -path=/afs/cern.ch/user/n/narayan/testarea/17.0.5/ZeeB/share/Generators/GenAnalysisTools  -quiet -without_version_directory $* >${cmtTruthUtilstempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt cleanup -csh -pack=TruthUtils -version=TruthUtils-00-00-12 -path=/afs/cern.ch/user/n/narayan/testarea/17.0.5/ZeeB/share/Generators/GenAnalysisTools  -quiet -without_version_directory $* >${cmtTruthUtilstempfile}"
  set cmtcleanupstatus=2
  /bin/rm -f ${cmtTruthUtilstempfile}
  unset cmtTruthUtilstempfile
  exit $cmtcleanupstatus
endif
set cmtcleanupstatus=0
source ${cmtTruthUtilstempfile}
if ( $status != 0 ) then
  set cmtcleanupstatus=2
endif
/bin/rm -f ${cmtTruthUtilstempfile}
unset cmtTruthUtilstempfile
exit $cmtcleanupstatus

