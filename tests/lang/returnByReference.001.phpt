--TEST--
Returning a reference from a function
--FILE--
<?php

function &returnByRef(&$arg1)
{
    return $arg1;
}
function fn1236206968()
{
    $a = 7;
    $b =& returnByRef($a);
    var_dump($b);
    $a++;
    var_dump($b);
}
fn1236206968();
--EXPECT--
int(7)
int(8)
