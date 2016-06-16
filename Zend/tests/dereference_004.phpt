--TEST--
Testing array dereference on __invoke() result
--FILE--
<?php

class foo
{
    public $x = array();
    public function __construct()
    {
        $h = array();
        $h[] = new stdclass();
        $this->x = $h;
    }
    public function __invoke()
    {
        return $this->x;
    }
}
function fn1747273944()
{
    error_reporting(E_ALL);
    $fo = new foo();
    var_dump($fo()[0]);
}
fn1747273944();
--EXPECTF--
object(stdClass)#%d (0) {
}
