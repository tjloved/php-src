--TEST--
Bug #72395 (list() regression)
--FILE--
<?php

function fn1485583190()
{
    list(, , $a, , $b, ) = array(1, 2, 3, 4, 5, 6);
    var_dump($a, $b);
}
fn1485583190();
--EXPECT--
int(3)
int(5)
