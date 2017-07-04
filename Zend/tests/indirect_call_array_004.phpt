--TEST--
Indirect method call by array - Testing exception and method magics
--FILE--
<?php

class foo
{
    public static function abc()
    {
        throw new Exception('foo');
    }
    public function __call($a, $b)
    {
        printf("From %s:\n", __METHOD__);
        throw new Exception($a);
    }
    public static function __callStatic($a, $b)
    {
        printf("From %s:\n", __METHOD__);
        throw new Exception($a);
    }
}
function fn1691842004()
{
    $arr = array('foo', 'abc');
    try {
        $arr();
    } catch (Exception $e) {
        echo $e->getMessage(), "\n";
    }
    $arr = array('foo', '123');
    try {
        $arr();
    } catch (Exception $e) {
        echo $e->getMessage(), "\n";
    }
    echo "------\n";
    $foo = new foo();
    $arr = array($foo, 'abc');
    try {
        $arr();
    } catch (Exception $e) {
        echo $e->getMessage(), "\n";
    }
    $foo = new foo();
    $arr = array($foo, '123');
    try {
        $arr();
    } catch (Exception $e) {
        echo $e->getMessage(), "\n";
    }
}
fn1691842004();
--EXPECTF--
foo
From foo::__callStatic:
123
------
foo
From foo::__call:
123
