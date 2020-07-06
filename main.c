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

int scan_options( const char* s, int* verbose, int* hexout, int* primeout )
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
                case 'c': *primeout = 0; break;
                case 'p': *primeout = 1; break;
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

void* qpmalloc( size_t size )
{
    void* p = malloc( size );
    if( !p )
    {
        fprintf( stderr, "\nFailed allocating %zu bytes of memory.\n", size );
        exit( 1 );
    }
    return p;
}

void printhelp( void )
{
    printf
    (
        "qprimes: Lists all prime numbers between <min> and <max> to stdout.\n"
        "\n"
        "Usage: qprimes [OPTION] MIN MAX\n"
        "\n"
        "Arguments:\n"
        "  MIN, MAX:\n"
        "    Unsigned integer below 2^64.\n"
        "    Prepending '0x' indicates hexadecimal form.\n"
        "\n"
        "  OPTION:\n"
        "    v    (default) verbose mode \n"
        "    s              silent mode\n"
        "    p    (default) outputs actual prime numbers\n"
        "    c              outputs prime-count for the given range.\n"
        "    d    (default) outputs numbers in decimal form\n"
        "    x              outputs numbers in hexadecimal form\n"
        "\n"
        "Examples:\n"
        "    qprimes 100000000000 100000001000\n"
        "    qprimes 0x1a30 0xfa30\n"
        "    qprimes cs 0x1a30 0xfa30\n"
        "\n"
    );
}

int main( int argc, char* argv[] )
{
    if( argc < 2 )
    {
        printhelp();
        return 1;
    }

    uint64_t val_min = 0;
    uint64_t val_max = 0;

    int verbose = 1;
    int hexout = 0;
    int primeout = 1;

    int argi = 1;
    if( argi < argc ) argi += scan_options( argv[ argi ], &verbose, &hexout, &primeout );
    if( argi < argc ) scan_u64( argv[ argi++ ], &val_min );
    if( argi < argc ) scan_u64( argv[ argi++ ], &val_max );

    val_max = val_max < val_min ? val_min : val_max;

    uint64_t val_range = val_max - val_min;

    const char* fout = hexout ? "%"PRIx64"\n" : "%"PRIu64"\n";

    // sieving initial primes ...
    uint64_t smax  = sqrt( val_max ) + 1;
    uint64_t sbits = (  smax >> 1 ) + 1;
    size_t d0size = ( sbits >> 3 ) + 1;
    uint8_t* d0 = qpmalloc( d0size );
    memset( d0, 0, d0size );

    d0[ 0 ] = 1;
    for( uint64_t n = 3; n * n < smax; n += 2 )
    {
        if( !( d0[ n >> 4 ] & ( 1 << ( ( n >> 1 ) & 7 ) ) ) )
        {
            for( uint64_t i = ( n * n ) >> 1; i < sbits; i += n )
            {
                d0[ i >> 3 ] |= ( 1 << ( i & 7 ) );
            }
        }
    }

    uint64_t pcount = 0;

    // output initial primes that overlap with test-range ...
    if( val_min <= 2 && val_max >= 2 )
    {
        if( primeout ) printf( fout, 2ull );
        pcount++;
    }

    for( uint64_t i = val_min >> 2; i < sbits; i++ )
    {
        if( !( d0[ i >> 3 ] & ( 1 << ( i & 7 ) ) ) )
        {
            uint64_t prime = i * 2 + 1;
            if( prime >= val_min && prime <= val_max )
            {
                if( primeout ) printf( fout, prime );
                pcount++;
            }
        }
    }

    // stepping through pages ...

    uint64_t pg_size_exp = 20;
    for( uint64_t i = 0; i < pg_size_exp; i++ )
    {
        if( ( 1ull << i ) > val_range )
        {
            pg_size_exp = i + 1;
            break;
        }
    }

    size_t d1size = 1ull << pg_size_exp;
    uint8_t* d1 = qpmalloc( d1size );

    uint64_t pg_start = val_min >> pg_size_exp;
    uint64_t pg_end   = val_max >> pg_size_exp;
    for( uint64_t pg = pg_start; pg <= pg_end; pg++ )
    {
        for( uint64_t j = 0; j < d1size; j++ ) d1[ j ] = ( j & 1 ) ? 0 : 1;
        for( uint64_t i = 1; i < sbits; i++ )
        {
            if( !( d0[ i >> 3 ] & ( 1 << ( i & 7 ) ) ) )
            {
                uint64_t prime = i * 2 + 1;
                uint64_t start = ( prime - ( ( pg << pg_size_exp ) % prime ) ) % prime;
                for( uint64_t j = start; j < d1size; j += prime ) d1[ j ] = 1;
            }
        }

        for( uint64_t j = 0; j < d1size; j++ )
        {
            if( !d1[ j ] )
            {
                uint64_t prime = ( pg << pg_size_exp ) + j;
                if( prime > val_max ) break;
                if( prime > 1 && prime >= val_min )
                {
                    if( primeout ) printf( fout, prime );
                    pcount++;
                }
            }
        }
    }

    if( !primeout ) printf( fout, pcount );

    if( verbose )
    {
        printf( "\n%"PRIu64" primes between %"PRIu64" and %"PRIu64"\n", pcount, val_min, val_max );
        printf( "Heap size: %zu Bytes\n", d0size + d1size );
    }

    free( d0 );
    free( d1 );

    return 0;
}

