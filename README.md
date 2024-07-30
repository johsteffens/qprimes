# Fast and Simple 64-bit Prime Generator

## What it is

**qprimes** is a fast console program, computing all prime numbers 
between a specified minimum and maximum value of range $`[0, 2^{64} - 1]`$.

## Usage

   * Build: `gcc main.c -O3 -lm -o qprimes`
   * Run `qprimes MIN MAX` to generate prime numbers between $`[MIN, MAX]`$.
   * $`MIN, MAX`$ can be expressed in decimal form or, if preceded by `0x`, in haxadecimal form.
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
## Speed and Memory

With $`n := \sqrt{ MAX }`$ and $`r := MAX - MIN`$:

   * Processing time is about $`O( n\ log( log( n ) ) )`$ for $`r << n`$.
   * Memory requirement is about $`O( n )`$.
   
The maximum possible heap memory usage is around 270 MBytes.
   
### Timing Example

Command: `qprimes 0xFFFFFFFFFFFFFF00 0xFFFFFFFFFFFFFFFF`
<br>This test computes the last few primes below 2<sup>64</sup>.
It is the worst case for the gven prime window size. Smaller primes will compute faster.

|Platform | Time |
| ------- | ---- |
| AMD Ryzen<sup>TM</sup> 9 7950X | 9 seconds |
| Intel<sup>R</sup> Core<sup>TM</sup> i7 6700 |  22 seconds |
| Raspberry Pi 3 Model B+  | 250 seconds | 
| Raspberry Pi 2 Model B   | 370 seconds |

## Method

**qprimes** uses a combination of [sieving](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes) 
and paging.

### Description

We begin with the prime definition: An integer q > 1 is prime exactly when no 
integer p > 1 and p < q divides q. 

Since any such p is either prime or composite, it is sufficient to test q by all primes below q. 
If any $p > \sqrt{q}$ divides q, so does ${q \over p}$, which is smaller than $\sqrt{q}$. Hence, we need only
test q with primes up to $\sqrt{q}$.

If a sequence of primes is needed, instead of explicitly testing for divisibility, it is 
generally much faster to simply cross out all non-primes in an interval by computing
multiples of primes gathered so far. This approach is called [sieving](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes)
and we use it to collect all primes up to $\sqrt{MAX}$.

Finally, we select an appropriate interval (called page) including the target range $[MIN,MAX]$ and
use previously gathered primes to sieve out non-primes in that interval. What remains is the set
of desired prime values.

Although the method has its roots in ancient times, it is still considered among the most
efficient ways to generate a sequence of prime numbers.

## Motivation

Prime numbers are useful in various disciplines of numerical processing such as
[LCGs](https://en.wikipedia.org/wiki/Linear_congruential_generator)
and [hash tables](https://en.wikipedia.org/wiki/Hash_table).

I experimented especially with [cuckoo hashing](https://en.wikipedia.org/wiki/Cuckoo_hashing)
to develop the specific associative binding and runtime type awareness technique
in [beth](https://github.com/johsteffens/beth). I prefer using LCGs for algorithm testing in
[monte carlo simulations](https://en.wikipedia.org/wiki/Monte_Carlo_method).

I wrote this simple prime generator as tool for developing/improving above techniques.

------

<sub>&copy; Johannes B. Steffens</sub>

