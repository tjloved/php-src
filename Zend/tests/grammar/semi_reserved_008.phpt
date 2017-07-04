--TEST--
Testing with comments around semi-reserved names (not intended to be legible)
--FILE--
<?php

trait TraitA
{
    public static function list()
    {
        echo __METHOD__, PHP_EOL;
    }
    public static function catch()
    {
        echo __METHOD__, PHP_EOL;
    }
    private static function throw()
    {
        echo __METHOD__, PHP_EOL;
    }
    private static function self()
    {
        echo __METHOD__, PHP_EOL;
    }
}
trait TraitB
{
    public static function exit()
    {
        echo __METHOD__, PHP_EOL;
    }
    protected static function try()
    {
        echo __METHOD__, PHP_EOL;
    }
}
class Foo
{
    use TraitA {
        TraitA::catch insteadof TraitB;
        TraitA::list as public foreach;
    }
    use TraitB {
        try as public attempt;
        exit as die;
        // non qualified
        \TraitB::exit as bye;
        // full qualified
        namespace\TraitB::exit as byebye;
        // even more full qualified
        TraitB::exit as farewell;
    }
}
function fn194889570()
{
    Foo::attempt();
    echo PHP_EOL, "Done", PHP_EOL;
}
fn194889570();
--EXPECTF--
TraitB::try

Done
