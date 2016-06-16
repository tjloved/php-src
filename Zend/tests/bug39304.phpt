--TEST--
Bug #39304 (Segmentation fault with list unpacking of string offset)
--FILE--
<?php

function fn1788374704()
{
    $s = "";
    list($a, $b) = $s[0];
    echo "I am alive";
}
fn1788374704();
--EXPECTF--
Notice: Uninitialized string offset: 0 in %sbug39304.php on line %d
I am alive

