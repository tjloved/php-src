--TEST--
Return type of parent is allowed in closure
--FILE--
<?php

class A
{
}
class B extends A
{
}
function fn868712748()
{
    $c = function (parent $x) : parent {
        return $x;
    };
    var_dump($c->bindTo(null, 'B')(new A()));
}
fn868712748();
--EXPECTF--
object(A)#%d (0) {
}
