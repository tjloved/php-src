--TEST--
Bug #69420 (Invalid read in zend_std_get_method)
--FILE--
<?php

trait T
{
    protected function test()
    {
        echo "okey";
    }
}
class A
{
    protected function test()
    {
    }
}
class B extends A
{
    use T;
    public function foo()
    {
        $this->test();
    }
}
function fn1656110584()
{
    $b = new B();
    $b->foo();
}
fn1656110584();
--EXPECT--
okey
