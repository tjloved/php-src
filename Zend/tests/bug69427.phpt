--TEST--
Bug #69427 (Segfault on magic method __call of private method in superclass)
--FILE--
<?php

class SubClass extends BaseClass
{
}
abstract class BaseClass
{
    public function __call($name, $arguments)
    {
        return $this->{$name}();
    }
    private function foobar()
    {
        return 'okey';
    }
}
function fn614855286()
{
    $test = new SubClass();
    echo $test->foobar();
}
fn614855286();
--EXPECT--
okey
