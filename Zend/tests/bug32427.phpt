--TEST--
Bug #32427 (Interfaces are not allowed 'static' access modifier)
--FILE--
<?php

interface Example
{
    public static function sillyError();
}
class ExampleImpl implements Example
{
    public static function sillyError()
    {
        echo "I am a silly error\n";
    }
}
function fn573806609()
{
    ExampleImpl::sillyError();
}
fn573806609();
--EXPECT--
I am a silly error
