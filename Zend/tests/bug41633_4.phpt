--TEST--
Bug #41633.4 (self:: doesn't work for constants)
--FILE--
<?php

class Foo
{
    const A = self::B;
    const B = "ok";
}
function fn1742924656()
{
    var_dump(defined("Foo::A"));
}
fn1742924656();
--EXPECT--
bool(true)
