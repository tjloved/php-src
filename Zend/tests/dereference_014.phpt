--TEST--
Trying to create an object from dereferencing uninitialized variable
--FILE--
<?php

class foo
{
    public $x;
    public static $y;
    public function a()
    {
        return $this->x;
    }
    public static function b()
    {
        return self::$y;
    }
}
function fn515073463()
{
    error_reporting(E_ALL);
    $foo = new foo();
    $h = $foo->a()[0]->a;
    var_dump($h);
    $h = foo::b()[1]->b;
    var_dump($h);
}
fn515073463();
--EXPECTF--
Notice: Trying to get property of non-object in %s on line %d
NULL

Notice: Trying to get property of non-object in %s on line %d
NULL
