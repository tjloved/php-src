--TEST--
Bug #70223 (Incrementing value returned by magic getter)
--FILE--
<?php

class A
{
    private $foo = 0;
    public function &__get($foo)
    {
        return $this->foo;
    }
}
function fn1044808575()
{
    $a = new A();
    echo $a->f++;
    echo $a->f++;
    echo $a->f++;
}
fn1044808575();
--EXPECT--
012
