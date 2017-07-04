--TEST--
Non-static closures can be generators
--FILE--
<?php

class Test
{
    public function getGenFactory()
    {
        return function () {
            (yield $this);
        };
    }
}
function fn1119890649()
{
    $genFactory = (new Test())->getGenFactory();
    var_dump($genFactory()->current());
}
fn1119890649();
--EXPECT--
object(Test)#1 (0) {
}
