--TEST--
Exception during read part of compound assignment operation on a property
--FILE--
<?php

class Test
{
    public function __get($name)
    {
        throw new Exception();
    }
}
function fn1957003788()
{
    $test = new Test();
    try {
        $test->prop += 42;
    } catch (Exception $e) {
    }
    var_dump($test);
}
fn1957003788();
--EXPECT--
object(Test)#1 (0) {
}
