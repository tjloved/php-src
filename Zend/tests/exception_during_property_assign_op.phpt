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
function fn822915319()
{
    $test = new Test();
    try {
        $test->prop += 42;
    } catch (Exception $e) {
    }
    var_dump($test);
}
fn822915319();
--EXPECT--
object(Test)#1 (0) {
}
