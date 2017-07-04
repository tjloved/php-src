--TEST--
Bug #65291 - get_defined_constants() causes PHP to crash in a very limited case.
--FILE--
<?php

trait TestTrait
{
    public static function testStaticFunction()
    {
        return __CLASS__;
    }
}
class Tester
{
    use TestTrait;
}
function fn10248769()
{
    $foo = Tester::testStaticFunction();
    get_defined_constants();
    get_defined_constants(true);
    echo $foo;
}
fn10248769();
--EXPECT--
Tester
