#ifndef __HERMES_CONFIG_H_
#define __HERMES_CONFIG_H_

#cmakedefine HERMES_STATIC_LIBS

// OpenMP
#cmakedefine NUM_THREADS ${NUM_THREADS}
#cmakedefine WITH_OPENMP

#cmakedefine HAVE_FMEMOPEN
#cmakedefine HAVE_LOG2
#cmakedefine EXTREME_QUAD

#cmakedefine WITH_UMFPACK
#cmakedefine WITH_PARALUTION
#cmakedefine WITH_MUMPS
#cmakedefine WITH_SUPERLU
#cmakedefine WITH_PETSC
#cmakedefine WITH_MATIO
#cmakedefine WITH_EXODUSII
#cmakedefine WITH_MPI

#cmakedefine WITH_TC_MALLOC
#cmakedefine WITH_PJLIB
#cmakedefine WITH_BSON
#cmakedefine WITH_MATIO
#cmakedefine MONGO_STATIC_BUILD
#cmakedefine UMFPACK_LONG_INT

// stacktrace
#cmakedefine WITH_WINDOWS_STACKWALKER
#cmakedefine EXECINFO_FOUND

// trilinos
#cmakedefine WITH_TRILINOS
#cmakedefine HAVE_AMESOS
#cmakedefine HAVE_AZTECOO
#cmakedefine HAVE_TEUCHOS
#cmakedefine HAVE_EPETRA
#cmakedefine HAVE_IFPACK
#cmakedefine HAVE_ML
#cmakedefine HAVE_NOX
#cmakedefine HAVE_KOMPLEX

// GLUT
#cmakedefine NOGLUT

// BFD
#cmakedefine HAVE_BFD

#endif
