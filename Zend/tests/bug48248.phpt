--TEST--
Bug #48248 (SIGSEGV when access to private property via &__get)
--FILE--
<?php

class A
{
    public function &__get($name)
    {
        return $this->test;
    }
}
class B extends A
{
    private $test;
}
function fn1696359536()
{
    $b = new B();
    var_dump($b->test);
}
fn1696359536();
--EXPECTF--
Notice: Undefined property: B::$test in %s on line %d

Notice: Only variable references should be returned by reference in %s on line %d
NULL
