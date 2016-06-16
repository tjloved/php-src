--TEST--
Bug #70478 (**= does no longer work)
--FILE--
<?php

class foo
{
    public $a = 3;
    private $b = 4;
    function __construct()
    {
        $this->a **= $this->b;
    }
}
function fn1413703647()
{
    $a = new foo();
    var_dump($a->a);
}
fn1413703647();
--EXPECT--
int(81)
