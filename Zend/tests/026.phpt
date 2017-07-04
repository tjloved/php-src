--TEST--
Trying assign value to property when an object is not returned in a function
--FILE--
<?php

class foo
{
    public function a()
    {
    }
}
function fn1764810107()
{
    $test = new foo();
    $test->a()->a;
    print "ok\n";
    $test->a()->a = 1;
    print "ok\n";
}
fn1764810107();
--EXPECTF--
Notice: Trying to get property of non-object in %s on line %d
ok

Warning: Creating default object from empty value in %s on line %d
ok
