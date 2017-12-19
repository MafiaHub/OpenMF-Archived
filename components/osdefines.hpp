#ifndef OSDEFINES_H
#define OSDEFINES_H

#if defined (__linux__)
#define OMF_SYSTEM_LINUX
#elif defined(_WIN32_)
#define OMF_SYSTEM_WINDOWS
#else
#error This OS is not supported yet!
#endif

#endif // OSDEFINES_H