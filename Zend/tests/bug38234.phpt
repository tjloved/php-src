--TEST--
Bug #38234 (Exception in __clone makes memory leak)
--FILE--
<?php

class Foo
{
    function __clone()
    {
        throw new Exception();
    }
}
function fn66176985()
{
    try {
        $x = new Foo();
        $y = clone $x;
    } catch (Exception $e) {
    }
    echo "ok\n";
}
fn66176985();
--EXPECT--
ok
