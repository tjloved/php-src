--TEST--
Zend signed multiply 32-bit
--SKIPIF--
<?php if ((1 << 31) > 0) print "skip Running on 64-bit target"; ?>
--FILE--
<?php

function fn1666940685()
{
    var_dump(0x8000 * -0xffff);
    var_dump(0x8001 * 0xfffe);
    var_dump(0x8001 * -0xffff);
}
fn1666940685();
--EXPECTF--
int(-2147450880)
int(2147483646)
float(-2147516415)
