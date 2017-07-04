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
function fn350720454()
{
    $a = new foo();
    var_dump($a->a);
}
fn350720454();
--EXPECT--
int(81)
