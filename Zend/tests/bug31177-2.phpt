--TEST--
Bug #31177 (memory corruption because of incorrect refcounting)
--FILE--
<?php

class foo
{
    function __construct($n = 0)
    {
        if ($n) {
            throw new Exception("new");
        }
    }
}
function fn566985599()
{
    $x = new foo();
    try {
        $y = $x->__construct(1);
    } catch (Exception $e) {
        var_dump($x);
    }
}
fn566985599();
--EXPECT--
object(foo)#1 (0) {
}
