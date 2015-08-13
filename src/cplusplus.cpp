
#include <stdlib.h>
#include <inttypes.h>
#include <ker/new.hpp>

void *operator new( size_t size )
{
	return calloc( size );
}

void *operator new[]( size_t  size )
{
	return calloc( size );
}

void operator delete( void *obj )
{
	free( obj );
}

void operator delete[]( void *obj )
{
	free( obj );
}
