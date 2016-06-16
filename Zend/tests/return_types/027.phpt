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
function fn1010969964()
{
    $c = function (parent $x) : parent {
        return $x;
    };
    var_dump($c->bindTo(null, 'B')(new A()));
}
fn1010969964();
--EXPECTF--
object(A)#%d (0) {
}
