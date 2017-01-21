dnl $Id$
dnl config.m4 for extension face

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(face, for face support,
dnl Make sure that the comment is aligned:
dnl [  --with-face             Include face support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(face, whether to enable face support,
Make sure that the comment is aligned:
[  --enable-face           Enable face support])

if test "$PHP_FACE" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-face -> check with-path
  SEARCH_PATH="/usr/local"
  SEARCH_FOR="/include/opencv2/opencv.hpp"
  if test -r $PHP_FACE/$SEARCH_FOR; then # path given as parameter
    FACE_DIR=$PHP_FACE
  else # search default path list
    AC_MSG_CHECKING([for face files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        FACE_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  
  if test -z "$FACE_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the face distribution])
  fi

  # --with-face -> add include path
  PHP_ADD_INCLUDE($FACE_DIR/include)

  
  PHP_ADD_LIBRARY_WITH_PATH(opencv_core, /usr/local/Cellar/opencv/2.4.13.2/lib, FACE_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(opencv_video, /usr/local/Cellar/opencv/2.4.13.2/lib, FACE_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(opencv_imgproc, /usr/local/Cellar/opencv/2.4.13.2/lib, FACE_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(opencv_highgui, /usr/local/Cellar/opencv/2.4.13.2/lib, FACE_SHARED_LIBADD)

  
  PHP_SUBST(FACE_SHARED_LIBADD)

  PHP_REQUIRE_CXX()
  PHP_SUBST(FACE_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, 1, FACE_SHARED_LIBADD)
  PHP_NEW_EXTENSION(face, face.cc, $ext_shared)
fi