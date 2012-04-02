# echo "setup ZeeB ZeeB-00-00-00 in /afs/cern.ch/user/n/narayan/testarea/17.0.5"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /cvmfs/atlas.cern.ch/repo/sw/software/i686-slc5-gcc43-opt/17.0.5/CMT/v1r23
endif
source ${CMTROOT}/mgr/setup.csh
set cmtZeeBtempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtZeeBtempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=ZeeB -version=ZeeB-00-00-00 -path=/afs/cern.ch/user/n/narayan/testarea/17.0.5  -no_cleanup $* >${cmtZeeBtempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt setup -csh -pack=ZeeB -version=ZeeB-00-00-00 -path=/afs/cern.ch/user/n/narayan/testarea/17.0.5  -no_cleanup $* >${cmtZeeBtempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmtZeeBtempfile}
  unset cmtZeeBtempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmtZeeBtempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmtZeeBtempfile}
unset cmtZeeBtempfile
exit $cmtsetupstatus

