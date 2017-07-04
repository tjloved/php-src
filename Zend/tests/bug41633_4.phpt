--TEST--
Bug #41633.4 (self:: doesn't work for constants)
--FILE--
<?php

class Foo
{
    const A = self::B;
    const B = "ok";
}
function fn1772068125()
{
    var_dump(defined("Foo::A"));
}
fn1772068125();
--EXPECT--
bool(true)
