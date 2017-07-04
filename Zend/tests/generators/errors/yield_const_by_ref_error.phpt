--TEST--
A notice is thrown when yielding a constant value by reference
--FILE--
<?php

function &gen()
{
    (yield "foo");
}
function fn934776479()
{
    $gen = gen();
    var_dump($gen->current());
}
fn934776479();
--EXPECTF--
Notice: Only variable references should be yielded by reference in %s on line %d
string(3) "foo"
