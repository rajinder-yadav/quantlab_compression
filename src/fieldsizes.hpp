//==================================================================================================
// The MIT License (MIT)
//
// Copyright (c) 2016 Rajinder Yadav <rajinder.yadav@hotmail.com>
//==================================================================================================

#ifndef _field_sizes_hpp_
#define _field_sizes_hpp_

/**
 * These definitions control the bits size of each filed. 
 * It controls how data is packed and unpacker from the bit buffer.
 */

const size_ft BUFFER_CAPACITY = 32; // bits
const size_ft TICKER_SIZE     = 14; // bits
const size_ft EXCHANGE_SIZE   = 1;  // char(8bits)
const size_ft SIDE_SIZE       = 2;  // bits
const size_ft CONDITION_SIZE  = 1;  // char(8bits)
const size_ft TIME_SIZE       = 37; // bits
const size_ft PRICE_SIZE      = 8;  // char(8bits)
const size_ft SHARE_SIZE      = 20; // bits

#endif // _field_sizes_hpp_
