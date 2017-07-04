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
function fn470201644()
{
    error_reporting(E_ALL);
    $fo = new foo();
    var_dump($fo()[0]);
}
fn470201644();
--EXPECTF--
object(stdClass)#%d (0) {
}
