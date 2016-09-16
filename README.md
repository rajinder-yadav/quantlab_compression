# Quantlab Compression Assignment

## Note About Testing

1. I developed on a 64-bits Linux system and coded for a 64bit system.

1. I had to convert the 'ebat.csv' file to UNIX end of line format as shown below:

```
unzip ebat.zip
dos2unix ebat.csv
```

After running the compression test, when a diff is done, it only differs on the last line, this is from a missing "end of line" on the input file, as shown below:

```
$ diff ebat.csv inflated 
400000c400000
< QHH8,F,A,0,3358199,3358199,266,3
\ No newline at end of file
---
> QHH8,F,A,0,3358199,3358199,266,3
```
Note: All field values are the same.

## Assumptions

 I made the following assumptions for encoding the tick fields:

* ticker - Max unique ticker symbols 10,000.

* exchange - 8 bits could be smaller if I knew all possible values.

* side - 2bits, assume only values are: Bid, Ask, Trade.

* time - Mico-sec since midnight, 24 hrs = 8.64e+10 used 37bits.

* reptime - Played safe, didn't use a delta (reptime - time) as I don't know the max time gap.

* price - Assumed a decimal precision of 12 digits (is configurable).

* size  - Assumed max shares 1048576 (20 bits).

## Configuration

* Field sizes are defined in file fieldsizes.hpp
* Field types are defined in file fieldtypes.hpp

## Compression Ratio

With the conservative values, the compression ratio is: 1.7620665330300422, a 76% compression ratio. 

Here are the files size (below) I used to calculate the ratio.

```
-rw-rw-r-- 1 yadav yadav  9500000 Sep 16 04:52 compressed
-rw-rw-r-- 1 yadav yadav     6376 Sep 16 04:52 compressed.table
-rw-rw-r-- 1 yadav yadav 16750867 Sep 16 04:52 ebat.csv
```
## Design

I took time to think about how to implement a flexible design that with minor changes could be tweaked to get improvements on the compression ratio as well as allow for flexibility of code changes.

To compress the ticker which is both variable in size as contains duplicate names, I decided to use an indexed symbol table.

* First time symbol is discovered, it's added to SymbolTable and a unique index is generated to be bit packed during compression.

* Each time an existing symbol is detected, it's previous index values is used and bit packed.

During the uncompress stage, the symbol table is used to fetch the ticker (symbol) name.

For field 'side' I was able to use 2 bits to encode the data.

* bit 0x00 - (B)id
* bit 0x01 - (A)sk
* bit 0x02 - (T)rade

I may have been able to reduce the size for field 'reptime' by saving it's delta from the 'time' field, but played it safe to just write it out.

## Bit Packing

  1. Initially I thought to use std::bitset, but it proved limited in API support.

  2. I looked at using a Packet of a fixed bit size, but then there would be wasted padding bits.

  3. I also considered using a class with bit fields to save the data.

```C++
// Size of class is 1 8bits, both a1 and a2 saved to same char field!
class A {
   char a1: 2;  // use 2bit
   char a2: 1;  // use 1bit
};
```

The design I chose for bit packing was to go with a bit buffer.

### Bit Buffer
The BitBuffer works like a queue, value are pushed from one end and read from the other end and no padding is required. The BitBuffer class takes cares of encoding a value with a bit representation. This seems to be the optimal design that has flexibility built in.

## Other Design Notes

I've kept the design flexible by allowing the field bit size to be configurable. If the csv input size change, it should be relatively easy to get the code working again.

I also made use of defensive programming and added assert which would be tripped in debug mode, this is both good for the API User as well as implementor.

## Building The Project
I used CMake to build my project which is a cross platform makefile generator, site: https://cmake.org/

I will assume you're working on Linux (use similar commands on Windows). Also I've provided a command to generate a NMake makefile for Windows, but unable to test it.

```
git clone https://github.com/rajinder-yadav/quantlab_compression.git

cd quantlab_compression
mkdir build
cp ebat.csv build/
cd build
dos2unix ebat.csv
cmake -G "Unix Makefiles" -D CMAKE_BUILD_TYPE="Release" ../src
make
```

This should start the build to create a release build of program "quantlab_compression".

For Windows, change the above cmake command to:
```
cmake -G "NMake Makefiles" -D CMAKE_BUILD_TYPE="Release" ../src
```

### Running The Program
To view the program switches, run the program with no arguments, like:
```
$ ./quantlab_compression 

Author: Rajinder Yadav <rajinder.yadav@hotmail.com>

Program Arguments: quantlab_compression <filename> <mode>

<mode> must be one of:

Pass single argument to run the following:

  c = Compress file.
  i = Inflate compressed file.
  b = Compress than inflate.
  t = Run tests.
  a = Run all, Test, Compress, Inflate.
```

To run all the tests, type: 
```
./quantlab_compression t t
```

Everything should pass!

Now to test compression, type:
```
./quantlab_compression ebat.csv c
```

You should see something like the following output:
```
Lines processed: 399997
Lines processed: 399998
Lines processed: 399999
Lines processed: 400000
Compressed successfully!
Filename: compressed
```

There should be two files: compressed and compressed.table

Now let's inflate the compressed file:
```
./quantlab_compression compressed i

Inflating file compressed ... Inflated successfully!
Filename: inflated
```

You should see a file called "inflated".

Now let's count the lines and run a diff to test input file against the uncompressed file:

**The difference is due to file 'ebar.csv' missing an 'end of line' on the last line!**

```
$ wc -l ebat.csv 
399999 ebat.csv

$ wc -l inflated 
400000 inflated


$ diff ebat.csv inflated 
400000c400000
< QHH8,F,A,0,3358199,3358199,266,3
\ No newline at end of file
---
> QHH8,F,A,0,3358199,3358199,266,3

``` 

To view the inflated file type, "less inflated".

Note: You can do the compress, uncompress steps with a single command like this:
```
./quantlab_compression ebat.csv b
```

# Work Log

Sunday 11, 2016
Spent time considering design ideas and basic research (1 hrs).

Monday 12, 2016
Lost time tracking strange errors due to ebat.csv (1 hrs),
sent email out asking if I got correct test file.
Waited from reply before starting to work on a solution.

Tuesday 13, 2016
Started to code at 12:35AM made my first commit.
Coded for roughly 6 hrs and took a break at 7:32 am.

Thursday 15, 2016
Code for 1.5 hours, fixed CSV line read to account for spaces in the ticker name.
Updated README
Update decompress code to stream to file rather than load entire file it memory.


Rajinder yadav

Sept 14, 2016
