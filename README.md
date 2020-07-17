# Fast and Simple 64-bit Prime Generator

## What it is

**qprimes** is a console program, computing all prime numbers 
between a specified minimum and maximum value of range [0, 2<sup>64</sup> - 1].

## Usage

   * Build: `gcc main.c -O3 -lm -o qprimes`
   * Run `qprimes MIN MAX` to generate prime numbers between [MIN, MAX].
   * MIN, MAX can be expressed in decimal form or, if preceded by `0x`, in haxadecimal form.
   * Run `qprimes` without arguments for more options.

## Quick Example

```
$ git clone https://github.com/johsteffens/qprimes
$ cd qprimes
$ gcc main.c -O3 -lm -o qprimes
$ ./qprimes 10000000001099 10000000001199
10000000001141
10000000001161
10000000001177
10000000001191

4 primes between 10000000001099 and 10000000001199
Heap size: 197899 Bytes
```
## Method

**qprimes** uses a mix of [sieving](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes) 
and paging.

First, a list of all primes between 2 and MAX<sup>1/2</sup> is generated.
That list is used to determine primes in the desired value range.

## Speed and Memory

With n := MAX<sup>1/2</sup> and r := MAX - MIN:

   * Processing time is about O( n log log n ) for r << n.
   * Memory requirement is about O( n ).
   
The maximum possible heap memory usage is around 270 MBytes.
   
### Timing Example

Computing the last few primes below 2<sup>64</sup>:
`qprimes 0xFFFFFFFFFFFFFF00 0xFFFFFFFFFFFFFFFF`

|Platform | Time |
| ------- | ---- |
| Intel<sup>R</sup> Core<sup>TM</sup> i7 6700 |  22 seconds |
| Raspberry Pi 3 Model B+  | 250 seconds | 
| Raspberry Pi 2 Model B   | 370 seconds |

## Motivation

Prime numbers are needed in various disciplines of numerical processing.
I wrote this generator because arbitrary prime numbers above, say 2<sup>40</sup>,
were not so easy to find on public sources.

This solution achieves adequate runtime efficiency given the code-simplicity. 
It is not the fastest possible algorithm but should well suffice for the intended use cases.

------

<sub>&copy; Johannes B. Steffens</sub>

