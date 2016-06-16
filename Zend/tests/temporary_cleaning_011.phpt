--TEST--
Live range & lists
--FILE--
<?php

class A
{
    function __destruct()
    {
        throw new Exception();
    }
}
function fn491101683()
{
    $b = new A();
    $x = 0;
    $c = [[$x, $x]];
    try {
        list($a, $b) = $c[0];
    } catch (Exception $e) {
        echo "exception\n";
    }
}
fn491101683();
--EXPECT--
exception
