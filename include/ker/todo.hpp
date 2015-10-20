#pragma once

#define DO_PRAGMA(x) _Pragma (#x)
#define TODO(...) DO_PRAGMA(message ("TODO - " #__VA_ARGS__))
#define FIXME(...) DO_PRAGMA(message ("FIX - " #__VA_ARGS__))
