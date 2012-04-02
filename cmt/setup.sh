# echo "setup ZeeB ZeeB-00-00-00 in /afs/cern.ch/user/n/narayan/testarea/17.0.5"

if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/atlas.cern.ch/repo/sw/software/i686-slc5-gcc43-opt/17.0.5/CMT/v1r23; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtZeeBtempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then cmtZeeBtempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=ZeeB -version=ZeeB-00-00-00 -path=/afs/cern.ch/user/n/narayan/testarea/17.0.5  -no_cleanup $* >${cmtZeeBtempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/mgr/cmt setup -sh -pack=ZeeB -version=ZeeB-00-00-00 -path=/afs/cern.ch/user/n/narayan/testarea/17.0.5  -no_cleanup $* >${cmtZeeBtempfile}"
  cmtsetupstatus=2
  /bin/rm -f ${cmtZeeBtempfile}
  unset cmtZeeBtempfile
  return $cmtsetupstatus
fi
cmtsetupstatus=0
. ${cmtZeeBtempfile}
if test $? != 0 ; then
  cmtsetupstatus=2
fi
/bin/rm -f ${cmtZeeBtempfile}
unset cmtZeeBtempfile
return $cmtsetupstatus

