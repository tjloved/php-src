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
function fn1341910682()
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
fn1341910682();
--EXPECT--
exception
