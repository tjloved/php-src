--TEST--
Bug #70987 (static::class within Closure::call() causes segfault)
--FILE--
<?php

class foo
{
}
function fn768642411()
{
    $bar = function () {
        return static::class;
    };
    var_dump($bar->call(new foo()));
}
fn768642411();
--EXPECTF--
string(3) "foo"
