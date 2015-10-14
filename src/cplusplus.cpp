
#include <kstdlib.h>
#include <console.h>
#include <inttypes.h>
#include <ker/new.hpp>

void *operator new( size_t size )
{
	void *ptr = malloc( size );
	memset(ptr, 0, size);
	return ptr;
}

void *operator new[]( size_t  size )
{
	void *ptr = malloc( size );
	memset(ptr, 0, size);
	return ptr;
}

void operator delete( void *obj )
{
	free( obj );
}

void operator delete[]( void *obj )
{
	free( obj );
}

extern "C" void __cxa_pure_virtual()
{
    kprintf("Pure virtual function call.\n");
}

extern "C" int *__errno_location()
{
    static int errno;
    return &errno;
}

extern "C" int fprintf ( void * , const char * , ... )
{
    kprintf("[some fprintf :P]");
    return 0;
}

extern "C"
{
    void *stdin, *stdout, *stderr;
}

// Lizenz: public domain

typedef void (*constructor)();

// Im Linkerskript definiert
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" void initialiseConstructors();

// Ruft die Konstruktoren für globale/statische Objekte auf
void initialiseConstructors()
{
    for (constructor* i = &start_ctors;i != &end_ctors;++i)
        (*i)();
}

extern "C" void cpp_init()
{
    initialiseConstructors();
}

typedef void (*destructor)();

// Im Linkerskript definiert
extern "C" destructor start_dtors;
extern "C" destructor end_dtors;

#include <types/compoundtype.hpp>
extern CompoundType type;

extern "C" void cpp_exit()
{
	for (destructor* i = &start_dtors; i != &end_dtors;++i) {
		kprintf("[dtor:%x]", *i);
		(*i)();
	}
	type.~CompoundType(); // TODO: Remove this. This is dirty. Get the destructor to make a clean exit.
}
