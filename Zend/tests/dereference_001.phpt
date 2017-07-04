--TEST--
Testing array dereference
--FILE--
<?php

function a()
{
    return array(1, array(5));
}
// int(5)
function b()
{
    return array();
}
// Notice: Undefined offset: 0
class foo
{
    public $y = 1;
    public function test()
    {
        return array(array(array('foobar')));
    }
}
function c()
{
    return array(new foo());
}
// int(1)
function d()
{
    $obj = new foo();
    return $obj->test();
}
// string(1) "b"
function e()
{
    $y = 'bar';
    $x = array('a' => 'foo', 'b' => $y);
    return $x;
}
function fn1624771744()
{
    error_reporting(E_ALL);
    var_dump(a()[1][0]);
    var_dump(b()[0]);
    var_dump(c()[0]->y);
    var_dump(d()[0][0][0][3]);
    var_dump(e()['b']);
    // string(3) "bar"
}
fn1624771744();
--EXPECTF--
int(5)

Notice: Undefined offset: 0 in %s on line %d
NULL
int(1)
string(1) "b"
string(3) "bar"
