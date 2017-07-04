--TEST--
Bug #70179 ($this refcount issue)
--FILE--
<?php

class X
{
    function __invoke()
    {
        var_dump($this);
    }
}
function fn1917106429()
{
    (new X())();
}
fn1917106429();
--EXPECT--
object(X)#1 (0) {
}
