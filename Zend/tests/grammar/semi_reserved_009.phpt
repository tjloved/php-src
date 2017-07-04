--TEST--
Edge case: T_STRING<as> as T_STRING<?>
--FILE--
<?php

trait TraitA
{
    public static function as()
    {
        echo __METHOD__, PHP_EOL;
    }
}
class Foo
{
    use TraitA {
        as as try;
    }
}
function fn208001263()
{
    Foo::try();
    echo PHP_EOL, "Done", PHP_EOL;
}
fn208001263();
--EXPECTF--
TraitA::as

Done
