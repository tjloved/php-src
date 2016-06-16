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
function fn172289094()
{
    $a = new A();
    echo $a->f++;
    echo $a->f++;
    echo $a->f++;
}
fn172289094();
--EXPECT--
012
