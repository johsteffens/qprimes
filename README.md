# Fast and Simple 64-bit Prime Generator

<a name="anchor_what_it_is"></a>
## What it is

**qprimes** is a console program, computing all prime numbers 
between a specified minimum and maximum value of range [0, 2^64 - 1].

## Usage

Build: `gcc main.c -O3 -lm -o qprimes`

Run `qprimes` without arguments for extended help.

Run `qprimes [svxd] <min> <max>` to generate prime numbers between `[<min>, <max>]`.

## Quick Example
```
$ git clone https://github.com/johsteffens/qprimes
$ cd qprimes
$ gcc main.c -O3 -lm -o qprimes
$ ./qprimes 0xf000000000000000 0xf000000000000100
17293822569102704683
17293822569102704699
17293822569102704759
17293822569102704761
17293822569102704771
17293822569102704803
17293822569102704887

7 primes between 17293822569102704640 and 17293822569102704896
$
```
## Speed and Memory
Processing time is O( n log log n ).
Memory requirement is O( n / log( n ) ).

In most practical situations `n` can be set to `squareroot( <max> )`.

Computation time is generally below 30 seconds
*(tested on Intel^(R) Core^TM i7 6700)*. If the value range `<max>-<min>`
is extremely large, the total time can be larger.

Maximum possible memory usage is around 1200 MBytes. 

## Method

**qprimes** uses a mix of [sieving](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes) 
and paging.

First, a list of all primes between 2 and `sqrt( <max> )` is generated.
Then that list is used to sieve out the primes in the specified value range.

## Motivation

I had some need for prime numbers for Monte Carlo randomization and realized that above, 
say 2^40 , prime numbers are not so easy to come by. So, I wrote this generator.

This solution is probably a good balance between speed and code-complexity and thus might be
interesting to a wider public. It is not the fastest algorithm but fast enough to allow waiting for
the result without getting too impatient.

The name is an abbreviation from *Quick Prime Numbers*

Hope you find it useful.

------

<sub>&copy; Johannes B. Steffens</sub>
