
#ifndef RD_CORE_API_H
#define RD_CORE_API_H

#if defined(_WIN32)
#ifdef RD_CORE_STATIC_DEFINE
#  define RD_CORE_API
#  define RD_CORE_NO_EXPORT
#else
#  ifndef RD_CORE_API
#    ifdef rd_core_cpp_EXPORTS
        /* We are building this library */
#      define RD_CORE_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define RD_CORE_API __declspec(dllimport)
#    endif
#  endif

#  ifndef RD_CORE_NO_EXPORT
#    define RD_CORE_NO_EXPORT
#  endif
#endif

#ifndef RD_CORE_DEPRECATED
#  define RD_CORE_DEPRECATED __declspec(deprecated)
#endif

#ifndef RD_CORE_DEPRECATED_EXPORT
#  define RD_CORE_DEPRECATED_EXPORT RD_CORE_API RD_CORE_DEPRECATED
#endif

#ifndef RD_CORE_DEPRECATED_NO_EXPORT
#  define RD_CORE_DEPRECATED_NO_EXPORT RD_CORE_NO_EXPORT RD_CORE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef RD_CORE_NO_DEPRECATED
#    define RD_CORE_NO_DEPRECATED
#  endif
#endif
#endif

#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__)
#ifdef RD_CORE_STATIC_DEFINE
#  define RD_CORE_API
#  define RD_CORE_NO_EXPORT
#else
#  ifndef RD_CORE_API
#    ifdef rd_core_cpp_EXPORTS
        /* We are building this library */
#      define RD_CORE_API __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define RD_CORE_API __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef RD_CORE_NO_EXPORT
#    define RD_CORE_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef RD_CORE_DEPRECATED
#  define RD_CORE_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef RD_CORE_DEPRECATED_EXPORT
#  define RD_CORE_DEPRECATED_EXPORT RD_CORE_API RD_CORE_DEPRECATED
#endif

#ifndef RD_CORE_DEPRECATED_NO_EXPORT
#  define RD_CORE_DEPRECATED_NO_EXPORT RD_CORE_NO_EXPORT RD_CORE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef RD_CORE_NO_DEPRECATED
#    define RD_CORE_NO_DEPRECATED
#  endif
#endif
#endif

#endif /* RD_CORE_API_H */
