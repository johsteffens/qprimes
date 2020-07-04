/**
Prime Number Generator

Author & Copyright (C) 2020 Johannes B. Steffens.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <math.h>
#include <inttypes.h>

int scan_options( const char* s, int* verbose, int* hexout )
{
    if( s[0] >= 'a' && s[0] <= 'z' )
    {
        for( ; s[0] != 0; s++ )
        {
            switch( s[0] )
            {
                case 's': *verbose = 0; break;
                case 'v': *verbose = 1; break;
                case 'd': *hexout  = 0; break;
                case 'x': *hexout  = 1; break;
                default: break;
            }
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

void scan_u64( const char* s, uint64_t* v )
{
    const char* f = (s[0]=='0'&&(s[1]=='x'||s[1]=='X')) ? "%"SCNx64 : "%"SCNu64;
    sscanf( s, f, v );
}

void out_of_memory( void )
{
    fprintf( stderr, "\nOut of memory.\n" );
    exit( 1 );
}

int main( int argc, char* argv[] )
{
    if( argc < 3 )
    {
        printf( "qprimes: Lists all prime numbers between <min> and <max> to stdout.\n" );
        printf( "\n" );
        printf( "Usage: qprimes [OPTION] MIN MAX\n" );
        printf( "\n" );
        printf( "Arguments:\n" );
        printf( "  MIN, MAX:\n" );
        printf( "    Unsigned integer below 2^64.\n" );
        printf( "    Prepending '0x' indicates hexadecimal form.\n" );
        printf( "\n" );
        printf( "  OPTION:\n" );
        printf( "    s    silent; (outputs just prime numbers)\n" );
        printf( "    v    verbose\n" );
        printf( "    x    outputs prime numbers in hexadecimal form\n" );
        printf( "    d    outputs prime numbers in decimal form\n" );
        printf( "\n" );
        printf( "Examples:\n" );
        printf( "    qprimes 100000000000 100000001000\n" );
        printf( "    qprimes 0x1a30 0xfa30\n" );
        printf( "    qprimes xs 0x1a30 0xfa30\n" );
        printf( "\n" );
        return 1;
    }

    uint64_t val_min = 0;
    uint64_t val_max = 0;

    int verbose = 1;
    int hexout = 0;

    int argi = 1;
    if( argi < argc ) argi += scan_options( argv[ argi ], &verbose, &hexout );
    if( argi < argc ) scan_u64( argv[ argi++ ], &val_min );
    if( argi < argc ) scan_u64( argv[ argi++ ], &val_max );

    val_max = val_max < val_min ? val_min : val_max;

    const char* fout = hexout ? "%"PRIx64"\n" : "%"PRIu64"\n";

    // sieving initial primes ...
    uint64_t smax  = sqrt( val_max ) + 1;
    size_t sbits = (  smax >> 1 ) + 1;
    size_t dsize = ( sbits >> 3 ) + 1;
    uint8_t* d = malloc( dsize );
    if( !d ) out_of_memory();
    memset( d, 0, dsize );

    d[ 0 ] = 1;
    for( size_t n = 3; n * n < smax; n += 2 )
    {
        if( ( d[ n >> 4 ] & ( 1 << ( ( n >> 1 ) & 7 ) ) ) == 0 )
        {
            for( size_t i = ( n * n ) >> 1; i < sbits; i += n )
            {
                d[ i >> 3 ] |= ( 1 << ( i & 7 ) );
            }
        }
    }

    // storing initial primes ...
    size_t plimit = ( ( 1.256 * ( smax + 2 ) ) / log( smax + 2 ) );
    uint32_t* parr = malloc( plimit * sizeof( uint32_t ) );
    if( !parr ) out_of_memory();

    size_t psize = 0;
    parr[ psize++ ] = 2;

    for( size_t i = 1; i < sbits; i++ )
    {
        if( ( d[ i >> 3 ] & ( 1 << ( i & 7 ) ) ) == 0 )
        {
            uint64_t prime = i * 2 + 1;
            if( prime <= smax )
            {
                parr[ psize++ ] = prime;
            }
        }
    }

    free( d );

    // output initial primes that overlap with test-range ...
    uint64_t pcount = 0;
    if( parr[ psize - 1 ] >= val_min )
    {
        for( size_t i = 0; i < psize; i++ )
        {
            uint64_t prime = parr[ i ];
            if( prime >= val_min && prime <= val_max )
            {
                printf( fout, prime );
                pcount++;
            }
        }
    }

    const uint64_t pg_size_exp = 20;
    uint64_t pg_size = 1ull << pg_size_exp;
    dsize = pg_size;
    d = malloc( dsize );
    if( !d ) out_of_memory();

    // stepping through pages ...
    for( uint64_t pg = val_min / pg_size; pg <= val_max / pg_size; pg++ )
    {
        memset( d, 0, dsize );
        for( size_t i = 0; i < psize; i++ )
        {
            uint64_t p = parr[ i ];
            for( size_t j = ( p - ( ( pg * pg_size ) % p ) ) % p; j < dsize; j += p )
            {
                d[ j ] = 1;
            }
        }

        for( size_t j = 0; j < dsize; j++ )
        {
            if( !d[ j ] )
            {
                uint64_t prime = pg * pg_size + j;
                if( prime > val_max ) break;
                if( prime > 1 && prime >= val_min )
                {
                    printf( fout, prime );
                    pcount++;
                }
            }
        }
    }

    free( d );
    free( parr );

    if( verbose )
    {
        printf( "\n%"PRIu64" primes between %"PRIu64" and %"PRIu64"\n", pcount, val_min, val_max );
    }

    return 0;
}

