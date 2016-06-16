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
function fn660130433()
{
    $x = new X();
    $x->f("this");
}
fn660130433();
--EXPECTF--
object(X)#%d (0) {
}
