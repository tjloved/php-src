--TEST--
Bug #41633.1 (self:: doesn't work for constants)
--FILE--
<?php

class Foo
{
    const A = self::B;
    const B = "ok";
}
function fn1724526498()
{
    echo Foo::A . "\n";
}
fn1724526498();
--EXPECT--
ok
