--TEST--
__TRAIT__: Use outside of traits.
--FILE--
<?php

class MyClass
{
    static function test()
    {
        return __TRAIT__;
    }
}
function someFun()
{
    return __TRAIT__;
}
function fn34119365()
{
    $t = __TRAIT__;
    var_dump($t);
    $t = MyClass::test();
    var_dump($t);
    $t = someFun();
    var_dump($t);
}
fn34119365();
--EXPECT--
string(0) ""
string(0) ""
string(0) ""