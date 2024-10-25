
#ifndef REMATCH_EXPORT_H
#define REMATCH_EXPORT_H

#ifdef REMATCH_STATIC_DEFINE
#  define REMATCH_EXPORT
#  define REMATCH_NO_EXPORT
#else
#  ifndef REMATCH_EXPORT
#    ifdef REmatch_EXPORTS
        /* We are building this library */
#      define REMATCH_EXPORT 
#    else
        /* We are using this library */
#      define REMATCH_EXPORT 
#    endif
#  endif

#  ifndef REMATCH_NO_EXPORT
#    define REMATCH_NO_EXPORT 
#  endif
#endif

#ifndef REMATCH_DEPRECATED
#  define REMATCH_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef REMATCH_DEPRECATED_EXPORT
#  define REMATCH_DEPRECATED_EXPORT REMATCH_EXPORT REMATCH_DEPRECATED
#endif

#ifndef REMATCH_DEPRECATED_NO_EXPORT
#  define REMATCH_DEPRECATED_NO_EXPORT REMATCH_NO_EXPORT REMATCH_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef REMATCH_NO_DEPRECATED
#    define REMATCH_NO_DEPRECATED
#  endif
#endif

#endif /* REMATCH_EXPORT_H */
