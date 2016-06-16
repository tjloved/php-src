--TEST--
Zend signed multiply 64-bit
--SKIPIF--
<?php if ((1 << 31) < 0) print "skip Running on 32-bit target"; ?>
--FILE--
<?php

function fn318868535()
{
    var_dump(0x80000000 * -0xffffffff);
    var_dump(0x80000001 * 0xfffffffe);
    var_dump(0x80000001 * -0xffffffff);
}
fn318868535();
--EXPECTF--
int(-9223372034707292160)
int(9223372036854775806)
float(-9.2233720390023E+18)
