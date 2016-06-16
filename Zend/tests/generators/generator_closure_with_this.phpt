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
function fn1539688708()
{
    $genFactory = (new Test())->getGenFactory();
    var_dump($genFactory()->current());
}
fn1539688708();
--EXPECT--
object(Test)#1 (0) {
}
