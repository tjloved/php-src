--TEST--
Bug #27268 (Bad references accentuated by clone)
--FILE--
<?php

class A
{
    public function &getA()
    {
        return $this->a;
    }
}
function fn2010455800()
{
    $A = new A();
    $A->a = array(1);
    $x = $A->getA();
    $clone = clone $A;
    $clone->a = array();
    print_r($A);
}
fn2010455800();
--EXPECT--
A Object
(
    [a] => Array
        (
            [0] => 1
        )

)
