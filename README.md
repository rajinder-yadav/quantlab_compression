# Quantlab Compression Assignment

## Problems Encountered

After having a working implementation I had nagging issues with the sample data (ebat.sip) provided. To check it was the data, I wrote 2 separate methods to parse the CSV tick data and both would core at line 200654.

Please see test code I used under folder "checkdata".


**Output from using function: checkfile_read1()**
```
200646 NQH8,F,B,0,60666897,60666897,1781,1
200647 NQH8,F,A,0,60666897,60666897,1788,6
200648 6AH8,F,A,0,60666908,60666908,90.53,3
200649 6AM8,F,A,0,60666922,60666922,89.44,1
200650 CLH8,F,B,0,60667248,60667248,95.9,1
200651 CLH8,F,A,0,60667248,60667248,95.9,1
200652 RBH8-RBJ8,F,B,0,60667519,60667519,-13.65,50
200653 RBH8-RBJ8,F,A,0,60667519,60667519,-13,1
Segmentation fault (core dumped)
```

**Output from using function: checkfile_read2()**
```
200647 NQH8,F,A,0,60666897,60666897,1788,6
200648 6AH8,F,A,0,60666908,60666908,90.53,3
200649 6AM8,F,A,0,60666922,60666922,89.44,1
200650 CLH8,F,B,0,60667248,60667248,95.9,1
200651 CLH8,F,A,0,60667248,60667248,95.9,1
200652 RBH8-RBJ8,F,B,0,60667519,60667519,-13.65,50
200653 RBH8-RBJ8,F,A,0,60667519,60667519,-13,1
200654 CL:C1
Error: Invalid exchange input skipping input
```

What I think is going on is that the files provided were created and zipped on a Windows system. I am using Linux and possibly two things are at play.

1. unzip utility is doing something odd, causing corruption?

2. End of line, Windows uses "\r\n" vs Unix newline "\n", will cause diff to fail.

Only after using dos2unix I was able to use the 1st 100,000 line of input from ebat.csv, still cores at same line.

I did the following with the input file to make use some of it:

```
unzip ebat.zip
dos2unix ebat.csv
head -n 100000 ebat.csv > testdata
```

So I used the file "testdata" as my actual test sample data which contains the first 1000,000 lines and has been provided with this project (see file testdata).


## Assumptions

 I made the following assumptions for the tick fields:

* ticker - Max unique ticker symbols 10,000.

* exchange - 8 bits could be smaller if I knew all possible values.

* side - 2bits Assume only values are: Bid, Ask, Trade.

* time - Mico-sec since midnight, 24 hrs = 8.64e+10 used 37bits.

* reptime - Played safe, didn't use a delta (reptime - time) as I don't know the max time gap.

* price - Assume max 8 digits including the decimal point.

* size  - Assumed Max shares 1048576 (20 bits).

For configuration, I used constant defined in file fieldsizes.hpp and their types in fieldtypes.hpp

## Compression Ratio

With the conservative values, the compression ratio is: 1.9263258666505005, almost 50% compression. 

Here is are the files size (below) I used to calculate the ratio.

```
-rw-rw-r-- 1 yadav yadav  4575702 Sep 13 17:41 testdata
-rw-rw-r-- 1 yadav yadav  2375000 Sep 14 06:20 compressed
-rw-rw-r-- 1 yadav yadav      352 Sep 14 06:20 compressed.table
```
## Design

I took time to think about how to implement a flexible design that with minor changes could be tweaked to get improvements on the compression ratio as well allow for flexibility of code changes.

To compress the ticker which is both both variable in size as well as duplicates will exists in the tick data. I decided to created an index symbol file.

1. First time symbol is discovered, it's added to SymbolTable and an unique index is generated to be bit packed.

2. Each time an existing symbol is detected, it's previous index values is used and bit packed.

During the uncompress stage Symbols table is used to fetch the ticker (symbol) name.

For field 'side' I was able to use 2 bit to encode the data.

* bit 0x00 - (B)id
* bit 0x01 - (A)sk
* bit 0x02 - (T)rade

3. I may have been able to reduce the size for field 'reptime' by saving it's delta, but played it safe!

  1. Initially I thought to use a std::bitset but it proved limited is API support. 

  2. So I thought maybe I will used a Packet with a fixed bit size, but then there would be wasted unused bits.

  3. Next I considered using a class with bit fields to save the data.


```C++
// Size of class is 1 8bits, both a1 and a2 saved to same char field!
class A {
   char a1: 2;  // use 2bit
   char a2: 1;  // use 1bit
};
```

### Bit Buffer
The design I decided with was to code up a BitBuffer that works like a queue. You push values with its bit size and the BitBuffer class takes care up packing the data. This seem to be the best and most flexible solution considered above.


 For time fields, I decided against packing a double value into a 32bit aligned buffer. I was not sure if data loss would occur converting between a double and two 32bit words. This is one area of improvement that could be made with more research and testing. So I opted for a conservative approach of using a string of max size 8 characters.

## Thoughts

My code could break due to the assumption I've made about field bit sizes to go with. However I've kept the design flexible by allowing the field bit size to be configurable and got with BitBuffer which provides a dense packing of bits.

I wasn't able to test with the entire input test file due to odd corruption, so there might be possible errors due to field sizing that I did not catch. I'm very confident I would be able to tackle these errors in a short amount of time. I also made use of defensive programming and added assert which would be tripped in debug mode, this is both good for the API user as well as implementor.

## Work Log

Sunday 11, 2016
Spent time considering design ideas and basic research (1 hrs).

Monday 12, 2016
Lost time tracking strange errors due to ebat.csv (1 hrs),
sent email out asking if I got correct test file.
Waited from reply before starting to work on a solution.

Tuesday 13, 2016
Started to code at 12:35AM made my first commit.
Coded for roughly 6 hrs and took a break at 7:32 am


Rajinder yadav

Sept 14, 2016
