--TEST--
closure binding to anonymous class
--FILE--
<?php

function fn1839504173()
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
fn1839504173();
--EXPECTF--
object(class@anonymous)#1 (0) {
}
