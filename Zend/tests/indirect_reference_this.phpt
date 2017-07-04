--TEST--
Indirect referenced $this
--FILE--
<?php

class X
{
    function f($x)
    {
        var_dump(${$x});
    }
}
function fn1655144984()
{
    $x = new X();
    $x->f("this");
}
fn1655144984();
--EXPECTF--
object(X)#%d (0) {
}
