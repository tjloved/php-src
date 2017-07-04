--TEST--
Don't optimize dynamic call to non-dynamic one if it drops the warning
--FILE--
<?php

function test()
{
    ((string) 'extract')(['a' => 42]);
}
function fn1485442798()
{
    test();
}
fn1485442798();
--EXPECTF--
Warning: Cannot call extract() dynamically in %s on line %d
