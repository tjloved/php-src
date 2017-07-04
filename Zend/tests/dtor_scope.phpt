--TEST--
Scoping in destructor call
--FILE--
<?php

class T
{
    private $var = array();
    public function add($a)
    {
        array_push($this->var, $a);
    }
    public function __destruct()
    {
        print_r($this->var);
    }
}
class TT extends T
{
}
function fn131023949()
{
    $t = new TT();
    $t->add("Hello");
    $t->add("World");
}
fn131023949();
--EXPECT--
Array
(
    [0] => Hello
    [1] => World
)
