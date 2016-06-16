--TEST--
Bug #41633.1 (self:: doesn't work for constants)
--FILE--
<?php

class Foo
{
    const A = self::B;
    const B = "ok";
}
function fn1684200664()
{
    echo Foo::A . "\n";
}
fn1684200664();
--EXPECT--
ok
