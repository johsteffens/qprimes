# Fast and Simple 64-bit Prime Generator

<a name="anchor_what_it_is"></a>
## What it is

**qprimes** is a console program, computing all prime numbers 
between a specified minimum and maximum value of range [0, 2^64 - 1].

## Usage

Build: `gcc main.c -O3 -lm -o qprimes`

Run `qprimes <min> <max>` to generate prime numbers between `[<min>, <max>]`.

`<min> <max>` can be given in decimal or preceded by `0x` in haxadecimal form.

Run `qprimes` without arguments for more options.

## Quick Example

```
$ git clone https://github.com/johsteffens/qprimes
$ cd qprimes
$ gcc main.c -O3 -lm -o qprimes
$ ./qprimes 10000000000000 10000000000100
10000000000037
10000000000051
10000000000099

3 primes between 10000000000000 and 10000000000100
Heap size: 197899 Bytes
```
## Method

**qprimes** uses a mix of [sieving](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes) 
and paging.

First, a list of all primes between 2 and `sqrt( <max> )` is generated.
That list is used to determine primes in the desired value range.

## Speed and Memory

With `n := squareroot( <max> )` and `r := <max> - <min>`:

   * Processing time is about O( n log log n ) + O( r ).
   * Memory requirement is about O( n ).
   
The maximum possible heap memory usage is around 270 MBytes.
   
### Timing for computing last few primes below 2^64

`qprimes 0xFFFFFFFFFFFFFF00 0xFFFFFFFFFFFFFFFF`

|Platform | Time |
| ------- | ---- |
| Intel^(R) Core^TM i7 6700 |  22 seconds |
| Raspberry Pi 3 Model B+  | 250 seconds | 
| Raspberry Pi 2 Model B   | 370 seconds |

## Motivation

Prime numbers are needed in different disciplines of numerical processing.
I wrote this generator because arbitrary prime numbers above, say 2^40 , are not so easy to find
on public sources.

This solution achieves adequate runtime efficiency given the code-simplicity. 
It is not the fastest possible algorithm but should well suffice for the intended use cases.

Hope you find it useful, too.

------

<sub>&copy; Johannes B. Steffens</sub>

