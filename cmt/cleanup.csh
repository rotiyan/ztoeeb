# echo "cleanup ZeeB ZeeB-00-00-00 in /afs/cern.ch/user/n/narayan/testarea/17.0.5"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/atlas/software/releases/17.0.5/CMT/v1r23
endif
source ${CMTROOT}/mgr/setup.csh
set cmtZeeBtempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtZeeBtempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt cleanup -csh -pack=ZeeB -version=ZeeB-00-00-00 -path=/afs/cern.ch/user/n/narayan/testarea/17.0.5  $* >${cmtZeeBtempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt cleanup -csh -pack=ZeeB -version=ZeeB-00-00-00 -path=/afs/cern.ch/user/n/narayan/testarea/17.0.5  $* >${cmtZeeBtempfile}"
  set cmtcleanupstatus=2
  /bin/rm -f ${cmtZeeBtempfile}
  unset cmtZeeBtempfile
  exit $cmtcleanupstatus
endif
set cmtcleanupstatus=0
source ${cmtZeeBtempfile}
if ( $status != 0 ) then
  set cmtcleanupstatus=2
endif
/bin/rm -f ${cmtZeeBtempfile}
unset cmtZeeBtempfile
exit $cmtcleanupstatus

