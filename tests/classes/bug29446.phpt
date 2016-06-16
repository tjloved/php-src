--TEST--
Bug #29446 (ZE allows multiple declarations of the same class constant)
--FILE--
<?php

class testClass
{
    const TEST_CONST = 'test';
    const TEST_CONST = 'test1';
    function testClass()
    {
        echo self::TEST_CONST;
    }
}
function fn1185712176()
{
    $test = new testClass();
}
fn1185712176();
--EXPECTF--
Fatal error: Cannot redefine class constant testClass::TEST_CONST in %s on line %d
