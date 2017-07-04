--TEST--
Trying to write on method return
--FILE--
<?php

class foo
{
    public $x = array();
    public function b()
    {
        return $this->x;
    }
    public function c()
    {
        return $x;
    }
    public static function d()
    {
    }
}
function fn1286948011()
{
    error_reporting(E_ALL);
    $foo = new foo();
    $foo->b()[0] = 1;
    $foo->c()[100] = 2;
    foo::d()[] = 3;
    print "ok\n";
}
fn1286948011();
--EXPECTF--
Notice: Undefined variable: x in %s on line %d
ok
