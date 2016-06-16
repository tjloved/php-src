--TEST--
closure binding to anonymous class
--FILE--
<?php

function fn583002413()
{
    $class = new class
    {
    };
    $foo = function () {
        return $this;
    };
    $closure = Closure::bind($foo, $class, $class);
    var_dump($closure());
}
fn583002413();
--EXPECTF--
object(class@anonymous)#1 (0) {
}
