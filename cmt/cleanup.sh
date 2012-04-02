# echo "cleanup ZeeB ZeeB-00-00-00 in /afs/cern.ch/user/n/narayan/testarea/17.0.5"

if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/atlas.cern.ch/repo/sw/software/i686-slc5-gcc43-opt/17.0.5/CMT/v1r23; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtZeeBtempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then cmtZeeBtempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt cleanup -sh -pack=ZeeB -version=ZeeB-00-00-00 -path=/afs/cern.ch/user/n/narayan/testarea/17.0.5  $* >${cmtZeeBtempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/mgr/cmt cleanup -sh -pack=ZeeB -version=ZeeB-00-00-00 -path=/afs/cern.ch/user/n/narayan/testarea/17.0.5  $* >${cmtZeeBtempfile}"
  cmtcleanupstatus=2
  /bin/rm -f ${cmtZeeBtempfile}
  unset cmtZeeBtempfile
  return $cmtcleanupstatus
fi
cmtcleanupstatus=0
. ${cmtZeeBtempfile}
if test $? != 0 ; then
  cmtcleanupstatus=2
fi
/bin/rm -f ${cmtZeeBtempfile}
unset cmtZeeBtempfile
return $cmtcleanupstatus

