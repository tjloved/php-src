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
function fn457322225()
{
    (new X())();
}
fn457322225();
--EXPECT--
object(X)#1 (0) {
}
