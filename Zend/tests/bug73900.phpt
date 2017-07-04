--TEST--
Bug #73900: Use After Free in unserialize() SplFixedArray
--FILE--
<?php

function fn721657467()
{
    $a = new stdClass();
    $b = new SplFixedArray(1);
    $b[0] = $a;
    $c =& $b[0];
    var_dump($c);
}
fn721657467();
--EXPECT--
object(stdClass)#1 (0) {
}
