--TEST--
When __call() is invoked via ::, ensure private implementation of __call() in superclass is accessed without error. 
--FILE--
<?php

class A
{
    private function __call($strMethod, $arrArgs)
    {
        echo "In " . __METHOD__ . "({$strMethod}, array(" . implode(',', $arrArgs) . "))\n";
        var_dump($this);
    }
}
class B extends A
{
    function test()
    {
        A::test1(1, 'a');
        B::test2(1, 'a');
        self::test3(1, 'a');
        parent::test4(1, 'a');
    }
}
function fn4470905()
{
    $b = new B();
    $b->test();
}
fn4470905();
--EXPECTF--
Warning: The magic method __call() must have public visibility and cannot be static in %s__call_005.php on line %d
In A::__call(test1, array(1,a))
object(B)#1 (0) {
}
In A::__call(test2, array(1,a))
object(B)#1 (0) {
}
In A::__call(test3, array(1,a))
object(B)#1 (0) {
}
In A::__call(test4, array(1,a))
object(B)#1 (0) {
}
