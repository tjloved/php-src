--TEST--
Edge case: T_STRING<insteadof> insteadof T_STRING<?>
--FILE--
<?php

trait TraitA
{
    public static function insteadof()
    {
        echo __METHOD__, PHP_EOL;
    }
}
trait TraitB
{
    public static function insteadof()
    {
        echo __METHOD__, PHP_EOL;
    }
}
class Foo
{
    use TraitA, TraitB {
        TraitB::insteadof insteadof TraitA;
    }
}
function fn1706632365()
{
    Foo::insteadof();
    echo PHP_EOL, "Done", PHP_EOL;
}
fn1706632365();
--EXPECTF--
TraitB::insteadof

Done
