--TEST--
Unused result of fetch operations
--FILE--
<?php

class Foo
{
    public $prop = array(3);
    function __get($name)
    {
        return array(4);
    }
}
function fn746068776()
{
    $x = array(1);
    $a = "x";
    ${$a};
    $x = array(array(2));
    $x[0];
    $x = "str";
    $x[0];
    $x[3];
    $x = new Foo();
    $x->prop;
    $x->y;
    echo "ok\n";
}
fn746068776();
--EXPECTF--
Notice: Uninitialized string offset: 3 in %sresult_unused.php on line %d
ok

